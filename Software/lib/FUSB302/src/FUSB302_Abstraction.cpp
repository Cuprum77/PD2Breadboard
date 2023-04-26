#include <FUSB302.hpp>

#pragma region Constructor and connection verification
/**
 * @brief USB PD constructor
 * @param wire Wire object
 * @param address USB PD address
 * @note This constructor will automatically call getAllRegisters() to get the current state of the USB PD chip
*/
FUSB302::FUSB302(unsigned char address, i2c_inst_t* wire)
{
    this->device_address = address;
    this->i2c = wire;
}

/**
 * @brief Verify that the INA219 is connected
 * @return true if the INA219 is connected, false otherwise
*/
bool FUSB302::verifyConnection()
{
    // check if we get a response from the eeprom by dummy writing to it
    unsigned char data;
    // if the dummy write fails, it should return a negative number
    int ret = i2c_read_blocking(this->i2c, this->device_address, &data, 1, false);
    return !(ret < 0);
}
#pragma endregion

#pragma region Device ID register abstraction
/**
 * @brief Get the device ID
 * @return the device ID
*/
unsigned char FUSB302::getID()
{
    return this->data.deviceid.VersionID;
}

/**
 * @brief Get the product ID
 * @return the product ID
*/
unsigned char FUSB302::getProductID()
{
    return this->data.deviceid.ProductID;
}

/**
 * @brief Get the revision ID
 * @return the revision ID
*/
unsigned char FUSB302::getRevisionID()
{
    return this->data.deviceid.RevisionID;
}
#pragma endregion

