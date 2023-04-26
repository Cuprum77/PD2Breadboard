#pragma once

#include "INA219_Data.hpp"
#include "INA219_Registers.hpp"
#include "INA219_Enums.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#define SHUNT_RESISTOR          0.001f      // 1mOhm
#define CURRENT_RESOLUTION      0.001f      // 1mA/bit
#define SHUNT_VOLTAGE_LSB_VALUE 0.00001f    // 10uV
#define BUS_VOLTAGE_LSB_VALUE   0.004f      // 4mV

#define ERROR_OK                "No errors!"
#define ERROR_CONFIG            "Configuration register error!"
#define ERROR_SHUNT_VOLTAGE     "Shunt voltage error!"
#define ERROR_BUS_VOLTAGE       "Bus voltage error!"
#define ERROR_CURRENT           "Current error!"
#define ERROR_POWER             "Power error!"
#define ERROR_CALIBRATION       "Calibration error!"

class INA219
{
public:
    INA219(unsigned int address, i2c_inst_t* i2c);
    void getData(bool all = false);
    void setData();

    void reset();
    void setBusVoltageRange(INA219_BusVoltageRange range);
    void setGain(INA219_Gain gain);
    void setBusADCResolution(INA219_ADCResolution resolution);
    void setShuntADCResolution(INA219_ADCResolution resolution);
    void setMode(INA219_Mode mode);
    INA219_BusVoltageRange getBusVoltageRange();
    INA219_Gain getGain();
    INA219_ADCResolution getBusADCResolution();
    INA219_ADCResolution getShuntADCResolution();
    INA219_Mode getMode();

    unsigned short getShuntVoltageRaw();
    unsigned short getBusVoltageRaw();
    unsigned short getCurrentRaw();
    unsigned short getPowerRaw();

    double getShuntVoltage();
    double getVoltage();
    double getCurrent();
    double getPower();

    unsigned short getCalibration();
    void setCalibration(unsigned short cal);
    void setCalibration();

    bool verifyConnection();
    int selfTest();
    const char* selfTestToString(int selfTestResult);

private:
    unsigned int device_address;
    INA219_Data data;
    i2c_inst_t* i2c;
    char errorBuffer[150];
    
    unsigned int countSetBits(unsigned int n);
    unsigned short readWord(unsigned char register_address);
    void writeWord(unsigned char register_address, unsigned short data);
};