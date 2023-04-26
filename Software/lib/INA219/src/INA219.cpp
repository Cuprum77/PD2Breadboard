#include "INA219.hpp"

/**
 * @brief Construct a new INA219:: INA219 object
 * @param address the address of the INA219
 * @param i2c the i2c bus the INA219 is connected to
*/
INA219::INA219(unsigned int register_address, i2c_inst_t* i2c)
{
    this->device_address = register_address;
    this->i2c = i2c;
}

/**
 * @brief get the data off the INA219
*/
void INA219::getData(bool all)
{
    this->data.shuntVoltage = readWord(INA219_SHUNT_VOLTAGE_ADDR);
    this->data.busVoltage = readWord(INA219_BUS_VOLTAGE_ADDR);
    this->data.power = readWord(INA219_POWER_ADDR);
    this->data.current = readWord(INA219_CURRENT_ADDR);

    // if we dont need all the data, return
    if(!all)
        return;

    this->data.configuration = readWord(INA219_CONFIGURATION_ADDR);
    this->data.calibration = readWord(INA219_CALIBRATION_ADDR);
}

/**
 * @brief set the data on the INA219
 * @note this ONLY sets the calibration and configuration registers!
*/
void INA219::setData()
{
    writeWord(INA219_CALIBRATION_ADDR, this->data.calibration);
    writeWord(INA219_CONFIGURATION_ADDR, this->data.configuration.get());
}

/**
 * @brief reset the INA219
*/
void INA219::reset()
{
    // reset the INA219 by writing 0x8000 to the configuration register
    writeWord(INA219_CONFIGURATION_ADDR, 0x8000);
}

/**
 * @brief set the bus voltage range
 * @param range the bus voltage range
*/
void INA219::setBusVoltageRange(INA219_BusVoltageRange range)
{
    // set the bus voltage range
    this->data.configuration.BRNG = range;
}

/**
 * @brief set the gain
 * @param gain the gain
*/
void INA219::setGain(INA219_Gain gain)
{
    // set the gain
    this->data.configuration.PG = gain;
}

/**
 * @brief set the bus ADC resolution
 * @param resolution the bus ADC resolution
*/
void INA219::setBusADCResolution(INA219_ADCResolution resolution)
{
    // set the bus ADC resolution
    this->data.configuration.BADC = resolution;
}

/**
 * @brief set the shunt ADC resolution
 * @param resolution the shunt ADC resolution
*/
void INA219::setShuntADCResolution(INA219_ADCResolution resolution)
{
    // set the shunt ADC resolution
    this->data.configuration.SADC = resolution;
}

/**
 * @brief set the mode
 * @param mode the mode
*/
void INA219::setMode(INA219_Mode mode)
{
    // set the mode
    this->data.configuration.MODE = mode;
}

/**
 * @brief get the bus voltage range
 * @return the bus voltage range
*/
INA219_BusVoltageRange INA219::getBusVoltageRange()
{
    return (INA219_BusVoltageRange)this->data.configuration.BRNG;
}

/**
 * @brief get the gain
 * @return the gain
*/
INA219_Gain INA219::getGain()
{
    return (INA219_Gain)this->data.configuration.PG;
}

/**
 * @brief get the bus ADC resolution
 * @return the bus ADC resolution
*/
INA219_ADCResolution INA219::getBusADCResolution()
{
    return (INA219_ADCResolution)this->data.configuration.BADC;
}

/**
 * @brief get the shunt ADC resolution
 * @return the shunt ADC resolution
*/
INA219_ADCResolution INA219::getShuntADCResolution()
{
    return (INA219_ADCResolution)this->data.configuration.SADC;
}

/**
 * @brief get the mode
 * @return the mode
*/
INA219_Mode INA219::getMode()
{
    return (INA219_Mode)this->data.configuration.MODE;
}

/**
 * @brief get the shunt raw value
 * @return the shunt raw value
*/
unsigned short INA219::getShuntVoltageRaw()
{
    return this->data.shuntVoltage;
}

/**
 * @brief get the bus raw value
 * @return the bus raw value
*/
unsigned short INA219::getBusVoltageRaw()
{
    // check if the bus voltage is valid, if not return 0
    if(this->data.busVoltage.OVF)
        return 0;

    return this->data.busVoltage.busVoltage;
}

/**
 * @brief get the power raw value
 * @return the power raw value
*/
unsigned short INA219::getPowerRaw()
{
    return this->data.power;
}

/**
 * @brief get the current raw value
 * @return the current raw value
*/
unsigned short INA219::getCurrentRaw()
{
    return this->data.current;
}


