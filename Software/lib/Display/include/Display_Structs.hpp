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
private:
    int x;
    int y;

public:
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
     * @brief Get the X coordinate of the point
     * @return int X coordinate of the point
    */
    int X()
    {
        return this->x;
    }

    /**
     * @brief Get the Y coordinate of the point
     * @return int Y coordinate of the point
    */
    int Y()
    {
        return this->y;
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

struct Rectangle
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
     * @brief Construct a new empty Rectangle object
    */
    Rectangle()
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
     * @brief Construct a new Rectangle object
     * @param corner1 Upper left corner of the rectangle
     * @param corner2 Lower right corner of the rectangle
    */
    Rectangle(Point corner1, Point corner2)
    {
        // Set the corners of the rectangle
        this->x = Point(corner1.X(), corner2.Y());
        this->y = Point(corner2.X(), corner1.Y());
        // Set the other variables of the rectangle
        this->width = corner2.X() - corner1.X();
        this->height = corner2.Y() - corner1.Y();
        this->bottom = this->y.Y();
        this->left = this->x.X();
        this->right = this->y.X();
        this->top = this->x.Y();
    }

    /**
     * @brief Construct a new Rectangle object
     * @param center Center of the rectangle
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @note The rectangle is constructed with the center as the center of the rectangle
    */
    Rectangle(Point center, unsigned int width, unsigned int height)
    {
        // Calculate the corners of the rectangle
        this->x = Point(center.X() - (width / 2), center.Y() - (height / 2));
        this->y = Point(center.X() + (width / 2), center.Y() + (height / 2));
        // Set the other variables of the rectangle
        this->width = width;
        this->height = height;
        this->bottom = this->y.Y();
        this->left = this->x.X();
        this->right = this->y.X();
        this->top = this->x.Y();
    }

    /**
     * @brief Get the center of the rectangle
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
     * @brief Get the first corner of the rectangle
     * @return Point
    */
    Point X()
    {
        return x;
    }

    /**
     * @brief Get the second corner of the rectangle
     * @return Point
    */
    Point Y()
    {
        return y;
    }

    /**
     * @brief Get the width of the rectangle
     * @return unsigned int
    */
    unsigned int Width()
    {
        return width;
    }

    /**
     * @brief Get the height of the rectangle
     * @return unsigned int
    */
    unsigned int Height()
    {
        return height;
    }

    /**
     * @brief Get the bottom of the rectangle
     * @return unsigned int that is the Y coordinate of the bottom of the rectangle
    */
    unsigned int Bottom()
    {
        return bottom;
    }

    /**
     * @brief Get the left of the rectangle
     * @return unsigned int that is the X coordinate of the left of the rectangle
    */
    unsigned int Left()
    {
        return left;
    }

    /**
     * @brief Get the right of the rectangle
     * @return unsigned int that is the X coordinate of the right of the rectangle
    */
    unsigned int Right()
    {
        return right;
    }

    /**
     * @brief Get the top of the rectangle
     * @return unsigned int that is the Y coordinate of the top of the rectangle
    */
    unsigned int Top()
    {
        return top;
    }
};
