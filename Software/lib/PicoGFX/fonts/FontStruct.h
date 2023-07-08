#pragma once

// Estimated total memory usage: 62948 bytes

// Struct for storing the location of the bitmap in memory
// Offset is the number of pixels from the upper left corner
typedef struct {
    unsigned int pointer;
    unsigned int length;
    unsigned char width;
    unsigned char height;
    signed char xOffset;
    signed char yOffset;
} FontCharacter;


// Struct for storing the font data
typedef struct {
    const unsigned int *bitmap;
    const FontCharacter *characters;
    unsigned int size;
    unsigned int newLineDistance;
} FontStruct;
