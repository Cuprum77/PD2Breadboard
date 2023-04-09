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
    this->setCursor({0, 0});

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
 * @brief Set the cursor position
 * @param Point Point to set the cursor to
*/
void Display::setCursor(Point Point)
{
    // set the pixel x address
    this->columnAddressSet(
        Point.x + this->params.columnOffset1, 
        (this->params.width - 1) + this->params.columnOffset2
    );
    // set the pixel y address
    this->rowAddressSet(
        Point.y + this->params.rowOffset1, 
        (this->params.height - 1) + this->params.rowOffset2
    );
}


/**
 * @brief Fill the display with a color
 * @param color Color to fill with
*/
void Display::fill(Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();
    
    // calculate the number of pixels and take the offset into account
    int numPixels = 
        (this->params.width + (this->params.columnOffset1 + this->params.columnOffset2)) * 
        (this->params.height + (this->params.rowOffset1 + this->params.rowOffset2));
    // set the cursor position to the top left
    Point Point = {0, 0};
    this->setCursor(Point);
    
    // write the pixels
    for(int i = 0; i < numPixels; i++)
    {
        this->writePixels(&color16, sizeof(color16));
    }
}

/**
 * @brief Place a pixel on the display
 * @param Point Points to draw the pixel at
 * @param color Color to draw in
*/
void Display::drawPixel(Point Point, Color color)
{
    // set the cursor position
    this->setCursor(Point);
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();
    // write the pixel
    this->writePixels(&color16, sizeof(color16));
}

/**
 * @brief Draw a line on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawLine(Point start, Point end, Color color, uint thickness)
{
    // Uses Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // move Points into local variables
    uint x0 = start.x;
    uint y0 = start.y;
    uint x1 = end.x;
    uint y1 = end.y;

    // get the difference between the x and y Points
    int dx = abs((int)end.x - (int)start.x);
    int sx = start.x < end.x ? 1 : -1;
    int dy = -abs((int)end.y - (int)start.y);
    int sy = start.y < end.y ? 1 : -1;
    // calculate the error
    int error = dx + dy;
    
    while(true)
    {
        // draw the pixel and the ones around it as defined by the thickness value
        if(thickness > 1)
        {
            // loop through the thickness
            for(int i = 0; i < thickness; i++)
            {
                // calculate the y Point
                uint yThicc = y0 + i - (thickness / 2);
                // draw the pixel
                this->drawPixel({x0, yThicc}, color);
            }
        }
        else
        {
            // draw the pixel
            this->drawPixel({x0, y0}, color);
        }

        // if we have reached the end Point, break
        if(x0 == x1 && y0 == y1) break;

        // calculate the error
        int e2 = 2 * error;
        // if the error is greater than the difference in y
        if(e2 >= dy)
        {
            // if we have reached the end Point, break
            if(x0 == x1) break;
            // increment the x Point
            error += dy;
            x0 += sx;
        }
        // if the error is greater than the difference in x
        if(e2 <= dx)
        {
            // if we have reached the end Point, break
            if(y0 == y1) break;
            // increment the y Point
            error += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief Draw a rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawRectangle(Point start, Point end, Color color, uint thickness)
{
    // draw the top line
    this->drawLine({start.x, start.y}, {end.x, start.y}, color, thickness);
    // draw the right line
    this->drawLine({end.x, start.y}, {end.x, end.y}, color, thickness);
    // draw the bottom line
    this->drawLine({end.x, end.y}, {start.x, end.y}, color, thickness);
    // draw the left line
    this->drawLine({start.x, end.y}, {start.x, start.y}, color, thickness);
}

/**
 * @brief Draw a filled rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Display::drawFilledRectangle(Point start, Point end, Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();

    // calculate the size of the rectangle
    uint width = end.x - start.x;
    uint height = end.y - start.y;
    
    this->setCursor({0, 0});
    
    // loop through the height
    for(int i = 0; i < height; i++)
    {
        // move the cursor to the next row
        this->setCursor({start.x, start.y + i});

        // loop through the width
        for(int j = 0; j < width; j++)
        {
            // write the pixel
            this->writePixels(&color16, sizeof(color16));
        }
    }
}

/**
 * @brief Draw a circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Display::drawCircle(Point center, uint radius, Color color, uint thickness)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    uint x0 = center.x;
    uint y0 = center.y;
    uint x = radius;
    uint y = 0;
    uint error = 0;

    // loop through the radius
    while(x >= y)
    {
        // draw the pixel
        this->drawPixel({x0 + x, y0 + y}, color);
        this->drawPixel({x0 + y, y0 + x}, color);
        this->drawPixel({x0 - y, y0 + x}, color);
        this->drawPixel({x0 - x, y0 + y}, color);
        this->drawPixel({x0 - x, y0 - y}, color);
        this->drawPixel({x0 - y, y0 - x}, color);
        this->drawPixel({x0 + y, y0 - x}, color);
        this->drawPixel({x0 + x, y0 - y}, color);

        // calculate the error
        error = 2 * (y + 1) - 1;
        // if the error is greater than the difference in y
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error -= 2 * x;
        }
        // increment the y Point
        y++;
    }
}


/**
 * @brief Write text on the display
 * @param text Text to draw
 * @param Point Point to draw at
 * @param size Text size
 */
void Display::print(const char* text, Point Point, uint size)
{

}

/**
 * @brief Write a double on the display
 * @param value Value to draw
 * @param Point Point to draw at
 * @param size Text size
*/
void Display::print(double value, Point Point, uint size)
{

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
    // configure the spi to 8 bit mode
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    this->dataMode = false;

    sleep_us(1);
    // set the display to write mode
    gpio_put(this->pins.cs, 0);
    gpio_put(this->pins.dc, 0);
    sleep_us(1);

    // send the command
    unsigned char commandByte = (unsigned char)command;
    spi_write_blocking(this->spi, &commandByte, sizeof(commandByte));

    // send the data
    sleep_us(1);
    gpio_put(this->pins.dc, 1);
    if (length)
    {
        sleep_us(1);
        spi_write_blocking(this->spi, data, length);
    }

    sleep_us(1);
    gpio_put(this->pins.cs, 1);
    gpio_put(this->pins.dc, 1);
    sleep_us(1);
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
    unsigned char command = (unsigned char)Display_Commands::RAMWR;
    spi_write_blocking(this->spi, &command, sizeof(command));

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