#pragma once

#define FUSB302_ERROR_OK    "No errors!"
#define FUSB302_ERROR_PD    "No PD detected!"
#define FUSB302_ERROR_PPS   "No PPS detected!"

// #define FUSB302_ERROR_


typedef enum : unsigned int
{
    FUSB302_SELF_TEST_OK = 0x0,
    FUSB302_SELF_TEST_PD_ERROR = 0x1,
    FUSB302_SELF_TEST_PPS_ERROR = 0x2,
} FUSB302_Self_Test;


typedef enum : unsigned int
{
    USB_PD_VOLTAGE_5V = 0x1,
    USB_PD_VOLTAGE_9V = 0x2,
    USB_PD_VOLTAGE_12V = 0x4,
    USB_PD_VOLTAGE_15V = 0x8,
    USB_PD_VOLTAGE_20V = 0x10,
} USB_PD_Voltages;