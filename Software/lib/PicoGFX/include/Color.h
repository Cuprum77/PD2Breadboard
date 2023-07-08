#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    
#define MAX_COLOR_DIFF 255

/**
 * @brief Colors enum
 * @note Colors are 16-bit values
*/
typedef enum
{
    Black = 0x0000,
    Navy = 0x000f,
    DarkGreen = 0x03e0,
    DarkCyan = 0x03ef,
    Maroon = 0x7800,
    Purple = 0x780f,
    Olive = 0x7be0,
    LightGreen = 0xc618,
    DarkGrey = 0x7bef,
    Blue = 0x001f,
    Green = 0x07e0,
    Cyan = 0x07ff,
    Red = 0xf800,
    Magenta = 0xf81f,
    Yellow = 0xffe0,
    White = 0xffff,
    Orange = 0xfd20,
    GreenYellow = 0xafe5,
    Pink = 0xf81f,
    Cum = 0xffbd,
    RaspberryRed = 0x9861,
    DarkOrange = 0xfc60,
    Gold = 0xfea0,
    Piss = 0xe702,
    Derg = 0x0dfb,
    OrangeRed = 0xf9e0,
    DarkYellow = 0xfe00
} Colors;

struct Color
{
    unsigned short r : 5;
    unsigned short g : 6;
    unsigned short b : 5;
    unsigned short a : 8;

    /**
     * @brief Creates a black color
    */
    Color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 0;
    }

    /**
     * @brief Creates a color from a Colors enum
     * @param color Colors enum
    */
    Color(Colors color)
    {
        this->r = (color >> 11) & 0x1F;
        this->g = (color >> 5) & 0x3F;
        this->b = color & 0x1F;
        this->a = 255;
    }

    /**
     * @brief Creates a color from a 8 bit value
     * @param color 8-bit color
    */
    Color(unsigned char color)
    {
        this->r = (color >> 5) & 0x03;
        this->g = (color >> 2) & 0x03;
        this->b = color & 0x02;
        this->a = 255;
    }

    /**
     * @brief Creates a color from a 16 bit value
     * @param color 16-bit color
    */
    Color(unsigned short color)
    {
        this->r = (color >> 11) & 0x1F;
        this->g = (color >> 5) & 0x3F;
        this->b = color & 0x1F;
        this->a = 255;
    }

    /**
     * @brief Applies a multiplier to a 24-bit color
     * @param color 24-bit color
     * @param multiplier Multiplier (0-31)
    */
    Color(Color color, unsigned short multiplier)
    {
        this->r = color.r * multiplier;
        this->g = color.g * multiplier;
        this->b = color.b * multiplier;
        this->a = color.a;
    }

    /**
     * @brief Applies a multiplier to a 16-bit color
     * @param color 16-bit color
     * @param multiplier Multiplier (0-31)
    */
    Color(unsigned short color, unsigned short multiplier)
    {
        this->r = ((color >> 11) & 0x1F) * multiplier;
        this->g = ((color >> 5) & 0x3F) * multiplier;
        this->b = (color & 0x1F) * multiplier;
        this->a = 255;
    }

    /**
     * @brief Creates a color from 3 values
     * @param r Red value (5 bits)
     * @param g Green value (6 bits)
     * @param b Blue value (5 bits)
    */
    Color(unsigned short r, unsigned short g, 
        unsigned short b, unsigned short a = 255)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    /**
     * @brief Returns the color as a 16 bit value
     * @return 16-bit color
    */
    unsigned short to16bit()
    {
        return (this->r << 11) | (this->g << 5) | this->b;
    }

    /**
     * @brief Converts a 24-bit color to a 16-bit color
     * @param color 24-bit color
     * @return 16-bit color
     */
    Color hexToColor(unsigned int color)
    {
        unsigned char r = (color >> 16) & 0xFF;
        unsigned char g = (color >> 8) & 0xFF;
        unsigned char b = color & 0xFF;

        this->r = r >> 3 & 0x1F;
        this->g = g >> 2 & 0x3F;
        this->b = b >> 3 & 0x1F;
        this->a = 255;

        return *this;
    }

    /**
     * @brief Gets the opposite color
     * @return Opposite color
    */
    Color getOppositeColor()
    {
        return Color(0x1f - this->r, 0x3f - this->g, 0x1f - this->b);
    }

    /**
     * @brief Blend two colors
     * @param color Color to blend with
     * @param ratio Ratio of the blend (0-1)
     * @return Blended color
    */
    Color blend(Color color, unsigned short ratio)
    {
        // split blue and red
        unsigned short rb = color.to16bit() & 0xf81f;
        rb += ((this->to16bit() & 0xf81f) - rb) * (ratio >> 2) >> 6;
        // split out green
        unsigned short g = color.to16bit() & 0x07e0;
        g += ((this->to16bit() & 0x07e0) - g) * ratio  >> 8;
        // recombine
        unsigned short result = (rb & 0xf81f) | (g & 0x07e0);
        return Color(result);
    }

    bool operator==(Color color)
    {
        return this->r == color.r && this->g == color.g && this->b == color.b;
    }

    bool operator!=(Color color)
    {
        return this->r != color.r || this->g != color.g || this->b != color.b;
    }

    Color operator+(Color color)
    {
        return Color(this->r + color.r, this->g + color.g, this->b + color.b);
    }

    Color operator-(Color color)
    {
        return Color(this->r - color.r, this->g - color.g, this->b - color.b);
    }

    Color operator*(Color color)
    {
        return Color(this->r * color.r, this->g * color.g, this->b * color.b);
    }

    Color operator/(Color color)
    {
        return Color(this->r / color.r, this->g / color.g, this->b / color.b);
    }

    Color operator<(Color color)
    {
        return Color(this->r < color.r, this->g < color.g, this->b < color.b);
    }

    Color operator>(Color color)
    {
        return Color(this->r > color.r, this->g > color.g, this->b > color.b);
    }

    Color operator<=(Color color)
    {
        return Color(this->r <= color.r, this->g <= color.g, this->b <= color.b);
    }

    Color operator>=(Color color)
    {
        return Color(this->r >= color.r, this->g >= color.g, this->b >= color.b);
    }

    Color biggest(Color color)
    {
        return Color(this->r > color.r ? this->r : color.r, this->g > color.g ? this->g : color.g, this->b > color.b ? this->b : color.b);
    }

    Color smallest(Color color)
    {
        return Color(this->r < color.r ? this->r : color.r, this->g < color.g ? this->g : color.g, this->b < color.b ? this->b : color.b);
    }
};

#ifdef __cplusplus
}
#endif