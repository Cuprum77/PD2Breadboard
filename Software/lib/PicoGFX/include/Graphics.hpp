#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "GfxMath.h"

class Graphics
{
public:
    Graphics(unsigned short* frameBuffer, Display_Params params);

    void fillGradientCool(Color startColor, Color endColor, Point start, Point end);
    void drawLine(Point start, Point end, Color color = Colors::White);
    void drawExpandingLine(Point start, Point end, unsigned int startThickness, unsigned int endThickness, Color color = Colors::White);
	void drawTriangle(Point p1, Point p2, Point p3, Color color = Colors::White);
	void drawTriangleFilled(Point p1, Point p2, Point p3, Color color = Colors::White);
    void drawRectangle(Point start, Point end, Color color = Colors::White);
    void drawRectangle(Rect rect, Color color = Colors::White);
    void drawRectangle(Point center, unsigned int width, unsigned int height, Color color = Colors::White);
    void drawFilledRectangle(Point start, Point end, Color color = Colors::White);
    void drawCircle(Point center, unsigned int radius, Color color = Colors::White);
    void drawFilledCircle(Point center, unsigned int radius, Color color = Colors::White);
    void drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color = Colors::White);
    void drawFilledDualArc(Point center, unsigned int innerRadius, unsigned int outerRadius, unsigned int startAngle, unsigned int endAngle, Color color);

    void drawBitmap(const unsigned char* bitmap, unsigned int width, unsigned int height);
    void drawBitmap(const unsigned short* bitmap, unsigned int width, unsigned int height);
private:
    unsigned short* frameBuffer;
    Display_Params params;
    size_t totalPixels;
};