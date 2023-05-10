#include "FUSB302.hpp"

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

/**
 * @brief Get the Data object
 * @return FUSB302_Data
 */
FUSB302_Data FUSB302::getData()
{
    return this->data;
}

/**
 * @brief Write message to the PD device
 * @param data to write
*/
void FUSB302::writeHeader(unsigned short data)
{
    for(int i = 0; i < 2; i++)
    {
        // if the TX FIFO is full, wait until it is empty
        while(!this->data.status1.TX_EMPTY)
            this->getStatus1();

        // write the data to the TX FIFO
        this->setTXFIFO(data >> (i * 8));
        // transmit the data to the PD device
        this->setFIFO();
        // set the start bit
        this->setTXStart();
        // send the Control0 register
        this->setControl0();
    }
}

/**
 * @brief Read message from the PD device
 * @return unsigned char
*/
unsigned short FUSB302::readHeader()
{
    unsigned short data;

    for(int i = 0; i < 2; i++)
    {
        // if the RX FIFO is empty, wait until it isn't
        while(this->data.status1.RX_EMPTY)
            this->getStatus1();

        // fetch the FIFO from the PD device
        this->getFIFO();
        // read the data from the RX FIFO
        data |= this->getRXFIFO() << (i * 8);
    }

    return data;
}

/**
 * @brief Parse the header of the PD message
 * @param info the info struct to store the header information
 * @param header the header to parse
*/
void FUSB302::parseHeader(FUSB302_Message_Info_t* info, unsigned short header)
{
}

/**
 * @brief Generate the header for the PD message
 * @param protocol the protocol to use
 * @param message_type the type of the message
 * @param object_count the number of objects in the message
 * @return unsigned short
*/
unsigned short FUSB302::generateHeader(FUSB302_Protocol_t* protocol, unsigned char message_type, unsigned char object_count)
{
    // Header is described in 6.2.1.1 of the USB PD specs
    unsigned short header = (message_type & 0x0f) << 0 |
                            (PD_SPEC_REV & 0x03) << 6 |
                            (protocol->message_id & 0x07) << 9 |
                            (object_count & 0x07) << 12;
    protocol->tx_message_header = header;
    return header;
}

/**
 * @private
 * @brief read a word from the USB PD
 * @param register_address the address to read from
 * @return the word read from the USB PD
*/
unsigned char FUSB302::readWord(unsigned char register_address)
{
    unsigned char data = 0;
    i2c_write_blocking(this->i2c, this->device_address, &register_address, 1, false);
    i2c_read_blocking(this->i2c, this->device_address, &data, 1, false);
    return data;
}

/**
 * @private
 * @brief write a word to the USB PD
 * @param register_address the address to write to
 * @param data the word to write to the USB PD
*/
void FUSB302::writeWord(unsigned char register_address, unsigned char data)
{
    unsigned char buffer[2] = {register_address, data};
    i2c_write_blocking(this->i2c, this->device_address, buffer, 2, false);
}
#pragma endregion

#pragma region Public methods
/**
 * @brief Set all the registers of the USB PD chip to the data object held in cache
*/
void FUSB302::setAllRegisters()
{
    this->setSwitches0();
    this->setSwitches1();
    this->setMeasure();
    this->setSlice();
    this->setControl0();
    this->setControl1();
    this->setControl2();
    this->setControl3();
    this->setMask1();
    this->setPower();
    this->setReset();
    this->setOCPreg();
    this->setMaskA();
    this->setMaskB();
    this->setControl4();
    this->setFIFO();
}

/**
 * @brief get all the data from the USB PD chip and store it in the data object held in cache
*/
void FUSB302::getAllRegisters()
{
    this->getDeviceID();
    this->getSwitches0();
    this->getSwitches1();
    this->getMeasure();
    this->getSlice();
    this->getControl0();
    this->getControl1();
    this->getControl2();
    this->getControl3();
    this->getMask1();
    this->getPower();
    this->getOCPreg();
    this->getMaskA();
    this->getMaskB();
    this->getControl4();
    this->getStatus0A();
    this->getStatus1A();
    this->getInterruptA();
    this->getInterruptB();
    this->getStatus0();
    this->getStatus1();
    this->getInterrupt();
    this->getFIFO();
}

