#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "Display_Commands.hpp"
#include "Display_Structs.hpp"
#include "Display_Color.hpp"

#define uint unsigned int
#define SPI_BAUDRATE 125000000  // 125 MHz

class Display
{
public:
    Display(spi_inst_t* spi, Display_Pins pins, Display_Params params, bool dimming = false);
    void clear();
    void displayOn();
    void displayOff();

    void print(const char* text, uint x, uint y, uint size);
    void print(double value, uint x, uint y, uint size);

    void fill(Color color);
    void fill(unsigned short color);
    void drawPixel(uint x, uint y, unsigned short color);
    void drawPixel(uint x, uint y, Color color);
    void drawPixel(Coordinate coordinate, Color color);

    void setCursor(uint x, uint y);
    void setCursor(Coordinate coordinate);

    void setBrightness(unsigned char brightness);
private:
    spi_inst_t* spi;
    Display_Pins pins;
    Display_Params params;
    bool dimmingEnabled = false;
    uint sliceNum;
    uint pwmChannel;
    bool dataMode = false;

    void writeData(Display_Commands command, const unsigned char* data, size_t length);
    void columnAddressSet(uint x0, uint x1);
    void rowAddressSet(uint y0, uint y1);
    void memoryWrite();
    void writePixels(const unsigned short* data, size_t length);
};