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
 *  7:4 | Version ID  [3:0]
 *  3:2 | Product ID  [1:0]
 *  1:0 | Revision ID [1:0]
 */
struct DeviceID
{
    unsigned char RevisionID : 2;
    unsigned char ProductID  : 2;
    unsigned char VersionID  : 4;

    DeviceID()
    {
        RevisionID = 0;
        ProductID = 0;
        VersionID = 0;
    }

    DeviceID(unsigned char value)
    {
        RevisionID = value & 0x3;
        ProductID = (value >> 2) & 0x3;
        VersionID = (value >> 4) & 0xf;
    }

    unsigned char get()
    {
        return RevisionID | (ProductID << 2) | (VersionID << 4);
    }
};

/*
 *  7   | PU_EN2
 *  6   | PU_EN1
 *  5   | VCONN_CC2
 *  4   | VCONN_CC1
 *  3   | MEAS_CC2
 *  2   | MEAS_CC1
 *  1   | PDWN2
 *  0   | PDWN1
 */
struct Switches0
{
    unsigned char PDWN1      : 1;
    unsigned char PDWN2      : 1;
    unsigned char MEAS_CC1   : 1;
    unsigned char MEAS_CC2   : 1;
    unsigned char VCONN_CC1  : 1;
    unsigned char VCONN_CC2  : 1;
    unsigned char PU_EN1     : 1;
    unsigned char PU_EN2     : 1;

    Switches0()
    {
        PDWN1 = 0;
        PDWN2 = 0;
        MEAS_CC1 = 0;
        MEAS_CC2 = 0;
        VCONN_CC1 = 0;
        VCONN_CC2 = 0;
        PU_EN1 = 0;
        PU_EN2 = 0;
    }

    Switches0(unsigned char value)
    {
        PDWN1 = value & 0x1;
        PDWN2 = (value >> 1) & 0x1;
        MEAS_CC1 = (value >> 2) & 0x1;
        MEAS_CC2 = (value >> 3) & 0x1;
        VCONN_CC1 = (value >> 4) & 0x1;
        VCONN_CC2 = (value >> 5) & 0x1;
        PU_EN1 = (value >> 6) & 0x1;
        PU_EN2 = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return PDWN1 | (PDWN2 << 1) | 
        (MEAS_CC1 << 2) | (MEAS_CC2 << 3) | 
        (VCONN_CC1 << 4) | (VCONN_CC2 << 5) | 
        (PU_EN1 << 6) | (PU_EN2 << 7);
    }
};

/*
 *  7   | POWER_ROLE
 *  6   | SPEC_REV1
 *  5   | SPEC_REV0
 *  4   | DATA_ROLE
 *  3   | Reserved
 *  2   | AUTO_CRC
 *  1   | TXCC2
 *  0   | TXCC1
 */
struct Switches1
{
    unsigned char TXCC1      : 1;
    unsigned char TXCC2      : 1;
    unsigned char AUTO_CRC   : 1;
    unsigned char            : 1;
    unsigned char DATA_ROLE  : 1;
    unsigned char SPEC       : 2;
    unsigned char POWER_ROLE : 1;

    Switches1()
    {
        TXCC1 = 0;
        TXCC2 = 0;
        AUTO_CRC = 0;
        DATA_ROLE = 0;
        SPEC = 0;
        POWER_ROLE = 0;
    }

    Switches1(unsigned char value)
    {
        TXCC1 = value & 0x1;
        TXCC2 = (value >> 1) & 0x1;
        AUTO_CRC = (value >> 2) & 0x1;
        DATA_ROLE = (value >> 4) & 0x1;
        SPEC = (value >> 5) & 0x3;
        POWER_ROLE = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return TXCC1 | (TXCC2 << 1) | 
        (AUTO_CRC << 2) | (DATA_ROLE << 4) | 
        (SPEC << 5) | (POWER_ROLE << 7);
    }
};

/*
 *  7   | Reserved
 *  6   | MEAS_VBUS
 *  5:0 | MDAC [5:0]
 */
struct Measure
{
    unsigned char MDAC       : 6;
    unsigned char MEAS_VBUS  : 1;
    unsigned char            : 1;

    Measure()
    {
        MDAC = 0;
        MEAS_VBUS = 0;
    }

