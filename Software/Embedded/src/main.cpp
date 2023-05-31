#include "include/Defines.hpp"

// set the variables to the default values in case we fail to retrieve the data from the EEPROM
unsigned int currentNegotiated = Device_Target_Current_Default;
unsigned int voltageNegotiated = Device_Target_Voltage_Default;
unsigned int currentLimit = PFuse_Trip_Current_Default;
unsigned int backlightBrightness = Display_Brightness_Default;

// overcurrent boolean
bool overcurrent = false;
bool outputEnabled = false;

// pps variables
bool ppsReady = false;
float maxCurrent = 0;
float maxVoltage = 0;
PD_power_option_t PDList[] = {
	PD_POWER_OPTION_MAX_5V, 
	PD_POWER_OPTION_MAX_9V, 
	PD_POWER_OPTION_MAX_12V, 
	PD_POWER_OPTION_MAX_15V, 
	PD_POWER_OPTION_MAX_20V
};

// set the display parameters
// the ifdefs are used to prevent the intellisense from complaining about the variables not being defined
Display_Pins displayPins = {
#ifndef __INTELLISENSE__
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.bl = DISP_PIN_BL
#endif
};

Display_Params displayParams = {
#ifndef __INTELLISENSE__
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION
#endif
};

// Create the objects
Button buttonUp(BUTTON_UP);
Button buttonMenu(BUTTON_MENU);
Button buttonDown(BUTTON_DOWN);
AdvancedGraphics display(spi0, displayPins, displayParams, display_type_t::ST7789, true);
Memory memory(EEPROM_ADDRESS, i2c0);
INA219 ina219(INA219_ADDRESS, i2c0);
Registers registers;
class PD_UFP_log_c PD_UFP(PD_LOG_LEVEL_VERBOSE);

/**
 * @brief Initialize the I2C busses
*/
void initI2C()
{
	// initialize the I2C0 bus
	gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C0_SDA);
	gpio_pull_up(I2C0_SCL);
	i2c_init(i2c0, I2C0_SPEED);
	// set the binary data to show the pins used for I2C0
	bi_decl(bi_2pins_with_func(I2C0_SDA, I2C0_SCL, GPIO_FUNC_I2C));

	// initialize the I2C1 bus
	gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C1_SDA);
	gpio_pull_up(I2C1_SCL);
	i2c_init(i2c1, I2C1_SPEED);
	// set the binary data to show the pins used for I2C1
	bi_decl(bi_2pins_with_func(I2C1_SDA, I2C1_SCL, GPIO_FUNC_I2C));
}

/**
 * @brief Initialize the LEDs
*/
void initLEDs()
{
	// init the pins
	gpio_init(LEFT_MOSFET_LED);
	gpio_init(RIGHT_MOSFET_LED);
	gpio_init(LEFT_MOSFET);
	gpio_init(RIGHT_MOSFET);

	// set the pins to output
	gpio_set_dir(LEFT_MOSFET_LED, GPIO_OUT);
	gpio_set_dir(RIGHT_MOSFET_LED, GPIO_OUT);
	gpio_set_dir(LEFT_MOSFET, GPIO_OUT);
	gpio_set_dir(RIGHT_MOSFET, GPIO_OUT);

	// set the binary data to show the pins used for the LEDs
	bi_decl(bi_4pins_with_names(
		LEFT_MOSFET_LED, "Left MOSFET LED", 
		LEFT_MOSFET, "Left MOSFET", 
		RIGHT_MOSFET_LED, "Right MOSFET LED", 
		RIGHT_MOSFET, "Right MOSFET")
	);
}

/**
 * @brief Error function
 * @note This is a blocking function that blinks the LEDS if an error occurs
*/
void Error()
{
	unsigned long lastErrorBlink = 0;
	uint ledErrorValue = 0;
	while(1)
	{
		if((time_us_32() - lastErrorBlink) > 100000)
		{
			gpio_put(LEFT_MOSFET_LED, ledErrorValue ^= 1);
			gpio_put(RIGHT_MOSFET_LED, ledErrorValue ^= 1);
			lastErrorBlink = time_us_32();
		}
	}
}

/**
 * @brief Get the data from the EEPROM
*/
void fetchDataFromEEPROM()
{
	// verify that the EEPROM is connected
	if(!memory.verifyConnection())
	{
		// eeprom not found, revert to default values
		printf("EEPROM not found!\n");
		return;
	}

	// eeprom found, fetch the values
	currentNegotiated = memory.readWord(MEMORY_CURRENT_ADDRESS);
	voltageNegotiated = memory.readWord(MEMORY_VOLTAGE_ADDRESS);
	currentLimit = memory.readWord(MEMORY_CURRENT_LIMIT_ADDRESS);
	backlightBrightness = memory.readWord(MEMORY_BACKLIGHT_ADDRESS);
}

