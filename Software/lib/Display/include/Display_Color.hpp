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

    Color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    Color(unsigned short color)
    {
        this->r = (color >> 11) & 0x1F;
        this->g = (color >> 5) & 0x3F;
        this->b = color & 0x1F;
    }

    Color(unsigned short r, unsigned short g, unsigned short b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    unsigned short to16bit()
    {
        return (this->r << 11) | (this->g << 5) | this->b;
    }
};