#pragma once

#include "FUSB302.hpp"
#include "PD_Errors.hpp"
#include "PD_Enums.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

class USB_PD : public FUSB302
{
public:
    USB_PD(unsigned char address, i2c_inst_t* wire, unsigned int pin);
    void init();
    void update();
    unsigned int getConnection();
    const char* typeToString(FUSB302_MessageType type);
    void reset();
    void enablePPS();
    bool pdCapable();
    bool ppsCapable();
    USB_PD_Voltages supportedVoltages();
    bool verifyConnection();
    int selfTest();
    const char* selfTestToString(int selfTestResult);
    void printRegisters();
    int deviceID();
    int productID();
    int revisionID();
    unsigned int voltage(unsigned int voltage);
    unsigned int current(unsigned int current);
private:
    unsigned int interruptPin;
    bool vbusOK = false;
    bool pdEnabled = false;
    bool ppsEnabled = false;
    unsigned int _voltage = 5;
    unsigned int _current = 1;
    char errorBuffer[200];
    USB_PD_Voltages supportedVoltage = USB_PD_Voltages::USB_PD_VOLTAGE_5V;

    void interruptHandler();
};