// pattern data
unsigned long lastWarningBlink = 0;
bool ledStates = false;
const long blinkIntervalSequence[4] = { 100000, 62500, 100000, 600000};
/**
 * @brief Blink both LEDs on the board in a pattern if the current is too high!
 * @note Has to be called every loop
*/
void overCurrentLEDs()
{
	static int overCurrentSequenceIndex = 0;

	// if we are not over current, reset the sequence index
	if(!overcurrent && ledStates)
	{
		ledStates = false;
		overCurrentSequenceIndex = 0;
		gpio_put(LEFT_MOSFET_LED, ledStates);
		gpio_put(RIGHT_MOSFET_LED, ledStates);
	}

	// if we havent reached our current interval yet, return
	if(((time_us_32() - lastWarningBlink) < blinkIntervalSequence[overCurrentSequenceIndex]) || !overcurrent)
		return;
	
	// increment the sequence index
	overCurrentSequenceIndex++;

	// if we have reached the end of the sequence, reset the index
	if(overCurrentSequenceIndex > 3)
		overCurrentSequenceIndex = 0;

	// toggle the LED
	gpio_put(LEFT_MOSFET_LED, ledStates);
	gpio_put(RIGHT_MOSFET_LED, ledStates);
	// set the last debounce time
	lastWarningBlink = time_us_32();
	ledStates = !ledStates;
}

/**
 * @brief Handle the register access from writing to them or reading from them
 * @note Has to be called every loop
 * @param all If true, all registers will be handled, if false, only the ones that have changed will be handled
*/
void RegisterHandler(bool all = false)
{
	if(all)
	{
		if(registers.getProtected(Register_Address::Device_Reboot_Bootloader))
		{
			// reset the device into bootloader mode
			// the intellisense doesnt like this function so we have to disable it until we build
#ifndef __INTELLISENSE__
			multicore_reset_core1();
			reset_usb_boot(0, 0);
			while(1);
#endif			
		}
		if(registers.getProtected(Register_Address::Device_Reset))
		{
			watchdog_reboot(0, SRAM_END, 0);
			while(1);
		}
		if(registers.getProtected(Register_Address::Device_Self_Test))
		{
		}
	}
	else
	{

	}
}

// empty buffer to store the data
char buffer[8] = {0};
/**
 * @brief Process the USB data
 * @note Has to be called every loop
 */
void processUSBData()
{
	// check if the USB has data for us, if not, return
	if(!tud_cdc_available())
		return;

	// read the data from the USB
	tud_cdc_read(buffer, 7);

	/*
		The new protocol works as follows:
		- The first character indicates read or write (0 = read, 1 = write)
		- The second character is the address
		(IF WRITING)
		{
			(IF ARRAY)
			- The third character is the index
			- The next 4 characters are the data
			(IF SINGLE)
			- The next 4 characters are the data
		}
		(IF READING)
		{
			(IF ARRAY)
			- The third character is the index
			- Return the data
			(IF SINGLE)
			- Return the data
		}
	*/

	// check if we are reading or writing
	bool isWrite = buffer[0] != 0;

	// are we writing?
	if(isWrite)
	{
		// is it an array?
		if(registers.isArray((Register_Address)buffer[1]))
		{
			unsigned int index = buffer[2];
			unsigned int data = buffer[3] << 24 | buffer[4] << 16 | buffer[5] << 8 | buffer[6];
			// write the data to the array
			registers.set((Register_Address)buffer[1], index, data);
		}
		else
		{
			unsigned int data = buffer[2] << 24 | buffer[3] << 16 | buffer[4] << 8 | buffer[5];
			// write the data to the register
			registers.set((Register_Address)buffer[1], data);
		}

		// go through all the registers to act on change
		RegisterHandler(true);
	}
	else
	{
		// is it an array?
		if(registers.isArray((Register_Address)buffer[1]))
		{
			unsigned int index = buffer[2];
			// read the data from the array
			unsigned int data = registers.get((Register_Address)buffer[1], index);
			// send the data back
			printf("%d\n", data);
		}
		else
		{
			// read the data from the register
			unsigned int data = registers.get((Register_Address)buffer[1]);
			// send the data back
			printf("%d\n", data);
		}
	}

	// clear the buffer
	memset(buffer, 0, sizeof(buffer));
}


