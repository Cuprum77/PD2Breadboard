#include "include/Defines.hpp"

// set the variables to the default values in case we fail to retrieve the data from the EEPROM
unsigned int currentNegotiated = Device_Target_Current_Default;
unsigned int voltageNegotiated = Device_Target_Voltage_Default;
unsigned int currentLimit = PFuse_Trip_Current_Default;
unsigned int backlightBrightness = Display_Brightness_Default;

// overcurrent boolean
bool overcurrent = false;
bool outputEnabled = false;

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
Display display(spi0, displayPins, displayParams, display_type_t::ST7789, true);
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
	if(buttonMenu.isLongPressed())
	{
		printf("MENU long pressed\n");
	}
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
	if(buttonMenu.isClicked())
	{
		printf("MENU\n");
	}
	if(buttonDown.isLongPressed())
	{
		printf("DOWN long pressed\n");
		display.clear();
	}
	if(buttonDown.isHeld())
	{
		printf("DOWN held\n");		
	}
	if(buttonDown.isClicked())
	{
		currentLimit -= 50; 
		printf("DOWN\n");
	}
	if(buttonUp.isHeld())
	{
		printf("UP held\n");
	}
	if(buttonUp.isClicked())
	{
		currentLimit += 50;
		printf("UP\n");
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
	{
		// Enter an infinite loop if the first core is not ready
		Error();
	}
	else
	{
		// tell the second core that we are ready
		multicore_fifo_push_blocking(MULTICORE_FLAG_VALUE);
	}
	
	int radius = 200;
	double theta = 0;
	double rotationSpeed = 0.05;

	Point cursor = Point(0, 5);
	Point center = display.getCenter();
	Point start = Point(
		(int)(center.x - radius * cos(theta)), 
		(int)(center.y - radius * sin(theta))
	);
	Point end = Point(
		(int)(center.x + radius * cos(theta)), 
		(int)(center.y + radius * sin(theta))
	);

	while(1)
	{
		unsigned long core1Time = time_us_32();
		display.fillGradient(Colors::DarkYellow, Colors::OrangeRed, start, end);
		display.setCursor(cursor);
		display.print(ina219.getCurrent() / 1000, 2, 2);
		display.println("A", 2);
		display.print(ina219.getVoltage(), 2, 2);
		display.println("V", 2);
		display.print(ina219.getPower() / 1000, 2, 2);
		display.println("W\n", 2);
		unsigned long core1RunTime = time_us_32() - core1Time;
		double hz = 1000000.0 / core1RunTime;
		display.setCursor({FONT_WIDTH * 14, 5});
		Color color = Colors::Black;
		display.print(hz, color.hexToColor(0x39ff14), 2, 1);
		display.writeBuffer();

		theta += rotationSpeed;
		start = Point(
			(int)(center.x - radius * cos(theta)), 
			(int)(center.y - radius * sin(theta))
		);
		end = Point(
			(int)(center.x + radius * cos(theta)), 
			(int)(center.y + radius * sin(theta))
		);
	}
}


/**
 * @brief Main function
 * @note This runs on the core 0
*/
int main()
{
	stdio_init_all();
	initI2C();
	initLEDs();
	fetchDataFromEEPROM();
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

	PD_UFP.init_PPS(PPS_V(4.2), PPS_A(6.9));

	// setup the second core
	multicore_launch_core1(core1Main);

	// wait for the second core to be ready
	uint g = multicore_fifo_pop_blocking();

	// check if the second core is ready
	if(g != MULTICORE_FLAG_VALUE)
	{
		// Enter an infinite loop if the second core is not ready
		Error();
	}
	else
	{
		// tell the second core that we are ready
		multicore_fifo_push_blocking(MULTICORE_FLAG_VALUE);
	}

	// run the main loop
	while(1)
	{
		ina219.getData();	
		processUSBData();
		RegisterHandler();
		buttonHandler();
		//overCurrentLEDs();
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