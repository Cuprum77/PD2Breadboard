#include <Display.hpp>

/**
 * @brief Display constructor
 * @param spi SPI bus
 * @param pins Pins
 * @param params Display parameters
*/
Display::Display(spi_inst_t* spi, Display_Pins pins, Display_Params params, bool dimming)
{
    this->spi = spi;
    this->pins = pins;
    this->params = params;

    // enable the SPI bus
    spi_init(this->spi, SPI_BAUDRATE);
    // set the pins to SPI function
    gpio_set_function(this->pins.sda, GPIO_FUNC_SPI);
    gpio_set_function(this->pins.scl, GPIO_FUNC_SPI);
    // init the rest of the pins
    gpio_init(this->pins.rst);
    gpio_init(this->pins.dc);
    gpio_init(this->pins.cs);
    // set the rest of the pins to GPIO output
    gpio_set_dir(this->pins.rst, GPIO_OUT);
    gpio_set_dir(this->pins.dc, GPIO_OUT);
    gpio_set_dir(this->pins.cs, GPIO_OUT);
    // set the pins to high
    gpio_put(this->pins.rst, 1);
    gpio_put(this->pins.dc, 1);
    gpio_put(this->pins.cs, 1);

    // set up the backlight pin depending on the dimming setting
    if(dimming)
    {
        // enable dimming
        this->dimmingEnabled = true;
        gpio_set_function(this->pins.bl, GPIO_FUNC_PWM);
        // get the PWM slice number
        uint sliceNum = pwm_gpio_to_slice_num(this->pins.bl);
        this->sliceNum = sliceNum;
        // get the PWM channel
        uint chan = pwm_gpio_to_channel(this->pins.bl);
        this->pwmChannel = chan;
        // turn on the PWM slice
        pwm_set_enabled(sliceNum, true);
        // set the PWM wrap value
        pwm_set_wrap(sliceNum, 255);
        // set the PWM value
        pwm_set_chan_level(sliceNum, chan, 127);
    }
    else
    {
        this->dimmingEnabled = false;
        gpio_init(this->pins.bl);
        gpio_set_dir(this->pins.bl, GPIO_OUT);
        gpio_put(this->pins.bl, 1);
    }

    sleep_ms(100);

    // set the SPI format
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // reset the display
    this->writeData(Display_Commands::SWRESET, NULL, 0);
    sleep_ms(150);

    // set the display to sleep out
    this->writeData(Display_Commands::SLPOUT, NULL, 0);
    sleep_ms(50);

    // set the display to interface pixel format
    // 0x5 << 4 = 65k of rgb interface
    // 0x5 = 16 bits per pixel
    unsigned char pixelFormat = 0x5 << 4 | 0x5;
    this->writeData(Display_Commands::COLMOD, &pixelFormat, 1);
    sleep_ms(10);

    // madctl = memory access control
    unsigned char madctl = 0x00;
    switch(this->params.rotation)
    {
        case 1:
            madctl = Display_MADCTL::MX || Display_MADCTL::MV || Display_MADCTL::RGB;
            break;
        case 2:
            madctl = Display_MADCTL::MX || Display_MADCTL::MY || Display_MADCTL::RGB;
            break;
        case 3:
            madctl = Display_MADCTL::MY || Display_MADCTL::MV || Display_MADCTL::RGB;
            break;
        default:
            madctl = Display_MADCTL::RGB;
            break;
    }
    this->writeData(Display_Commands::MADCTL, 0x00, 1);

    // set the display to memory access control
    this->columnAddressSet(
        0 + this->params.columnOffset1, 
        (this->params.width - 1) + this->params.columnOffset2
    );
    this->rowAddressSet(
        0 + this->params.rowOffset1, 
        (this->params.height - 1) + this->params.rowOffset2
    );

    // display inversion on
    this->writeData(Display_Commands::INVON, NULL, 0);
    sleep_ms(10);

    // normal display mode on
    this->writeData(Display_Commands::NORON, NULL, 0);
    sleep_ms(10);

    // display on
    this->writeData(Display_Commands::DISPON, NULL, 0);
    sleep_ms(10);
}

/**
 * @brief Clear the display by drawing a black rectangle
*/
void Display::clear()
{
    this->fill(0x0000);
}

/**
 * @brief Turn the display on
*/
void Display::displayOn()
{
    this->writeData(Display_Commands::DISPON, NULL, 0);
    sleep_ms(10);
}

/**
 * @brief Turn the display off
*/
void Display::displayOff()
{
    this->writeData(Display_Commands::DISPOFF, NULL, 0);
    sleep_ms(10);
}

/**
 * @brief Write text on the display
 * @param text Text to draw
 * @param x X position
 * @param y Y position
 * @param size Text size
 */
void Display::print(const char* text, uint x, uint y, uint size)
{

}

/**
 * @brief Write a double on the display
 * @param value Value to draw
 * @param x X position
 * @param y Y position
 * @param size Text size
*/
void Display::print(double value, uint x, uint y, uint size)
{

}

/**
 * @brief Fill the display with a color
 * @param color Color to fill with
*/
void Display::fill(unsigned short color)
{
    // calculate the number of pixels
    uint numPixels = this->params.width * this->params.height;
    // set the cursor position
    this->setCursor(0, 0);
    // write the pixels
    for(int i = 0; i < numPixels; i++)
    {
        this->writePixels(&color, sizeof(color));
    }
}

