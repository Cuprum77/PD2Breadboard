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

struct Point
{
    unsigned int x;
    unsigned int y;

    Point()
    {
        this->x = 0;
        this->y = 0;
    }

    Point(Point Point, unsigned int offset)
    {
        this->x = Point.x + offset;
        this->y = Point.y + offset;
    }

    Point(Point Point, unsigned int xOffset, unsigned int yOffset)
    {
        this->x = Point.x + xOffset;
        this->y = Point.y + yOffset;
    }

    Point(unsigned int x, unsigned int y)
    {
        this->x = x;
        this->y = y;
    }

    bool operator==(const Point& other)
    {
        return this->x == other.x && this->y == other.y;
    }

    bool operator!=(const Point& other)
    {
        return !(*this == other);
    }

    Point operator+(const Point& other)
    {
        return Point(this->x + other.x, this->y + other.y);
    }

    Point operator-(const Point& other)
    {
        return Point(this->x - other.x, this->y - other.y);
    }

    Point operator*(const Point& other)
    {
        return Point(this->x * other.x, this->y * other.y);
    }

    Point operator/(const Point& other)
    {
        return Point(this->x / other.x, this->y / other.y);
    }
};