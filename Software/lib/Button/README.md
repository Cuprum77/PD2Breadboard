# Button Library
This library provides a simple interface to the buttons on the USB-PD board.
This adds a few features:
- Debouncing
- Button press detection in varying predefined lengths
- Button release detection

## Usage
To use the library, simply include the header file in your code:
```cpp
#include "Button.hpp"
```

### Initialization
To initialize the Button library, simply create a new Button object where you provide the pin that the button is connected to.
```cpp
// Create a new Button object that is connected to pin 0
Button button(0);
```

### Usage
To use the button library, you have to call the `update` function in your main loop. You should not use any form of delay in the loop!
#### Button pressed
To check if the button is pressed, simply call the `isPressed` function.
```cpp
// Check if the button is pressed
bool pressed = button.isPressed();
```
#### Button released
To check if the button is released, simply call the `isReleased` function.
```cpp
// Check if the button is released
bool released = button.isReleased();
```
#### Button clicked
To check if the button is clicked, simply call the `isClicked` function.
```cpp
// Check if the button is clicked
bool clicked = button.isClicked();
```
#### Button held
To check if the button is held, simply call the `isHeld` function.
```cpp
// Check if the button is held
bool held = button.isHeld();
```
#### Button long pressed
To check if the button is long pressed, simply call the `isLongPressed` function.
```cpp
// Check if the button is long pressed
bool longPressed = button.isLongPressed();
```

## Advanced initialization
To better control the button parameters, you can provide additional parameters to the Button object.
Note that the parameters are in the RP2040 defaults which are microseconds!
```cpp
// Create a new Button object that is connected to pin 0
// The button should use the internal pullup resistor
// The button is debounced with a 10ms delay
// The button is considered pressed if it is held for 500ms
// The button is considered held if it is held for 1000ms
// The button is considered long pressed if it is held for 2000ms
// The button signal is inverted, as in it is pulled low when pressed
Button button(0, true, 10000, 500000, 1000000, 2000000, true);
```

