#include "Graphics.hpp"
#include <stdio.h>

/**
 * @brief Construct a new Graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
Graphics::Graphics(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->params = params;
    this->totalPixels = params.width * params.height;
}

/**
 * @brief Fill the display with a color gradient
 * @param startColor Color to start with
 * @param endColor Color to end with
 * @param start Start Point
 * @param end End Point
 * @note The start and end points are only used to find the direction of the gradient, it will still fill the entire display!
*/
void Graphics::fillGradientCool(Color startColor, Color endColor, Point start, Point end)
{
    // check if the start and end Points are the same
    if(start == end)
        return;

    // set the start to end if end is less than start
    if(end.X() < start.Y() && end.Y() < start.Y())
    {
        Point temp = start;
        start = end;
        end = temp;
    }

    // calculate the length of the line
    unsigned int length = start.Distance(end);

    // calculate the direction of the gradient
    unsigned int deltaX = end.X() - start.X();
    unsigned int deltaY = end.Y() - start.Y();
    unsigned int magnitude = isqrt((deltaX * deltaX) + (deltaY * deltaY));
    float gradX = deltaX / magnitude;
    float gradY = deltaY / magnitude;

    // loop through each pixel in the buffer
    for(int y = 0; y < this->params.height; y++)
    {
        for(int x = 0; x < this->params.width; x++)
        {
            // calculate the position along the gradient direction
            float position = (gradX * x) + (gradY * y);

            // interpolate the color based on the position
            Color color;
            color.r = startColor.r + ((endColor.r - startColor.r) * position / length);
            color.g = startColor.g + ((endColor.g - startColor.g) * position / length);
            color.b = startColor.b + ((endColor.b - startColor.b) * position / length);

            // draw the pixel
            this->frameBuffer[x + y * this->params.width] = color.to16bit();
        }
    }
}

/**
 * @brief Draw a line on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawLine(Point start, Point end, Color color)
{
    // Uses Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // move Points into local variables
    unsigned int x0 = start.X();
    unsigned int y0 = start.Y();
    unsigned int x1 = end.X();
    unsigned int y1 = end.Y();

    // get the difference between the x and y Points
    int dx = iabs((int)end.X() - (int)start.X());
    int sx = start.X() < end.X() ? 1 : -1;
    int dy = -(int)iabs((int)end.Y() - (int)start.Y());
    int sy = start.Y() < end.Y() ? 1 : -1;
    // calculate the error
    int error = dx + dy;
    
    while(true)
    {
        // set the pixel in the frame buffer
        this->frameBuffer[x0 + y0 * this->params.width] = color.to16bit();

        // if we have reached the end Point, break
        if(x0 == x1 && y0 == y1) break;

        // calculate the error
        int e2 = 2 * error;
        // if the error is greater than the difference in y
        if(e2 >= dy)
        {
            // if we have reached the end Point, break
            if(x0 == x1) break;
            // increment the x Point
            error += dy;
            x0 += sx;
        }
        // if the error is greater than the difference in x
        if(e2 <= dx)
        {
            // if we have reached the end Point, break
            if(y0 == y1) break;
            // increment the y Point
            error += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief Draw a line on the display that expands in thickness
 * @param start Start Point
 * @param end End Point
 * @param startThickness Thickness at the start point
 * @param endThickness Thickness at the end point
 * @param color Color to draw in
*/
void Graphics::drawExpandingLine(Point start, Point end, unsigned int startThickness, unsigned int endThickness, Color color)
{
    // Calculate the length and angle of the line
    int dx = end.x - start.x;
    int dy = end.y - start.y;
    unsigned int length = isqrt(dx * dx + dy * dy); // integer square root
    int index = dy / imax(dx, 1);
    index *= ANGLE_SCALE;
    index = iabs(index);
    printf("Index: %d\n", index);
    unsigned int angle = atanTable[index];

    // Combine the color to 16 bit
    unsigned short color16bit = color.to16bit();

    // Loop over the length of the line
    for (unsigned int i = 0; i <= length; i++)
    {
        // Interpolate the thickness of the line at this point
        int thickness = startThickness + i * (endThickness - startThickness) / length;

        // Loop over the thickness of the line
        for (int j = -thickness / 2; j <= thickness / 2; j++)
        {
            // Calculate the position of the pixel
            unsigned int x = start.x + i * cosTable[angle] / FIXED_POINT_SCALE;
            unsigned int y = start.y + i * sinTable[angle] / FIXED_POINT_SCALE;

            // Calculate the offset of the pixel within the thickness of the line
            unsigned int index = (unsigned int)((angle + 900) % NUMBER_OF_ANGLES);
            unsigned int offsetX = j;
            offsetX *= sinTable[index];
            offsetX /= FIXED_POINT_SCALE;
            unsigned int offsetY = j;
            offsetY *= cosTable[index];
            offsetY /= FIXED_POINT_SCALE;

            // Set the pixel in the frame buffer, if it's within bounds
            if (x + offsetX >= 0 && x + offsetX < this->params.width && y + offsetY >= 0 && y + offsetY < this->params.height)
            {
                this->frameBuffer[(x + offsetX) + (y + offsetY) * this->params.width] = color16bit;
            }
        }
    }
}

