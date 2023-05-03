#include "USB_PD.hpp"

/**
 * @brief Construct a new USB_PD::USB_PD object
*/
USB_PD::USB_PD(unsigned char address, i2c_inst_t* wire, unsigned int pin) : FUSB302(address, wire)
{
    // set the interrupt pin as an input, we will not use it as a pure interrupt
    this->interruptPin = pin;
    gpio_init(this->interruptPin);
    gpio_set_dir(this->interruptPin, GPIO_IN);
    gpio_pull_up(this->interruptPin);
}

/**
 * @brief Initialize the USB PD controller
*/
void USB_PD::init()
{
    // we do not need to check if a power supply is activated
    // as it literally provides all the power to the device!

    // despite the chip resetting on power up, we reset it anyway for good measure
    this->reset();

    // fetch all the data from the FUSB302
    this->getAllRegisters();

    // check if VBUS is OK
    if(this->getVBUSOK())
        this->vbusOK = true;

    // to enable source mode, we need to enable pull down on both CC lines
    this->setPullDown(FUSB302_CC::Both_CC);
}

/**
 * @brief Runs at every loop, is essential for the USB PD controller to function
*/
void USB_PD::update()
{
    // get the most essential data from the FUSB302
    this->getStatus0A();
    this->getStatus1A();
    this->getInterruptA();
    this->getInterruptB();
    this->getStatus0();
    this->getStatus1();
    this->getInterrupt();
}

/**
 * @brief Get the connection status of the USB PD controller
 * @note This will check if the power supply is PD and PPS capable
*/
void USB_PD::getConnection()
{
    // get the device voltages
    //this->getDeviceCaps();
}

/**
 * @brief Reset the USB PD controller
*/
void USB_PD::reset()
{
    // set the reset register
    this->setSWReset();
    // transmit the reset register
    this->setReset();
}

/**
 * @brief Enable PPS on the USB PD controller
*/
void USB_PD::enablePPS()
{

}

/**
 * @brief Returns true if the power supply is PD capable
 * @return true if PD capable
*/
bool USB_PD::pdCapable()
{
    return this->pdEnabled;
}

/**
 * @brief Returns true if the power supply is PPS capable
 * @return true if PPS capable
*/
bool USB_PD::ppsCapable()
{
    return this->ppsEnabled;
}

/**
 * @brief Return the supported voltages of the power supply
 * @return the supported voltages of the power supply
*/
USB_PD_Voltages USB_PD::supportedVoltages()
{
    return this->supportedVoltage;
}

/**
 * @brief Verify the connection to the USB PD controller
 * @return true if the connection is valid
*/
bool USB_PD::verifyConnection()
{
    return FUSB302::verifyConnection();
}

/**
 * @brief Perform a self test on the USB PD controller
 * @return the result of the self test as an integer (0 = OK, any other value = error)
 * @note This function is not implemented yet!
*/
int USB_PD::selfTest()
{
    // create a variable to store the result of the self test
    int selfTestResult = FUSB302_SELF_TEST_OK;

    // perform the self test

    // check if PD is enabled
    if(!this->pdEnabled)
        selfTestResult |= FUSB302_SELF_TEST_PD_ERROR;
    // check if PPS is enabled
    if(!this->ppsEnabled)
        selfTestResult |= FUSB302_SELF_TEST_PPS_ERROR;

    // return the errors
    return selfTestResult;
}

