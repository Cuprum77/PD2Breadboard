#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/divider.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"

#include "HardwareSPI.hpp"
#include "Commands.hpp"
#include "Structs.h"
#include "Shapes.h"
#include "Color.h"

// Limit of the ST7789 driver
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320
#define FRAMEBUFFER_SIZE (ST7789_WIDTH * ST7789_HEIGHT)

typedef enum
{
    DEG_0 = 0,
    DEG_90 = 1,
    DEG_180 = 2,
    DEG_270 = 3
} displayRotation_t;

class Display
{
public:
    Display(HardwareSPI* spi, Display_Pins* pins, Display_Params* params);
    bool writeReady(void) { return !this->spi->dma_busy(); }
    void setBrightness(unsigned char brightness);
    void setRotation(displayRotation_t rotation);
    void displayOn(void);
    void displayOff(void);
    void clear(void);
    void fill(Color color);
    Color getFillColor(void);

    void update(bool framecounter = false);
    void setPixel(Point point, Color color);
    void setPixel(unsigned int point, unsigned short color);
    Color getPixel(Point point);
    unsigned short getPixel(unsigned int point);
    size_t getBufferSize(void);

    void setCursor(Point point);
    Point getCursor(void);
    Point getCenter(void);

    int getFrameCounter() { return this->frames; }
    unsigned int getWidth(void) { return this->params->width; }
    unsigned int getHeight(void)  { return this->params->height; }
    unsigned short* getFrameBuffer(void) { return this->frameBuffer; }

protected:
    HardwareSPI* spi;
    Display_Pins* pins;
    Display_Params* params;
    bool dimmingEnabled = false;
    unsigned int sliceNum;
    unsigned int pwmChannel;
    bool dataMode = false;
    unsigned short frameBuffer[FRAMEBUFFER_SIZE + 1];
    Color fillColor;
    Point cursor = {0, 0};
    bool backlight;
    display_type_t type;
    bool BGR = false;
    unsigned int maxWidth;
    unsigned int maxHeight;
    unsigned int totalPixels;

    // timer for the framerate calculation
    int framecounter = 0;
    int frames = 0;
    unsigned long timer = 0;

    void ST7789_Init(void);
    void ST7789_SetRotation(displayRotation_t rotation);

    void GC9A01_Init(void);
    void GC9A01_SoftReset(void);
    void GC9A01_HardReset(void);
    void GC9A01_SetRotation(displayRotation_t rotation);

    void writeData(Display_Commands command, 
        const unsigned char* data, size_t length) { writeData((unsigned char)command, data, length); }
    void writeData(Display_Commands command, unsigned char data) { writeData((unsigned char)command, &data, 1); }
    void writeData(Display_Commands command) { writeData(command, nullptr, 0); }
    void writeData(unsigned char command, const unsigned char* data, size_t length);
    void writeData(unsigned char command, unsigned char data) { writeData(command, &data, 1); }
    void writeData(unsigned char command) { writeData(command, nullptr, 0); }
    inline void columnAddressSet(unsigned int x0, unsigned int x1);
    inline void rowAddressSet(unsigned int y0, unsigned int y1);
    void writePixels(const unsigned short* data, size_t length);
};