    Measure(unsigned char value)
    {
        MDAC = value & 0x3f;
        MEAS_VBUS = (value >> 6) & 0x1;
    }

    unsigned char get()
    {
        return MDAC | (MEAS_VBUS << 6);
    }
};

/*
 *  7:6 | SDAC_HYS [1:0]
 *  5:0 | SDAC     [5:0]
 */

struct Slice
{
    unsigned char SDAC       : 6;
    unsigned char SDAC_HYS   : 2;

    Slice()
    {
        SDAC = 0;
        SDAC_HYS = 0;
    }

    Slice(unsigned char value)
    {
        SDAC = value & 0x3f;
        SDAC_HYS = (value >> 6) & 0x3;
    }

    unsigned char get()
    {
        return SDAC | (SDAC_HYS << 6);
    }
};

/*
 *  7   | Reserved
 *  6   | TX_FLUSH
 *  5   | INT_MASK
 *  4   | Reserved
 *  3:2 | HOST_CUR [1:0]
 *  1   | AUTO_PRE
 *  0   | TX_START
 */
struct Control0
{
    unsigned char TX_START   : 1;
    unsigned char AUTO_PRE   : 1;
    unsigned char HOST_CUR   : 2;
    unsigned char            : 1;
    unsigned char INT_MASK   : 1;
    unsigned char TX_FLUSH   : 1;
    unsigned char            : 1;

    Control0()
    {
        TX_START = 0;
        AUTO_PRE = 0;
        HOST_CUR = 0;
        INT_MASK = 0;
        TX_FLUSH = 0;
    }

    Control0(unsigned char value)
    {
        TX_START = value & 0x1;
        AUTO_PRE = (value >> 1) & 0x1;
        HOST_CUR = (value >> 2) & 0x3;
        INT_MASK = (value >> 5) & 0x1;
        TX_FLUSH = (value >> 6) & 0x1;
    }

    // Will clear the Write/Clear bits!
    unsigned char get()
    {
        unsigned char data = TX_START | (AUTO_PRE << 1) | 
        (HOST_CUR << 2) | (INT_MASK << 5) | 
        (TX_FLUSH << 6);
        // Clear the Write/Clear bits
        TX_START = 0;
        TX_FLUSH = 0;
        return data;
    }
};

/*
 *  7   | Reserved
 *  6   | ENSOP2DB
 *  5   | ENSOP1DB
 *  4   | BIST_MODE2
 *  3   | Reserved
 *  2   | RX_FLUSH
 *  1   | ENSOP2
 *  0   | ENSOP1
 */
struct Control1
{
    unsigned char ENSOP1     : 1;
    unsigned char ENSOP2     : 1;
    unsigned char RX_FLUSH   : 1;
    unsigned char            : 1;
    unsigned char BIST_MODE2 : 1;
    unsigned char ENSOP1DB   : 1;
    unsigned char ENSOP2DB   : 1;
    unsigned char            : 1;

    Control1()
    {
        ENSOP1 = 0;
        ENSOP2 = 0;
        RX_FLUSH = 0;
        BIST_MODE2 = 0;
        ENSOP1DB = 0;
        ENSOP2DB = 0;
    }

    Control1(unsigned char value)
    {
        ENSOP1 = value & 0x1;
        ENSOP2 = (value >> 1) & 0x1;
        RX_FLUSH = (value >> 2) & 0x1;
        BIST_MODE2 = (value >> 4) & 0x1;
        ENSOP1DB = (value >> 5) & 0x1;
        ENSOP2DB = (value >> 6) & 0x1;
    }

    // Will clear the Write/Clear bits!
    unsigned char get()
    {
        unsigned char data = ENSOP1 | (ENSOP2 << 1) | 
        (RX_FLUSH << 2) | (BIST_MODE2 << 4) | 
        (ENSOP1DB << 5) | (ENSOP2DB << 6);
        // Clear the Write/Clear bits
        RX_FLUSH = 0;
        return data;
    }
};

/*
 *  7:6 | TOG_SAVE_PWR
 *  5   | TOG_RD_ONLY
 *  4   | Reserved
 *  3   | WAKE_EN
 *  2:1 | MODE
 *  0   | TOGGLE
 */