#pragma region Switches0 register abstraction
/**
 * @brief Pull up the CC pin
 * @param enable Enable or disable the pull up
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setPullUp(FUSB302_CC enable)
{    
    this->data.switches0.MEAS_CC1 = enable & 0x1;
    this->data.switches0.MEAS_CC2 = enable & 0x2 >> 1;
}

/**
 * @brief Enable VCONN current on the CC lines
 * @param enable Enable or disable VCONN
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setVCONN(FUSB302_CC enable)
{
    this->data.switches0.VCONN_CC1 = enable & 0x1;
    this->data.switches0.VCONN_CC2 = enable & 0x2 >> 1;
}

/**
 * @brief Enable measuring the CC lines
 * @param enable Enable or disable measuring
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMeasureCC(FUSB302_CC enable)
{
    this->data.switches0.MEAS_CC1 = enable & 0x1;
    this->data.switches0.MEAS_CC2 = enable & 0x2 >> 1;
}

/**
 * @brief Enable the CC pulldown
 * @param enable Enable or disable the pulldown
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setPullDown(FUSB302_CC enable)
{
    this->data.switches0.PDWN1 = enable & 0x1;
    this->data.switches0.PDWN2 = enable & 0x2 >> 1;
}

/**
 * @brief Get the CC pullup
 * @return The CC pullup
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_CC FUSB302::getPullUp()
{
    return (FUSB302_CC)(this->data.switches0.MEAS_CC1 | this->data.switches0.MEAS_CC2 << 1);
}

/**
 * @brief Get the VCONN current
 * @return The VCONN current
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_CC FUSB302::getVCONN()
{
    return (FUSB302_CC)(this->data.switches0.VCONN_CC1 | this->data.switches0.VCONN_CC2 << 1);
}

/**
 * @brief Get the CC measurement
 * @return The CC measurement
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_CC FUSB302::getMeasureCC()
{
    return (FUSB302_CC)(this->data.switches0.MEAS_CC1 | this->data.switches0.MEAS_CC2 << 1);
}

/**
 * @brief Get the CC pulldown
 * @return The CC pulldown
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_CC FUSB302::getPullDown()
{
    return (FUSB302_CC)(this->data.switches0.PDWN1 | this->data.switches0.PDWN2 << 1);
}
#pragma endregion

#pragma region Switches1 register abstraction
/**
 * @brief Set the power role
 * @param role The power role
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setPowerRole(FUSB302_Role role)
{
    this->data.switches1.POWER_ROLE = role;
}

/**
 * @brief Set the spec revision
 * @param revision The spec revision
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSpecRevision(FUSB302_Revision revision)
{
    this->data.switches1.SPEC = revision;
}

/**
 * @brief Set the data role
 * @param role The data role
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setDataRole(FUSB302_Role role)
{
    this->data.switches1.DATA_ROLE = role;
}

/**
 * @brief Set the auto CRC
 * @param enable Enable or disable auto CRC
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setAutoCRC(bool enable)
{
    this->data.switches1.AUTO_CRC = enable;
}

/**
 * @brief Enable the BMC drivers on the CC lines
 * @param enable Enable or disable the BMC drivers
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setBMC(FUSB302_CC enable)
{
    this->data.switches1.TXCC1 = enable & 0x1;
    this->data.switches1.TXCC2 = enable & 0x2 >> 1;
}

/**
 * @brief Get the Power Role
 * @return The power role
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Role FUSB302::getPowerRole()
{
    return (FUSB302_Role)this->data.switches1.POWER_ROLE;
}

/**
 * @brief Get the Spec Revision
 * @return The spec revision
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Revision FUSB302::getSpecRevision()
{
    return (FUSB302_Revision)this->data.switches1.SPEC;
}

/**
 * @brief Get the Data Role
 * @return The data role
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Role FUSB302::getDataRole()
{
    return (FUSB302_Role)this->data.switches1.DATA_ROLE;
}

/**
 * @brief Get the Auto CRC
 * @return The auto CRC
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAutoCRC()
{
    return (bool)this->data.switches1.AUTO_CRC;
}

/**
 * @brief Get the BMC drivers
 * @return The BMC drivers
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_CC FUSB302::getBMC()
{
    return (FUSB302_CC)(this->data.switches1.TXCC1 | this->data.switches1.TXCC2 << 1);
}
#pragma endregion

#pragma region Measure register abstraction
/**
 * @brief Set the Measure VBUS
 * @param value The value to set (0 or 1)
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMeasureVBUS(unsigned char value)
{
    this->data.measure.MDAC = value & 0x1;
}

/**
 * @brief Set the MDAC
 * @param value The value to set (0 - 63)
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMDAC(unsigned char value)
{
    this->data.measure.MDAC = value & 0x3F;
}

/**
 * @brief Get the Measure VBUS
 * @return The Measure VBUS
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getMeasureVBUS()
{
    return this->data.measure.MDAC & 0x1;
}

/**
 * @brief Get the MDAC
 * @return The MDAC value
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getMDAC()
{
    return this->data.measure.MDAC & 0x3F;
}
#pragma endregion

#pragma region Slice register abstraction
/**
 * @brief Set the SDAC hysterisis
 * @param hysterisis The hysterisis to set
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSDACHysterisis(FUSB302_Hysterisis hysterisis)
{
    this->data.slice.SDAC_HYS = hysterisis;
}

/**
 * @brief Set the SDAC
 * @param value The value to set (0 - 63)
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSDAC(unsigned char value)
{
    this->data.slice.SDAC = value & 0x3F;
}

/**
 * @brief Get the SDAC hysterisis
 * @return The SDAC hysterisis
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Hysterisis FUSB302::getSDACHysterisis()
{
    return (FUSB302_Hysterisis)this->data.slice.SDAC_HYS;
}

/**
 * @brief Get the SDAC
 * @return The SDAC value
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getSDAC()
{
    return this->data.slice.SDAC & 0x3F;
}
#pragma endregion

#pragma region Control0 register abstraction
/**
 * @brief Flush the TX FIFO
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setTXFlush()
{
    this->data.control0.TX_FLUSH = 0x1;
}

/**
 * @brief Set the power down
 * @param enable Enable or disable the power down
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setIntMask(bool enable)
{
    this->data.control0.INT_MASK = enable & 0x1;
}

/**
 * @brief Set the power down
 * @param enable Enable or disable the power down
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setHostCurrent(FUSB302_Current current)
{
    this->data.control0.HOST_CUR = current & 0x3;
}

/**
 * @brief Set the Auto Preamble
 * @param enable Enable or disable the Auto Preamble
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setAutoPreamble(bool enable)
{
    this->data.control0.AUTO_PRE = enable & 0x1;
}

/**
 * @brief Enable the transmitter
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setTXStart()
{
    this->data.control0.TX_START = 0x1;
}

/**
 * @brief Get the Interrupt Mask
 * @return The interrupt mask
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getIntMask()
{
    return (bool)this->data.control0.INT_MASK;
}

/**
 * @brief Get the Host Current
 * @return The host current
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Current FUSB302::getHostCurrent()
{
    return (FUSB302_Current)this->data.control0.HOST_CUR;
}

/**
 * @brief Get the Auto Preamble
 * @return The auto preamble
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAutoPreamble()
{
    return (bool)this->data.control0.AUTO_PRE;
}
#pragma endregion

#pragma region Control1 register abstraction
/**
 * @brief Set the enable SOP debug
 * @param sop The SOP to enable debug for
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSOPDebug(FUSB302_SOP sop)
{
    this->data.control1.ENSOP1DB = sop & 0x1;
    this->data.control1.ENSOP2DB = sop & 0x2 >> 1;
}

/**
 * @brief Set the Sent BIST
 * @param enable Enable or disable the Sent BIST
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSentBIST(bool enable)
{
    this->data.control1.BIST_MODE2 = enable & 0x1;
}

/**
 * @brief Flush the RX FIFO
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setRXFlush()
{
    this->data.control1.RX_FLUSH = 0x1;
}

/**
 * @brief Set the SOP packet type
 * @param sop The SOP packet type
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setSOP(FUSB302_SOP sop)
{
    this->data.control1.ENSOP1 = sop & 0x1;
    this->data.control1.ENSOP2 = sop & 0x2 >> 1;
}

/**
 * @brief Get the SOP debug
 * @return The SOP debug
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_SOP FUSB302::getSOPDebug()
{
    return (FUSB302_SOP)(this->data.control1.ENSOP1DB | this->data.control1.ENSOP2DB << 1);
}

/**
 * @brief Get the Sent BIST
 * @return The sent BIST
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getSentBIST()
{
    return (bool)this->data.control1.BIST_MODE2;
}

/**
 * @brief Get the SOP
 * @return The SOP
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_SOP FUSB302::getSOP()
{
    return (FUSB302_SOP)(this->data.control1.ENSOP1 | this->data.control1.ENSOP2 << 1);
}
#pragma endregion

#pragma region Control2 register abstrction
/**
 * @brief set Toggle Save Power Mode
 * @param togsavepwr Set the toggle cycle save power mode
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setToggleSavePower(FUSB302_TOGSAVEPWR togsavepwr)
{
    this->data.control2.TOG_SAVE_PWR = togsavepwr & 0x3;
}

/**
* @brief Set the toggle read only
* @param enable Enable or disable the toggle read only
* @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setToggleReadOnly(bool enable)
{
    this->data.control2.TOG_RD_ONLY = enable & 0x1;
}

/**
* @brief Set the Wake Enable
* @param enable Enable or disable the wake enable
* @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setWakeEnable(bool enable)
{
        this->data.control2.WAKE_EN = enable & 0x1;
}

/**
 * @brief Set the toggle mode
 * @param mode The toggle mode
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMode(FUSB302_Mode mode)
{
    this->data.control2.MODE = mode & 0x3;
}

/**
 * @brief Set the Toggle
 * @param enable Enable or disable the Toggle
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setToggle(bool enable)
{
    this->data.control2.TOGGLE = enable & 0x1;
}

/**
 * @brief Get the Toggle Save Power Mode
 * @return The toggle save power mode
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_TOGSAVEPWR FUSB302::getToggleSavePower()
{
    return (FUSB302_TOGSAVEPWR)this->data.control2.TOG_SAVE_PWR;
}

/**
 * @brief Get the Toggle Read Only
 * @return The toggle read only
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getToggleReadOnly()
{
    return (bool)this->data.control2.TOG_RD_ONLY;
}

/**
 * @brief Get the Wake Enable
 * @return The wake enable
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getWakeEnable()
{
    return (bool)this->data.control2.WAKE_EN;
}

/**
 * @brief Get the Mode
 * @return The mode
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mode FUSB302::getMode()
{
    return (FUSB302_Mode)this->data.control2.MODE;
}

/**
 * @brief Get the Toggle
 * @return The toggle
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getToggle()
{
    return (bool)this->data.control2.TOGGLE;
}
#pragma endregion

#pragma region Control3 register abstraction
/**
 * @brief Set the SEND_HARD_RESET
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setSendHardReset()
{
    this->data.control3.SEND_HARD_RESET = 0x1;
}

/**
 * @brief Set BIST T Mode
 * @param enable Enable for BIST, disable for normal
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setBISTTMode(bool enable)
{
    this->data.control3.BIST_TMODE = enable & 0x1;
}

/**
 * @brief Set the Auto Hard Reset
 * @param enable Enable or disable the auto hard reset
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setAutoHardReset(bool enable)
{
    this->data.control3.AUTO_HARDRESET = enable & 0x1;
}

/**
 * @brief Set the Auto Soft Reset
 * @param enable Enable or disable the auto soft reset
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setAutoSoftReset(bool enable)
{
    this->data.control3.AUTO_SOFTRESET = enable & 0x1;
}

/**
 * @brief Set the number of retries
 * @param retries The number of retries (0-3) where 0 is none, and 1-3 is N+1 retries
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setNRetries(unsigned char retries)
{
    this->data.control3.N_RETRIES = retries & 0x3;
}

/**
 * @brief Set the Auto Retry
 * @param enable Enable or disable the auto retry
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setAutoRetry(bool enable)
{
    this->data.control3.AUTO_RETRY = enable & 0x1;
}

/**
 * @brief Get the BIST T Mode
 * @return The BIST T Mode
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getBISTTMode()
{
    return (bool)this->data.control3.BIST_TMODE;
}

/**
 * @brief Get the Auto Hard Reset
 * @return The auto hard reset
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAutoHardReset()
{
    return (bool)this->data.control3.AUTO_HARDRESET;
}

/**
 * @brief Get the Auto Soft Reset
 * @return The auto soft reset
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAutoSoftReset()
{
    return (bool)this->data.control3.AUTO_SOFTRESET;
}

/**
 * @brief Get the number of retries
 * @return The number of retries (0-3) where 0 is none, and 1-3 is N+1 retries
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getNRetries()
{
    return this->data.control3.N_RETRIES;
}

/**
 * @brief Get the Auto Retry
 * @return The auto retry
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAutoRetry()
{
    return (bool)this->data.control3.AUTO_RETRY;
}
#pragma endregion

#pragma region MASK register abstraction
/**
 * @brief Set the VBUS OK Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskVBUSOK(FUSB302_Mask mask)
{
    this->data.mask.M_VBUSOK = mask & 0x1;
}

/**
 * @brief Set the Activity Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskActivity(FUSB302_Mask mask)
{
    this->data.mask.M_ACTIVITY = mask & 0x1;
}

/**
 * @brief Set the COMP_CHNG Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskCompChng(FUSB302_Mask mask)
{
    this->data.mask.M_COMP_CHNG = mask & 0x1;
}

/**
 * @brief Set the CRC_CHK Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskCRCChng(FUSB302_Mask mask)
{
    this->data.mask.M_CRC_CHK = mask & 0x1;
}

/**
 * @brief Set the ALERT Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskAlert(FUSB302_Mask mask)
{
    this->data.mask.M_ALERT = mask & 0x1;
}

/**
 * @brief Set the WAKE Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskWake(FUSB302_Mask mask)
{
    this->data.mask.M_WAKE = mask & 0x1;
}

/**
 * @brief Set the COLLISION Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskCollision(FUSB302_Mask mask)
{
    this->data.mask.M_COLLISION = mask & 0x1;
}

/**
 * @brief Set the BC_LVL Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskBCLVL(FUSB302_Mask mask)
{
    this->data.mask.M_BC_LVL = mask & 0x1;
}

/**
 * @brief Get the VBUS OK Mask
 * @return The VBUS OK Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskVBUSOK()
{
    return (FUSB302_Mask)this->data.mask.M_VBUSOK;
}

/**
 * @brief Get the Activity Mask
 * @return The Activity Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskActivity()
{
    return (FUSB302_Mask)this->data.mask.M_ACTIVITY;
}

/**
 * @brief Get the COMP_CHNG Mask
 * @return The COMP_CHNG Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskCompChng()
{
    return (FUSB302_Mask)this->data.mask.M_COMP_CHNG;
}

/**
 * @brief Get the CRC_CHK Mask
 * @return The CRC_CHK Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskCRCChng()
{
    return (FUSB302_Mask)this->data.mask.M_CRC_CHK;
}

/**
 * @brief Get the ALERT Mask
 * @return The ALERT Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskAlert()
{
    return (FUSB302_Mask)this->data.mask.M_ALERT;
}

/**
 * @brief Get the WAKE Mask
 * @return The WAKE Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskWake()
{
    return (FUSB302_Mask)this->data.mask.M_WAKE;
}

/**
 * @brief Get the COLLISION Mask
 * @return The COLLISION Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskCollision()
{
    return (FUSB302_Mask)this->data.mask.M_COLLISION;
}

/**
 * @brief Get the BC_LVL Mask
 * @return The BC_LVL Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskBCLVL()
{
    return (FUSB302_Mask)this->data.mask.M_BC_LVL;
}
#pragma endregion

#pragma region POWER register abstraction
/**
 * @brief Set the Bandgap and Wake circuitry power
 * @param enable Enable or disable
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setBandgapAndWakePower(bool enable)
{
    this->data.power.STATUS = enable & 0x1;
}

/**
 * @brief Set the Receiver and Current Reference circuitry power
 * @param enable Enable or disable
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setReceiverAndCurrentReferencePower(bool enable)
{
    this->data.power.STATUS = enable & 0x1 << 1;
}

/**
 * @brief Set the Measure circuitry power
 * @param enable Enable or disable
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMeasurePower(bool enable)
{
    this->data.power.STATUS = enable & 0x1 << 2;
}

/**
 * @brief Set the Internal Oscillator circuitry power
 * @param enable Enable or disable
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setInternalOscillatorPower(bool enable)
{
    this->data.power.STATUS = enable & 0x1 << 3;
}
#pragma endregion

#pragma region RESET register abstraction
/**
 * @brief Set the SW Reset
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setSWReset()
{
    this->data.reset.SW_RES = 1;
}

/**
 * @brief Set the PD Reset
 * @note Does not write to the registers, only sets the local cache
 * @note This is a Write/Clear register!
*/
void FUSB302::setPDReset()
{
    this->data.reset.PD_RESET = 1;
}
#pragma endregion

