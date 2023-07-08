#include "Print.hpp"

/**
 * @brief Construct a new Print object
 * @param display Display to print on
*/
Print::Print(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->width = params.width;
    this->height = params.height;
    this->totalPixels = params.width * params.height;
    this->color = Colors::White;
    this->font = nullptr;
}

/**
 * @brief Set the font to use
 * @param font Font to use
*/
void Print::setFont(FontStruct* font)
{
    this->font = font;
}

/**
 * @brief Write a number on the display
 * @param number Number to print
*/
void Print::print(long number, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
*/
void Print::print(unsigned long number, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
*/
void Print::print(double number, unsigned int precision)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE] = {0};    // largest number a double can represent is 1.79769e+308
    this->floatToString(number, buffer, precision);
    // write the string
    this->print(buffer);
}

/**
 * @brief Write a character on the display
 * @param character Character to print
*/
void Print::print(const char* text)
{
    // get the length of the text
    unsigned int length = strlen(text);

    // loop through the text
    for(int i = 0; i < length; i++)
    {
        // draw the character
        this->drawAscii(text[i]);
    }
}

/**
 * @brief Print a boolean on the display
 * @param bool Boolean to print
 */
void Print::print(bool value)
{
	this->print(value ? "true" : "false");
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::println(long number, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->println(buffer);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::println(unsigned long number, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->println(buffer);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
*/
void Print::println(double number, unsigned int precision)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE] = {0};    // largest number a double can represent is 1.79769e+308
    this->floatToString(number, buffer, precision);
    // write the string
    this->println(buffer);
}

/**
 * @brief Print a character on the display
 * @param character Character to print
*/
void Print::println(const char* text)
{
    this->print(text);
    this->print("\n");
}

/**
 * @brief Write a character on the display
 * @param value Boolean to print
*/
void Print::println(bool value)
{
    this->println(value ? TRUE : FALSE);
}

/**
 * @brief Print a newline
*/
void Print::println(void)
{
    this->println("\n");
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(long num, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
	sprintf(buffer, "%ld", num);
    printf("buffer: %s\n", buffer);
    return this->getPixelWidth(buffer, strlen(buffer));
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(unsigned long num, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
	sprintf(buffer, "%lu", num);
	printf("buffer: %s\n", buffer);
    return this->getPixelWidth(buffer, strlen(buffer));
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param precision Number of decimal places to print
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(double num, unsigned char precision)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
	sprintf(buffer, "%.*f", precision, num);
    printf("buffer: %s\n", buffer);
    return this->getPixelWidth(buffer, strlen(buffer));
}

/**
 * @brief Get the length of a string
 * @param text String to get the length of
 * @param size Size of the string
 * @return Length of the string
*/
unsigned int Print::getStringLength(const char* text)
{
    printf("buffer: %s\n", text);
    return this->getPixelWidth(text, strlen(text));
}

/**
 * @brief Get the length of a string
 * @param value String to get the length of
 * @param size Size of the string
 * @return Length of the string
*/
unsigned int Print::getStringLength(bool value)
{
    if(value)
        return this->getPixelWidth(TRUE, strlen(TRUE));
    else
        return this->getPixelWidth(FALSE, strlen(FALSE));
}

/**
 * @private
 * @brief Convert float to string
 * @param value Value to convert
 * @param buffer Buffer to write to
 * @param precision Precision of the value
*/
void Print::floatToString(double num, char* buffer, unsigned int precision)
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
	for(unsigned char i = 0; i < (precision + 1); ++i)
		rounding /= 10.0;
	num += rounding;

	// print the integer part
	unsigned long integer = (unsigned long)num;
	double remainder = num - (double)integer;

    // store the number of integers for fast reversing later
    unsigned int integers = 0;
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

void Print::reverse(char* buffer, unsigned int length)
{
    for(int i = 0; i < length/2; i++) {
        char temp = *(buffer + i);
        *(buffer + i) = *(buffer + length - i - 1);
        *(buffer + length - i - 1) = temp;
    }
}

/**
 * @private
 * @brief Draw an ascii character on the display
 * @param character Character to draw
 * @param Point Point to draw at
 * @param size Text size
 * @param color Color to draw the character
*/
void Print::drawAscii(const char character)
{
    // check if the font is a null pointer
    if(this->font == nullptr)
        return;
    
    // get the bitmap data
    const unsigned int* bitmap = this->font->bitmap;
    // get the character
    FontCharacter charData = this->font->characters[character - 0x20];

    // if the bitmap is a null pointer or overflows the frame buffer, return 0
    if (!((bitmap != nullptr) && ((charData.width * charData.height) < this->totalPixels)))
        return;

    // handle edge cases
    if(character == 0x20) // space
    {
        // move the cursor by the width of the character
        this->cursor += charData.width;
        return;
    }
    else if(character == 0x0A) // new line
    {
        // move the cursor to the next line
        this->cursor += (this->width - (this->cursor % this->width)) + this->width * this->font->newLineDistance;
        return;
    }
    else if(character == 0x0D) // carriage return
    {
        // move the cursor to the beginning of the line
        this->cursor -= (this->cursor % this->width);
        return;
    }
    else if (character == 0x09) // tab
    {
        // move the cursor by the width of the character
        this->cursor += (charData.width * 4);
        return;
    }

    // get our current framebuffer pointer location
    unsigned int bufferPosition = this->cursor;
    // calculate the row size
    unsigned int rowSize = charData.width;

    // make sure the character is not placed off screen in the x direction, if so, move the character to the next line
    if (((bufferPosition % this->width) + charData.width) > this->width)
    {
        // move the cursor to the next line
        bufferPosition += (this->width - (this->cursor % this->width)) + this->width * this->font->newLineDistance;
    }
    // make sure the character is not placed off screen in the y direction, if so, return to the top
    if (((bufferPosition / this->width) + charData.height) > this->height)
    {
        // move the cursor to the top of the screen
        bufferPosition = 0;
    }

    // move the cursor by the y offset
    bufferPosition += charData.yOffset * this->width;
    // keep track of the current row position
    unsigned int rowPosition = 0;

    // loop constraints
    unsigned int loopEnd = charData.length - charData.pointer;

    // loop through the bitmap data
    for (int j = 0; j < loopEnd; j++)
    {
        // get the distance to move the cursor
        unsigned int data = bitmap[j + charData.pointer];

        // move the pointer by the number of pixels as defined by the distance
        for (int i = 0; i < data; i++)
        {
            // every other distance should be drawn, the first distance is always the number of pixels to skip
            if (j & 0x1) this->frameBuffer[rowPosition + bufferPosition] = this->color;

            // increment the row position
            rowPosition++;

            // if the row position is equal to the row size, we have completed a row
            if (rowPosition >= rowSize)
            {
                rowPosition = 0;
                bufferPosition += this->width;
            }
        }
    }

    // set the cursor to the end of the character
    this->cursor += rowSize;
}

/**
 * @private
 * @brief Get the length of a string in pixels with the given font
 * @param text String to get the length of
 * @param size Size of the string
 * @return Length of the string in pixels
*/
size_t Print::getPixelWidth(const char* text, size_t size)
{
    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for(int i = 0; i < size; i++)
    {
        FontCharacter character = this->font->characters[text[i] - 0x20];
        pixels += character.width;
    }

    // return the number of pixels
    return pixels;
}