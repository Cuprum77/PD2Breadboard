#pragma once


/*
 *  NOTE
 *  
 *  All registers are 8 bit wide, however the datasheet shows them in big endian formatting
 *  This means that the first bit is the MSB, and the last bit is the LSB
 *  
 *  However the microcontroller is little endian, so the first bit is the LSB, and the last bit is the MSB!
 */

/*
 *  Configuration Register
 *  
 *  15    | RST
 *  14    | Reserved
 *  13    | BRNG
 *  12:11 | PG [1:0]
 *  10:7  | BADC [3:0]
 *  6:3   | SADC [3:0]
 *  2:0   | MODE [2:0]
 */
struct Configuration
{
    unsigned short MODE   : 3;
    unsigned short SADC   : 4;
    unsigned short BADC   : 4;
    unsigned short PG     : 2;
    unsigned short BRNG   : 1;
    unsigned short        : 1;
    unsigned short RST    : 1;

    Configuration()
    {
        MODE = 0;
        SADC = 0;
        BADC = 0;
        PG = 0;
        BRNG = 0;
        RST = 0;
    }

    Configuration(unsigned short value)
    {
        MODE = value & 0x7;
        SADC = (value >> 3) & 0xf;
        BADC = (value >> 7) & 0xf;
        PG = (value >> 11) & 0x3;
        BRNG = (value >> 13) & 0x1;
        RST = (value >> 15) & 0x1;
    }

    unsigned short get()
    {
        return MODE | (SADC << 3) | 
        (BADC << 7) | (PG << 11) | 
        (BRNG << 13) | (RST << 15);
    }
};

/*
 *  Bus Voltage Register
 *
 *  15:3  | Bus Voltage [12:0]
 *  2     | Reserved
 *  1     | CNVR
 *  0     | OVF
 */
struct BusVoltage
{
    unsigned short OVF        : 1;
    unsigned short CNVR       : 1;
    unsigned short            : 1;
    unsigned short busVoltage : 13;

    BusVoltage()
    {
        OVF = 0;
        CNVR = 0;
        busVoltage = 0;
    }

    BusVoltage(unsigned short value)
    {
        OVF = value & 0x1;
        CNVR = (value >> 1) & 0x1;
        busVoltage = (value >> 3) & 0x1fff;
    }

    unsigned short get()
    {
        return OVF | (CNVR << 1) | (busVoltage << 3);
    }
};

/**
 * @brief the data structure for the INA219
 * @param configuration
 * @param shuntVoltage
 * @param busVoltage
 * @param power
 * @param current
 * @param calibration
*/
struct INA219_Data
{
    Configuration   configuration;
    unsigned short  shuntVoltage;
    BusVoltage      busVoltage;
    unsigned short  power;
    unsigned short  current;
    unsigned short  calibration;
};