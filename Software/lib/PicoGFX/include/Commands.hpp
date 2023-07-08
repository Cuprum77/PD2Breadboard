#pragma once

typedef enum
{
    // NO OPERATION
    NOP = 0x00,
    // Software Reset
    SWRESET = 0x01,
    // Read Display ID
    RDDID = 0x04,
    // Read Display Status
    RDDST = 0x09,
    // Read Display Power Mode
    RDDPM = 0x0a,
    // Read Display MADCTL
    RDDMADCTL = 0x0b,
    // Read Display Pixel Format
    RDDCOLMOD = 0x0c,
    // Read Display Image Mode
    RDDIM = 0x0d,
    // Read Display Signal Mode
    RDDSM = 0x0e,
    // Read Display Self-Diagnostic Result
    RDDSDR = 0x0f,
    // Sleep In
    SLPIN = 0x10,
    // Sleep Out
    SLPOUT = 0x11,
    // Partial Display Mode On
    PTLON = 0x12,
    // Normal Display Mode Off
    NORON = 0x13,
    // Display Inversion Off
    INVOFF = 0x20,
    // Display Inversion On
    INVON = 0x21,
    // Gamma Set
    GAMSET = 0x26,
    // Display off
    DISPOFF = 0x28,
    // Display on
    DISPON = 0x29,
    // Column Address Set
    CASET = 0x2a,
    // Row Address Set
    RASET = 0x2b,
    // Memory Write
    RAMWR = 0x2c,
    // Memory Read
    RAMRD = 0x2e,
    // Partial Area
    PTLAR = 0x30,
    // Vertical Scrolling Definition
    VSCRDEF = 0x33,
    // Tearing Effect Line Off
    TEOFF = 0x34,
    // Tearing Effect Line On
    TEON = 0x35,
    // Memory Access Control
    MADCTL = 0x36,
    // Vertical Scrolling Start Address of RAM
    VSCSAD = 0x37,
    // Idle Mode Off
    IDMOFF = 0x38,
    // Idle Mode On
    IDMON = 0x39,
    // Interface Pixel Format
    COLMOD = 0x3a,
    // Write Memory Continue
    WRMEMC = 0x3c,
    // Read Memory Continue
    RDMEMC = 0x3e,
    // Set Tear Scanline
    STE = 0x44,
    // Get Scanline
    GSCAN = 0x45,
    // Write Display Brightness
    WRDISBV = 0x51,
    // Read Display Brightness
    RDDISBV = 0x52,
    // Write CTRL Display
    WRCTRLD = 0x53,
    // Read CTRL Display
    RDCTRLD = 0x54,
    // Write Content Adaptive Brightness Control and Color Enhancement
    WRCACE = 0x55,
    // Read Content Adaptive Brightness Control and Color Enhancement
    RDCABC = 0x56,
    // Write CABC Minimum Brightness
    WRCABCMB = 0x5e,
    // Read CABC Minimum Brightness
    RDCABCMB = 0x5f,
    // Read ID1
    RDID1 = 0xda,
    // Read ID2
    RDID2 = 0xdb,
    // Read ID3
    RDID3 = 0xdc,
    // Read ID4
    RDID4 = 0xdd,
    // RAM Control
    RAMCTRL = 0xb0,
    // RGB Interface Signal Control
    RGBCTRL = 0xb1,
    // Porch Setting
    PORCTRL = 0xb2,
    // Frame Rate Control 1
    FRMCTR1 = 0xb3,
    // Partial Control
    PCTLR = 0xb5,
    // Gate Control
    GCTRL = 0xb7,
    // Gate On Timing Adjustment
    GONCTRL = 0xb8,
    // Digital Gamma Enable
    DGMEN = 0xba,
    // VCOM Setting
    VCOMS = 0xbb,
    // Power Saving Mode
    POWSAVE = 0xbc,
    // Display off power save
    DLPOFFSAVE = 0xbd,
    // LCM Control
    LCMCTRL = 0xc0,
    // ID Code Setting
    IDSET = 0xc1,
    // VDV and VRH Command Enable
    VDVVRHEN = 0xc2,
    // VRH Set
    VRHS = 0xc3,
    // VDV Set
    VDVS = 0xc4,
    // VCOMH Offset Set
    VCMOFSET = 0xc5,
    // Frame Rate Control in Normal Mode
    FRCTRL2 = 0xc6,
    // CABC Control
    CABCCTRL = 0xc7,
    // Register Value Selection 1
    REGSEL1 = 0xc8,
    // Register Value Selection 2
    REGSEL2 = 0xca,
    // PWM Frequency Selection
    PWMFRSEL = 0xcc,
    // Power Control 1
    PWCTRL1 = 0xd0,
    // Enable VAP/VAN Signal Output
    VAPVANEN = 0xd2,
    // Command 2 Enable
    CMD2EN = 0xdf,
    // Positive Voltage Gamma Control
    PVGAMCTRL = 0xe0,
    // Negative Voltage Gamma Control
    NVGAMCTRL = 0xe1,
    // Digital Gamma Look-Up Table for Red
    DGMLUTR = 0xe2,
    // Digital Gamma Look-Up Table for Blue
    DGMLUTB = 0xe3,
    // Gate Control
    GATECTRL = 0xe4,
    // SPI2 Enable
    SPI2EN = 0xe7,
    // Power Control 2
    PWCTRL2 = 0xe8,
    // Equalize Time Control
    EQTIME = 0xe9,
    // Program Mode Control
    PROMCTRL = 0xec,
    // Program Mode Enable
    PROMEN = 0xfa,
    // NVM Setting
    NVMSET = 0xfc,
    // Program Action
    PROMACT = 0xfe,
} Display_Commands;

typedef enum
{
    // Page Address Order
    MY = 0x80,
    // Column Address Order
    MX = 0x40,
    // Page/Column Order
    MV = 0x20,
    // Line Address Order
    ML = 0x10,
    // BGR Order
    BGR = 0x08,
    // Display Data Latch Data Order
    MH = 0x04,
    // RGB Order
    RGB = 0x00,
} Display_MADCTL;