#pragma region OCPREG register abstraction
/**
 * @brief Set the OCP Range
 * @param range The range
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setOCPRange(FUSB302_OCP_Range range)
{
    this->data.ocpreg.OCP_RANGE = range & 0x1;
}

/**
 * @brief Set the OCP Current
 * @param current The current
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setOCPCurrent(FUSB302_OCP_Current current)
{
    this->data.ocpreg.OCP_CUR = current & 0x3;
}

/**
 * @brief Get the OCP Range
 * @return The OCP Range
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_OCP_Range FUSB302::getOCPRange()
{
    return (FUSB302_OCP_Range)this->data.ocpreg.OCP_RANGE;
}

/**
 * @brief Get the OCP Current
 * @return The OCP Current
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_OCP_Current FUSB302::getOCPCurrent()
{
    return (FUSB302_OCP_Current)this->data.ocpreg.OCP_CUR;
}
#pragma endregion

#pragma region MASK A register abstraction
/**
 * @brief Set the OCP Temperature Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskOCPTemperature(FUSB302_Mask mask)
{
    this->data.maska.M_OCP_TEMP = mask & 0x1;
}

/**
 * @brief Set the Toggle Done Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskToggleDone(FUSB302_Mask mask)
{
    this->data.maska.M_TOGDONE = mask & 0x1;
}

/**
 * @brief Set the Soft Fail Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskSoftFail(FUSB302_Mask mask)
{
    this->data.maska.M_SOFTFAIL = mask & 0x1;
}

/**
 * @brief Set the Retry Fail Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskRetryFail(FUSB302_Mask mask)
{
    this->data.maska.M_RETRYFAIL = mask & 0x1;
}

/**
 * @brief Set the Hard Sent Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskHardSent(FUSB302_Mask mask)
{
    this->data.maska.M_HARDSENT = mask & 0x1;
}

/**
 * @brief Set the TX Sent Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskTxSent(FUSB302_Mask mask)
{
    this->data.maska.M_TXSENT = mask & 0x1;
}

/**
 * @brief Set the Soft Reset Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskSoftReset(FUSB302_Mask mask)
{
    this->data.maska.M_SOFTRST = mask & 0x1;
}

/**
 * @brief Set the Hard Reset Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskHardReset(FUSB302_Mask mask)
{
    this->data.maska.M_HARDRST = mask & 0x1;
}

/**
 * @brief Get the OCP Temperature Mask
 * @return The OCP Temperature Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskOCPTemperature()
{
    return (FUSB302_Mask)this->data.maska.M_OCP_TEMP;
}

/**
 * @brief Get the Toggle Done Mask
 * @return The Toggle Done Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskToggleDone()
{
    return (FUSB302_Mask)this->data.maska.M_TOGDONE;
}

/**
 * @brief Get the Soft Fail Mask
 * @return The Soft Fail Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskSoftFail()
{
    return (FUSB302_Mask)this->data.maska.M_SOFTFAIL;
}

/**
 * @brief Get the Retry Fail Mask
 * @return The Retry Fail Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskRetryFail()
{
    return (FUSB302_Mask)this->data.maska.M_RETRYFAIL;
}

/**
 * @brief Get the Hard Sent Mask
 * @return The Hard Sent Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskHardSent()
{
    return (FUSB302_Mask)this->data.maska.M_HARDSENT;
}

/**
 * @brief Get the TX Sent Mask
 * @return The TX Sent Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskTxSent()
{
    return (FUSB302_Mask)this->data.maska.M_TXSENT;
}

/**
 * @brief Get the Soft Reset Mask
 * @return The Soft Reset Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskSoftReset()
{
    return (FUSB302_Mask)this->data.maska.M_SOFTRST;
}

/**
 * @brief Get the Hard Reset Mask
 * @return The Hard Reset Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskHardReset()
{
    return (FUSB302_Mask)this->data.maska.M_HARDRST;
}
#pragma endregion

#pragma region MASK B register abstraction
/**
 * @brief Set the G CRC SENT Mask
 * @param mask The mask
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setMaskGCRCSent(FUSB302_Mask mask)
{
    this->data.maskb.M_GCRCSENT = mask & 0x1;
}

/**
 * @brief Get the G CRC SENT Mask
 * @return The G CRC SENT Mask
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_Mask FUSB302::getMaskGCRCSent()
{
    return (FUSB302_Mask)this->data.maskb.M_GCRCSENT;
}
#pragma endregion

#pragma region Control 4 register abstraction
/**
 * @brief Set the Toggle Exit Audio Accessory Mode
 * @param mode The mode
 * @note Does not write to the registers, only sets the local cache
*/
void FUSB302::setToggleExitAudio(bool mode)
{
    this->data.control4.TOG_EXIT_AUD = mode & 0x1;
}

