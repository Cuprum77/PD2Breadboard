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
    uchar pixelFormat = 0x5 << 4 | 0x5;
    this->writeData(Display_Commands::COLMOD, &pixelFormat, 1);
    sleep_ms(10);

    // madctl = memory access control
    uchar madctl = 0x00;
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
    this->fill(Colors::Black);
    this->fillColor = Colors::Black;
    this->setCursor({0, 0});
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
void Display::setCursor(Point point)
{
    // set the pixel x address
    this->columnAddressSet(
        point.X() + this->params.columnOffset1, 
        (this->params.width - 1) + this->params.columnOffset2
    );
    // set the pixel y address
    this->rowAddressSet(
        point.Y() + this->params.rowOffset1, 
        (this->params.height - 1) + this->params.rowOffset2
    );
    // set the internal cursor position
    this->cursor = point;
}

/**
 * @brief Get the center of the display
 * @return Point The center of the display
*/
Point Display::getCenter()
{
    Point Point = {
        this->params.width / 2,
        this->params.height / 2
    };
    return Point;
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

    // set the fill color variable
    this->fillColor = color;
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
void Display::drawLine(Point start, Point end, Color color)
{
    // Uses Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // move Points into local variables
    uint x0 = start.X();
    uint y0 = start.Y();
    uint x1 = end.X();
    uint y1 = end.Y();

    // get the difference between the x and y Points
    int dx = abs((int)end.X() - (int)start.X());
    int sx = start.X() < end.X() ? 1 : -1;
    int dy = -abs((int)end.Y() - (int)start.Y());
    int sy = start.Y() < end.Y() ? 1 : -1;
    // calculate the error
    int error = dx + dy;
    
    while(true)
    {
        // draw the pixel
        this->drawPixel({x0, y0}, color);

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
void Display::drawRectangle(Point start, Point end, Color color)
{
    // draw the rectangle
    this->drawLine({start.X(), start.Y()}, {end.X(), start.Y()}, color);
    this->drawLine({end.X(), start.Y()}, {end.X(), end.Y()}, color);
    this->drawLine({end.X(), end.Y()}, {start.X(), end.Y()}, color);
    this->drawLine({start.X(), end.Y()}, {start.X(), start.Y()}, color);
}

/**
 * @brief Draw a rectangle on the display
 * @param rect Rectangle to draw
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawRectangle(Rectangle rect, Color color)
{
    // draw the rectangle
    this->drawRectangle(rect.X(), rect.Y(), color);
}

/**
 * @brief Draw a center aligned rectangle on the display
 * @param center Center Point
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Display::drawRectangle(Point center, uint width, uint height, Color color)
{
    // calculate the start and end Points
    Point start = {center.X() - (width / 2), center.Y() - (height / 2)};
    Point end = {center.X() + (width / 2), center.Y() + (height / 2)};

    // draw the rectangle
    this->drawRectangle(start, end, color);
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
    uint width = end.X() - start.X();
    uint height = end.Y() - start.Y();
    
    this->setCursor({0, 0});
    
    // loop through the height
    for(int i = 0; i < height; i++)
    {
        // move the cursor to the next row
        this->setCursor({start.X(), start.Y() + i});

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
void Display::drawCircle(Point center, uint radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

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

        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}

/**
 * @brief Draw a filled circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Display::drawFilledCircle(Point center, uint radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // loop through the radius
    while(x >= y)
    {
        // draw the pixel
        this->drawLine({x0 + x, y0 + y}, {x0 - x, y0 + y}, color);
        this->drawLine({x0 + y, y0 + x}, {x0 - y, y0 + x}, color);
        this->drawLine({x0 + x, y0 - y}, {x0 - x, y0 - y}, color);
        this->drawLine({x0 + y, y0 - x}, {x0 - y, y0 - x}, color);

        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}


/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Display::drawBitmap(const uchar* bitmap, uint width, uint height)
{
    this->drawBitmap((const unsigned short*)bitmap, width, height);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Display::drawBitmap(const unsigned short* bitmap, uint width, uint height)
{
    // get the cursor location
    Point location = this->cursor;

    // write the bitmap
    int offset = 0;
    for(int y = 0; y < height; y++)
    {
        // set the cursor
        this->setCursor(Point(0 + location.X(), y + location.Y()));
        // write the row of pixels, we need to multiply the width by 2 because we are using 16 bit colors
        this->writePixels(&bitmap[offset], width * 2);
        // increment the offset
        offset += width;
    }
}


/**
 * @brief Set the backlight brightness
 * @param brightness Brightness (0-255) if dimming is enabled, brightness (0-1) if dimming is disabled
*/
void Display::setBrightness(uchar brightness)
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
void Display::writeData(Display_Commands command, const uchar* data, size_t length)
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
    uchar commandByte = (uchar)command;
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
    uchar data[4] = { 
        (uchar)(x0 >> 8), 
        (uchar)(x0 & 0xff), 
        (uchar)(x1 >> 8), 
        (uchar)(x1 & 0xff) 
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
    uchar data[4] = { 
        (uchar)(y0 >> 8), 
        (uchar)(y0 & 0xff), 
        (uchar)(y1 >> 8), 
        (uchar)(y1 & 0xff) 
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
    uchar command = (uchar)Display_Commands::RAMWR;
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

/**
 * @private
 * @brief Draw an ascii character on the display
 * @param character Character to draw
 * @param Point Point to draw at
 * @param size Text size
 * @param color Color to draw the character
 * @return Width of the character
*/
uint Display::drawAscii(const char character, Point point, uint size, Color color, Color background)
{
    // get the relevant bitmap data which is indexed according to the ascii table
    const uint* bitmap = FONT(character);

    // if the bitmap is a null pointer, return
    if (bitmap == nullptr)
        return 0;

    // check if size is 0
    if (size == 0)
        size = 1;

    // make sure the font size will not overflow the buffer
    if((FONT_WIDTH * FONT_HEIGHT) * size > sizeof(this->frameBuffer))
        return 0;

    // keep track of the row position
    uint rowPosition = 0;
    // keep track of the column position
    uint columnPosition = 0;
    // save the row size
    uint rowSize = FONT_WIDTH * size;

    // loop through the bitmap data
    for(int j = 0; j < FONT_DATA; j++)
    {
        // get the current data
        uint data = bitmap[j];

        // if the current data is 0, we have completed our loop
        if (data == 0)
            break;

        // set the color of the pixel based on the index
        // this works by checking if the least significant bit is 1 or 0
        // if it is 1, the pixel is the foreground color, otherwise it is the background color
        uint pixel = ((j & 0x1) ? color : background).to16bit();

        // multiply the data length by the size
        data *= size;

        // add the number of pixels to the buffer as specified by the data
        for(int i = 0; i < data; i++)
        {
            // add the pixel to the buffer
            this->frameBufferColumn[rowPosition++] = pixel;

            // check if we have reached the end of the row
            if (rowPosition == rowSize)
            {
                // reset the row position
                rowPosition = 0;

                // copy the column to the buffer as many times as specified by the size
                for(int j = 0; j < size; j++)
                {
                    // copy the column to the buffer
                    memcpy(&this->frameBuffer[(columnPosition * rowSize)], this->frameBufferColumn, rowSize * sizeof(unsigned short));
                    columnPosition++;
                }

                // reset the column
                memset(this->frameBufferColumn, 0, sizeof(this->frameBufferColumn));
            }
        }
    }

    // set the cursor position
    this->setCursor(point);
    // write the pixels to the display
    this->drawBitmap(this->frameBuffer, (FONT_WIDTH * size), (FONT_HEIGHT * size));

    // return the character width
    return FONT_WIDTH * size;
}