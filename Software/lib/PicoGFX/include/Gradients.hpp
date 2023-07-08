#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "GfxMath.h"

class Gradients
{
public:
    Gradients(unsigned short* frameBuffer, Display_Params params);

    void fillGradient(Color startColor, Color endColor, Point start, Point end);
    void drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end);
    void drawRotRectGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end);
private:
    unsigned short* frameBuffer;
    Display_Params params;
    size_t totalPixels;

    unsigned int theta; // The angle of the rotating gradient
    const int firstQuadrant = NUMBER_OF_ANGLES / 4;
    const int secondQuadrant = NUMBER_OF_ANGLES / 2;
    const int thirdQuadrant = NUMBER_OF_ANGLES * 3 / 4;
};