/**
 * @brief Draw a triangle on the display
 * @param p1 First point
 * @param p2 Second point
 * @param p3 Third point
 * @param color Color to draw in
*/
void Graphics::drawTriangle(Point p1, Point p2, Point p3, Color color)
{
	// Draw the three lines of the triangle
	this->drawLine(p1, p2, color);
	this->drawLine(p2, p3, color);
	this->drawLine(p3, p1, color);
}

/**
 * @brief Draw a filled triangle on the display
 * @param p1 First point
 * @param p2 Second point
 * @param p3 Third point
 * @param color Color to draw in
*/
void Graphics::drawTriangleFilled(Point p1, Point p2, Point p3, Color color)
{
    // calculate the bounding box of the triangle
    int minX = imin(imin(p1.x, p2.x), p3.x);
    int maxX = imax(imax(p1.x, p2.x), p3.x);
    int minY = imin(imin(p1.y, p2.y), p3.y);
    int maxY = imax(imax(p1.y, p2.y), p3.y);

    // convert the color to unsigned short
    unsigned short color16 = color.to16bit();

    // iterate over each row within the bounding box
    for (int y = minY; y <= maxY; y++)
    {
		// find the start x by interpolating between p1 and p2
		int divisor = (p2.y - p1.y) == 0 ? 1 : p2.y - p1.y;
		int startX = p1.x + (y - p1.y) * (p2.x - p1.x) / divisor;
		// find the end x by interpolating between p2 and p3
		divisor = (p3.y - p2.y) == 0 ? 1 : p3.y - p2.y;
		int endX = p2.x + (y - p2.y) * (p3.x - p2.x) / divisor;

        // ensure startX <= endX
        if (startX > endX)
        {
            int temp = startX;
            startX = endX;
            endX = temp;
        }

		// clamp the start and end points the screen
        startX = imax(startX, 0);
		endX = imin(endX, this->params.width);

        // fill the pixels between the intersection points
        for (int x = startX; x <= endX; x++)
        {
            this->frameBuffer[x + y * this->params.width] = color16;
        }
    }
}

/**
 * @brief Draw a rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Point start, Point end, Color color)
{
    // draw the rectangle
    this->drawLine({start.X(), start.Y()}, {end.X(), start.Y()}, color);
    this->drawLine({end.X(), start.Y()}, {end.X(), end.Y()}, color);
    this->drawLine({end.X(), end.Y()}, {start.X(), end.Y()}, color);
    this->drawLine({start.X(), end.Y()}, {start.X(), start.Y()}, color);
}

/**
 * @brief Draw a rectangle on the display
 * @param rect Rectangle to draw
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Rect rect, Color color)
{
    // draw the rectangle
    this->drawRectangle(rect.X(), rect.Y(), color);
}

/**
 * @brief Draw a center aligned rectangle on the display
 * @param center Center Point
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param color Color to draw in
 * @param thickness Thickness of the line
*/
void Graphics::drawRectangle(Point center, unsigned int width, unsigned int height, Color color)
{
    // calculate the start and end Points
    Point start = {center.X() - (width / 2), center.Y() - (height / 2)};
    Point end = {center.X() + (width / 2), center.Y() + (height / 2)};

    // draw the rectangle
    this->drawRectangle(start, end, color);
}

/**
 * @brief Draw a filled rectangle on the display
 * @param start Start Point
 * @param end End Point
 * @param color Color to draw in
*/
void Graphics::drawFilledRectangle(Point start, Point end, Color color)
{
    // convert color to 16 bit
    unsigned short color16 = color.to16bit();

    // calculate the size of the rectangle
    unsigned int width = end.X() - start.X();
    unsigned int height = end.Y() - start.Y();
    
    // loop through the height
    for(int i = 0; i < height; i++)
    {
        // loop through the width
        for(int j = 0; j < width; j++)
        {
            // write the pixel
            this->frameBuffer[(start.X() + j) + (start.Y() + i) * this->params.width] = color16;
        }
    }
}

