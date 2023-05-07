#include "Defines.hpp"

// set the variables to the default values in case we fail to retrieve the data from the EEPROM
unsigned int currentNegotiated = CURRENT_SETTING_DEFAULT;
unsigned int voltageNegotiated = VOLTAGE_SETTING_DEFAULT;
unsigned int currentLimit = CURRENT_LIMIT_DEFAULT;
unsigned int backlightBrightness = BACKLIGHT_DEFAULT;

// store the runtimes of the cores
unsigned long core0RunTime = 0;
unsigned long core1RunTime = 0;

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
Display display(spi0, displayPins, displayParams, true);
Memory memory(EEPROM_ADDRESS, i2c0);
USB_PD usbPD(FUSB302_ADDRESS, i2c1, PD_INT_N);
INA219 ina219(INA219_ADDRESS, i2c0);

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
 * @brief Check if the string is a number
 * @param str The string to check
 * @return true if the string is a number
*/
bool isNumber(char* str)
{
	// loop through each digit
	for(int i = 0; i < strlen(str); i++)
	{
		// check its ascii code to see if its a number
		if(str[i] < 48 || str[i] > 57)
			return false;
	}
	// return true as the string is a number
	return true;
}

/**
 * @brief Convert the ascii number to an integer
 * @param str The string to convert
 * @return The integer value of the string
*/
unsigned int asciiToInt(char* str)
{
	// the number to return
	int num = 0;
	// loop through each digit
	for(int i = 0; i < strlen(str); i++)
	{
		// convert the ascii code to a number
		int digit = str[i] - 48;
		// multiply the number by 10 and add the digit
		num = (num * 10) + digit;
	}
	// return the number
	return num;
}