/**
 * @brief Button handler
 * @note Has to be called every loop
*/
void buttonHandler()
{
	if(buttonMenu.isHeld())
	{
		static bool state = false;
		state = !state;
		gpio_put(LEFT_MOSFET, state);
		gpio_put(RIGHT_MOSFET, state);
		gpio_put(LEFT_MOSFET_LED, state);
		gpio_put(RIGHT_MOSFET_LED, state);
		printf("MENU held\n");
	}

	buttonUp.update();
	buttonMenu.update();
	buttonDown.update();
}

/**
 * @brief Main function
 * @note This runs on the core 1
*/
void core1Main()
{
	// tell the other core that we are ready
	multicore_fifo_push_blocking(MULTICORE_FLAG_VALUE);
	// wait for the other core to be ready
	uint g = multicore_fifo_pop_blocking();

	// check if the second core is ready
	if(g != MULTICORE_FLAG_VALUE)
		// Enter an infinite loop if the first core is not ready
		Error();
	else
		// tell the second core that we are ready
		multicore_fifo_push_blocking(MULTICORE_FLAG_VALUE);
	
	// create points for important locations
	Point cursor = Point(0, 10);
	Point center = display.getCenter();

	// timer for avoiding too fast screen updates
	unsigned long lastUpdate = 0;
	// should be updated every 10fps
	unsigned long updateInterval = 100000;
	unsigned long timer = 0, lastTimer = 0;
	double framerate = 0;

	while(1)
	{
		lastTimer = time_us_32();
		// draw the background
		display.drawRotRectGradient(center, display.getWidth(), display.getHeight(), 10, Colors::OrangeRed, Colors::DarkYellow);
		display.setCursor(cursor);

		// draw the current
		double current = ina219.getCurrent() / 1000;
		if(current > 10)
			display.print(current, 1, 2);
		else
			display.print(current, 2, 2);
		display.print("A", 2);
		display.print(" ", 2);
		display.print(framerate, Colors::GreenYellow, 2, 1);
		display.println(" ", 2);

		// draw the voltage
		double voltage = ina219.getVoltage();
		if(voltage > 10)
			display.print(voltage, 1, 2);
		else
			display.print(voltage, 2, 2);
		display.println("V", 2);

		// draw the power
		double power = ina219.getPower() / 1000;
		if(power > 10)
			display.print(power, 1, 2);
		else
			display.print(power, 2, 2);
		display.print("W\n", 2);

		// output the data to the display
		display.writeBuffer();

		// reset the timer
		lastUpdate = time_us_32();
		timer = time_us_32() - lastTimer;
		framerate = 1000000.0 / timer;
		printf("%lu (%.2fhz)\n", timer, framerate);
	}
}

float toVolt(int volt, bool pps = false);
float toAmp(int amp, bool pps = false);

void prepPSU()
{
	PD_UFP.init_PPS(PPS_V(5.0), PPS_A(1.0), PD_POWER_OPTION_MAX_5V);

	unsigned long time = time_us_32();
	while(time_us_32() - time < 1000)
	{
		PD_UFP.run();
		if(PD_UFP.is_PPS_ready())
		{
			ppsReady = true;
			break;
		}
	}

	if(ppsReady)
	{
		// check current
		for(int i = 0; i < PPS_A(5.0); i += PPS_A(0.05))
		{
			if(PD_UFP.set_PPS(PPS_V(5.0), i))
			{
				while(PD_UFP.is_ps_transition())
					PD_UFP.run();
				
				if(PD_UFP.is_PPS_ready())
					maxCurrent = toAmp(PD_UFP.get_current(), ppsReady);
			}
		}

		// check voltage
		for(int i = PPS_V(3.3); i <= PPS_V(21.0); i += PPS_V(1.0))
		{
			if(PD_UFP.set_PPS(i, PPS_A(maxCurrent)))
			{
				while(PD_UFP.is_ps_transition())
					PD_UFP.run();
				
				if(PD_UFP.is_PPS_ready())
					maxVoltage = toVolt(PD_UFP.get_voltage(), ppsReady);
			}
		}
	}
	else
	{
		// check voltage
		PD_UFP.set_power_option(PD_POWER_OPTION_MAX_VOLTAGE);
		while(PD_UFP.is_ps_transition())
			PD_UFP.run();

		maxVoltage = toVolt(PD_UFP.get_voltage(), ppsReady);

		// check current
		PD_UFP.set_power_option(PD_POWER_OPTION_MAX_CURRENT);
		while(PD_UFP.is_ps_transition())
			PD_UFP.run();

		maxCurrent = toAmp(PD_UFP.get_current(), ppsReady);
	}
}