/**
 * @brief Write data from cache to Switches0 register
*/
void FUSB302::setSwitches0()
{
    this->writeWord(FUSB302_SWITCHES0_ADDR, this->data.switches0.get());
}

/**
 * @brief Write data from cache to Switches1 register
*/
void FUSB302::setSwitches1()
{
    this->writeWord(FUSB302_SWITCHES1_ADDR, this->data.switches1.get());
}

/**
 * @brief Write data from cache to Measure register
*/
void FUSB302::setMeasure()
{
    this->writeWord(FUSB302_MEASURE_ADDR, this->data.measure.get());
}

/**
 * @brief Write data from cache to Slice register
*/
void FUSB302::setSlice()
{
    this->writeWord(FUSB302_SLICE_ADDR, this->data.slice.get());
}

/**
 * @brief Write data from cache to Control0 register
*/
void FUSB302::setControl0()
{
    this->writeWord(FUSB302_CONTROL0_ADDR, this->data.control0.get());
}

/**
 * @brief Write data from cache to Control1 register
*/
void FUSB302::setControl1()
{
    this->writeWord(FUSB302_CONTROL1_ADDR, this->data.control1.get());
}

/**
 * @brief Write data from cache to Control2 register
*/
void FUSB302::setControl2()
{
    this->writeWord(FUSB302_CONTROL2_ADDR, this->data.control2.get());
}

/**
 * @brief Write data from cache to Control3 register
*/
void FUSB302::setControl3()
{
    this->writeWord(FUSB302_CONTROL3_ADDR, this->data.control3.get());
}

/**
 * @brief Write data from cache to Mask1 register
*/
void FUSB302::setMask1()
{
    this->writeWord(FUSB302_MASK_ADDR, this->data.mask.get());
}

/**
 * @brief Write data from cache to Power register
*/
void FUSB302::setPower()
{
    this->writeWord(FUSB302_POWER_ADDR, this->data.power.get());
}

/**
 * @brief Write data from cache to Reset register
*/
void FUSB302::setReset()
{
    this->writeWord(FUSB302_RESET_ADDR, this->data.reset.get());
}

/**
 * @brief Write data from cache to OCPreg register
*/
void FUSB302::setOCPreg()
{
    this->writeWord(FUSB302_OCPREG_ADDR, this->data.ocpreg.get());
}

/**
 * @brief Write data from cache to MaskA register
*/
void FUSB302::setMaskA()
{
    this->writeWord(FUSB302_MASKA_ADDR, this->data.maska.get());
}

/**
 * @brief Write data from cache to MaskB register
*/
void FUSB302::setMaskB()
{
    this->writeWord(FUSB302_MASKB_ADDR, this->data.maskb.get());
}

/**
 * @brief Write data from cache to Control4 register
*/
void FUSB302::setControl4()
{
    this->writeWord(FUSB302_CONTROL4_ADDR, this->data.control4.get());
}

/**
 * @brief Write data from cache to FIFO register
 * @note Writes the local TX FIFO to the USB PD chip
*/
void FUSB302::setFIFO()
{
    this->writeWord(FUSB302_FIFO_ADDR, this->data.fifo.getTX());
}

/**
 * @brief Read data from Device ID register
*/
void FUSB302::getDeviceID()
{
    this->data.deviceid = this->readWord(FUSB302_DEVICE_ID_ADDR);
}

/**
 * @brief Read data from Switches0 register
*/
void FUSB302::getSwitches0()
{
    this->data.switches0 = this->readWord(FUSB302_SWITCHES0_ADDR);
}