struct Control2
{
    unsigned char TOGGLE        : 1;
    unsigned char MODE          : 2;
    unsigned char WAKE_EN       : 1;
    unsigned char               : 1;
    unsigned char TOG_RD_ONLY   : 1;
    unsigned char TOG_SAVE_PWR  : 2;

    Control2()
    {
        TOGGLE = 0;
        MODE = 0;
        WAKE_EN = 0;
        TOG_RD_ONLY = 0;
        TOG_SAVE_PWR = 0;
    }

    Control2(unsigned char value)
    {
        TOGGLE = value & 0x1;
        MODE = (value >> 1) & 0x3;
        WAKE_EN = (value >> 3) & 0x1;
        TOG_RD_ONLY = (value >> 5) & 0x1;
        TOG_SAVE_PWR = (value >> 6) & 0x3;
    }

    unsigned char get()
    {
        return TOGGLE | (MODE << 1) | 
        (WAKE_EN << 3) | (TOG_RD_ONLY << 5) | 
        (TOG_SAVE_PWR << 6);
    }
};

/*
 *  7   | Reserved
 *  6   | SEND_HARD_RESET
 *  5   | BIST_TMODE
 *  4   | AUTO_HARDRESET
 *  3   | AUTO_SOFTRESET
 *  2:1 | N_RETRIES [1:0]
 *  0   | AUTO_RETRY
 */
struct Control3
{
    unsigned char AUTO_RETRY         : 1;
    unsigned char N_RETRIES          : 2;
    unsigned char AUTO_SOFTRESET     : 1;
    unsigned char AUTO_HARDRESET     : 1;
    unsigned char BIST_TMODE         : 1;
    unsigned char SEND_HARD_RESET    : 1;
    unsigned char                    : 1;

    Control3()
    {
        AUTO_RETRY = 0;
        N_RETRIES = 0;
        AUTO_SOFTRESET = 0;
        AUTO_HARDRESET = 0;
        BIST_TMODE = 0;
        SEND_HARD_RESET = 0;
    }

    Control3(unsigned char value)
    {
        AUTO_RETRY = value & 0x1;
        N_RETRIES = (value >> 1) & 0x3;
        AUTO_SOFTRESET = (value >> 3) & 0x1;
        AUTO_HARDRESET = (value >> 4) & 0x1;
        BIST_TMODE = (value >> 5) & 0x1;
        SEND_HARD_RESET = (value >> 6) & 0x1;
    }
    
    // Will clear the Write/Clear bits!
    unsigned char get()
    {
        unsigned char data = AUTO_RETRY | (N_RETRIES << 1) | 
        (AUTO_SOFTRESET << 3) | (AUTO_HARDRESET << 4) | 
        (BIST_TMODE << 5) | (SEND_HARD_RESET << 6);
        // Clear the Write/Clear bits
        SEND_HARD_RESET = 0;
        return data;
    }
};

/*
 *  7   | M_VBUSOK
 *  6   | M_ACTIVITY
 *  5   | M_COMP_CHNG
 *  4   | M_CRC_CHK
 *  3   | M_ALERT
 *  2   | M_WAKE
 *  1   | M_COLLISION
 *  0   | M_BC_LVL
 */
struct Mask
{
    unsigned char M_BC_LVL       : 1;
    unsigned char M_COLLISION    : 1;
    unsigned char M_WAKE         : 1;
    unsigned char M_ALERT        : 1;
    unsigned char M_CRC_CHK      : 1;
    unsigned char M_COMP_CHNG    : 1;
    unsigned char M_ACTIVITY     : 1;
    unsigned char M_VBUSOK       : 1;

    Mask()
    {
        M_BC_LVL = 0;
        M_COLLISION = 0;
        M_WAKE = 0;
        M_ALERT = 0;
        M_CRC_CHK = 0;
        M_COMP_CHNG = 0;
        M_ACTIVITY = 0;
        M_VBUSOK = 0;
    }

