// This is a header guard. It prevents the file from being included more than once.
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
#define LEFT_MOSFET_LED     15
#define LEFT_MOSFET         14
#define RIGHT_MOSFET_LED    25
#define RIGHT_MOSFET        24



/***
 *     __     __         _       _     _           
 *     \ \   / /_ _ _ __(_) __ _| |__ | | ___  ___ 
 *      \ \ / / _` | '__| |/ _` | '_ \| |/ _ \/ __|
 *       \ V / (_| | |  | | (_| | |_) | |  __/\__ \
 *        \_/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
 *                                                 
 */

// Display constants
#define DISP_WIDTH          172     // 172 pixels wide
#define DISP_HEIGHT         320     // 320 pixels tall
#define DISP_OFFSET_X0      34      // cut off on the left side of the display
#define DISP_OFFSET_X1      34      // cut off on the right side of the display
#define DISP_OFFSET_Y0      0       // cut off on the top of the display
#define DISP_OFFSET_Y1      0       // ut off on the bottom of the display
#define DISP_ROTATION       0       // 0, 1, 2, or 3

// I2C constants
#define I2C0_SPEED          1000000  // 1 MHz
#define I2C1_SPEED          1000000  // 1 MHz

// System constants
#define MULTICORE_FLAG_VALUE        0x69

// default values
#define CURRENT_LIMIT_DEFAULT       3000    // mA
#define CURRENT_SETTING_DEFAULT     1000    // mA
#define VOLTAGE_SETTING_DEFAULT     5000    // mV
#define BACKLIGHT_DEFAULT           255     // 0-255
#define VOLTAGE_STEPS               20      // USB PPS voltage steps in mV
#define CURRENT_STEPS               50      // USB PPS current steps in mA

// serial interface codes
#define REBOOT_TO_BOOTLOADER_CODE   "reboot_to_bootloader"
#define RESET_CODE                  "reset"
#define GET_SETTINGS_CODE           "get_settings"
#define GET_HASH                    "get_hash"
#define GET_BRANCH                  "get_branch"

// INA219 codes
#define CURRENT_LIMIT_CODE          "current_limit:"
#define CURRENT_SETTING_CODE        "current_setting:"
#define VOLTAGE_SETTING_CODE        "voltage_setting:"
#define GET_READINGS_CODE           "get_readings"
#define GET_READINGS_RAW_CODE       "get_readings_raw"

// FUSB302 codes
#define FUSB302_DUMP_DATA           "dump_usb"
#define FUSB302_GET_IDENTITY        "get_identity"
#define FUSB302_GET_CAPABILITY      "get_capability"

// display codes
#define BACKLIGHT_CODE              "backlight:"

// general peripheral codes
#define TEST_CODE                   "test"



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
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "tusb.h"

#include "Button.hpp"
#include "Display.hpp"
#include "USB_PD.hpp"
#include "INA219.hpp"
#include "Memory.hpp"
#include "version.h"



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
bi_decl(bi_program_version_string(GIT_COMMIT_HASH));
bi_decl(bi_program_url("https://github.com/Cuprum77/PD2Breadboard"));

bi_decl(bi_4pins_with_names(DISP_PIN_CS, "SPI Chip Select", DISP_PIN_SCK, "SPI Clock", DISP_PIN_MOSI, "SPI Data", DISP_PIN_RST, "SPI Reset"));
bi_decl(bi_2pins_with_names(DISP_PIN_DC, "SPI Data Control", DISP_PIN_BL, "Display Backlight"));
bi_decl(bi_3pins_with_names(BUTTON_UP, "Button Up", BUTTON_MENU, "Button Menu", BUTTON_DOWN, "Button Down"));
bi_decl(bi_1pin_with_name(PD_INT_N, "USB-PD Interrupt"));
