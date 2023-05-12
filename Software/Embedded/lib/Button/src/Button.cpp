#include "Button.hpp"

/**
 * @brief Construct a new Button:: Button object
 * @param pin the pin the button is connected to
*/
Button::Button(unsigned int pin)
{
    // enable the pin and set to input
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);

    this->pin = pin;
}

/**
 * @brief Construct a new Button:: Button object
 * @param pin the pin the button is connected to
 * @param inverted whether the button is inverted or not
*/
Button::Button(unsigned int pin, bool inverted)
{
    // enable the pin and set to input
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);

    this->pin = pin;
    this->inverted = inverted; 
}

/**
 * @brief Construct a new Button:: Button object
 * @param pin the pin the button is connected to
 * @param mode which mode the pin should be set to
 * @param inverted whether the button is inverted or not
*/
Button::Button(unsigned int pin, bool pullup, bool inverted)
{
    // enable the pin and set to input
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);

    // set the pin to pull up if needed
    if (pullup)
        gpio_pull_up(pin);

    this->pin = pin;
    this->inverted = inverted; 
}

/**
 * @brief Construct a new Button:: Button object
 * @param pin the pin the button is connected to
 * @param mode which mode the pin should be set to
 * @param debounceTime the time the button should be debounced in microseconds
 * @param pressTime the time the button should be pressed in microseconds
 * @param holdTime the time the button should be held in microseconds
 * @param longPressTime the time the button should be long pressed in microseconds
 * @param inverted whether the button is inverted or not
*/
Button::Button(unsigned int pin, bool pullup, unsigned int debounceTime, unsigned int pressTime,
    unsigned int holdTime, unsigned int longPressTime, bool inverted)
{
    // enable the pin and set to input
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);

    // set the pin to pull up if needed
    if (pullup)
        gpio_pull_up(pin);

    this->pin = pin;
    this->inverted = inverted; 
    this->debounceTime = debounceTime;
    this->pressTime = pressTime;
    this->holdTime = holdTime;
    this->longPressTime = longPressTime;
}

/**
 * @brief Check if the button is pressed
 * @return true if the button is pressed
 * @return false if the button is not pressed
*/
bool Button::isPressed()
{
    return this->buttonState;
}

/**
 * @brief Check if the button is released
 * @return true if the button is released
 * @return false if the button is not released
*/
bool Button::isReleased()
{
    return !this->buttonState;
}

/**
 * @brief Check if the button is clicked
 * @note Read the state of the button will clear the state
 * @return boolean true if the button is clicked
*/
bool Button::isClicked()
{
    // get the state of the button
    bool state = this->state & 0x01;
    // after we read it, clear the original state only if the button was held
    if (state)
        this->state = BUTTON_RELEASED;
    return state;
}

/**
 * @brief Check if the button is held
 * @note Read the state of the button will clear the state
 * @return boolean true if the button is held
*/
bool Button::isHeld()
{
    // get the state of the button
    bool state = (this->state >> 1) & 0x01;
    // after we read it, clear the original state only if the button was held
    if (state)
        this->state = BUTTON_RELEASED;
    return state;
}

/**
 * @brief Check if the button is long pressed
 * @note Read the state of the button will clear the state
 * @return boolean true if the button is long pressed
*/
bool Button::isLongPressed()
{
    // get the state of the button
    bool state = (this->state >> 2) & 0x01;
    // after we read it, clear the original state only if the button was held
    if (state)
        this->state = BUTTON_RELEASED;
    return state;
}

/**
 * @brief Update the button state
*/
void Button::update()
{
    // read the pin and invert if necessary through an xor operation
    unsigned char reading = gpio_get(this->pin) ^ this->inverted;

    // update the last reading if the reading has changed
    if (reading != this->lastReading)
        this->lastDebounceTime = time_us_32();

    // debounce the button
    if((time_us_32() - this->lastDebounceTime) > this->debounceTime)
    {
        if(reading != this->buttonState)
        {
            this->buttonState = reading;
            this->state = this->currentState;
        }
    }

    // if the button is currently being pressed
    if(this->buttonState == 0x1)
    {
        // if the button is being held
        if((time_us_32() - this->lastDebounceTime) > this->longPressTime)
            this->currentState = BUTTON_LONG_PRESSED;
        else if((time_us_32() - this->lastDebounceTime) > this->holdTime)
            this->currentState = BUTTON_HELD;
        else if((time_us_32() - this->lastDebounceTime) > this->pressTime)
            this->currentState = BUTTON_CLICKED;
    }
    else
        this->currentState = BUTTON_RELEASED;

    this->lastReading = reading;
}