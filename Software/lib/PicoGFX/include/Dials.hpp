#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "Print.hpp"
#include "Graphics.hpp"

struct Gauge
{
    int currentValue;
    int radius;
    int minValue;
    int maxValue;
    Color minColor;
    Color maxColor;
	Point location;
    unsigned short* gaugeBuffer;
};

class Dials
{
public:
    Dials(Print* print, Graphics* graphics, unsigned short* frameBuffer, Display_Params param);
    
    void drawGauge(Gauge* gauge);
    void updateGauge(Gauge* gauge, int newValue);
private:
    Print* print;
    Graphics* gfx;
	unsigned short* frameBuffer;
    unsigned int width;
    unsigned int height;
    unsigned int centerX;
    unsigned int centerY;
    size_t totalPixels;
};