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
#define PIN_FUSB302_INT     4

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
#define DISP_OFFSET_X0      0       // cut off on the left side of the display
#define DISP_OFFSET_X1      0       // cut off on the right side of the display
#define DISP_OFFSET_Y0      34      // cut off on the top of the display
#define DISP_OFFSET_Y1      34      // ut off on the bottom of the display
#define DISP_ROTATION       3       // 0, 1, 2, or 3

// I2C constants
#define I2C0_SPEED          1000000  // 1 MHz
#define I2C1_SPEED          100000  // 1 MHz

// System constants
#define MULTICORE_FLAG_VALUE        0x69



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
#include "pico/time.h"
#include "tusb.h"

#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"

#include "Button.hpp"
#include "INA219.hpp"
#include "Memory.hpp"
#include "version.h"
#include "Registers.hpp"
#include "PD_UFP.h"
#include "PicoGFX.hpp"
#include "RobotoMono48.h"
#include "ComicSans24.h"
#include "ComicSans48.h"
#include "bg.h"
#include "test.h"

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
bi_decl(bi_1pin_with_name(PIN_FUSB302_INT, "USB-PD Interrupt"));