/**
 * @brief Get the Toggle Exit Audio Accessory Mode
 * @return The Toggle Exit Audio Accessory Mode
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getToggleExitAudio()
{
    return (bool)this->data.control4.TOG_EXIT_AUD;
}
#pragma endregion

#pragma region Status 0A register abstraction
/**
 * @brief Get the Soft Fail
 * @return The Soft Fail
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getSoftFail()
{
    return (bool)this->data.status0a.SOFTFAIL;
}

/**
 * @brief Get the Retry Fail
 * @return The Retry Fail
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRetryFail()
{
    return (bool)this->data.status0a.RETRYFAIL;
}

/**
 * @brief Get the Power3
 * @return The Power3
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getPower3()
{
    return (bool)(this->data.status0a.POWER & 0x2 >> 1);
}

/**
 * @brief Get the Power2
 * @return The Power2
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getPower2()
{
    return (bool)(this->data.status0a.POWER & 0x1);
}

/**
 * @brief Get Soft Reset
 * @return The Soft Reset
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getSoftReset()
{
    return (bool)this->data.status0a.SOFTRST;
}

/**
 * @brief Get the Hard Reset
 * @return The Hard Reset
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getHardReset()
{
    return (bool)this->data.status0a.HARDRST;
}
#pragma endregion

#pragma region Status 1A register abstraction
/**
 * @brief Get the Toggle Status
 * @return The Toggle Status
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_ToggleFunctionality FUSB302::getToggleStatus()
{
    return (FUSB302_ToggleFunctionality)this->data.status1a.TOGSS;
}

/**
 * @brief Check if last packet was a SOP2 Debug type
 * @return True if last packet was a SOP2 Debug type
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXSOP2Debug()
{
    return (bool)this->data.status1a.RXSOP2DB;
}

/**
 * @brief Check if last packet was a SOP1 Debug type
 * @return True if last packet was a SOP1 Debug type
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXSOP1Debug()
{
    return (bool)this->data.status1a.RXSOP1DB;
}

/**
 * @brief Check if last packet was a SOP type
 * @return True if last packet was a SOP type
 * @note Does not read from the registers, only reads the local cache 
*/
bool FUSB302::getRXSOP()
{
    return (bool)this->data.status1a.RXSOP;
}
#pragma endregion