/**
 * @brief get the shunt voltage
 * @return the shunt voltage in millivolts
*/
double INA219::getShuntVoltage()
{
    // get the programmable gain amplifier
    unsigned short gain = this->data.configuration.PG;
    // fetch the shunt voltage
    unsigned short shunt_voltage = this->getShuntVoltageRaw();
    // store the sign bit
    unsigned short sign_bit = shunt_voltage & 0x8000;
    // depending on gain, we need to remove extra sign bits
    // gain 1 has 4 sign bits, gain 2 has 3 sign bits, gain 4 has 2 sign bits, gain 8 has 1 sign bit
    // we need to remove all but the first sign bit
    shunt_voltage &= 0x7fff >> (3 - gain);
    // add the sign bit back
    shunt_voltage |= sign_bit;
    // convert the shunt voltage to a double
    double voltage = (double)shunt_voltage;
    // multiply the shunt voltage by the LSB value which is defined by SHUNT_VOLTAGE_LSB_VALUE
    voltage *= SHUNT_VOLTAGE_LSB_VALUE;
    return voltage * 1000;
}

/**
 * @brief get the bus voltage
 * @return the bus voltage in volts
*/
double INA219::getVoltage()
{
    // get the bus voltage
    double bus_voltage = (double)this->getBusVoltageRaw();
    // multiply the voltage by the LSB value which is defined by BUS_VOLTAGE_LSB_VALUE
    bus_voltage *= BUS_VOLTAGE_LSB_VALUE;
    // return the bus voltage
    return bus_voltage;
}

/**
 * @brief get the current
 * @return the current in milliamps
*/
double INA219::getCurrent()
{
    // calculate the current
    double current = ((double)this->getCurrentRaw()) * CURRENT_RESOLUTION;
    // multiply the current by 1000 to get milli amps
    return current * 1000;
}

/**
 * @brief get the power
 * @return the power in milliwatts
*/
double INA219::getPower()
{
    // calculate the power LSB
    double power_lsb = CURRENT_RESOLUTION * 20;

    // calculate the power
    double power = ((double)this->getPowerRaw()) * power_lsb;
    // multiply the power by 1000 to get milli watts
    return power * 1000;
}

/**
 * @brief get the calibration register
 * @return the calibration register
*/
unsigned short INA219::getCalibration()
{
    return this->data.calibration;
}

/**
 * @brief set the calibration register
 * @param cal the calibration register
*/
void INA219::setCalibration(unsigned short cal)
{
    this->data.calibration = cal;
}

/**
 * @brief set the calibration register
 * @param r_shunt the shunt resistor value in ohms
 * @param i_max_expected the maximum expected current in amps
*/
void INA219::setCalibration()
{
    // calculate the calibration register
    unsigned short cal = (unsigned short)((0.04096) / (SHUNT_RESISTOR * CURRENT_RESOLUTION));
    // set the calibration register
    this->data.calibration = cal;
}

/**
 * @brief Verify that the INA219 is connected
 * @return true if the INA219 is connected, false otherwise
*/
bool INA219::verifyConnection()
{
    // check if we get a response from the eeprom by dummy writing to it
    unsigned char data;
    // if the dummy write fails, it should return a negative number
    int ret = i2c_read_blocking(this->i2c, this->device_address, &data, 1, false);
    return !(ret < 0);
}

/**
 * @brief Test the INA219
 * @return errors in the INA219 test
 * @note This works by comparing the values read directly from the INA219 with the values already set in the library
*/
int INA219::selfTest()
{
    // create a variable to store the errors in
    int errors = 0;
    // however, the voltage and current values may change since they were fetched from the INA219
    // so we need to fetch them again
    this->getData(true);
    // allow a slight deviation in the voltage and current values
    // we can be quite generous as its not critical to have the exact voltage
    int allowed_deviation = 100;

    // check if the configuration register is correct
    unsigned short data = this->readWord(INA219_CONFIGURATION_ADDR);
    if(data != this->data.configuration.get())
        errors |= INA219_SELF_TEST_CONFIGURATION_ERROR;

    // check if the shunt voltage is correct
    data = this->readWord(INA219_SHUNT_VOLTAGE_ADDR);
    if(data != this->data.shuntVoltage &&
        data > (this->data.shuntVoltage + allowed_deviation)
        || data < (this->data.shuntVoltage - allowed_deviation))
        errors |= INA219_SELF_TEST_SHUNT_VOLTAGE_ERROR;

    // check if the bus voltage is correct
    data = this->readWord(INA219_BUS_VOLTAGE_ADDR);
    if(data != this->data.busVoltage.get() &&
        data > (this->data.busVoltage.get() + allowed_deviation)
        || data < (this->data.busVoltage.get() - allowed_deviation))
        errors |= INA219_SELF_TEST_BUS_VOLTAGE_ERROR;

    // check if the current is correct
    data = this->readWord(INA219_CURRENT_ADDR);
    if(data != this->data.current &&
        data > (this->data.current + allowed_deviation)
        || data < (this->data.current - allowed_deviation))
        errors |= INA219_SELF_TEST_CURRENT_ERROR;

    // check if the power is correct
    data = this->readWord(INA219_POWER_ADDR);
    if(data != this->data.power &&
        data > (this->data.power + allowed_deviation)
        || data < (this->data.power - allowed_deviation))
        errors |= INA219_SELF_TEST_POWER_ERROR;

    // check if the calibration register is correct
    data = this->readWord(INA219_CALIBRATION_ADDR);
    if(data != this->data.calibration)
        errors |= INA219_SELF_TEST_CALIBRATION_ERROR;

    // return the errors
    return errors;
}

