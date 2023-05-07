#pragma once

// here we use unsigned ints as that would correspond to a word
// this allows speed optimization as we can use the word size 
// of a modern microcontroller

typedef enum : unsigned int
{
    // Neither
    None,
    // CC1 only
    CC1,
    // CC2 only
    CC2,
    // CC1 and CC2
    Both_CC
} FUSB302_CC;

typedef enum : unsigned int
{
    Source,
    Sink
} FUSB302_Role;

typedef enum : unsigned int
{
    Revision1_0,
    Revision2_0
} FUSB302_Revision;

typedef enum : unsigned int
{
    // No hysterisis
    NoHysterisis,
    // 85mV hysterisis
    Hysterisis85mV,
    // 170mV hysterisis
    Hysterisis170mV,
    // 255mV hysterisis
    Hysterisis255mV
} FUSB302_Hysterisis;

typedef enum : unsigned int
{
    // 0A of current
    NoCurrent,
    // 0.5A of current
    DefaultUSBPower,
    // 1.5A of current
    MediumCurrentMode,
    // 3A of current
    HighCurrentMode
} FUSB302_Current;

typedef enum : unsigned int
{
    // Ignore both SOP1 and SOP2
    Ignore,
    // Enable SOP1 and ignore SOP2
    SOP1,
    // Enable SOP2 and ignore SOP1
    SOP2,
    // Enable both SOP1 and SOP2
    Both_SOP
} FUSB302_SOP;

typedef enum : unsigned int
{
    // Don't go into DISABLE state after one cycle of toggle
    NoDisable,
    // Wait between toggle cycles for 40ms
    Wait40ms,
    // Wait between toggle cycles for 80ms
    Wait80ms,
    // Wait between toggle cycles for 160ms
    Wait160ms
} FUSB302_TOGSAVEPWR;

typedef enum : unsigned int
{
    // Do not mask
    NoMask,
    // Mask interrupt bit
    MaskBit
} FUSB302_Mask;

typedef enum : unsigned int
{
    // Do not use
    NoUse,
    // Enable DRP polling functionalify if Toggle = 1
    EnableDRP,
    // Enable SNK polling functionalify if Toggle = 1
    EnableSNK,
    // Enable SRC polling functionalify if Toggle = 1
    EnableSRC
} FUSB302_Mode;

typedef enum : unsigned int
{
    // OCP Range between 10-80 mA (max_range = 80mA)
    Range10_80mA,
    // OCP Range between 100-800 mA (max_range = 800mA)
    Range100_800mA
} FUSB302_OCP_Range;

typedef enum : unsigned int
{
    // Max range / 8
    Range1_8,
    // 2 * Max range / 8
    Range2_8,
    // 3 * Max range / 8
    Range3_8,
    // 4 * Max range / 8
    Range4_8,
    // 5 * Max range / 8
    Range5_8,
    // 6 * Max range / 8
    Range6_8,
    // 7 * Max range / 8
    Range7_8,
    // Max range
    RangeMax
} FUSB302_OCP_Current;

typedef enum : unsigned int
{
    // Toggle logic running
    ToggleRunning = 0x0,
    // Toggle functionality has settled to SRCon CC1
    STOP_SRC1 = 0x1,
    // Toggle functionality has settled to SRCon CC2
    STOP_SRC2 = 0x2,
    // Toggle functionality has settled to SNKon CC1
    STOP_SNK1 = 0x5,
    // Toggle functionality has settled to SNKon CC2
    STOP_SNK2 = 0x6,
    // Toggle functionality has detected Audio Accessory
    STOP_ACC = 0x7,
} FUSB302_ToggleFunctionality;

typedef enum : unsigned int
{
    // <200mV
    BC_LVL_0_200mV,
    // >200mV - <660mV
    BC_LVL_200_660mV,
    // >660mV - <1.23V
    BC_LVL_660_1230mV,
    // >1.23V
    BC_LVL_1230mV_MAX
} FUSB302_BC_LVL;