// empty buffer to store the data
char buffer[100] = {0};
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
	tud_cdc_read(buffer, 99);

	// check if the data is a reset command
	if(strcmp(buffer, REBOOT_TO_BOOTLOADER_CODE) == 0)
	{
		// reset the device into usb boot
		// the intellisense doesnt like this function so we have to disable it until we build
#ifndef __INTELLISENSE__
		reset_usb_boot(LEFT_MOSFET_LED | RIGHT_MOSFET_LED, 0);
		watchdog_reboot(0, SRAM_END, 0);
		while(1);
#endif
	}
	else if(strcmp(buffer, RESET_CODE) == 0)
	{
		watchdog_reboot(0, SRAM_END, 0);
		while(1);
	}
	// check if we sent the command to change the current limit
	else if(strncmp(buffer, CURRENT_LIMIT_CODE, sizeof(CURRENT_LIMIT_CODE) - 1) == 0)
	{
		// get the data from after the command
		char* data = buffer + sizeof(CURRENT_LIMIT_CODE) - 1;
		// verify that the data is a number
		if(!isNumber(data))
		{
			printf("not a valid input!\n");
			memset(buffer, 0, sizeof(buffer));
			return;
		}

		// convert string to number
		unsigned int _currentLimit = asciiToInt(data);
		printf("current_limit:%d\n", _currentLimit);
		// set the current limit in EEPROM and local variable
		memory.writeWord(MEMORY_CURRENT_LIMIT_ADDRESS, _currentLimit);
		currentLimit = _currentLimit;
	}
	// check if we sent the command to change the current setting
	else if(strncmp(buffer, CURRENT_SETTING_CODE, sizeof(CURRENT_SETTING_CODE) - 1) == 0)
	{
		// get the data from after the command
		char* data = buffer + sizeof(CURRENT_SETTING_CODE) - 1;
		// verify that the data is a number
		if(!isNumber(data))
		{
			printf("not a valid input!\n");
			memset(buffer, 0, sizeof(buffer));
			return;
		}

		// convert string to number
		unsigned int currentSetting = asciiToInt(data);
		printf("current_setting:%d\n", currentSetting);
		// set the current setting in EEPROM and local variable
		memory.writeWord(MEMORY_CURRENT_ADDRESS, currentSetting);
		currentNegotiated = currentSetting;
	}
	// check if we sent the command to change the voltage setting
	else if(strncmp(buffer, VOLTAGE_SETTING_CODE, sizeof(VOLTAGE_SETTING_CODE) - 1) == 0)
	{
		// get the data from after the command
		char* data = buffer + sizeof(VOLTAGE_SETTING_CODE) - 1;
		// verify that the data is a number
		if(!isNumber(data))
		{
			printf("not a valid input!\n");
			memset(buffer, 0, sizeof(buffer));
			return;
		}

		// convert string to number
		unsigned int voltageSetting = asciiToInt(data);
		printf("voltage_setting:%d\n", voltageSetting);
		// set the voltage setting in EEPROM and local variable
		memory.writeWord(MEMORY_VOLTAGE_ADDRESS, voltageSetting);
		voltageNegotiated = voltageSetting;
	}
	// check if we sent the command to get all the measurements
	else if(strcmp(buffer, GET_READINGS_CODE) == 0)
	{
		// print the measurements
		printf("V: %.3f V, I: %.3f A, P: %.3f W\n", ina219.getVoltage(), ina219.getCurrent() / 1000, ina219.getPower() / 1000);
	}
	// check if we sent the command to get all the raw measurements
	else if(strcmp(buffer, GET_READINGS_RAW_CODE) == 0)
	{
		// print the measurements
		printf("V: %d, I: %d, P: %d\n", ina219.getBusVoltageRaw(), ina219.getShuntVoltageRaw(), ina219.getPowerRaw());
	}
	// check if we sent the command to change the display brightness
	else if(strncmp(buffer, BACKLIGHT_CODE, sizeof(BACKLIGHT_CODE) - 1) == 0)
	{
		// get the data from after the command
		char* data = buffer + sizeof(BACKLIGHT_CODE) - 1;
		// verify that the data is a number
		if(!isNumber(data))
		{
			printf("not a valid input!\n");
			memset(buffer, 0, sizeof(buffer));
			return;
		}

		// convert string to number
		unsigned char brightness = (unsigned char)asciiToInt(data);
		printf("backlight:%d\n", brightness);
		// set the display brightness
		memory.writeWord(MEMORY_BACKLIGHT_ADDRESS, brightness);
		backlightBrightness = brightness;
		display.setBrightness(brightness);
	}
	// check if we want to return the settings in console
	else if(strcmp(buffer, GET_SETTINGS_CODE) == 0)
	{
		printf("Current limit: %d,  ", currentLimit);
		printf("Current setting: %d,  ", currentNegotiated);
		printf("Voltage setting: %d,  ", voltageNegotiated);
		printf("Backlight: %.2f %% (%d)\n", (((float)backlightBrightness) / 255) * 100, backlightBrightness);
	}
	// verify that all peripherals are connected
	else if(strcmp(buffer, TEST_CODE) == 0)
	{
		const char* connectedString = "YES";
		const char* notConnectedString = "NO";

		printf("EEPROM connected: %s\n",
			memory.verifyConnection() ? 
				connectedString : notConnectedString);
		printf("INA219 connected: %s\n", 
			ina219.verifyConnection() ? 
				connectedString : notConnectedString);
		printf("FUSB302 connected: %s\n", 
			usbPD.verifyConnection() ? 
				connectedString : notConnectedString);

		if(memory.verifyConnection())
		{
			printf("\nVerifying that the EEPROM is working...\n");
			int errors = memory.selfTest();
			if(errors == 0)
				printf("EEPROM self test passed!\n");
			else if(errors > 0)
				printf("EEPROM self test failed! Bad read/writes: %d\n", errors);
		}

		if(ina219.verifyConnection())
		{
			printf("\nVerifying that the INA219 is working...\n");
			int errors = ina219.selfTest();
			const char* errorStringINA = ina219.selfTestToString(errors);
			printf("INA219 self test results: %s\n", errorStringINA);
		}

		if(usbPD.verifyConnection())
		{
			printf("\nVerifying that the FUSB302 is working...\n");
			int errors = usbPD.selfTest();
			const char* errorStringPD = usbPD.selfTestToString(errors);
			printf("FUSB302 self test results: %s\n", errorStringPD);
		}
	}
	else if(strcmp(buffer, FUSB302_DUMP_DATA) == 0)
	{
		usbPD.printRegisters();
	}
	else if(strcmp(buffer, FUSB302_GET_IDENTITY) == 0)
	{
		int id = usbPD.deviceID();
		printf("ID: %d\n", id);
		id = usbPD.productID();
		printf("Product ID: %d\n", id);
		id = usbPD.revisionID();
		printf("Revision ID: %d\n", id);
	}
	else if(strcmp(buffer, FUSB302_GET_CAPABILITY) == 0)
	{
		
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
		currentLimit -= CURRENT_STEPS; 
		printf("DOWN\n");
	}
	if(buttonUp.isHeld())
	{
		printf("UP held\n");
	}
	if(buttonUp.isClicked())
	{
		currentLimit += CURRENT_STEPS;
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

	display.fill(Colors::RaspberryRed);
	//display.drawBitmap(Point(), BACKGROUND_PIXEL_DATA, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
	
	Point start = Point(0, 5);

	unsigned long lastUpdate = 0;
	while(1)
	{
		unsigned long core1Time = time_us_32();
		display.setCursor(start);
		display.write(ina219.getCurrent() / 1000, 2);
		display.write("/");
		display.write((float)currentLimit / 1000);
		display.print("A");
		display.write(ina219.getVoltage(), 2);
		display.write("/");
		display.write((float)voltageNegotiated / 1000);
		display.print("V");
		display.write(ina219.getPower() / 1000, 2);
		display.print("W\n");

		core1RunTime = time_us_32() - core1Time;
		display.write("C0:");
		double hz = 1/((float)core0RunTime * 0.000001)/1000;
		display.write(hz, 2);
		display.print("kHz");
		display.write("C1:");
		hz = 1/((float)core1RunTime * 0.000001);
		display.write(hz, 2);
		display.print("Hz");
		display.write("PD: ");
		display.print(usbPD.pdCapable());
		display.write("PPS: ");
		display.print(usbPD.ppsCapable());
		display.print();
		display.write("V_CAP: ");
		
		auto voltages = usbPD.supportedVoltages();
		if(voltages & USB_PD_VOLTAGE_5V)
		{
			display.write("5V");
			if(voltages == 0x1)
				display.write("?");
			else
				display.print();
		}
		else
			display.print();
		if(voltages & USB_PD_VOLTAGE_9V)
			display.write("9V ");
		else 
			display.write("	");
		if(voltages & USB_PD_VOLTAGE_12V)
			display.print("12V ");
		else 
			display.print();
		if(voltages & USB_PD_VOLTAGE_15V)
			display.write("15V ");
		else 
			display.write("	");
		if(voltages & USB_PD_VOLTAGE_20V)
			display.print("20V ");
		else
			display.print();

		

		// redo the background every 5 seconds
		if((time_us_32() - lastUpdate) > 5000000)
		{
			display.fill(Colors::RaspberryRed);
			lastUpdate = time_us_32();
		}
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

	usbPD.init();

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
		unsigned long core0Time = time_us_32();
		usbPD.update();
		ina219.getData();	
		processUSBData();
		buttonHandler();
		//overCurrentLEDs();

		if(ina219.getCurrent() > currentLimit)
			overcurrent = true;
		else if (ina219.getCurrent() < (float)currentLimit * 0.8)
			overcurrent = false;

		core0RunTime = time_us_32() - core0Time;
	}
}