/**
 * @brief Convert the self test errors to a string
 * @param selfTestResult the result to convert
 * @return the string representation of the errors
*/
const char* INA219::selfTestToString(int selfTestResult)
{
    // create an array of strings to store the errors in

    // store the seperator and space in static variables so we don't have to create them every time
    static char seperator[] = "\n  - ";

    // clear the error buffer
    for(int i = 0; i < sizeof(this->errorBuffer); i++)
        this->errorBuffer[i] = 0;

    // store index
    int index = 0;
    // count the number of errors
    int errorCount = this->countSetBits(selfTestResult);

    // add the seperator
    strcpy(this->errorBuffer + index, seperator);
    // increment the index
    index += strlen(seperator);

    // get all the errors from the result
    if(selfTestResult == INA219_Self_Test::INA219_SELF_TEST_OK)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_OK);
        return this->errorBuffer;
    }
    
    if(selfTestResult & INA219_SELF_TEST_CONFIGURATION_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_CONFIG);
        // increment the index
        index += strlen(ERROR_CONFIG);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & INA219_SELF_TEST_SHUNT_VOLTAGE_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_SHUNT_VOLTAGE);
        // increment the index
        index += strlen(ERROR_SHUNT_VOLTAGE);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & INA219_SELF_TEST_BUS_VOLTAGE_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_BUS_VOLTAGE);
        // increment the index
        index += strlen(ERROR_BUS_VOLTAGE);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & INA219_SELF_TEST_CURRENT_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_CURRENT);
        // increment the index
        index += strlen(ERROR_CURRENT);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & INA219_SELF_TEST_POWER_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_POWER);
        // increment the index
        index += strlen(ERROR_POWER);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & INA219_SELF_TEST_CALIBRATION_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, ERROR_CALIBRATION);
        // increment the index
        index += strlen(ERROR_CALIBRATION);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }

    return this->errorBuffer;
}


/**
 * @private
 * @brief Return how many bits are set in a number
 * @param number the number to check
 * @return the number of bits set
*/
unsigned int INA219::countSetBits(unsigned int number)
{
    unsigned int count = 0;
    while(number)
    {
        count += number & 1;
        number >>= 1;
    }
    return count;
}

/**
 * @private
 * @brief read a word from the INA219
 * @param target_address the address to read from
 * @param data the buffer to store the data in
 * @note as the RP2040 is a 32 bit microcontroller, a word is 4 bytes
*/
unsigned short INA219::readWord(unsigned char register_address)
{
    // create a two byte buffer to store the data in
    unsigned char buffer[2];
    // read the data from the INA219
    i2c_write_blocking(i2c, this->device_address, &register_address, 1, true);
    i2c_read_blocking(i2c, this->device_address, buffer, 2, false);

    // convert the data to a word
    unsigned short data = (buffer[0] << 8) | buffer[1];
    return data;
}

/**
 * @private
 * @brief write a word to the INA219
 * @param target_address the address to write to
 * @param data the word to write to the INA219
 * @note as the RP2040 is a 32 bit microcontroller, a word is 4 bytes
*/
void INA219::writeWord(unsigned char target_address, unsigned short data)
{
    // load the word into a 3 byte buffer, where first byte is the register address
    unsigned char bytes[3] = 
    { 
        target_address, 
        (unsigned char)((data >> 8) & 0xFF), 
        (unsigned char)(data & 0xFF)
    };
    // write the bytes to the INA219
    i2c_write_blocking(i2c, this->device_address, bytes, 3, false);
}