#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"

#include "pio_spi.pio.h"
#include "Commands.hpp"
#include "Structs.h"

#define SERIAL_CLK_DIV 1.0f
#define SPI_BAUDRATE 125000000  // 125 MHz

typedef struct pio_spi_inst {
    PIO pio;
    uint sm;
    uint cs_pin;
} pio_spi_inst_t;


class HardwareSPI
{
public:
    HardwareSPI(Display_Pins pins, Display_Params params, spi_inst_t *spi = nullptr);

    void spi_write_data(uint8_t command, const uint8_t* data, size_t length);
    void spi_set_data_mode(uint8_t command);
    void spi_write_pixels(const uint16_t* data, size_t length);

    bool dma_busy(void);

private:
    // general stuff
    SPI_Interface_t hw_interface;

    // spi stuff
    spi_inst_t* spi;
    uint8_t scl;
    uint8_t sda;
    uint8_t dc;
    uint8_t cs;

    // dma stuff
    uint dma_tx;
    dma_channel_config dma_config;

    // pio stuff
    PIO pio;
    uint sm;
    uint offset;

    // private functions
    void initSPI(void);
    void initDMA(void);
    void initPIO(void);
    inline void set_dc_cs(bool dc, bool cs);
};