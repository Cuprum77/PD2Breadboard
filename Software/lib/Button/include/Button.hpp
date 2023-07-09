#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#define DEBOUNCE_TIME       20000   // 20ms
#define PRESS_TIME          30000   // 30ms
#define HOLD_TIME           750000  // 750ms
#define LONG_PRESS_TIME     3000000 // 3s

// should only take a single byte
typedef enum
{
    BUTTON_RELEASED = 0x00,
    BUTTON_CLICKED = 0x01,
    BUTTON_HELD = 0x02,
    BUTTON_LONG_PRESSED = 0x6,
} buttonState_t;

class Button
{
public:
    Button(unsigned int pin);
    Button(unsigned int pin, bool inverted);
    Button(unsigned int pin, bool pullup, bool inverted);
    Button(unsigned int pin, bool pullup, unsigned int debounceTime, unsigned int pressTime, 
        unsigned int holdTime, unsigned int longPressTime, bool inverted);

    bool isPressed();
    bool isReleased();

    bool isHeld();
    bool isClicked();
    bool isLongPressed();

    void update();
private:
    unsigned int pin = 0;
    unsigned int inverted = true;

    unsigned char buttonState = 0;
    buttonState_t currentState = BUTTON_RELEASED;
    unsigned int lastDebounceTime = 0;
    unsigned int lastReading = 0;	

    unsigned int debounceTime = DEBOUNCE_TIME;
    unsigned int pressTime = PRESS_TIME;
    unsigned int holdTime = HOLD_TIME;
    unsigned int longPressTime = LONG_PRESS_TIME;
    buttonState_t state;
};