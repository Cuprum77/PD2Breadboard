# INA219 Library
This library provides a simple interface to the INA219 current sensor chip onboard the USB-PD board. The INA219 chip is a current monitoring IC with power monitoring capabilities. It can read up to 32V and supports a wide range of currents. The library provides a simple interface to read the current, voltage, and power from the INA219 chip. Additionally, the library provides a simple interface to configure the INA219 chip.

## Usage
To use the library, simply include the header file in your code:
```cpp
#include "INA219.hpp"
```

### Initialization
To initialize the INA219 library, simply create a new INA219 object where you provide the address of the INA219 chip and the I2C bus that it is connected to.
```cpp
// Create a new INA219 object that is connected to the INA219 chip at address 0x40 on I2C bus 0
INA219 ina219(0x40, i2c0);
```

## Accessing data
The library has been designed around the idea that it should only poll the device when the user request to. This provides a fast response time and prevents delays during critical operations.

### Reading data
To read data from the INA219 chip, call the `getData` function. This features a boolean parameter that allows the user to fetch all data or just the important data (current, voltage, and power). The function will not return anything, but load the data into the class variables that can be accessed later.
```cpp
// Read all data from the INA219 chip
ina219.getData(true);
// Read only the important data from the INA219 chip
ina219.getData(false);
```

### Writing configuration
To move the configuration data from the library variables to the device, the function `setData` must be called. This will write both the configuration and calibration data to the chip.
```cpp
// Write the configuration data to the INA219 chip
ina219.setData();
```

### Reset
To perform a soft reset, call the `reset` function. This will reset the configuration data to the default values.
Note: This will be sent directly to the chip!
```cpp
// Perform a soft reset
ina219.reset();
```

## Shunt voltage
### Get raw data
To get the raw data from the shunt voltage register, call the `getShuntVoltageRaw` function. This will return the raw data from the shunt voltage register.
```cpp
// Get the raw data from the shunt voltage register
unsigned short shuntVoltageRaw = ina219.getShuntVoltageRaw();
```
### Get shunt voltage
To get the shunt voltage from the INA219 chip, call the `getShuntVoltage` function. This will return the shunt voltage in mV as a double.
```cpp
// Get the shunt voltage from the INA219 chip
double shuntVoltage = ina219.getShuntVoltage();
```

## Current
To get the current from the INA219 chip directly, you have to write the calibration data to the device first!
### Get raw data
To get the raw data from the current register, call the `getCurrentRaw` function. This will return the raw data from the current register.
```cpp
// Get the raw data from the current register
unsigned short currentRaw = ina219.getCurrentRaw();
```
### Get current
To get the current from the INA219 chip, call the `getCurrent` function. This will return the current in mA as a double.
```cpp
// Get the current from the INA219 chip
double current = ina219.getCurrent();
```

## Voltage
### Get raw data
To get the raw data from the voltage register, call the `getVoltageRaw` function. This will return the raw data from the voltage register.
```cpp
// Get the raw data from the voltage register
unsigned short voltageRaw = ina219.getVoltageRaw();
```
### Get voltage
To get the voltage from the INA219 chip, call the `getVoltage` function. This will return the voltage in mV as a double.
```cpp
// Get the voltage from the INA219 chip
double voltage = ina219.getVoltage();
```

## Power
To get the power from the INA219 chip directly, you have to write the calibration data to the device first!
### Get raw data
To get the raw data from the power register, call the `getPowerRaw` function. This will return the raw data from the power register.
```cpp
// Get the raw data from the power register
unsigned short powerRaw = ina219.getPowerRaw();
```
### Get power
To get the power from the INA219 chip, call the `getPower` function. This will return the power in mW as a double.
```cpp
// Get the power from the INA219 chip
double power = ina219.getPower();
```

## Configuration

### Bus voltage range
#### Set the range
To set the bus voltage range, call the `setBusVoltageRange` function. This function takes a `INA219_BusVoltageRange` enum as a parameter. The enum has the following values:
* `INA219_BUS_VOLTAGE_RANGE_16V` - 16V
* `INA219_BUS_VOLTAGE_RANGE_32V` - 32V
```cpp
// Set the bus voltage range to 16V
ina219.setBusVoltageRange(INA219_BUS_VOLTAGE_RANGE_16V);
```
#### Get the range
To get the bus voltage range, call the `getBusVoltageRange` function. This function returns a `INA219_BusVoltageRange` enum.
```cpp
// Get the bus voltage range
INA219_BusVoltageRange busVoltageRange = ina219.getBusVoltageRange();
```

### Gain
#### Set the gain
To set the gain, call the `setGain` function. This function takes a `INA219_Gain` enum as a parameter. The enum has the following values:
* `INA219_GAIN_40MV` - 40mV
* `INA219_GAIN_80MV` - 80mV
* `INA219_GAIN_160MV` - 160mV
* `INA219_GAIN_320MV` - 320mV
```cpp
// Set the gain to 40mV
ina219.setGain(INA219_GAIN_40MV);
```
#### Get the gain
To get the gain, call the `getGain` function. This function returns a `INA219_Gain` enum.
```cpp
// Get the gain
INA219_Gain gain = ina219.getGain();
```

