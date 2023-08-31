#include "include/main.h"

// set the variables to the default values in case we fail to retrieve the data from the EEPROM
unsigned int currentNegotiated = Device_Target_Current_Default;
unsigned int voltageNegotiated = Device_Target_Voltage_Default;
unsigned int currentLimit = PFuse_Trip_Current_Default;
unsigned int backlightBrightness = Display_Brightness_Default;

// overcurrent boolean
bool overcurrent = false;
bool outputEnabled = false;

// set the display parameters
display_spi_config_t spi_config {
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.pio = false,
	.spi_instance = spi0,
	.baudrate = 125000000,
};

display_config_t config = {
	.backlightPin = DISP_PIN_BL,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION,
	.spi = spi_config
};

// Create the display object
Driver spi(&config);
ST7789 display(&spi, &config);
// Create the GFX objects
Print print(display.getFrameBuffer(), &config);
Graphics graphics(display.getFrameBuffer(), &config);
Gradients gradients(display.getFrameBuffer(), &config);
// Create the PicoGFX object
PicoGFX picoGFX(&print, &graphics, &gradients, nullptr);

// Create the objects
Button buttonUp(BUTTON_UP);
Button buttonMenu(BUTTON_MENU);
Button buttonDown(BUTTON_DOWN);
Memory memory(EEPROM_ADDRESS, i2c0);
INA219 ina219(INA219_ADDRESS, i2c0);
Registers registers;

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
			//multicore_reset_core1();
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

void getFormat(double value, char unit);

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

	// Initialize the display
	spi.init();
	display.init();

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

	// create points for important locations
	Point cursor = Point(0, 0);
	Point center = display.getCenter();
	picoGFX.getPrint().setColor(Colors::White);

	// timer for the framerate calculation
	int framecounter = 0;
	int frames = 0;
	unsigned long timer = 0;

	// run the main loop
	while(1)
	{
		ina219.getData();	
		processUSBData();
		RegisterHandler();
		buttonHandler();

		// transfer the data from the INA219 to the registers for external access
		registers.setProtected(Register_Address::Bus_Voltage, ina219.getBusVoltageRaw());
		registers.setProtected(Register_Address::Shunt_Voltage, ina219.getShuntVoltage());
		registers.setProtected(Register_Address::Current, ina219.getCurrentRaw());
		registers.setProtected(Register_Address::Power, ina219.getPowerRaw());

		// draw the background
		picoGFX.getGradients().drawRotCircleGradient(center, DISP_HEIGHT, 10, Colors::OrangeRed, Colors::DarkYellow);
		//picoGFX.getDisplay().fill(Colors::Derg);
		//picoGFX.getGraphics().drawBitmap(background_image, 320, 172);

		picoGFX.getPrint().setCursor(cursor);
		picoGFX.getPrint().setFont(&RobotoMono48);
		picoGFX.getPrint().setColor(Colors::White);

		double voltage = ina219.getVoltage();
		double current = ina219.getCurrent();
		double power = ina219.getPower();

		// draw the voltage
		//picoGFX.getPrint().setCursor({0, 78});
		getFormat(voltage, 'V');
		picoGFX.getPrint().center(Alignment_t::HorizontalCenter);
		picoGFX.getPrint().print();

		// draw the current
		getFormat(current, 'A');
		picoGFX.getPrint().center(Alignment_t::HorizontalCenter);
		picoGFX.getPrint().print();

		// draw the power
		picoGFX.getPrint().moveCursor(0, 10);
		getFormat(power, 'W');
		picoGFX.getPrint().center(Alignment_t::HorizontalCenter);
		picoGFX.getPrint().print();

		// draw the frame counter
		picoGFX.getPrint().setCursor({230, 10});
		picoGFX.getPrint().setColor(Colors::GreenYellow);
		picoGFX.getPrint().setFont(&RobotoMono24);
		picoGFX.getPrint().setString("%d fps", display.getFrameCounter());
		picoGFX.getPrint().print();

		// output the data to the display
		display.update(true);
	}
}

void getFormat(double value, char unit)
{
	// At 10 and above, we remove the decimal point
	if(value >= 10.0f)
		picoGFX.getPrint().setString("%.0f%c\n", value, unit);
	// At 1 and above, we keep one decimal point
	else if(value >= 1.0f)
		picoGFX.getPrint().setString("%.1f%c\n", value, unit);
	// Else we convert to milli and keep no decimal
	else
		picoGFX.getPrint().setString("%.0fm%c\n", value * 1000, unit);
}