    Mask(unsigned char value)
    {
        M_BC_LVL = value & 0x1;
        M_COLLISION = (value >> 1) & 0x1;
        M_WAKE = (value >> 2) & 0x1;
        M_ALERT = (value >> 3) & 0x1;
        M_CRC_CHK = (value >> 4) & 0x1;
        M_COMP_CHNG = (value >> 5) & 0x1;
        M_ACTIVITY = (value >> 6) & 0x1;
        M_VBUSOK = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return M_BC_LVL | (M_COLLISION << 1) | 
        (M_WAKE << 2) | (M_ALERT << 3) | 
        (M_CRC_CHK << 4) | (M_COMP_CHNG << 5) | 
        (M_ACTIVITY << 6) | (M_VBUSOK << 7);
    }
};

/*
 *  7:4 | Reserved
 *  3:0 | STATUS [3:0]
 */
struct Power
{
    unsigned char STATUS     : 4;
    unsigned char            : 4;

    Power()
    {
        STATUS = 0;
    }

    Power(unsigned char value)
    {
        STATUS = value & 0xF;
    }

    unsigned char get()
    {
        return STATUS;
    }
};

/*
 *  7:2 | Reserved [5:0]
 *  1   | PD_RESET
 *  0   | SW_RES 
 */
struct Reset
{
    unsigned char SW_RES     : 1;
    unsigned char PD_RESET   : 1;
    unsigned char            : 6;

    Reset()
    {
        SW_RES = 0;
        PD_RESET = 0;
    }

    Reset(unsigned char value)
    {
        SW_RES = value & 0x1;
        PD_RESET = (value >> 1) & 0x1;
    }

    // Will clear the Write/Clear bits!
    unsigned char get()
    {
        unsigned char data = SW_RES | (PD_RESET << 1);
        // Clear the Write/Clear bits
        PD_RESET = 0;
        SW_RES = 0;
        return data;
    }
};

/*
 *  7:4 | Reserved [3:0]
 *  3   | OCP_RANGE
 *  2:0 | OCP_CUR [2:0]
 */
struct OCPreg
{
    unsigned char OCP_CUR    : 3;
    unsigned char OCP_RANGE  : 1;
    unsigned char            : 4;

    OCPreg()
    {
        OCP_CUR = 0;
        OCP_RANGE = 0;
    }

    OCPreg(unsigned char value)
    {
        OCP_CUR = value & 0x7;
        OCP_RANGE = (value >> 3) & 0x1;
    }

    unsigned char get()
    {
        return OCP_CUR | (OCP_RANGE << 3);
    }
};

/*
 *  7   | M_OCP_TEMP
 *  6   | M_TOGDONE
 *  5   | M_SOFTFAIL
 *  4   | M_RETRYFAIL
 *  3   | M_HARDSENT
 *  2   | M_TXSENT
 *  1   | M_SOFTRST
 *  0   | M_HARDRST
 */
struct MaskA
{
    unsigned char M_HARDRST      : 1;
    unsigned char M_SOFTRST      : 1;
    unsigned char M_TXSENT       : 1;
    unsigned char M_HARDSENT     : 1;
    unsigned char M_RETRYFAIL    : 1;
    unsigned char M_SOFTFAIL     : 1;
    unsigned char M_TOGDONE      : 1;
    unsigned char M_OCP_TEMP     : 1;

    MaskA()
    {
        M_HARDRST = 0;
        M_SOFTRST = 0;
        M_TXSENT = 0;
        M_HARDSENT = 0;
        M_RETRYFAIL = 0;
        M_SOFTFAIL = 0;
        M_TOGDONE = 0;
        M_OCP_TEMP = 0;
    }

    MaskA(unsigned char value)
    {
        M_HARDRST = value & 0x1;
        M_SOFTRST = (value >> 1) & 0x1;
        M_TXSENT = (value >> 2) & 0x1;
        M_HARDSENT = (value >> 3) & 0x1;
        M_RETRYFAIL = (value >> 4) & 0x1;
        M_SOFTFAIL = (value >> 5) & 0x1;
        M_TOGDONE = (value >> 6) & 0x1;
        M_OCP_TEMP = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return M_HARDRST | (M_SOFTRST << 1) | 
        (M_TXSENT << 2) | (M_HARDSENT << 3) | 
        (M_RETRYFAIL << 4) | (M_SOFTFAIL << 5) | 
        (M_TOGDONE << 6) | (M_OCP_TEMP << 7);
    }
};

