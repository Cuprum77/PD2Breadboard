#pragma once

#include <stdio.h>
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

#define uint unsigned int
#define uchar unsigned char
#define SPI_BAUDRATE 125000000  // 125 MHz

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define sq(x) ((x) * (x))

class Display
{
public:
    Display(spi_inst_t* spi, Display_Pins pins, 
        Display_Params params, bool dimming = false);
    void clear();
    void displayOn();
    void displayOff();
    void setCursor(Point Point);
    Point getCenter();

    void fill(Color color);
    void drawPixel(Point Point, Color color);
    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rectangle rect, Color color = Colors::White);
    void drawRectangle(Point center, uint width, uint height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);
    void drawCircle(Point center, uint radius, Color color = Colors::White);
    void drawFilledCircle(Point center, uint radius, Color color = Colors::White);

    void drawBitmap(Point point, const unsigned char* bitmap, uint width, uint height);
    void drawBitmap(Point point, const unsigned short* bitmap, uint width, uint height);

    void write(const char* text, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);
    void write(long long value, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);
    void write(double value, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);
    
    void print(const char* text, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);
    void print(long long value, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);
    void print(double value, Point Point, uint size = 3, Color color = Colors::White, Color background = Colors::Black);

    void setBrightness(unsigned char brightness);
private:
    spi_inst_t* spi;
    Display_Pins pins;
    Display_Params params;
    bool dimmingEnabled = false;
    uint sliceNum;
    uint pwmChannel;
    bool dataMode = false;

    void writeData(Display_Commands command, 
        const unsigned char* data, size_t length);
    void columnAddressSet(uint x0, uint x1);
    void rowAddressSet(uint y0, uint y1);
    void memoryWrite();
    void writePixels(const unsigned short* data, size_t length);
    
    uint drawAscii(const char c, Point Point, uint size, Color color, Color background);
};