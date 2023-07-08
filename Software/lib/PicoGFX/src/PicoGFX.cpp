#include "PicoGFX.hpp"

/**
 * @brief Get the Display object
 * @return Display&
*/
Display& PicoGFX::getDisplay()
{
    return *display;
}

/**
 * @brief Get the Print object
 * @return Print&
*/
Print& PicoGFX::getPrint()
{
    return *print;
}

/**
 * @brief Get the Graphics object
 * @return Graphics&
*/
Graphics& PicoGFX::getGraphics()
{
    return *graphics;
}

/**
 * @brief Get the AdvancedGraphics object
 * @return AdvancedGraphics&
*/
Gradients& PicoGFX::getGradients()
{
    return *gradients;
}