/**
 * @brief Convert the self test result to a string
 * @param selfTestResult the result of the self test as an integer (0 = OK, any other value = error)
 * @return the result of the self test as a string
*/
const char* USB_PD::selfTestToString(int selfTestResult)
{
    // store the seperator and space in static variables so we don't have to create them every time
    static char seperator[] = "\n  - ";

    // clear the error buffer
    for(int i = 0; i < sizeof(this->errorBuffer); i++)
        this->errorBuffer[i] = 0;

    // store index
    int index = 0;
    // count the number of errors
    int errorCount = 0;
    int number = selfTestResult;

    while(number)
    {
        errorCount += number & 1;
        number >>= 1;
    }

    // add the seperator
    strcpy(this->errorBuffer + index, seperator);
    // increment the index
    index += strlen(seperator);

    // get all the errors from the result
    if(selfTestResult == FUSB302_Self_Test::FUSB302_SELF_TEST_OK)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, FUSB302_ERROR_OK);
        return this->errorBuffer;
    }
    
    if(selfTestResult & FUSB302_Self_Test::FUSB302_SELF_TEST_PD_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, FUSB302_ERROR_PD);
        // increment the index
        index += strlen(FUSB302_ERROR_PD);
        errorCount--;

        if(errorCount)
        {
            // add seperator
            strcpy(this->errorBuffer + index, seperator);
            // increment the index
            index += strlen(seperator);
        }
    }
    if(selfTestResult & FUSB302_Self_Test::FUSB302_SELF_TEST_PPS_ERROR)
    {
        // copy the error to the buffer
        strcpy(this->errorBuffer + index, FUSB302_ERROR_PPS);
        // increment the index
        index += strlen(FUSB302_ERROR_PPS);
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
 * @brief Interrupt handler for the USB PD controller
*/
void USB_PD::interruptHandler()
{
    // fetch all the data from the FUSB302
    this->getAllRegisters();
}

/**
 * @brief Get all the registers from the USB PD controller and print them to the console
*/
void USB_PD::printRegisters()
{
    this->getAllRegisters();
    auto data = this->getData();
    printf("FUSB302 Registers:\n");
    printf("  - Device ID:  0x%02X\n", data.deviceid.get());
    printf("  - Switch 0:   0x%02X\n", data.switches0.get());
    printf("  - Switch 1:   0x%02X\n", data.switches1.get());
    printf("  - Measure:    0x%02X\n", data.measure.get());
    printf("  - Slice:      0x%02X\n", data.slice.get());
    printf("  - Control 0:  0x%02X\n", data.control0.get());
    printf("  - Control 1:  0x%02X\n", data.control1.get());
    printf("  - Control 2:  0x%02X\n", data.control2.get());
    printf("  - Control 3:  0x%02X\n", data.control3.get());
    printf("  - Mask:       0x%02X\n", data.mask.get());
    printf("  - Power:      0x%02X\n", data.power.get());
    printf("  - Reset:      0x%02X\n", data.reset.get());
    printf("  - OCPREG:     0x%02X\n", data.ocpreg.get());
    printf("  - MaskA:      0x%02X\n", data.maska.get());
    printf("  - MaskB:      0x%02X\n", data.maskb.get());
    printf("  - Control 4:  0x%02X\n", data.control4.get());
    printf("  - Status 0A:  0x%02X\n", data.status0a.get());
    printf("  - Status 1A:  0x%02X\n", data.status1a.get());
    printf("  - InterruptA: 0x%02X\n", data.interrupta.get());
    printf("  - InterruptB: 0x%02X\n", data.interruptb.get());
    printf("  - Status 0:   0x%02X\n", data.status0.get());
    printf("  - Status 1:   0x%02X\n", data.status1.get());
    printf("  - Interrupt:  0x%02X\n", data.interrupt.get());
    printf("  - FIFOS:      0x%02X\n", data.fifo.getTX());
}

/**
 * @brief Return the device ID of the USB PD controller
 * @return the device ID of the USB PD controller
*/
int USB_PD::deviceID()
{
    // return the device ID
    return this->getID();
}

/**
 * @brief Return the product ID of the USB PD controller
 * @return the product ID of the USB PD controller
*/
int USB_PD::productID()
{
    // return the product ID
    return this->getProductID();
}

/**
 * @brief Return the revision ID of the USB PD controller
 * @return the revision ID of the USB PD controller
*/
int USB_PD::revisionID()
{
    // return the revision ID
    return this->getRevisionID();
}
