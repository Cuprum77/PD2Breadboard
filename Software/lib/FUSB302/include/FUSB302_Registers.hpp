#pragma once

#define FUSB302_DEVICE_ID_ADDR  0x01
#define FUSB302_SWITCHES0_ADDR  0x02
#define FUSB302_SWITCHES1_ADDR  0x03
#define FUSB302_MEASURE_ADDR    0x04
#define FUSB302_SLICE_ADDR      0x05
#define FUSB302_CONTROL0_ADDR   0x06
#define FUSB302_CONTROL1_ADDR   0x07
#define FUSB302_CONTROL2_ADDR   0x08
#define FUSB302_CONTROL3_ADDR   0x09
#define FUSB302_MASK_ADDR       0x0A
#define FUSB302_POWER_ADDR      0x0B
#define FUSB302_RESET_ADDR      0x0C
#define FUSB302_OCPREG_ADDR     0x0D
#define FUSB302_MASKA_ADDR      0x0E
#define FUSB302_MASKB_ADDR      0x0F
#define FUSB302_CONTROL4_ADDR   0x10
#define FUSB302_STATUS0A_ADDR   0x3C
#define FUSB302_STATUS1A_ADDR   0x3D
#define FUSB302_INTERRUPTA_ADDR 0x3E
#define FUSB302_INTERRUPTB_ADDR 0x3F
#define FUSB302_STATUS0_ADDR    0x40
#define FUSB302_STATUS1_ADDR    0x41
#define FUSB302_INTERRUPT_ADDR  0x42
#define FUSB302_FIFO_ADDR       0x43


struct FUSB302_Register
{
    unsigned char DeviceID      = FUSB302_DEVICE_ID_ADDR;
    unsigned char Switches0     = FUSB302_SWITCHES0_ADDR;
    unsigned char Switches1     = FUSB302_SWITCHES1_ADDR;
    unsigned char Measure       = FUSB302_MEASURE_ADDR;
    unsigned char Slice         = FUSB302_SLICE_ADDR;
    unsigned char Control0      = FUSB302_CONTROL0_ADDR;
    unsigned char Control1      = FUSB302_CONTROL1_ADDR;
    unsigned char Control2      = FUSB302_CONTROL2_ADDR;
    unsigned char Control3      = FUSB302_CONTROL3_ADDR;
    unsigned char Mask          = FUSB302_MASK_ADDR;
    unsigned char Power         = FUSB302_POWER_ADDR;
    unsigned char Reset         = FUSB302_RESET_ADDR;
    unsigned char OCPReg        = FUSB302_OCPREG_ADDR;
    unsigned char MaskA         = FUSB302_MASKA_ADDR;
    unsigned char MaskB         = FUSB302_MASKB_ADDR;
    unsigned char Control4      = FUSB302_CONTROL4_ADDR;
    unsigned char Status0A      = FUSB302_STATUS0A_ADDR;
    unsigned char Status1A      = FUSB302_STATUS1A_ADDR;
    unsigned char InterruptA    = FUSB302_INTERRUPTA_ADDR;
    unsigned char InterruptB    = FUSB302_INTERRUPTB_ADDR;
    unsigned char Status0       = FUSB302_STATUS0_ADDR;
    unsigned char Status1       = FUSB302_STATUS1_ADDR;
    unsigned char Interrupt     = FUSB302_INTERRUPT_ADDR;
    unsigned char FIFO          = FUSB302_FIFO_ADDR;
};