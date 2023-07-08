#include "Gradients.hpp"
#include <stdio.h>

// create a global instance of the lookup tables
unsigned short colorLUT[MAX_COLOR_DIFF + 1];

/**
 * @brief Construct a new Advanced Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
Gradients::Gradients(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->params = params;
    this->totalPixels = params.width * params.height;
    this->theta = 0;
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor Color to start with
 * @param endColor Color to end with
 * @param start Start Point
 * @param end End Point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
*/
void Gradients::fillGradient(Color startColor, Color endColor, Point start, Point end)
{
    // check if the start and end Points are the same
    if(start == end)
    {
        unsigned short startColor16 = startColor.to16bit();
        for(int i = 0; i < this->totalPixels; i++)
            this->frameBuffer[i] = startColor16;

        return;
    }

    // calculate the direction of the gradient
    int deltaX = end.X() - start.X();
    int deltaY = end.Y() - start.Y();
    int magnitudeSquared = (deltaX * deltaX + deltaY * deltaY);

    // find the maximum difference between the color components
    int dr = iabs(endColor.r - startColor.r);
    int dg = iabs(endColor.g - startColor.g);
    int db = iabs(endColor.b - startColor.b);
    int maxDiff = imax(dr, imax(dg, db));

    // create the lookup tables based on the maximum difference
    unsigned int numPositions = maxDiff + 1;

    // loop through each position in the gradient
    for(int i = 0; i < numPositions; i++)
    {
        // interpolate the color components based on the position and add them to the lookup tables
        unsigned char r = (((endColor.r - startColor.r) * i) / maxDiff + startColor.r) & 0x1f;
        unsigned char g = (((endColor.g - startColor.g) * i) / maxDiff + startColor.g) & 0x3f;
        unsigned char b = (((endColor.b - startColor.b) * i) / maxDiff + startColor.b) & 0x1f;
		colorLUT[i] = (r << 11) | (g << 5) | b;
    }

    // precalculate the divisor
    int magnitudeInverse = (FIXED_POINT_SCALE_HIGH_RES + magnitudeSquared >> 1) / magnitudeSquared;  // add magnitudeSquared / 2 for rounding

    // loop through each pixel in the buffer
    for(int x = 0; x < this->params.width; x++)
    {
        for (int y = 0; y < this->params.height; y++)
        {
            // calculate the vector from the start to the current pixel
            int vectorX = x - start.X();
            int vectorY = y - start.Y();

            // calculate the distance along the gradient direction
            int dotProduct = (vectorX * deltaX + vectorY * deltaY);
            int position = ((dotProduct * maxDiff) * magnitudeInverse) >> FIXED_POINT_SCALE_HIGH_RES_BITS;
            //int position = (dotProduct * maxDiff) / magnitudeSquared;

            // clamp the position within the valid range
            position = (position < 0) ? 0 : (position > maxDiff) ? maxDiff : position;

            // draw the pixel
			this->frameBuffer[x + y * this->params.width] = colorLUT[position];
        }
    }
}

/**
 * @brief Draw a circle gradient
 * @param center The center of the circle
 * @param radius The radius of the circle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void Gradients::drawRotCircleGradient(Point center, int radius, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % NUMBER_OF_ANGLES;

    int cosTheta = cosTable[theta];
    int sinTheta = sinTable[theta];

    Point rotGradStart = Point(
        center.x - (radius * cosTheta) / FIXED_POINT_SCALE,
        center.y - (radius * sinTheta) / FIXED_POINT_SCALE
    );
    Point rotGradEnd = Point(
        center.x + (radius * cosTheta) / FIXED_POINT_SCALE,
        center.y + (radius * sinTheta) / FIXED_POINT_SCALE
    );

    this->fillGradient(start, end, rotGradStart, rotGradEnd);
}

/**
 * @brief Draw a rectangle gradient
 * @param center The center of the rectangle
 * @param width The width of the rectangle
 * @param height The height of the rectangle
 * @param rotationSpeed The speed at which the gradient rotates
 * @param start The color to start the gradient with
 * @param end The color to end the gradient with
*/
void Gradients::drawRotRectGradient(Point center, int width, int height, int rotationSpeed, Color start, Color end)
{
    this->theta += rotationSpeed;
    this->theta = this->theta % NUMBER_OF_ANGLES;
    Point rotGradStart, rotGradEnd;

    // follow the quadrants of the unit circle
    if(this->theta < this->firstQuadrant)
        rotGradStart = Point(
            center.x + (width >> 1),
            center.y - (height >> 1) + (this->theta * height / this->firstQuadrant)
        );
    else if(this->theta < this->secondQuadrant)
        rotGradStart = Point(
            center.x + (width >> 1) - ((this->theta - this->firstQuadrant) * width / this->firstQuadrant),
            center.y + (height >> 1)
        );
    else if(this->theta < this->thirdQuadrant)
        rotGradStart = Point(
            center.x - (width >> 1),
            center.y + (height >> 1) - ((this->theta - this->secondQuadrant) * height / this->firstQuadrant)
        );
    else
        rotGradStart = Point(
            center.x - (width >> 1) + ((this->theta - this->thirdQuadrant) * width / this->firstQuadrant),
            center.y - (height >> 1)
        );

    // create the end point by making the start point the opposite corner
    rotGradEnd = Point(
        center.x - (rotGradStart.x - center.x),
        center.y - (rotGradStart.y - center.y)
    );

    this->fillGradient(start, end, rotGradStart, rotGradEnd);
}