/*
 *  7:1 | Reserved [7:1]
 *  0   | M_GCRCSENT
 */
struct MaskB
{
    unsigned char M_GCRCSENT : 1;
    unsigned char            : 7;

    MaskB()
    {
        M_GCRCSENT = 0;
    }

    MaskB(unsigned char value)
    {
        M_GCRCSENT = value & 0x1;
    }

    unsigned char get()
    {
        return M_GCRCSENT;
    }
};

/*
 *  7:1 | Reserved [7:1]
 *  0   | TOG_EXIT_AUD
 */
struct Control4
{
    unsigned char TOG_EXIT_AUD   : 1;
    unsigned char                : 7;

    Control4()
    {
        TOG_EXIT_AUD = 0;
    }

    Control4(unsigned char value)
    {
        TOG_EXIT_AUD = value & 0x1;
    }

    unsigned char get()
    {
        return TOG_EXIT_AUD;
    }
};

/*
 *  7:6 | Reserved
 *  5   | SOFTFAIL
 *  4   | RETRYFAIL
 *  3:2 | POWER [1:0]
 *  1   | SOFTRST
 *  0   | HARDRST
 */
struct Status0A
{
    unsigned char HARDRST    : 1;
    unsigned char SOFTRST    : 1;
    unsigned char POWER      : 2;
    unsigned char RETRYFAIL  : 1;
    unsigned char SOFTFAIL   : 1;
    unsigned char            : 2;

    Status0A()
    {
        HARDRST = 0;
        SOFTRST = 0;
        POWER = 0;
        RETRYFAIL = 0;
        SOFTFAIL = 0;
    }

    Status0A(unsigned char value)
    {
        HARDRST = value & 0x1;
        SOFTRST = (value >> 1) & 0x1;
        POWER = (value >> 2) & 0x3;
        RETRYFAIL = (value >> 4) & 0x1;
        SOFTFAIL = (value >> 5) & 0x1;
    }

    unsigned char get()
    {
        return HARDRST | (SOFTRST << 1) | 
        (POWER << 2) | (RETRYFAIL << 4) | 
        (SOFTFAIL << 5);
    }
};

/*
 *  7:6 | Reserved
 *  5:3 | TOGSS [2:0]
 *  2   | RXSOP2DB
 *  1   | RXSOP1DB
 *  0   | RXSOP
 */
struct Status1A
{
    unsigned char RXSOP      : 1;
    unsigned char RXSOP1DB   : 1;
    unsigned char RXSOP2DB   : 1;
    unsigned char TOGSS      : 3;
    unsigned char            : 2;

    Status1A()
    {
        RXSOP = 0;
        RXSOP1DB = 0;
        RXSOP2DB = 0;
        TOGSS = 0;
    }

    Status1A(unsigned char value)
    {
        RXSOP = value & 0x1;
        RXSOP1DB = (value >> 1) & 0x1;
        RXSOP2DB = (value >> 2) & 0x1;
        TOGSS = (value >> 3) & 0x7;
    }

    unsigned char get()
    {
        return RXSOP | (RXSOP1DB << 1) | (RXSOP2DB << 2) | (TOGSS << 3);
    }
};

/*
 *  7   | I_OCP_TEMP
 *  6   | I_TOGDONE
 *  5   | I_SOFTFAIL
 *  4   | I_RETRYFAIL
 *  3   | I_HARDSENT
 *  2   | I_TXSENT
 *  1   | I_SOFTRST
 *  0   | I_HARDRST
 */
struct InterruptA
{
    unsigned char I_HARDRST      : 1;
    unsigned char I_SOFTRST      : 1;
    unsigned char I_TXSENT       : 1;
    unsigned char I_HARDSENT     : 1;
    unsigned char I_RETRYFAIL    : 1;
    unsigned char I_SOFTFAIL     : 1;
    unsigned char I_TOGDONE      : 1;
    unsigned char I_OCP_TEMP     : 1;

    InterruptA()
    {
        I_HARDRST = 0;
        I_SOFTRST = 0;
        I_TXSENT = 0;
        I_HARDSENT = 0;
        I_RETRYFAIL = 0;
        I_SOFTFAIL = 0;
        I_TOGDONE = 0;
        I_OCP_TEMP = 0;
    }

