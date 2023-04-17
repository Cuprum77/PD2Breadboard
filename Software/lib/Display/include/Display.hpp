#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "Display_Commands.hpp"
#include "Display_Structs.hpp"
#include "Display_Color.hpp"
#include "Display_Enums.hpp"

#include "Font.h"


// Typedefs
#define ulong unsigned long
#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char
#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16

// SPI
#define SPI_BAUDRATE 125000000  // 125 MHz

// Constants
#define CHARACTER_BUFFER_SIZE 128

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define sq(x) ((x) * (x))
// Limit of the ST7789 driver
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320
#define FRAMEBUFFER_SIZE (ST7789_WIDTH * ST7789_HEIGHT)
// String behavior
#define TAB_SIZE 4  // how many spaces a tab is worth


class Display
{
public:
    Display(spi_inst_t* spi, Display_Pins pins, 
        Display_Params params, bool dimming = false);
    void clear(void);
    void displayOn(void);
    void displayOff(void);
    void setCursor(Point point);
    Point getCursor(void);
    Point getCenter(void);

    void fill(Color color);
    void drawPixel(Point Point, Color color);
    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rectangle rect, Color color = Colors::White);
    void drawRectangle(Point center, uint width, uint height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);
    void drawCircle(Point center, uint radius, Color color = Colors::White);
    void drawFilledCircle(Point center, uint radius, Color color = Colors::White);

    void drawBitmap(const unsigned char* bitmap, uint width, uint height);
    void drawBitmap(const unsigned short* bitmap, uint width, uint height);

#pragma region Write
    void write(char c, uchar size = 1, uchar base = DEC);
    void write(char num, Color color, uchar size = 1, uchar base = DEC);
    void write(char num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(uchar c, uchar size = 1, uchar base = DEC);
    void write(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void write(uchar num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(short num, uchar size = 1, uchar base = DEC);
    void write(short num, Color color, uchar size = 1, uchar base = DEC);
    void write(short num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(ushort num, uchar size = 1, uchar base = DEC);
    void write(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void write(ushort num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(int num, uchar size = 1, uchar base = DEC);
    void write(int num, Color color, uchar size = 1, uchar base = DEC);
    void write(int num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(uint num, uchar size = 1, uchar base = DEC);
    void write(uint num, Color color, uchar size = 1, uchar base = DEC);
    void write(uint num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(long num, uchar size = 1, uchar base = DEC);
    void write(long num, Color color, uchar size = 1, uchar base = DEC);
    void write(long num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(ulong num, uchar size = 1, uchar base = DEC);
    void write(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void write(ulong num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void write(double num, uint precision = 2, uchar size = 1);
    void write(double num, Color color, uint precision = 2, uchar size = 1);
    void write(double num, Color color, Color background, uint precision = 2, uchar size = 1);
    void write(const char* text, uchar size = 1);
    void write(const char* text, Color color, uchar size = 1);
    void write(const char* text, Color color, Color background, uchar size = 1);
#pragma endregion
    
#pragma region Print
    void print(char c, uchar size = 1, uchar base = DEC);
    void print(char num, Color color, uchar size = 1, uchar base = DEC);
    void print(char num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(uchar c, uchar size = 1, uchar base = DEC);
    void print(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void print(uchar num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(int num, uchar size = 1, uchar base = DEC);
    void print(int num, Color color, uchar size = 1, uchar base = DEC);
    void print(int num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(uint num, uchar size = 1, uchar base = DEC);
    void print(uint num, Color color, uchar size = 1, uchar base = DEC);
    void print(uint num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(short num, uchar size = 1, uchar base = DEC);
    void print(short num, Color color, uchar size = 1, uchar base = DEC);
    void print(short num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(ushort num, uchar size = 1, uchar base = DEC);
    void print(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void print(ushort num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(long num, uchar size = 1, uchar base = DEC);
    void print(long num, Color color, uchar size = 1, uchar base = DEC);
    void print(long num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(ulong num, uchar size = 1, uchar base = DEC);
    void print(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void print(ulong num, Color color, Color background, uchar size = 1, uchar base = DEC);
    void print(double num, uint precision = 2, uchar size = 1);
    void print(double num, Color color, uint precision = 2, uchar size = 1);
    void print(double num, Color color, Color background, uint precision = 2, uchar size = 1);
    void print(const char* text, uchar size = 1);
    void print(const char* text, Color color, uchar size = 1);
    void print(const char* text, Color color, Color background, uchar size = 1);
    void print(void);
#pragma endregion

    void setBrightness(unsigned char brightness);
private:
    spi_inst_t* spi;
    Display_Pins pins;
    Display_Params params;
    bool dimmingEnabled = false;
    uint sliceNum;
    uint pwmChannel;
    bool dataMode = false;
    ushort frameBufferColumn[ST7789_WIDTH + 1] = {0};
    ushort frameBuffer[FRAMEBUFFER_SIZE + 1] = {0};
    Color fillColor;
    Point cursor = {0, 0};

    void writeData(Display_Commands command, 
        const unsigned char* data, size_t length);
    void columnAddressSet(uint x0, uint x1);
    void rowAddressSet(uint y0, uint y1);
    void memoryWrite(void);
    void writePixels(const unsigned short* data, size_t length);
    
    uint drawAscii(const char c, Point Point, uint size, Color color, Color background);
    void floatToString(double num, char* buffer, uint precision);
    void reverse(char* str, uint length);
};