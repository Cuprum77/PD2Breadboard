#pragma once

#include "Color.h"
#include "Structs.h"
#include "Shapes.h"
#include "FontStruct.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// String behavior
#define CHARACTER_BUFFER_SIZE 128
#define TAB_SIZE 4      // how many spaces a tab is worth
#define FALSE "false"   // string representation of false
#define TRUE "true"     // string representation of true

// Typedefs for number bases
typedef enum
{
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16
} number_base_t;

class Print
{
public:
    Print(unsigned short* frameBuffer, Display_Params params);
    void setColor(Color color) { this->color = color.to16bit(); }
    Color getColor(void) { return Color(this->color); }
    void setCursor(Point point) { this->cursor = (unsigned int)(point.x) + (unsigned int)(point.y * this->width); }
    void moveCursor(Point point) { this->cursor += (unsigned int)(point.x) + (unsigned int)(point.y * this->width); }
    Point getCursor(void) { return {(unsigned int)(this->cursor % this->width), (unsigned int)(this->cursor / this->width)}; }
    void setFont(FontStruct* font);

    void write(char c, number_base_t base = number_base_t::DEC) { this->print((long)c, base); }
    void print(unsigned char c, number_base_t base = number_base_t::DEC)  { this->print((unsigned long)c, base); }
    void print(short num, number_base_t base = number_base_t::DEC)  { this->print((long)num, base); }
    void print(unsigned short num, number_base_t base = number_base_t::DEC) { this->print((unsigned long)num, base); }
    void print(int num, number_base_t base = number_base_t::DEC) { this->print((long)num, base); }
    void print(unsigned int num, number_base_t base = number_base_t::DEC) { this->print((unsigned long)num, base); }
    void print(long num, number_base_t base = number_base_t::DEC);
    void print(unsigned long num, number_base_t base = number_base_t::DEC);
    void print(double num, unsigned int precision = 2);
    void print(const char* text);
    void print(bool value);

    void println(char c, number_base_t base = number_base_t::DEC) { this->println((long)c, base); }
    void println(unsigned char c, number_base_t base = number_base_t::DEC) { this->println((unsigned long)c, base); }
    void println(int num, number_base_t base = number_base_t::DEC) { this->println((long)num, base); }
    void println(unsigned int num, number_base_t base = number_base_t::DEC) { this->println((unsigned long)num, base); }
    void println(short num, number_base_t base = number_base_t::DEC) { this->println((long)num, base); }
    void println(unsigned short num, number_base_t base = number_base_t::DEC) { this->println((unsigned long)num, base); }
    void println(long num, number_base_t base = number_base_t::DEC);
    void println(unsigned long num, number_base_t base = number_base_t::DEC);
    void println(double num, unsigned int precision = 2);
    void println(const char* text);
    void println(bool value);
    void println(void);

    unsigned int getStringLength(char num, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, base); }
    unsigned int getStringLength(unsigned char num, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, base); }
    unsigned int getStringLength(int num, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, base); }
    unsigned int getStringLength(unsigned int num, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, base); }
    unsigned int getStringLength(short num, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, base); }
    unsigned int getStringLength(unsigned short num, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, base); }
    unsigned int getStringLength(long num, number_base_t base = number_base_t::DEC);
    unsigned int getStringLength(unsigned long num, number_base_t base = number_base_t::DEC);
    unsigned int getStringLength(double num, unsigned char precision = 2);
    unsigned int getStringLength(const char* text);
    unsigned int getStringLength(bool value);
private:
    unsigned short* frameBuffer;
    unsigned int width;
    unsigned int height;
    size_t totalPixels;
    unsigned long cursor;
    unsigned short color;
    FontStruct* font;

    void drawAscii(const char c);
    void floatToString(double num, char* buffer, unsigned int precision);
    void reverse(char* str, unsigned int length);
    size_t getPixelWidth(const char* text, size_t length);
};