/**
 * @brief Fill the display with a color
 * @param color Color to fill with
*/
void Display::fill(Color color)
{
    this->fill(color.to16bit());
}

/**
 * @brief Place a pixel on the display
 * @param x X position
 * @param y Y position
 * @param color Color to draw in
*/
void Display::drawPixel(uint x, uint y, unsigned short color)
{
    // set the cursor position
    this->setCursor(x, y);
    // write the pixel
    this->writePixels(&color, sizeof(color));
}

/**
 * @brief Place a pixel on the display
 * @param x X position
 * @param y Y position
 * @param color Color to draw in
*/
void Display::drawPixel(uint x, uint y, Color color)
{
    this->drawPixel(x, y, color.to16bit());
}

/**
 * @brief Place a pixel on the display
 * @param coordinate Coordinates to draw the pixel at
 * @param color Color to draw in
*/
void Display::drawPixel(Coordinate coordinate, Color color)
{
    this->drawPixel(coordinate.x, coordinate.y, color.to16bit());
}

/**
 * @brief Set the cursor position
 * @param x X position
 * @param y Y position
*/
void Display::setCursor(uint x, uint y)
{
    this->columnAddressSet(
        0 + this->params.columnOffset1, 
        (this->params.width - 1) + this->params.columnOffset2
    );
    this->rowAddressSet(
        0 + this->params.rowOffset1, 
        (this->params.height - 1) + this->params.rowOffset2
    );
}

/**
 * @brief Set the cursor position
 * @param coordinate Coordinates to set the cursor to
*/
void Display::setCursor(Coordinate coordinate)
{
    this->setCursor(coordinate.x, coordinate.y);
}

/**
 * @brief Set the backlight brightness
 * @param brightness Brightness (0-255) if dimming is enabled, brightness (0-1) if dimming is disabled
*/
void Display::setBrightness(unsigned char brightness)
{
    // check if dimming is enabled
    if (this->dimmingEnabled)
    {
        // set the brightness
        pwm_set_chan_level(this->sliceNum, this->pwmChannel, brightness);
    }
    else
    {
        // make sure the brightness is between 0 and 1
        brightness = brightness & 0x01;
        // toggle the backlight pin based on the brightness
        gpio_put(this->pins.bl, brightness);
    }
}

/**
 * @private
 * @brief Write data to the display
 * @param command Command to send
 * @param data Data to send
 * @param length Length of the data
*/
void Display::writeData(Display_Commands command, const unsigned char* data, size_t length)
{
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    this->dataMode = false;
    // delay
    sleep_us(1);
    // set the display to write mode
    gpio_put(this->pins.cs, 0);
    gpio_put(this->pins.dc, 0);
    // delay
    sleep_us(1);
    // send the command
    unsigned char commandByte = (unsigned char)command;
    spi_write_blocking(this->spi, &commandByte, sizeof(commandByte));

    // send the data
    gpio_put(this->pins.dc, 1);
    sleep_us(1);
    if (length)
    {
        spi_write_blocking(this->spi, data, length);
        sleep_us(1);
    }
    gpio_put(this->pins.cs, 1);
}

/**
 * @private
 * @brief Set the column address
 * @param x0 Start column
 * @param x1 End column
*/
void Display::columnAddressSet(uint x0, uint x1)
{
    // pack the data
    unsigned char data[4] = { 
        (unsigned char)(x0 >> 8), 
        (unsigned char)(x0 & 0xff), 
        (unsigned char)(x1 >> 8), 
        (unsigned char)(x1 & 0xff) 
    };

    // write the data
    this->writeData(Display_Commands::CASET, data, sizeof(data));
}

/**
 * @private
 * @brief Set the row address
 * @param y0 Start row
 * @param y1 End row
*/
void Display::rowAddressSet(uint y0, uint y1)
{
    // pack the data
    unsigned char data[4] = { 
        (unsigned char)(y0 >> 8), 
        (unsigned char)(y0 & 0xff), 
        (unsigned char)(y1 >> 8), 
        (unsigned char)(y1 & 0xff) 
    };

    // write the data
    this->writeData(Display_Commands::RASET, data, sizeof(data));
}

/**
 * @private
 * @brief Write to the memory
*/
void Display::memoryWrite()
{
    sleep_us(1);
    gpio_put(this->pins.cs, 0);
    gpio_put(this->pins.dc, 0);
    sleep_us(1);
    // memory write
    spi_write_blocking(this->spi, (unsigned char*)Display_Commands::RAMWR, 
        sizeof(Display_Commands::RAMWR));

    sleep_us(1);
    gpio_put(this->pins.cs, 0);
    gpio_put(this->pins.dc, 1);
    sleep_us(1);
}

/**
 * @private
 * @brief Write pixels to the display
 * @param data data to write
 * @param length Length of the data
*/
void Display::writePixels(const unsigned short* data, size_t length)
{
    if(!this->dataMode)
    {
        this->memoryWrite();
        spi_set_format(this->spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        this->dataMode = true;
    }

    spi_write16_blocking(this->spi, data, length / 2);
}