// convert the voltage to the correct values
float toVolt(int volt, bool pps)
{
	float result = 0.0;

	if(pps)
	{
		result = (volt - 0.01) / 50.0;
		result = result < 0 ? 0.0 : result;
	}
	else
	{
		result = (volt - 0.01) / 20.0;
		result = result < 0 ? 0.0 : result;
	}
	
	return result;
}

// convert the current to the correct values
float toAmp(int amp, bool pps)
{
	float result = 0.0;

	if(pps)
	{
		result = (amp - 0.01) / 20.0;
		result = result < 0 ? 0.0 : result;
	}
	else
	{
		result = (amp - 0.01) / 100.0;
		result = result < 0 ? 0.0 : result;
	}

	return result;
}

void request(int* volt, int* current, bool pps)
{
	// if the PPS is ready, request the voltage and current
	if(pps)
	{
		if(*volt <= PPS_V(maxVoltage) && *current <= PPS_A(maxCurrent))
		{
			if(*volt < PPS_V(3.3))
				*volt = PPS_V(3.3);
			if(*current < PPS_A(0.01))
				*current = PPS_A(0.01);

			PD_UFP.set_PPS(*volt, *current);

			while(PD_UFP.is_ps_transition())
				PD_UFP.run();
		}
	}
	else
	{
		// as we dont have PPS, the voltage is fixed to either 5, 9, 12, 15 or 20 volts and we need to find which one is the closest to the requested voltage
		if(*volt <= 5)
			*volt = 5;
		else if(*volt <= 9)
			*volt = 9;
		else if(*volt <= 12)
			*volt = 12;
		else if(*volt <= 15)
			*volt = 15;
		else
			*volt = 20;

		PD_power_option_t option = PDList[(int)((*volt - 5) / 3)];
		PD_UFP.set_power_option(option);

		while(PD_UFP.is_ps_transition())
			PD_UFP.run();
	}
}

/**
 * @brief Main function
 * @note This runs on the core 0
*/
int main()
{
	// setup the microcontroller
	stdio_init_all();
	initI2C();
	initLEDs();
	fetchDataFromEEPROM();

	// setup the ina219 current sensor
	ina219.reset();
	ina219.getData(true);
	ina219.setCalibration();
	ina219.setBusVoltageRange(INA219_BUS_VOLTAGE_RANGE_32V);
	ina219.setGain(INA219_GAIN_320MV);
	ina219.setBusADCResolution(INA219_64SAMPLES_34MS);
	ina219.setShuntADCResolution(INA219_64SAMPLES_34MS);
	ina219.setMode(INA219_MODE_SHUNT_AND_BUS_VOLTAGE_CONTINUOUS);
	ina219.setData();
	ina219.getData(true);

	//PD_UFP.init_PPS(PPS_V(4.2), PPS_A(6.9));
	PD_UFP.clock_prescale_set(2);
	prepPSU();
	int volt = 20;
	int current = 1;
	request(&volt, &current, ppsReady);

	// setup the second core
	multicore_launch_core1(core1Main);

	// wait for the second core to be ready
	uint g = multicore_fifo_pop_blocking();

	// check if the second core is ready
	if(g != MULTICORE_FLAG_VALUE)
		// Enter an infinite loop if the second core is not ready
		Error();
	else
		// tell the second core that we are ready
		multicore_fifo_push_blocking(MULTICORE_FLAG_VALUE);

	// run the main loop
	while(1)
	{
		ina219.getData();	
		processUSBData();
		RegisterHandler();
		buttonHandler();
		request(&volt, &current, ppsReady);
		PD_UFP.run();

		// transfer the data from the INA219 to the registers for external access
		registers.setProtected(Register_Address::Bus_Voltage, ina219.getBusVoltageRaw());
		registers.setProtected(Register_Address::Shunt_Voltage, ina219.getShuntVoltage());
		registers.setProtected(Register_Address::Current, ina219.getCurrentRaw());
		registers.setProtected(Register_Address::Power, ina219.getPowerRaw());

		if(ina219.getCurrent() > currentLimit)
			overcurrent = true;
		else if (ina219.getCurrent() < (float)currentLimit * 0.8)
			overcurrent = false;
	}
}
