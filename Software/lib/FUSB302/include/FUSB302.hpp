#pragma once

#include "FUSB302_Registers.hpp"
#include "FUSB302_Data.hpp"
#include "FUSB302_Enum.hpp"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

class FUSB302
{
protected:
#pragma region Constructor and connection verification
    FUSB302(unsigned char address, i2c_inst_t* wire);
    bool verifyConnection();
    void getAllRegisters();
    void setAllRegisters();
#pragma endregion

#pragma region transfer registers abstraction
    void setSwitches0();
    void setSwitches1();
    void setMeasure();
    void setSlice();
    void setControl0();
    void setControl1();
    void setControl2();
    void setControl3();
    void setMask1();
    void setPower();
    void setReset();
    void setOCPreg();
    void setMaskA();
    void setMaskB();
    void setControl4();
    void setFIFO();

    void getDeviceID();
    void getSwitches0();
    void getSwitches1();
    void getMeasure();
    void getSlice();
    void getControl0();
    void getControl1();
    void getControl2();
    void getControl3();
    void getMask1();
    void getPower();
    void getOCPreg();
    void getMaskA();
    void getMaskB();
    void getControl4();
    void getStatus0A();
    void getStatus1A();
    void getInterruptA();
    void getInterruptB();
    void getStatus0();
    void getStatus1();
    void getInterrupt();
    void getFIFO();
#pragma endregion

#pragma region Device ID register abstraction
    unsigned char getID();
    unsigned char getProductID();
    unsigned char getRevisionID();
#pragma endregion

#pragma region Switches0 register abstraction
    void setPullUp(FUSB302_CC enable);
    void setVCONN(FUSB302_CC enable);
    void setMeasureCC(FUSB302_CC enable);
    void setPullDown(FUSB302_CC enable);
    FUSB302_CC getPullUp();
    FUSB302_CC getVCONN();
    FUSB302_CC getMeasureCC();
    FUSB302_CC getPullDown();
#pragma endregion

#pragma region Switches1 register abstraction
    void setPowerRole(FUSB302_Role role);
    void setSpecRevision(FUSB302_Revision revision);
    void setDataRole(FUSB302_Role role);
    void setAutoCRC(bool enable);
    void setBMC(FUSB302_CC enable);
    FUSB302_Role getPowerRole();
    FUSB302_Revision getSpecRevision();
    FUSB302_Role getDataRole();
    bool getAutoCRC();
    FUSB302_CC getBMC();
#pragma endregion

#pragma region Measure register abstraction
    void setMeasureVBUS(unsigned char value);
    void setMDAC(unsigned char value);
    unsigned char getMeasureVBUS();
    unsigned char getMDAC();
#pragma endregion

#pragma region Slice register abstraction
    void setSDACHysterisis(FUSB302_Hysterisis hysterisis);
    void setSDAC(unsigned char value);
    FUSB302_Hysterisis getSDACHysterisis();
    unsigned char getSDAC();
#pragma endregion

#pragma region Control0 register abstraction
    void setTXFlush();
    void setIntMask(bool enable);
    void setHostCurrent(FUSB302_Current enable);
    void setAutoPreamble(bool enable);
    void setTXStart();
    bool getIntMask();
    FUSB302_Current getHostCurrent();
    bool getAutoPreamble();
#pragma endregion

#pragma region Control1 register abstraction
    void setSOPDebug(FUSB302_SOP sop);
    void setSentBIST(bool enable);
    void setRXFlush();
    void setSOP(FUSB302_SOP sop);
    FUSB302_SOP getSOPDebug();
    bool getSentBIST();
    FUSB302_SOP getSOP();
#pragma endregion

#pragma region Control2 register abstrction
    void setToggleSavePower(FUSB302_TOGSAVEPWR togsavepwr);
    void setToggleReadOnly(bool enable);
    void setWakeEnable(bool enable);
    void setMode(FUSB302_Mode mode);
    void setToggle(bool enable);
    FUSB302_TOGSAVEPWR getToggleSavePower();
    bool getToggleReadOnly();
    bool getWakeEnable();
    FUSB302_Mode getMode();
    bool getToggle();
#pragma endregion

#pragma region Control3 register abstraction
    void setSendHardReset();
    void setBISTTMode(bool enable);
    void setAutoHardReset(bool enable);
    void setAutoSoftReset(bool enable);
    void setNRetries(unsigned char value);
    void setAutoRetry(bool enable);
    bool getBISTTMode();
    bool getAutoHardReset();
    bool getAutoSoftReset();
    unsigned char getNRetries();
    bool getAutoRetry();
#pragma endregion

#pragma region MASK register abstraction
    void setMaskVBUSOK(FUSB302_Mask mask);
    void setMaskActivity(FUSB302_Mask mask);
    void setMaskCompChng(FUSB302_Mask mask);
    void setMaskCRCChng(FUSB302_Mask mask);
    void setMaskAlert(FUSB302_Mask mask);
    void setMaskWake(FUSB302_Mask mask);
    void setMaskCollision(FUSB302_Mask mask);
    void setMaskBCLVL(FUSB302_Mask mask);
    FUSB302_Mask getMaskVBUSOK();
    FUSB302_Mask getMaskActivity();
    FUSB302_Mask getMaskCompChng();
    FUSB302_Mask getMaskCRCChng();
    FUSB302_Mask getMaskAlert();
    FUSB302_Mask getMaskWake();
    FUSB302_Mask getMaskCollision();
    FUSB302_Mask getMaskBCLVL();
#pragma endregion

#pragma region POWER register abstraction
    void setBandgapAndWakePower(bool enable);
    void setReceiverAndCurrentReferencePower(bool enable);
    void setMeasurePower(bool enable);
    void setInternalOscillatorPower(bool enable);
    bool getBandgapAndWakePower();
    bool getReceiverAndCurrentReferencePower();
    bool getMeasurePower();
    bool getInternalOscillatorPower();
#pragma endregion

#pragma region RESET register abstraction
    void setSWReset();
    void setPDReset();
#pragma endregion

#pragma region OCPreg register abstraction
    void setOCPRange(FUSB302_OCP_Range range);
    void setOCPCurrent(FUSB302_OCP_Current value);
    FUSB302_OCP_Range getOCPRange();
    FUSB302_OCP_Current getOCPCurrent();
#pragma endregion

#pragma region MASK A register abstraction
    void setMaskOCPTemperature(FUSB302_Mask mask);
    void setMaskToggleDone(FUSB302_Mask mask);
    void setMaskSoftFail(FUSB302_Mask mask);
    void setMaskRetryFail(FUSB302_Mask mask);
    void setMaskHardSent(FUSB302_Mask mask);
    void setMaskTxSent(FUSB302_Mask mask);
    void setMaskSoftReset(FUSB302_Mask mask);
    void setMaskHardReset(FUSB302_Mask mask);
    FUSB302_Mask getMaskOCPTemperature();
    FUSB302_Mask getMaskToggleDone();
    FUSB302_Mask getMaskSoftFail();
    FUSB302_Mask getMaskRetryFail();
    FUSB302_Mask getMaskHardSent();
    FUSB302_Mask getMaskTxSent();
    FUSB302_Mask getMaskSoftReset();
    FUSB302_Mask getMaskHardReset();
#pragma endregion

#pragma region MASK B register abstraction
    void setMaskGCRCSent(FUSB302_Mask mask);
    FUSB302_Mask getMaskGCRCSent();
#pragma endregion

#pragma region Control 4 register abstraction
    void setToggleExitAudio(bool enable);
    bool getToggleExitAudio();
#pragma endregion

#pragma region Status 0A register abstraction
    bool getSoftFail();
    bool getRetryFail();
    bool getPower3();
    bool getPower2();
    bool getSoftReset();
    bool getHardReset();
#pragma endregion

#pragma region Status 1A register abstraction
    FUSB302_ToggleFunctionality getToggleStatus();
    bool getRXSOP2Debug();
    bool getRXSOP1Debug();
    bool getRXSOP();
#pragma endregion

#pragma region Interrupt A register abstraction
    bool getIntOCPTemperature();
    bool getIntToggleDone();
    bool getIntSoftFail();
    bool getIntRetryFail();
    bool getIntHardSent();
    bool getIntTXSent();
    bool getIntSoftReset();
    bool getIntHardReset();
#pragma endregion

#pragma region Interrupt B register abstraction
    bool getIntGCRCSent();
#pragma endregion

#pragma region Status 0 register abstraction
    bool getVBUSOK();
    bool getActivity();
    bool getComp();
    bool getCRCCheck();
    bool getAlert();
    bool getWake();
    FUSB302_BC_LVL getBCLVL();
#pragma endregion

#pragma region Status 1 register abstraction
    bool getRXSOP2();
    bool getRXSOP1();
    bool getRXEmpty();
    bool getRXFull();
    bool getTXEmpty();
    bool getTXFull();
    bool OVRTEMP();
    bool OCP();
#pragma endregion

#pragma region Interrupt register abstraction
    bool getIntVBUSOK();
    bool getIntActivity();
    bool getIntCompChng();
    bool getIntCRCChng();
    bool getIntAlert();
    bool getIntWake();
    bool getIntCollision();
    bool getIntBCLVL();
#pragma endregion

#pragma region FIFO register abstraction
    void setRXFIFO(unsigned char data);
    void setTXFIFO(unsigned char data);
    unsigned char getRXFIFO();
    unsigned char getTXFIFO();
#pragma endregion

    FUSB302_Data getData();
    void setSend();
    void writeMessage(unsigned char message);
    unsigned char readMessage();
private:
    unsigned char device_address;
    i2c_inst_t* i2c;
    FUSB302_Data data;

    unsigned char readWord(unsigned char register_address);
    void writeWord(unsigned char register_address, unsigned char data);
};