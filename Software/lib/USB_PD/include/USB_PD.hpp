#pragma once

#include "FUSB302.hpp"

class USB_PD : public FUSB302
{
public:
    USB_PD(unsigned char address, i2c_inst_t* wire) : FUSB302(address, wire) {};
private:
};