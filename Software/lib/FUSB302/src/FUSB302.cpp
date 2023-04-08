#include <FUSB302.hpp>

/**
 * @brief USB PD constructor
 * @param wire Wire object
 * @param address USB PD address
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