#pragma once

struct Display_Pins
{
    unsigned int rst;
    unsigned int dc;
    unsigned int cs;
    unsigned int sda;
    unsigned int scl;
    unsigned int bl;
};

struct Display_Params
{
    unsigned int height;
    unsigned int width;
    unsigned int columnOffset1;
    unsigned int columnOffset2;
    unsigned int rowOffset1;
    unsigned int rowOffset2;   
    unsigned int rotation;
};

struct Coordinate
{
    unsigned int x;
    unsigned int y;

    Coordinate()
    {
        this->x = 0;
        this->y = 0;
    }

    Coordinate(unsigned int x, unsigned int y)
    {
        this->x = x;
        this->y = y;
    }

    bool operator==(const Coordinate& other)
    {
        return this->x == other.x && this->y == other.y;
    }

    bool operator!=(const Coordinate& other)
    {
        return !(*this == other);
    }

    Coordinate operator+(const Coordinate& other)
    {
        return Coordinate(this->x + other.x, this->y + other.y);
    }

    Coordinate operator-(const Coordinate& other)
    {
        return Coordinate(this->x - other.x, this->y - other.y);
    }

    Coordinate operator*(const Coordinate& other)
    {
        return Coordinate(this->x * other.x, this->y * other.y);
    }

    Coordinate operator/(const Coordinate& other)
    {
        return Coordinate(this->x / other.x, this->y / other.y);
    }
};