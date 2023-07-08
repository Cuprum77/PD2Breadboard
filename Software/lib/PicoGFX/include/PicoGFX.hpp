#pragma once

#include "HardwareSPI.hpp"
#include "Display.hpp"
#include "Graphics.hpp"
#include "Print.hpp"
#include "Gradients.hpp"

class PicoGFX {
public:
    PicoGFX(Display* display, Print* print, Graphics* graphics, Gradients* gradients) :
        display(display),
        print(print),
        graphics(graphics),
        gradients(gradients) {
        fillLookupTables();
    }

    Display& getDisplay();
    Print& getPrint();
    Graphics& getGraphics();
    Gradients& getGradients();

private:
    Display* display;
    Print* print;
    Graphics* graphics;
    Gradients* gradients;
};