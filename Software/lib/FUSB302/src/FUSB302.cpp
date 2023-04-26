#include "FUSB302.hpp"

/**
 * @brief Get the Data object
 * @return FUSB302_Data
 */
FUSB302_Data FUSB302::getData()
{
    return this->data;
}

/**
 * @brief get all the data from the USB PD and store it in the data object held in cache
*/
void FUSB302::getAllData()
{
    // read all the data from the USB PD
    this->data.deviceid = this->readWord(FUSB302_DEVICE_ID_ADDR);
    this->data.switches0 = this->readWord(FUSB302_SWITCHES0_ADDR);
    this->data.switches1 = this->readWord(FUSB302_SWITCHES1_ADDR);
    this->data.measure = this->readWord(FUSB302_MEASURE_ADDR);
    this->data.slice = this->readWord(FUSB302_SLICE_ADDR);
    this->data.control0 = this->readWord(FUSB302_CONTROL0_ADDR);
    this->data.control1 = this->readWord(FUSB302_CONTROL1_ADDR);
    this->data.control2 = this->readWord(FUSB302_CONTROL2_ADDR);
    this->data.control3 = this->readWord(FUSB302_CONTROL3_ADDR);
    this->data.mask = this->readWord(FUSB302_MASK_ADDR);
    this->data.power = this->readWord(FUSB302_POWER_ADDR);
    this->data.ocpreg = this->readWord(FUSB302_OCPREG_ADDR);
    this->data.maska = this->readWord(FUSB302_MASKA_ADDR);
    this->data.maskb = this->readWord(FUSB302_MASKB_ADDR);
    this->data.control4 = this->readWord(FUSB302_CONTROL4_ADDR);
    this->data.status0a = this->readWord(FUSB302_STATUS0A_ADDR);
    this->data.status1a = this->readWord(FUSB302_STATUS1A_ADDR);
    this->data.interrupta = this->readWord(FUSB302_INTERRUPTA_ADDR);
    this->data.interruptb = this->readWord(FUSB302_INTERRUPTB_ADDR);
    this->data.status0 = this->readWord(FUSB302_STATUS0_ADDR);
    this->data.status1 = this->readWord(FUSB302_STATUS1_ADDR);
    this->data.interrupt = this->readWord(FUSB302_INTERRUPT_ADDR);
    this->data.fifo.RXFIFO = this->readWord(FUSB302_FIFO_ADDR);
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