/**
 * @brief Read data from Switches1 register
*/
void FUSB302::getSwitches1()
{
    this->data.switches1 = this->readWord(FUSB302_SWITCHES1_ADDR);
}

/**
 * @brief Read data from Measure register
*/
void FUSB302::getMeasure()
{
    this->data.measure = this->readWord(FUSB302_MEASURE_ADDR);
}

/**
 * @brief Read data from Slice register
*/
void FUSB302::getSlice()
{
    this->data.slice = this->readWord(FUSB302_SLICE_ADDR);
}

/**
 * @brief Read data from Control0 register
*/
void FUSB302::getControl0()
{
    this->data.control0 = this->readWord(FUSB302_CONTROL0_ADDR);
}

/**
 * @brief Read data from Control1 register
*/
void FUSB302::getControl1()
{
    this->data.control1 = this->readWord(FUSB302_CONTROL1_ADDR);
}

/**
 * @brief Read data from Control2 register
*/
void FUSB302::getControl2()
{
    this->data.control2 = this->readWord(FUSB302_CONTROL2_ADDR);
}

/**
 * @brief Read data from Control3 register
*/
void FUSB302::getControl3()
{
    this->data.control3 = this->readWord(FUSB302_CONTROL3_ADDR);
}

/**
 * @brief Read data from Mask1 register
*/
void FUSB302::getMask1()
{
    this->data.mask = this->readWord(FUSB302_MASK_ADDR);
}

/**
 * @brief Read data from Power register
*/
void FUSB302::getPower()
{
    this->data.power = this->readWord(FUSB302_POWER_ADDR);
}

/**
 * @brief Read data from OCPreg register
*/
void FUSB302::getOCPreg()
{
    this->data.ocpreg = this->readWord(FUSB302_OCPREG_ADDR);
}

/**
 * @brief Read data from MaskA register
*/
void FUSB302::getMaskA()
{
    this->data.maska = this->readWord(FUSB302_MASKA_ADDR);
}

/**
 * @brief Read data from MaskB register
*/
void FUSB302::getMaskB()
{
    this->data.maskb = this->readWord(FUSB302_MASKB_ADDR);
}

/**
 * @brief Read data from Control4 register
*/
void FUSB302::getControl4()
{
    this->data.control4 = this->readWord(FUSB302_CONTROL4_ADDR);
}

/**
 * @brief Read data from Status0A register
*/
void FUSB302::getStatus0A()
{
    this->data.status0 = this->readWord(FUSB302_STATUS0A_ADDR);
}

/**
 * @brief Read data from Status1A register
*/
void FUSB302::getStatus1A()
{
    this->data.status1 = this->readWord(FUSB302_STATUS1A_ADDR);
}

/**
 * @brief Read data from InterruptA register
*/
void FUSB302::getInterruptA()
{
    this->data.interrupta = this->readWord(FUSB302_INTERRUPTA_ADDR);
}

/**
 * @brief Read data from InterruptB register
*/
void FUSB302::getInterruptB()
{
    this->data.interruptb = this->readWord(FUSB302_INTERRUPTB_ADDR);
}

/**
 * @brief Read data from Status0 register
*/
void FUSB302::getStatus0()
{
    this->data.status0 = this->readWord(FUSB302_STATUS0_ADDR);
}

/**
 * @brief Read data from Status1 register
*/
void FUSB302::getStatus1()
{
    this->data.status1 = this->readWord(FUSB302_STATUS1_ADDR);
}

/**
 * @brief Read data from Interrupt register
*/
void FUSB302::getInterrupt()
{
    this->data.interrupt = this->readWord(FUSB302_INTERRUPT_ADDR);
}

/**
 * @brief Read data from FIFO register
 * @note Read the FIFO registers into the local RX FIFO cache!
*/
void FUSB302::getFIFO()
{
    this->data.fifo.setRX(this->readWord(FUSB302_FIFO_ADDR));
}
#pragma endregion