### Bus voltage ADC resolution
#### Set the resolution
To set the bus voltage ADC resolution, call the `setBusADCResolution` function. This function takes a `INA219_ADCResolution` enum as a parameter. The enum has the following values:
* `INA219_ADC_RESOLUTION_9BIT` - 9-bit
* `INA219_ADC_RESOLUTION_10BIT` - 10-bit
* `INA219_ADC_RESOLUTION_11BIT` - 11-bit
* `INA219_ADC_RESOLUTION_12BIT` - 12-bit
* `INA219_2SAMPLES_1060US` - 2 samples
* `INA219_4SAMPLES_2130US` - 4 samples
* `INA219_8SAMPLES_4260US` - 8 samples
* `INA219_16SAMPLES_8510US` - 16 samples
* `INA219_32SAMPLES_17MS` - 32 samples
* `INA219_64SAMPLES_34MS` - 64 samples
* `INA219_128SAMPLES_69MS` - 128 samples
```cpp
// Set the bus voltage ADC resolution to 12-bit
ina219.setBusADCResolution(INA219_ADC_RESOLUTION_12BIT);
```
#### Get the resolution
To get the bus voltage ADC resolution, call the `getBusADCResolution` function. This function returns a `INA219_ADCResolution` enum.
```cpp
// Get the bus voltage ADC resolution
INA219_ADCResolution busADCResolution = ina219.getBusADCResolution();
```

### Shunt voltage ADC resolution
#### Set the resolution
To set the shunt voltage ADC resolution, call the `setShuntADCResolution` function. This function takes a `INA219_ADCResolution`.
```cpp
// Set the shunt voltage ADC resolution to 12-bit
ina219.setShuntADCResolution(INA219_ADC_RESOLUTION_12BIT);
```
#### Get the resolution
To get the shunt voltage ADC resolution, call the `getShuntADCResolution` function. This function returns a `INA219_ADCResolution` enum.
```cpp
// Get the shunt voltage ADC resolution
INA219_ADCResolution shuntADCResolution = ina219.getShuntADCResolution();
```

### Mode
#### Set the mode
To set the mode, call the `setMode` function. This function takes a `INA219_Mode` enum as a parameter. The enum has the following values:
* `INA219_MODE_POWER_DOWN` - Power down
* `INA219_MODE_SHUNT_VOLTAGE_TRIGGERED` - Shunt voltage, triggered
* `INA219_MODE_BUS_VOLTAGE_TRIGGERED` - Bus voltage, triggered
* `INA219_MODE_SHUNT_AND_BUS_VOLTAGE_TRIGGERED` - Shunt and bus voltage, triggered
* `INA219_MODE_ADC_OFF` - ADC off
* `INA219_MODE_SHUNT_VOLTAGE_CONTINUOUS` - Shunt voltage, continuous
* `INA219_MODE_BUS_VOLTAGE_CONTINUOUS` - Bus voltage, continuous
* `INA219_MODE_SHUNT_AND_BUS_VOLTAGE_CONTINUOUS` - Shunt and bus voltage, continuous
```cpp
// Set the mode to shunt and bus voltage, continuous
ina219.setMode(INA219_MODE_SHUNT_AND_BUS_VOLTAGE_CONTINUOUS);
```
#### Get the mode
To get the mode, call the `getMode` function. This function returns a `INA219_Mode` enum.
```cpp
// Get the mode
INA219_Mode mode = ina219.getMode();
```

## Calibration
### Get the calibration
To get the calibration, call the `getCalibration` function. This function returns the calibration as an unsigned short.
```cpp
// Get the calibration
unsigned short calibration = ina219.getCalibration();
```
### Set the calibration manually
To set the calibration manually, call the `setCalibration` function. This function takes an unsigned short as a parameter.
To calculate the calibration value, use the following formula:
```
calibration = 0.04096 / (current_LSB * r_shunt)
```
Where:
* `current_LSB` is the desired resolution. Realistically this should be 1mA or above. Which means the value should be 0.001.
* `r_shunt` is the shunt resistor value in ohms.
```cpp
// Set the calibration to 40960
ina219.setCalibration(40960);
```

### Set the calibration automatically
To set the calibration as per the project requirements, call the `setCalibration` function. This will set the calibration data to the project defaults which happens to be a 1mOhm shunt resistor with a resolution of 1mA
```cpp
// Set the calibration to the project defaults
ina219.setCalibration();
```

## Test functionality
### Verify the connection
To verify the connection, call the `verifyConnection` function. This function returns a boolean value. If the connection is successful, the function will return `true`. If the connection is unsuccessful, the function will return `false`.
```cpp
// Verify the connection
bool connection = ina219.verifyConnection();
```

## Example
```cpp
#include <INA219.h>

// Create an instance of the INA219 class with the address set to 0x40 and the I2C bus set to I2C0
INA219 ina219(0x40, i2c0);

int main()
{
    // Reset the chip to its factory defaults
    ina219.reset();
    // Fetch the default chip configuration
    ina219.getData(true);
    // Set the calibration data to the default values
    ina219.setCalibration();
    // Write the configuration to the chip
    ina219.setData();

    // Loop forever
    while(1)
    {
        // Get the voltage
        double voltage = ina219.getVoltage();
        // Get the current
        double current = ina219.getCurrent();
        // Get the power
        double power = ina219.getPower();

        // Print the data
        printf("Voltage: %fV\r\n", busVoltage);
        printf("Current: %fmA\r\n", current);
        printf("Power: %fmW\r\n", power);
        printf("\r\n");

        // Wait for 1 second
        sleep_ms(1000);
    }
}