/**
 * @brief Draw a circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Graphics::drawCircle(Point center, unsigned int radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // convert the color to 16 bit
    unsigned short color16 = color.to16bit();

    // loop through the radius
    while(x >= y)
    {
        // draw the pixels in the frame buffer
        this->frameBuffer[(x0 + x) + (y0 + y) * this->params.width] = color16;
        this->frameBuffer[(x0 + y) + (y0 + x) * this->params.width] = color16;
        this->frameBuffer[(x0 - y) + (y0 + x) * this->params.width] = color16;
        this->frameBuffer[(x0 - x) + (y0 + y) * this->params.width] = color16;
        this->frameBuffer[(x0 - x) + (y0 - y) * this->params.width] = color16;
        this->frameBuffer[(x0 - y) + (y0 - x) * this->params.width] = color16;
        this->frameBuffer[(x0 + y) + (y0 - x) * this->params.width] = color16;
        this->frameBuffer[(x0 + x) + (y0 - y) * this->params.width] = color16;
        
        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}

/**
 * @brief Draw a filled circle on the display
 * @param center Center Point
 * @param radius Radius of the circle
 * @param color Color to draw in
*/
void Graphics::drawFilledCircle(Point center, unsigned int radius, Color color)
{
    // Uses Bresenham's circle algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

    // move Points into local variables
    int x0 = center.X();
    int y0 = center.Y();
    int x = radius;
    int y = 0;
    int error = 3 - 2 * x;

    // loop through the radius
    while(x >= y)
    {
        // draw the pixel
        this->drawLine({x0 + x, y0 + y}, {x0 - x, y0 + y}, color);
        this->drawLine({x0 + y, y0 + x}, {x0 - y, y0 + x}, color);
        this->drawLine({x0 + x, y0 - y}, {x0 - x, y0 - y}, color);
        this->drawLine({x0 + y, y0 - x}, {x0 - y, y0 - x}, color);

        // if the error is greater than 0
        if(error > 0)
        {
            // decrement the x Point
            x--;
            // calculate the error
            error = error + 4 * (y - x) + 10;
        }
        else
        {
            // calculate the error
            error = error + 4 * y + 6;
        }

        // increment the y Point
        y++;
    }
}

/**
 * @brief Draw an arc to the display
 * @param center Center point
 * @param radius Radius of the arc
 * @param start_angle Start angle of the arc
 * @param end_angle End angle of the arc
 * @param color Color to draw in
*/
void Graphics::drawArc(Point center, unsigned int radius, unsigned int start_angle, unsigned int end_angle, Color color)
{
    unsigned int imageWidth = params.width;
    unsigned int imageHeight = params.height;

	// Swap angles if start_angle is greater than end_angle
    if (end_angle < start_angle) 
    {
        unsigned int temp = end_angle;
        end_angle = start_angle;
        start_angle = temp;
    }

	// clamp the input variables to be between 0 and 3600
	start_angle = imin(start_angle, NUMBER_OF_ANGLES);
	end_angle = imin(end_angle, NUMBER_OF_ANGLES);

	// convert the color to 16 bit
    unsigned short color16bit = color.to16bit();

    // loop through the angles
    for (int angle = start_angle; angle < end_angle; angle++) 
    {
		// Get the coordinates of the pixel
        unsigned int x = 0;
        unsigned int y = 0;
		pointOnCircle(radius, angle, center.x, center.y, &x, &y);

		// avoid overflowing the buffer
        if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight)
            this->frameBuffer[x + y * imageWidth] = color16bit;
    }
}

/**
 * @brief Draw two arcs and fill the gap between them
 * @param center Center point
 * @param innerRadius Radius for the inner most arc
 * @param outerRadius Radius for the outer most arc
 * @param startAngle Angle in degrees for both arcs
 * @param endAngle Angle in degrees for both arcs
 * @color Color to draw the arc in
 */
void Graphics::drawFilledDualArc(Point center, unsigned int innerRadius, unsigned int outerRadius, unsigned int startAngle, unsigned int endAngle, Color color)
{
    unsigned int imageWidth = params.width;
    unsigned int imageHeight = params.height;

    // Swap angles if start_angle is greater than end_angle
    if (endAngle < startAngle)
    {
        unsigned int temp = endAngle;
        endAngle = startAngle;
        startAngle = temp;
    }

    // clamp the input variables to be between 0 and 360
    startAngle = imin(startAngle, 360);
    endAngle = imin(endAngle, 360);

    // convert the color to 16 bit
    unsigned short color16bit = color.to16bit();

    // Loop through the angles
    for (int angle = startAngle; angle < endAngle; angle++)
    {
        // Get the coordinates of the inner pixel
        unsigned int angleXInner = 0;
        unsigned int angleYInner = 0;
        pointOnCircle(innerRadius, angle, center.x, center.y, &angleXInner, &angleYInner);

        // Get the coordinates of the outer pixel
        unsigned int angleXOuter = 0;
        unsigned int angleYOuter = 0;
        pointOnCircle(outerRadius, angle, center.x, center.y, &angleXOuter, &angleYOuter);

        // Create the start and end points for the line
        Point start = { angleXInner, angleYInner };
        Point end = { angleXOuter, angleYOuter };

        // Calculate the thickness at the start and end of the line
        unsigned int startThickness = 1; // or calculate based on your needs
        unsigned int endThickness = 5; // or calculate based on your needs

        // Draw the expanding line
        drawExpandingLine(start, end, startThickness, endThickness, color);
    }
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Graphics::drawBitmap(const unsigned char* bitmap, unsigned int width, unsigned int height)
{
    this->drawBitmap((const unsigned short*)bitmap, width, height);
}

/**
 * @brief Draw a 16 bit bitmap on the display
 * @param location Location to draw the bitmap
 * @param bitmap Array containing the bitmap
 * @param width Width of the bitmap
 * @param height Height of the bitmap
*/
void Graphics::drawBitmap(const unsigned short* bitmap, unsigned int width, unsigned int height)
{
    // write the entire bitmap directly to the framebuffer using the setPixel function
    for(int i = 0; i < width * height; i++)
    {
        // write the pixel
        this->frameBuffer[i] = bitmap[i];
    }
}