#pragma once

#include "Display_Structs.hpp"

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
} Colors;

struct Color
{
    unsigned short r : 5;
    unsigned short g : 6;
    unsigned short b : 5;

    /**
     * @brief Creates a black color
    */
    Color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
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
    }

    /**
     * @brief Creates a color from 3 values
     * @param r Red value (5 bits)
     * @param g Green value (6 bits)
     * @param b Blue value (5 bits)
    */
    Color(unsigned short r, unsigned short g, unsigned short b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
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

        return *this;
    }
};