#pragma region Interrupt A register abstraction
/**
 * @brief Get the interrupt for Over Current Protection event or Temperature
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntOCPTemperature()
{
    return (bool)this->data.interrupta.I_OCP_TEMP;
}

/**
 * @brief Get the interrupt for Toggle Done
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntToggleDone()
{
    return (bool)this->data.interrupta.I_TOGDONE;
}

/**
 * @brief Get the interrupt for Soft Fail
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntSoftFail()
{
    return (bool)this->data.interrupta.I_SOFTFAIL;
}

/**
 * @brief Get the interrupt for Retry Fail
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntRetryFail()
{
    return (bool)this->data.interrupta.I_RETRYFAIL;
}

/**
 * @brief Get the interrupt for Hard Sent
 * @return True if the interrupt is set
 * @note Does not read from the registers, and will be cleared by next read unless set again!
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntHardSent()
{
    return (bool)this->data.interrupta.I_HARDSENT;
}

/**
 * @brief Get the interrupt for TX Sent
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntTXSent()
{
    return (bool)this->data.interrupta.I_TXSENT;
}

/**
 * @brief Get the interrupt for Soft Reset
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntSoftReset()
{
    return (bool)this->data.interrupta.I_SOFTRST;
}

/**
 * @brief Get the interrupt for Hard Reset
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntHardReset()
{
    return (bool)this->data.interrupta.I_HARDRST;
}
#pragma endregion

#pragma region Interrupt B register abstraction
/**
 * @brief Get the interrupt for the Good CRC aknowledgement
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntGCRCSent()
{
    return (bool)this->data.interruptb.I_GCRCSENT;
}
#pragma endregion

#pragma region Status 0 register abstraction
/**
 * @brief Get the status for VBUS OK
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getVBUSOK()
{
    return (bool)this->data.status0.VBUSOK;
}

/**
 * @brief Get the status for Activity
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getActivity()
{
    return (bool)this->data.status0.ACTIVITY;
}

/**
 * @brief Get the status for CC Comparator
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getComp()
{
    return (bool)this->data.status0.COMP;
}

/**
 * @brief Get the status for the CRC Check
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getCRCCheck()
{
    return (bool)this->data.status0.CRC_CHK;
}

/**
 * @brief Get the status for the Alert
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getAlert()
{
    return (bool)this->data.status0.ALERT;
}

/**
 * @brief Get the status for the Wake Detect
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getWake()
{
    return (bool)this->data.status0.WAKE;
}

/**
 * @brief Get the status for the BC Level
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
FUSB302_BC_LVL FUSB302::getBCLVL()
{
    return (FUSB302_BC_LVL)this->data.status0.BC_LVL;
}
#pragma endregion

#pragma region Status 1 register abstraction
/**
 * @brief Get the status for the RX SOP2
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXSOP2()
{
    return (bool)this->data.status1.RXSOP2;
}

/**
 * @brief Get the status for the RX SOP1
 * @return True if the status is set
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXSOP1()
{
    return (bool)this->data.status1.RXSOP1;
}

/**
 * @brief Check if RX FIFO is empty
 * @return True if the RX FIFO is empty
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXEmpty()
{
    return (bool)this->data.status1.RX_EMPTY;
}

/**
 * @brief Check if RX FIFO is full
 * @return True if the RX FIFO is full
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getRXFull()
{
    return (bool)this->data.status1.RX_FULL;
}

/**
 * @brief Check if TX FIFO is empty
 * @return True if the TX FIFO is empty
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getTXEmpty()
{
    return (bool)this->data.status1.TX_EMPTY;
}

/**
 * @brief Check if TX FIFO is full
 * @return True if the TX FIFO is full
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::getTXFull()
{
    return (bool)this->data.status1.TX_FULL;
}

/**
 * @brief Check if Temperature of package is too high
 * @return True if the temperature is too high
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::OVRTEMP()
{
    return (bool)this->data.status1.OVRTEMP;
}

/**
 * @brief Check if over-current event or short-circuit event has been detected
 * @return True if an over-current or short-circuit event has been detected
 * @note Does not read from the registers, only reads the local cache
*/
bool FUSB302::OCP()
{
    return (bool)this->data.status1.OCP;
}
#pragma endregion

