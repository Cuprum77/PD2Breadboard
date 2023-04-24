#pragma once

#include "FUSB302_Registers.hpp"
#include "FUSB302_Data.hpp"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

class FUSB302
{
public:
    FUSB302(unsigned char address, i2c_inst_t* wire);

    void enablePPS();

    void setCurrent(unsigned int current);
    void setVoltage(unsigned int voltage);
    
    void getCurrent();
    void setCurrent();
private:
    unsigned char device_address;
    i2c_inst_t* i2c;
    FUSB302_Data data;

    unsigned char readWord(unsigned char register_address);
    void writeWord(unsigned char register_address, unsigned char data);

    void getData();
    void setData();

    bool verifyConnection();
};