#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "GfxMath.h"

struct Point
{
    int x;
    int y;

    /**
     * @brief Construct a new empty Point object
    */
    Point()
    {
        this->x = 0;
        this->y = 0;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(unsigned int x, unsigned int y)
    {
        this->x = (int)x;
        this->y = (int)y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(unsigned int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(int x, unsigned int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Get the X coordinate of the point
     * @return int X coordinate of the point
    */
    int X()
    {
        return this->x;
    }

    /**
     * @brief Set the X coordinate of the point
     * @param x X coordinate of the point
    */
    void X(int x)
    {
        this->x = x;
    }

    /**
     * @brief Get the Y coordinate of the point
     * @return int Y coordinate of the point
    */
    int Y()
    {
        return this->y;
    }

    /**
     * @brief Set the Y coordinate of the point
     * @param y Y coordinate of the point
    */
    void Y(int y)
    {
        this->y = y;
    }

    /**
     * @brief Calculate the distance between two points
     * @param other The other point
     * @return unsigned int The distance between the two points
    */
    unsigned int Distance(Point other)
    {
        return isqrt(ipow(this->x - other.x, 2) + ipow(this->y - other.y, 2));
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

struct Rect
{
private:
    Point x;
    Point y;

    unsigned int width;
    unsigned int height;

    unsigned int bottom;
    unsigned int left;
    unsigned int right;
    unsigned int top;

public:
    /**
     * @brief Construct a new empty Rect object
    */
    Rect()
    {
        this->x = Point();
        this->y = Point();
        this->width = 0;
        this->height = 0;
        this->bottom = 0;
        this->left = 0;
        this->right = 0;
        this->top = 0;
    }

    /**
     * @brief Construct a new Rect object
     * @param corner1 Upper left corner of the Rect
     * @param corner2 Lower right corner of the Rect
    */
    Rect(Point corner1, Point corner2)
    {
        // Set the corners of the Rect
        this->x = Point(corner1.X(), corner2.Y());
        this->y = Point(corner2.X(), corner1.Y());
        // Set the other variables of the Rect
        this->width = corner2.X() - corner1.X();
        this->height = corner2.Y() - corner1.Y();
        this->bottom = this->y.Y();
        this->left = this->x.X();
        this->right = this->y.X();
        this->top = this->x.Y();
    }

    /**
     * @brief Construct a new Rect object
     * @param center Center of the Rect
     * @param width Width of the Rect
     * @param height Height of the Rect
     * @note The Rect is constructed with the center as the center of the Rect
    */
    Rect(Point center, unsigned int width, unsigned int height)
    {
        // Calculate the corners of the Rect
        this->x = Point(center.X() - (width / 2), center.Y() - (height / 2));
        this->y = Point(center.X() + (width / 2), center.Y() + (height / 2));
        // Set the other variables of the Rect
        this->width = width;
        this->height = height;
        this->bottom = this->y.Y();
        this->left = this->x.X();
        this->right = this->y.X();
        this->top = this->x.Y();
    }

    /**
     * @brief Get the center of the Rect
     * @return Point
    */
    Point GetCenter()
    {
        return Point(
            (x.X() + y.X()) / 2,
            (x.Y() + y.Y()) / 2
        );
    }

    /**
     * @brief Get the first corner of the Rect
     * @return Point
    */
    Point X()
    {
        return x;
    }

    /**
     * @brief Get the second corner of the Rect
     * @return Point
    */
    Point Y()
    {
        return y;
    }

    /**
     * @brief Get the width of the Rect
     * @return unsigned int
    */
    unsigned int Width()
    {
        return width;
    }

    /**
     * @brief Get the height of the Rect
     * @return unsigned int
    */
    unsigned int Height()
    {
        return height;
    }

    /**
     * @brief Get the bottom of the Rect
     * @return unsigned int that is the Y coordinate of the bottom of the Rect
    */
    unsigned int Bottom()
    {
        return bottom;
    }

    /**
     * @brief Get the left of the Rect
     * @return unsigned int that is the X coordinate of the left of the Rect
    */
    unsigned int Left()
    {
        return left;
    }

    /**
     * @brief Get the right of the Rect
     * @return unsigned int that is the X coordinate of the right of the Rect
    */
    unsigned int Right()
    {
        return right;
    }

    /**
     * @brief Get the top of the Rect
     * @return unsigned int that is the Y coordinate of the top of the Rect
    */
    unsigned int Top()
    {
        return top;
    }
};

inline void swap(Point& a, Point& b)
{
	Point temp = a;
	a = b;
	b = temp;
}

#ifdef __cplusplus
}
#endif