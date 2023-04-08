#pragma once

/***
 *      ____  _           
 *     |  _ \(_)_ __  ___ 
 *     | |_) | | '_ \/ __|
 *     |  __/| | | | \__ \
 *     |_|   |_|_| |_|___/
 *                        
 */

// I2C bus that the EEPROM and INA219 are connected to
#define I2C0_SDA            12
#define I2C0_SCL            13
// I2C bus that the FUSB302 is connected to
#define I2C1_SDA            2
#define I2C1_SCL            3
#define PD_INT_N            4
// Display constants
#define DISP_WIDTH          172
#define DISP_HEIGHT         320
#define DISP_OFFSET_X       34
#define DISP_OFFSET_Y       0
#define DISP_ROTATION       0
// Display pins
#define DISP_PIN_DC         16
#define DISP_PIN_CS         17
#define DISP_PIN_SCK        18
#define DISP_PIN_MOSI       19
#define DISP_PIN_RST        20
#define DISP_PIN_BL         21
// Button pins
#define BUTTON_UP           9
#define BUTTON_MENU         10
#define BUTTON_DOWN         11
// Mosfet pins and LEDs
#define LEFT_MOSFET_LED     14
#define LEFT_MOSFET         15
#define RIGHT_MOSFET_LED    22
#define RIGHT_MOSFET        26



/***
 *     __     __         _       _     _           
 *     \ \   / /_ _ _ __(_) __ _| |__ | | ___  ___ 
 *      \ \ / / _` | '__| |/ _` | '_ \| |/ _ \/ __|
 *       \ V / (_| | |  | | (_| | |_) | |  __/\__ \
 *        \_/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
 *                                                 
 */

// default variables
#define CURRENT_LIMIT_DEFAULT       3000    // mA
#define CURRENT_SETTING_DEFAULT     1000    // mA
#define VOLTAGE_SETTING_DEFAULT     5000    // mV
#define BACKLIGHT_DEFAULT           255     // 0-255

// serial interface codes
#define REBOOT_TO_BOOTLOADER_CODE   "reboot_to_bootloader"
#define RESET_CODE                  "reset"
#define GET_SETTINGS_CODE           "get_settings"

// INA219 codes
#define CURRENT_LIMIT_CODE          "current_limit:"
#define CURRENT_SETTING_CODE        "current_setting:"
#define VOLTAGE_SETTING_CODE        "voltage_setting:"
#define GET_READINGS_CODE           "get_readings"

// display codes
#define BACKLIGHT_CODE              "backlight:"

// general peripheral codes
#define TEST_CODE                   "test"



/***
 *      ___            _           _           
 *     |_ _|_ __   ___| |_   _  __| | ___  ___ 
 *      | || '_ \ / __| | | | |/ _` |/ _ \/ __|
 *      | || | | | (__| | |_| | (_| |  __/\__ \
 *     |___|_| |_|\___|_|\__,_|\__,_|\___||___/
 *                                             
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "tusb.h"

#include "Button.hpp"
#include "Display.hpp"
#include "FUSB302.hpp"
#include "INA219.hpp"
#include "Memory.hpp"



/***
 *         _       _     _                             
 *        / \   __| | __| |_ __ ___  ___ ___  ___  ___ 
 *       / _ \ / _` |/ _` | '__/ _ \/ __/ __|/ _ \/ __|
 *      / ___ \ (_| | (_| | | |  __/\__ \__ \  __/\__ \
 *     /_/   \_\__,_|\__,_|_|  \___||___/___/\___||___/
 *                                                     
 */

#define FUSB302_ADDRESS     0x22
#define INA219_ADDRESS      0x40
#define EEPROM_ADDRESS      0x50



/***
 *      ____  _         _____           _    ____        _        
 *     |  _ \(_) ___ __|_   _|__   ___ | |  |  _ \  __ _| |_ __ _ 
 *     | |_) | |/ __/ _ \| |/ _ \ / _ \| |  | | | |/ _` | __/ _` |
 *     |  __/| | (_| (_) | | (_) | (_) | |  | |_| | (_| | || (_| |
 *     |_|   |_|\___\___/|_|\___/ \___/|_|  |____/ \__,_|\__\__,_|
 *                                                               
 */

bi_decl(bi_program_name("USB-PD Power Supply"));
bi_decl(bi_program_description("Your own USB-PD variable power supply with an active circuit breaker."));
bi_decl(bi_program_version_string("1.0.0"));
bi_decl(bi_program_url("https://github.com/Cuprum77/PD2Breadboard"));

bi_decl(bi_4pins_with_names(DISP_PIN_CS, "SPI Chip Select", DISP_PIN_SCK, "SPI Clock", DISP_PIN_MOSI, "SPI Data", DISP_PIN_RST, "SPI Reset"));
bi_decl(bi_2pins_with_names(DISP_PIN_DC, "SPI Data Control", DISP_PIN_BL, "Display Backlight"));
bi_decl(bi_3pins_with_names(BUTTON_UP, "Button Up", BUTTON_MENU, "Button Menu", BUTTON_DOWN, "Button Down"));
bi_decl(bi_1pin_with_name(PD_INT_N, "USB-PD Interrupt"));