#pragma region Interrupt register abstraction
/**
 * @brief Get the interrupt flag for VBUS OK
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntVBUSOK()
{
    return (bool)this->data.interrupt.I_VBUSOK;
}

/**
 * @brief Get the interrupt flag for Activity
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntActivity()
{
    return (bool)this->data.interrupt.I_ACTIVITY;
}

/**
 * @brief Get the interrupt flag for if the CC Comparator value has changed
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntCompChng()
{
    return (bool)this->data.interrupt.I_COMP_CHNG;
}

/**
 * @brief Get the interrupt flag for if the CRC Check is valid
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntCRCChng()
{
    return (bool)this->data.interrupt.I_CRC_CHK;
}

/**
 * @brief Get the interrupt flag for if an Alert has been triggered
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntAlert()
{
    return (bool)this->data.interrupt.I_ALERT;
}

/**
 * @brief Get the interrupt flag for if a Wake Detect has been triggered
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntWake()
{
    return (bool)this->data.interrupt.I_WAKE;
}

/**
 * @brief Get the interrupt flag for a collision has been detected
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntCollision()
{
    return (bool)this->data.interrupt.I_COLLISION;
}

/**
 * @brief Get the interrupt flag for if the BC_LVL has changed
 * @return True if the interrupt is set
 * @note Does not read from the registers, only reads the local cache
 * @note This is a Read/Clear register, and will be cleared by next read unless set again!
*/
bool FUSB302::getIntBCLVL()
{
    return (bool)this->data.interrupt.I_BC_LVL;
}
#pragma endregion

#pragma region FIFO abstraction
/**
 * @brief Set the RX FIFO value
 * @param value The value to set the RX FIFO to
 * @note Does not write to the registers, only writes the local cache
*/
void FUSB302::setRXFIFO(unsigned char value)
{
    this->data.fifo.RXFIFO = value;
}

/**
 * @brief Set the TX FIFO value
 * @param value The value to set the TX FIFO to
 * @note Does not write to the registers, only writes the local cache
*/
void FUSB302::setTXFIFO(unsigned char value)
{
    this->data.fifo.TXFIFO = value;
}

/**
 * @brief Get the RX FIFO value
 * @return The value of the RX FIFO
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getRXFIFO()
{
    return this->data.fifo.RXFIFO;
}

/**
 * @brief Get the TX FIFO value
 * @return The value of the TX FIFO
 * @note Does not read from the registers, only reads the local cache
*/
unsigned char FUSB302::getTXFIFO()
{
    return this->data.fifo.TXFIFO;
}
#pragma endregion
