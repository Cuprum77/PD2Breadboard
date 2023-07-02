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
	.interface = SPI_Interface_t::PIO_HW,
	.type = display_type_t::ST7789,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION
#endif
};

// Create the display object
HardwareSPI spi(displayPins, displayParams);
Display display(&spi, &displayPins, &displayParams);
// Create the GFX objects
Print print(display.getFrameBuffer(), displayParams);
Graphics graphics(display.getFrameBuffer(), displayParams);
AdvancedGraphics advancedGraphics(display.getFrameBuffer(), displayParams);
// Create the PicoGFX object
PicoGFX picoGFX(&display, &print, &graphics, &advancedGraphics);

// Create the objects
Button buttonUp(BUTTON_UP);
Button buttonMenu(BUTTON_MENU);
Button buttonDown(BUTTON_DOWN);
Memory memory(EEPROM_ADDRESS, i2c0);
INA219 ina219(INA219_ADDRESS, i2c0);
Registers registers;
PD_UFP_log_c PD_UFP(PD_LOG_LEVEL_VERBOSE);

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

	// set mosfet to high to disable the output
	gpio_put(LEFT_MOSFET, 1);
	gpio_put(RIGHT_MOSFET, 1);

	// set the binary data to show the pins used for the LEDs
	bi_decl(bi_4pins_with_names(
		LEFT_MOSFET_LED, "Left MOSFET LED", 
		LEFT_MOSFET, "Left MOSFET", 
		RIGHT_MOSFET_LED, "Right MOSFET LED", 
		RIGHT_MOSFET, "Right MOSFET")
	);
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
		static bool state = true;
		state = !state;
		gpio_put(LEFT_MOSFET, !state);
		gpio_put(RIGHT_MOSFET, !state);
		gpio_put(LEFT_MOSFET_LED, state);
		gpio_put(RIGHT_MOSFET_LED, state);
		printf("MENU held\n");
	}

	buttonUp.update();
	buttonMenu.update();
	buttonDown.update();
}

float toVolt(int volt, bool pps = false);
float toAmp(int amp, bool pps = false);

void prepPSU()
{
	PD_UFP.clock_prescale_set(4);
	PD_UFP.init_PPS(PPS_V(15.0), PPS_A(1.0), PD_POWER_OPTION_MAX_POWER);

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

void request(float* volt, float* current, bool pps)
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

	//PD_UFP.init_PPS(PPS_V(4.20), PPS_A(0.69));
	prepPSU();
	float volt = 20;
	float current = 1;
	request(&volt, &current, ppsReady);

	// create points for important locations
	Point cursor = Point(0, 0);
	Point center = picoGFX.getDisplay().getCenter();
	picoGFX.getPrint().setColor(Colors::White);

	// timer for the framerate calculation
	unsigned long timer = 0, lastTimer = 0;
	double framerate = 0;

	// run the main loop
	while(1)
	{
		ina219.getData();	
		processUSBData();
		RegisterHandler();
		buttonHandler();
		//request(&volt, &current, ppsReady);
		PD_UFP.run();

		// check if serial is ready
		if(tud_cdc_connected())
			PD_UFP.print_status();

		// transfer the data from the INA219 to the registers for external access
		registers.setProtected(Register_Address::Bus_Voltage, ina219.getBusVoltageRaw());
		registers.setProtected(Register_Address::Shunt_Voltage, ina219.getShuntVoltage());
		registers.setProtected(Register_Address::Current, ina219.getCurrentRaw());
		registers.setProtected(Register_Address::Power, ina219.getPowerRaw());

		// if the screen is not ready, we skip the drawing and continue to poll the sensor
		if(!picoGFX.getDisplay().writeReady())
			continue;

		// start measuring the refresh rate, the reason we do this here is because we want to measure the time it takes to draw the screen
		// if this is done earlier, the time it takes to poll the sensors and treat that data, can wrongly be included in the time it takes to draw the screen
		lastTimer = time_us_32();
		// draw the background
		picoGFX.getAdvancedGraphics().drawRotRectGradient(center, display.getWidth(), display.getHeight(), 10, Colors::OrangeRed, Colors::DarkYellow);
		//picoGFX.getAdvancedGraphics().fillGradient(Colors::Derg, Colors::Pink, {50,50}, {320-50, 172-50});
		//picoGFX.getDisplay().fill(Colors::Derg);
		//picoGFX.getGraphics().drawBitmap(background_image, 320, 172);

		picoGFX.getPrint().setCursor(cursor);
		picoGFX.getPrint().setFont(&RobotoMono48);
		picoGFX.getPrint().setColor(Colors::White);

		// draw the current
		picoGFX.getPrint().moveCursor({10,0});
		int current = (ina219.getCurrentRaw() * 100) * CURRENT_RESOLUTION;
		// it overflows if the current is zero for some reason, so we manually set it to zero if that happens
		current = current > 5000 ? 0 : current;
		int current_int = current / 100;
		int current_fraction = current_int > 10 ? (current % 10) : (current % 100);
		picoGFX.getPrint().print(current_int);
		picoGFX.getPrint().print(".");
		// if the current is less than 10, we need to draw a zero
		if(current_int < 10 && current_fraction != 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().print(current_fraction);
		// draw additional zeros if needed
		if(current_fraction == 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().println("A");

		// draw the voltage
		picoGFX.getPrint().moveCursor({10, 0});
		int volt = ina219.getBusVoltageRaw() * 100 * BUS_VOLTAGE_LSB_VALUE;
		int volt_int = volt / 100;
		int volt_fraction = volt_int > 10 ? (volt % 10) : (volt % 100);
		picoGFX.getPrint().print(volt_int);
		picoGFX.getPrint().print(".");
		// if the current is less than 10, we need to draw a zero
		if(volt_fraction < 10 && volt_fraction != 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().print(volt_fraction);
		// draw additional zeros if needed
		if(volt_fraction == 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().println("V");

		// draw the power
		picoGFX.getPrint().moveCursor({10,0});
		int power = (ina219.getPowerRaw() * 2000) * CURRENT_RESOLUTION;
		int power_int = power / 100;
		int power_fraction = power_int > 10 ? (power % 10) : (power % 100);
		picoGFX.getPrint().print(power_int);
		picoGFX.getPrint().print(".");
		// if the current is less than 10, we need to draw a zero
		if(power_fraction < 10 && power_fraction != 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().print(power_fraction);
		// draw additional zeros if needed
		if(power_fraction == 0)
			picoGFX.getPrint().print("0");
		picoGFX.getPrint().print("W\n");

		// draw the frame counter
		picoGFX.getPrint().setCursor({230, 10});
		picoGFX.getPrint().setColor(Colors::GreenYellow);
		picoGFX.getPrint().setFont(&ComicSans24);
		picoGFX.getPrint().print(framerate);

		// output the data to the display
		picoGFX.getDisplay().update();

		// reset the timer
		timer = time_us_32() - lastTimer;
		framerate = 1000000.0 / timer;
	}
}
