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
#define ushort unsigned short
#define uchar unsigned char
#define SPI_BAUDRATE 125000000  // 125 MHz

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
    void clear();
    void displayOn();
    void displayOff();
    void setCursor(Point point);
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

    void drawBitmap(const unsigned char* bitmap, uint width, uint height);
    void drawBitmap(const unsigned short* bitmap, uint width, uint height);

    void write(char c, uint size = 1);
    void write(const char* text, uint size = 1);
    void write(const char* text, Color color, uint size = 1);
    void write(const char* text, Color color, Color background, uint size = 1);
    
    void print(char c, uint size = 1);
    void print(const char* text, uint size = 1);
    void print(const char* text, Color color, uint size = 1);
    void print(const char* text, Color color, Color background, uint size = 1);

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
    void memoryWrite();
    void writePixels(const unsigned short* data, size_t length);
    
    uint drawAscii(const char c, Point Point, uint size, Color color, Color background);
};