    InterruptA(unsigned char value)
    {
        I_HARDRST = value & 0x1;
        I_SOFTRST = (value >> 1) & 0x1;
        I_TXSENT = (value >> 2) & 0x1;
        I_HARDSENT = (value >> 3) & 0x1;
        I_RETRYFAIL = (value >> 4) & 0x1;
        I_SOFTFAIL = (value >> 5) & 0x1;
        I_TOGDONE = (value >> 6) & 0x1;
        I_OCP_TEMP = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return I_HARDRST | (I_SOFTRST << 1) | 
        (I_TXSENT << 2) | (I_HARDSENT << 3) | 
        (I_RETRYFAIL << 4) | (I_SOFTFAIL << 5) | 
        (I_TOGDONE << 6) | (I_OCP_TEMP << 7);
    }
};

/*
 *  7:1 | Reserved [6:0]
 *  0   | I_GCRCSENT
 */
struct InterruptB
{
    unsigned char I_GCRCSENT : 1;
    unsigned char            : 7;

    InterruptB()
    {
        I_GCRCSENT = 0;
    }

    InterruptB(unsigned char value)
    {
        I_GCRCSENT = value & 0x1;
    }

    unsigned char get()
    {
        return I_GCRCSENT;
    }
};

/*
 *  7   | VBUSOK
 *  6   | ACTIVITY
 *  5   | COMP
 *  4   | CRC_CHK
 *  3   | ALERT
 *  2   | WAKE
 *  1:0 | BC_LVL [1:0]
 */
struct Status0
{
    unsigned char BC_LVL     : 2;
    unsigned char WAKE       : 1;
    unsigned char ALERT      : 1;
    unsigned char CRC_CHK   : 1;
    unsigned char COMP       : 1;
    unsigned char ACTIVITY   : 1;
    unsigned char VBUSOK     : 1;

    Status0()
    {
        BC_LVL = 0;
        WAKE = 0;
        ALERT = 0;
        CRC_CHK = 0;
        COMP = 0;
        ACTIVITY = 0;
        VBUSOK = 0;
    }

    Status0(unsigned char value)
    {
        BC_LVL = value & 0x3;
        WAKE = (value >> 2) & 0x1;
        ALERT = (value >> 3) & 0x1;
        CRC_CHK = (value >> 4) & 0x1;
        COMP = (value >> 5) & 0x1;
        ACTIVITY = (value >> 6) & 0x1;
        VBUSOK = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return BC_LVL | (WAKE << 2) | (ALERT << 3) | 
        (CRC_CHK << 4) | (COMP << 5) | (ACTIVITY << 6) | 
        (VBUSOK << 7);
    }
};

/*
 *  7    | RXSOP2
 *  6    | RXSOP1
 *  5    | RX_EMPTY
 *  4    | RX_FULL
 *  3    | TX_EMPTY
 *  2    | TX_FULL
 *  1    | OVRTEMP
 *  0    | OCP
 */
struct Status1
{
    unsigned char OCP        : 1;
    unsigned char OVRTEMP    : 1;
    unsigned char            : 2;
    unsigned char TX_FULL    : 1;
    unsigned char TX_EMPTY   : 1;
    unsigned char RX_FULL    : 1;
    unsigned char RX_EMPTY   : 1;
    unsigned char RXSOP1     : 1;
    unsigned char RXSOP2     : 1;

    Status1()
    {
        OCP = 0;
        OVRTEMP = 0;
        TX_FULL = 0;
        TX_EMPTY = 0;
        RX_FULL = 0;
        RX_EMPTY = 0;
        RXSOP1 = 0;
        RXSOP2 = 0;
    }

    Status1(unsigned char value)
    {
        OCP = value & 0x1;
        OVRTEMP = (value >> 1) & 0x1;
        TX_FULL = (value >> 4) & 0x1;
        TX_EMPTY = (value >> 5) & 0x1;
        RX_FULL = (value >> 6) & 0x1;
        RX_EMPTY = (value >> 7) & 0x1;
        RXSOP1 = (value >> 8) & 0x1;
        RXSOP2 = (value >> 9) & 0x1;
    }

