#include "Display.hpp"

#pragma region Write char
/**
 * @brief Write a character on the display
 * @param c Character to print
 * @param size Size of the character
*/
void Display::write(char c, uchar size, uchar base)
{
    this->write((long)c, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a character on the display
 * @param c Character to print
 * @param color Character color
 * @param size Size of the character
*/
void Display::write(char c, Color color, uchar size, uchar base)
{
    this->write((long)c, color, this->fillColor, size, base);
}

/**
 * @brief Write a character on the display
 * @param c Character to print
 * @param color Character color
 * @param backgroundColor Background color
 * @param size Size of the character
*/

void Display::write(char c, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->write((long)c, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Write uchar
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::write(uchar c, uchar size, uchar base)
{
    this->write((ulong)c, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::write(uchar c, Color color, uchar size, uchar base)
{
    this->write((ulong)c, color, this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/

void Display::write(uchar c, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->write((ulong)c, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Write int
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::write(int number, uchar size, uchar base)
{
    this->write((long)number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::write(int number, Color color, uchar size, uchar base)
{
    this->write((long)number, color, this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::write(int number, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->write((long)number, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Write uint
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::write(uint number, uchar size, uchar base)
{
    this->write((ulong)number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::write(uint number, Color color, uchar size, uchar base)
{
    this->write((ulong)number, color, this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::write(uint number, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->write((ulong)number, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Write long
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::write(long number, uchar size, uchar base)
{
    this->write(number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::write(long number, Color color, uchar size, uchar base)
{
    this->write(number, color, this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::write(long number, Color color, Color backgroundColor, uchar size, uchar base)
{
    // set base to decimal 
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];
    itoa(number, buffer, base);
    // write the string
    this->write(buffer, color, backgroundColor, size);
}
#pragma endregion

#pragma region Write ulong
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::write(ulong number, uchar size, uchar base)
{
    this->write(number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::write(ulong number, Color color, uchar size, uchar base)
{
    this->write(number, color, this->fillColor, size, base);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::write(ulong number, Color color, Color backgroundColor, uchar size, uchar base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->write(buffer, color, backgroundColor, size);
}
#pragma endregion

#pragma region Write float
/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::write(double number, uint precision, uchar size)
{
    this->write(number, this->fillColor.getOppositeColor(), this->fillColor, precision, size);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::write(double number, Color color, uint precision, uchar size)
{
    this->write(number, color, this->fillColor, precision, size);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::write(double number, Color color, Color backgroundColor, uint precision, uchar size)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE] = {0};    // largest number a double can represent is 1.79769e+308
    this->floatToString(number, buffer, precision);
    // write the string
    this->write(buffer, color, backgroundColor, size);
}
#pragma endregion

#pragma region Write string
/**
 * @brief Write a character on the display
 * @param character Character to print
 * @param size Size of the character
*/
void Display::write(const char* text, uchar size)
{
    this->write(text, this->fillColor.getOppositeColor(), this->fillColor, size);
}

/**
 * @brief Write a character on the display
 * @param character Character to print
 * @param color Character color
 * @param size Size of the character
*/
void Display::write(const char* text, Color color, uchar size)
{
    this->write(text, color, this->fillColor, size);
}

/**
 * @brief Write a character on the display
 * @param character Character to print
 * @param color Character color
 * @param background Background color
 * @param size Size of the character
*/
void Display::write(const char* text, Color color, Color background, uchar size)
{
    // copy the Point to local variables
    Point location = this->cursor;
    // copy the Point to local variables
    uint x = location.X();
    uint y = location.Y();
    // get the length of the text
    uint length = strlen(text);

    // loop through the text
    for(int i = 0; i < length; i++)
    {
        // if the text is a new line, move the text to the next line
        if (text[i] == '\n')
        {
            x = 0;
            y += FONT_HEIGHT * size;
            continue;
        }
        // if the text is a tab move the text to the next tab stop
        else if (text[i] == '\t')
        {
            x += FONT_WIDTH * size * TAB_SIZE;
            continue;
        }
        // check if the text is going to go off the screen by checking the future x Point with the width of the screen
        else if ((x + FONT_WIDTH * size) > this->params.width)
        {
            // move the text to the next line
            x = 0;
            y += FONT_HEIGHT * size;
        }
        // if we overflowed the screen, begin from the top again
        if ((y + FONT_HEIGHT * size) > this->params.height)
        {
            y = 0;
        }

        // increment the Point
        x += this->drawAscii(text[i], {x, y}, size, color, background);
    }

    // set the cursor
    this->setCursor(Point(x, y));
}

/**
 * @brief Write a character on the display
 * @param value Boolean to print
*/
void Display::write(bool value)
{
    this->write(value ? "true" : "false");
}
#pragma endregion

#pragma region Print char
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(char c, uchar size, uchar base)
{
    this->print((long)c, this->fillColor.getOppositeColor(), this->fillColor, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(char c, Color color, uchar size, uchar base)
{
    this->print((long)c, color, this->fillColor, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(char c, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->print((long)c, color, backgroundColor, size);
}
#pragma endregion

#pragma region Print uchar
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(uchar number, uchar size, uchar base)
{
    this->print((long)number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(uchar number, Color color, uchar size, uchar base)
{
    this->print((long)number, color, this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(uchar number, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->print((long)number, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Print int
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(int number, uchar size, uchar base)
{
    this->print((long)number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(int number, Color color, uchar size, uchar base)
{
    this->print((long)number, color, this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(int number, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->print((long)number, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Print uint
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(uint number, uchar size, uchar base)
{
    this->print((ulong)number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(uint number, Color color, uchar size, uchar base)
{
    this->print((ulong)number, color, this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(uint number, Color color, Color backgroundColor, uchar size, uchar base)
{
    this->print((ulong)number, color, backgroundColor, size, base);
}
#pragma endregion

#pragma region Print long
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(long number, uchar size, uchar base)
{
    this->print(number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(long number, Color color, uchar size, uchar base)
{
    this->print(number, color, this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(long number, Color color, Color backgroundColor, uchar size, uchar base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer, color, backgroundColor, size);
}
#pragma endregion

#pragma region Print ulong
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Display::print(ulong number, uchar size, uchar base)
{
    this->print(number, this->fillColor.getOppositeColor(), this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param size Size of the number
*/
void Display::print(ulong number, Color color, uchar size, uchar base)
{
    this->print(number, color, this->fillColor, size, base);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param size Size of the number
*/
void Display::print(ulong number, Color color, Color backgroundColor, uchar size, uchar base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer, color, backgroundColor, size);
}
#pragma endregion

#pragma region Print float
/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::print(double number, uint precision, uchar size)
{
    this->print(number, this->fillColor.getOppositeColor(), this->fillColor, precision, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::print(double number, Color color, uint precision, uchar size)
{
    this->print(number, color, this->fillColor, precision, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param color Number color
 * @param backgroundColor Background color
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Display::print(double number, Color color, Color backgroundColor, uint precision, uchar size)
{
    this->write(number, color, backgroundColor, precision, size);
    this->write("\n");
}
#pragma endregion

#pragma region Print string
/**
 * @brief Print a character on the display
 * @param character Character to print
 * @param size Size of the character
*/
void Display::print(const char* text, uchar size)
{
    this->print(text, this->fillColor.getOppositeColor(), this->fillColor, size);
}

/**
 * @brief Print a character on the display
 * @param character Character to print
 * @param color Character color
 * @param size Size of the character
*/
void Display::print(const char* text, Color color, uchar size)
{
    this->print(text, color, this->fillColor, size);
}

/**
 * @brief Print a character on the display
 * @param character Character to print
 * @param color Character color
 * @param background Background color
 * @param size Size of the character
*/
void Display::print(const char* text, Color color, Color background, uchar size)
{
    this->write(text, color, background, size);
    this->write("\n", color, background, size);
}
#pragma endregion

#pragma region Print void
/**
 * @brief Print a newline
*/
void Display::print(void)
{
    this->print("\n");
}
#pragma endregion

#pragma region Convert float to string
/**
 * @private
 * @brief Convert float to string
 * @param value Value to convert
 * @param buffer Buffer to write to
 * @param precision Precision of the value
*/
void Display::floatToString(double num, char* buffer, uint precision)
{
    // if precision is 0, just return the integer part
    if(precision == 0)
    {
        itoa((long)num, buffer, 10);
        return;
    }

    // print the sign if the number is negative
	if(num < 0.0)
	{
		*buffer++ = '-';
		num = -num;
	}

	// round the number to the precision
	double rounding = 0.5;
	for(uchar i = 0; i < (precision + 1); ++i)
		rounding /= 10.0;
	num += rounding;

	// print the integer part
	unsigned long integer = (unsigned long)num;
	double remainder = num - (double)integer;

    // store the number of integers for fast reversing later
    uint integers = 0;
    // loop until the integer is 0 at least once
    do
    {
        // add the first digit to the buffer
        *buffer++ = '0' + (integer % 10);
        integer /= 10;
        integers++;
    } while(integer > 0);

    // reverse the buffer to get the correct order
    this->reverse(buffer - integers, integers);

	// print the decimal point
	if(precision > 0)
		*buffer++ = '.';

	// print the decimal part
	while(precision-- > 0)
	{
		remainder *= 10.0;
		int digit = int(remainder);
        *buffer++ = '0' + digit;
		remainder -= digit;
        integers++;
	}
}

void Display::reverse(char* buffer, uint length)
{
    for(int i = 0; i < length/2; i++) {
        char temp = *(buffer + i);
        *(buffer + i) = *(buffer + length - i - 1);
        *(buffer + length - i - 1) = temp;
    }
}
#pragma endregion

#pragma region Write Ascii to display
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

/**
 * @brief Write a character on the display
 * @param value Boolean to print
*/
void Display::print(bool value)
{
    this->print(value ? "true" : "false");
}
#pragma endregion