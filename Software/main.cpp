#include "Defines.hpp"

// set the variables to the default values in case we fail to retrieve the data from the EEPROM
unsigned int currentNegotiated = CURRENT_SETTING_DEFAULT;
unsigned int voltageNegotiated = VOLTAGE_SETTING_DEFAULT;
unsigned int currentLimit = CURRENT_LIMIT_DEFAULT;
unsigned int backlightBrightness = BACKLIGHT_DEFAULT;

// set the display parameters
Display_Pins displayPins = {
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.bl = DISP_PIN_BL
};

Display_Params displayParams = {
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION
};

// Create the objects
Button buttonUp(BUTTON_UP);
Button buttonMenu(BUTTON_MENU);
Button buttonDown(BUTTON_DOWN);
Display display(spi0, displayPins, displayParams, true);
Memory memory(EEPROM_ADDRESS, i2c0);
FUSB302 usbPD(FUSB302_ADDRESS, i2c1);
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
	i2c_init(i2c0, 400000);
	// set the binary data to show the pins used for I2C0
	bi_decl(bi_2pins_with_func(I2C0_SDA, I2C0_SCL, GPIO_FUNC_I2C));

	// initialize the I2C1 bus
	gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C1_SDA);
	gpio_pull_up(I2C1_SCL);
	i2c_init(i2c1, 400000);
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
	gpio_init(PICO_DEFAULT_LED_PIN);

	// set the pins to output
	gpio_set_dir(LEFT_MOSFET_LED, GPIO_OUT);
	gpio_set_dir(RIGHT_MOSFET_LED, GPIO_OUT);
	gpio_set_dir(LEFT_MOSFET, GPIO_OUT);
	gpio_set_dir(RIGHT_MOSFET, GPIO_OUT);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	// set the binary data to show the pins used for the LEDs
	bi_decl(bi_4pins_with_names(
		LEFT_MOSFET_LED, "Left MOSFET LED", 
		LEFT_MOSFET, "Left MOSFET", 
		RIGHT_MOSFET_LED, "Right MOSFET LED", 
		RIGHT_MOSFET, "Right MOSFET")
	);

	// for debugging purposes
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
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

// this is just for fun lmao
unsigned long lastDebounceTime = 0;
bool ledState = false;
const long intervalSequence[4] = { 200000, 125000, 200000, 600000};
/**
 * @brief Heartbeat function
 * @note Has to be called every loop
*/
void heartBeat()
{
	static int sequenceIndex = 0;

	// if we havent reached our current interval yet, return
	if((time_us_32() - lastDebounceTime) < intervalSequence[sequenceIndex])
		return;
	
	// increment the sequence index
	sequenceIndex++;

	// if we have reached the end of the sequence, reset the index
	if(sequenceIndex >= 4)
		sequenceIndex = 0;

	// toggle the LED
	gpio_put(PICO_DEFAULT_LED_PIN, ledState);
	// set the last debounce time
	lastDebounceTime = time_us_32();
	ledState = !ledState;
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
		// note, VSCode might complain that the device is not found
		// but it will compile (at least on my machine lmao)
		reset_usb_boot(0, 0);
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
		char connected = memory.verifyConnection() ? 'Y' : 'N';
		printf("EEPROM connected: %c\n", connected);
		connected = ina219.verifyConnection() ? 'Y' : 'N';
		printf("INA219 connected: %c\n", connected);
		connected = usbPD.verifyConnection() ? 'Y' : 'N';
		printf("FUSB302 connected: %c\n\n", connected);

		if(memory.verifyConnection())
		{
			printf("Verifying that the EEPROM is working...\n");
			int errors = memory.selfTest();
			if(errors == 0)
				printf("EEPROM self test passed!\n");
			else if(errors > 0)
				printf("EEPROM self test failed! Bad read/writes: %d\n", errors);
		}
	}

	// clear the buffer
	memset(buffer, 0, sizeof(buffer));
}