    unsigned char get()
    {
        return OCP | (OVRTEMP << 1) | (TX_FULL << 4) | 
        (TX_EMPTY << 5) | (RX_FULL << 6) | (RX_EMPTY << 7) | 
        (RXSOP1 << 8) | (RXSOP2 << 9);
    }
};

/*
 *  7   | I_VBUSOK
 *  6   | I_ACTIVITY
 *  5   | I_COMP_CHNG
 *  4   | I_CRC_CHK
 *  3   | I_ALERT
 *  2   | I_WAKE
 *  1   | I_COLLISION
 *  0   | I_BC_LVL
 */
struct Interrupt
{
    unsigned char I_BC_LVL       : 1;
    unsigned char I_COLLISION    : 1;
    unsigned char I_WAKE         : 1;
    unsigned char I_ALERT        : 1;
    unsigned char I_CRC_CHK      : 1;
    unsigned char I_COMP_CHNG    : 1;
    unsigned char I_ACTIVITY     : 1;
    unsigned char I_VBUSOK       : 1;

    Interrupt()
    {
        I_BC_LVL = 0;
        I_COLLISION = 0;
        I_WAKE = 0;
        I_ALERT = 0;
        I_CRC_CHK = 0;
        I_COMP_CHNG = 0;
        I_ACTIVITY = 0;
        I_VBUSOK = 0;
    }

    Interrupt(unsigned char value)
    {
        I_BC_LVL = value & 0x1;
        I_COLLISION = (value >> 1) & 0x1;
        I_WAKE = (value >> 2) & 0x1;
        I_ALERT = (value >> 3) & 0x1;
        I_CRC_CHK = (value >> 4) & 0x1;
        I_COMP_CHNG = (value >> 5) & 0x1;
        I_ACTIVITY = (value >> 6) & 0x1;
        I_VBUSOK = (value >> 7) & 0x1;
    }

    unsigned char get()
    {
        return I_BC_LVL | (I_COLLISION << 1) | (I_WAKE << 2) | 
        (I_ALERT << 3) | (I_CRC_CHK << 4) | (I_COMP_CHNG << 5) | 
        (I_ACTIVITY << 6) | (I_VBUSOK << 7);
    }
};

/*
 *  7:0 | TX/RX Token
 *  However, this stores both the TX and RX FIFOs separately
 *  This allows for easier access to the FIFOs
 */
struct FIFO
{
    unsigned char RXFIFO;
    unsigned char TXFIFO;

    FIFO()
    {
        RXFIFO = 0;
        TXFIFO = 0;
    }

    FIFO(unsigned char rx, unsigned char tx)
    {
        RXFIFO = rx;
        TXFIFO = tx;
    }

    void setRX(unsigned char rx)
    {
        RXFIFO = rx;
    }

    void setTX(unsigned char tx)
    {
        TXFIFO = tx;
    }

    unsigned char getTX()
    {
        return RXFIFO;
    }

    unsigned char getRX()
    {
        return TXFIFO;
    }
};

/**
 * @brief Structs for the PD data
 * @note This is a direct copy of the datasheet registers
 * 
 * @param DeviceID
 * @param Switches0
 * @param Switches1
 * @param Mesaure
 * @param Slice
 * @param Control0
 * @param Control1
 * @param Control2
 * @param Control3
 * @param Mask
 * @param Power
 * @param Reset
 * @param OCPreg
 * @param MaskA
 * @param MaskB
 * @param Control4
 * @param Status0A
 * @param Status1A
 * @param InterruptA
 * @param InterruptB
 * @param Status0
 * @param Status1
 * @param Interrupt
 * @param FIFO
 */
struct FUSB302_Data
{
    DeviceID deviceid;
    Switches0 switches0;
    Switches1 switches1;
    Measure measure;
    Slice slice;
    Control0 control0;
    Control1 control1;
    Control2 control2;
    Control3 control3;
    Mask mask;
    Power power;
    Reset reset;
    OCPreg ocpreg;
    MaskA maska;
    MaskB maskb;
    Control4 control4;
    Status0A status0a;
    Status1A status1a;
    InterruptA interrupta;
    InterruptB interruptb;
    Status0 status0;
    Status1 status1;
    Interrupt interrupt;
    FIFO fifo;
};