/**
 * @brief Draw the Norwegian flag 
*/
void drawNorwegianFlag()
{
    // https://no.wikipedia.org/wiki/Norges_flagg

    // "official" flag colors
    unsigned short høyRød = 0xb865;
    unsigned short mørkeBlå = 0x010b;
    unsigned short hvit = 0xffff;

	// custom width and height
	uint height = 220;
	uint width = 160;

	// calculate offset
	uint xOffset = (displayParams.width - width) / 2;
	uint yOffset = (displayParams.height - height) / 2;

    // get the proportions of the flag
    float horizontalUnit = 10;
    float verticalUnit = 10;

    // the first red section is 6 units wide and 6 units high
    // fill in the background using the høyrød color
	Point start = {(uint)0 + xOffset, (uint)0 + yOffset};
	Point end = {(uint)width + xOffset, (uint)height + yOffset};
    display.drawFilledRectangle(start, end, høyRød);

    // draw the white vertical stripe
    start = {(uint)0 + xOffset, (uint)(horizontalUnit * 6) + yOffset};
    end = {(uint)width + xOffset, (uint)(horizontalUnit * 10) + yOffset};
    display.drawFilledRectangle(start, end, hvit);
	// draw the white horizontal stripe
	start = {(uint)(verticalUnit * 6) + xOffset, (uint)0 + yOffset};
	end = {(uint)(verticalUnit * 10) + xOffset, (uint)height + yOffset};
	display.drawFilledRectangle(start, end, hvit);

	// draw the blue vertical stripe
	start = {(uint)0 + xOffset, (uint)(horizontalUnit * 7) + yOffset};
    end = {(uint)width + xOffset, (uint)(horizontalUnit * 9) + yOffset};
    display.drawFilledRectangle(start, end, mørkeBlå);
	// draw the blue horizontal stripe
	start = {(uint)(verticalUnit * 7) + xOffset, (uint)0 + yOffset};
	end = {(uint)(verticalUnit * 9) + xOffset, (uint)height + yOffset};
	display.drawFilledRectangle(start, end, mørkeBlå);
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
		gpio_put(LEFT_MOSFET_LED, 0);
		gpio_put(LEFT_MOSFET, 0);
		printf("MENU held\n");
	}
	if(buttonMenu.isClicked())
	{
		gpio_put(LEFT_MOSFET_LED, 1);
		gpio_put(LEFT_MOSFET, 1);
		printf("MENU\n");
	}
	if(buttonDown.isHeld())
	{
		printf("DOWN held\n");
		display.clear();
	}
	if(buttonDown.isClicked())
	{
		drawNorwegianFlag();
		printf("DOWN\n");
	}
	if(buttonUp.isHeld())
	{
		gpio_put(RIGHT_MOSFET_LED, 0);
		gpio_put(RIGHT_MOSFET, 0);

		printf("UP held\n");
	}
	if(buttonUp.isClicked())
	{
		gpio_put(RIGHT_MOSFET_LED, 1);
		gpio_put(RIGHT_MOSFET, 1);

		printf("UP\n");
	}

	buttonUp.update();
	buttonMenu.update();
	buttonDown.update();
}

unsigned long lastPolling = 0;
/**
 * @brief Periodically poll the INA219 for data
 * @note Has to be called every loop
*/
void pollINA()
{
	if((time_us_32() - lastPolling) < 1000)
		return;

	ina219.getData(true);
}


/**
 * @brief Main function
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
	ina219.setBusADCResolution(INA219_12BIT_532US);
	ina219.setShuntADCResolution(INA219_12BIT_532US);
	ina219.setMode(INA219_MODE_SHUNT_AND_BUS_VOLTAGE_CONTINUOUS);
	ina219.setData();

	display.clear();
	display.setBrightness(backlightBrightness);
	display.drawCircle(display.getCenter(), 50, Colors::White);
	display.drawLine(Point(0, 0), Point(172, 320), Colors::White);
	display.drawLine(Point(0, 320), Point(172, 0), Colors::White);
	display.drawLine(Point((uint)0, displayParams.height/2), Point(displayParams.width, displayParams.height/2), Colors::White);
	display.drawLine(Point(displayParams.width/2, (uint)0), Point(displayParams.width/2, displayParams.height), Colors::White);
	display.drawRectangle(display.getCenter(), 71, 71, Colors::White);
	//display.drawFilledCircle(Point(displayParams.width/2, displayParams.height/2), 40, Colors::Pink);

	/*Rectangle rect = Rectangle(
		Point(),
		Point(displayParams.width, displayParams.height),
		Point(),
		Point()
	);*/

	while(1)
	{
		heartBeat();
		pollINA();
		processUSBData();
		buttonHandler();
	}
}