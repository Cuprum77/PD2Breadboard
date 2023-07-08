#include "HardwareSPI.hpp"

/**
 * @brief Construct a new HardwareSPI::HardwareSPI object
 * @param pins The struct containing the pins to use
 * @param params The struct containing the parameters to use
 * @param spi The SPI bus to use, can be ignored if using DMA or PIO
 * @note If SPI is in use, and the instance is not specified, spi0 will be used
*/
HardwareSPI::HardwareSPI(Display_Pins pins, Display_Params params, spi_inst_t *spi)
{
    this->scl = pins.scl;
    this->sda = pins.sda;
    this->dc = pins.dc;
    this->cs = pins.cs;

    switch(params.hw_interface)
    {
        case(SPI_Interface_t::DMA_HW):
            this->hw_interface = SPI_Interface_t::DMA_HW;
            this->initDMA();
            break;
        case(SPI_Interface_t::PIO_HW):
            this->hw_interface = SPI_Interface_t::PIO_HW;
            this->initPIO();
            break;
        default:
            if (spi == nullptr)
                this->spi = spi0;
            else
                this->spi = spi;
            
            this->hw_interface = SPI_Interface_t::SPI_HW;
            this->initSPI();
            break;
    }
}

/**
 * @brief Write data to the SPI bus
 * @param command The command to send
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void HardwareSPI::spi_write_data(uint8_t command, const uint8_t* data, size_t length)
{
    switch(hw_interface)
    {
        case(SPI_Interface_t::PIO_HW):
            pio_spi_program_wait_idle(this->pio, this->sm);
            this->set_dc_cs(0, 0);
            pio_spi_program_put(this->pio, this->sm, command);
            if(length)
            {
                pio_spi_program_wait_idle(this->pio, this->sm);
                this->set_dc_cs(1, 0);
                for(size_t i = 0; i < length; i++)
                    pio_spi_program_put(this->pio, this->sm, *data++);
            }
            pio_spi_program_wait_idle(this->pio, this->sm);
            this->set_dc_cs(1, 1);
            break;
        default:
            // configure the spi to 8 bit mode
            spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            // set the display to write mode
            gpio_put(this->dc, 0);
            // send the command
            spi_write_blocking(this->spi, &command, 1);
            // send the data
            gpio_put(this->dc, 1);
            // if there is data to send, send it
            if (length)
                spi_write_blocking(this->spi, data, length);
            break;
    }
}

/**
 * @brief Write data mode to the SPI bus
 * @param command The command to send
 * @return bytes written on success, -1 on failure
*/
void HardwareSPI::spi_set_data_mode(uint8_t command)
{
    switch(hw_interface)
    {
        case(SPI_Interface_t::PIO_HW):
            this->spi_write_data(command, nullptr, 0);
            this->set_dc_cs(1, 0);
            break;
        default:
            gpio_put(this->dc, 0);
            spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            spi_write_blocking(this->spi, &command, sizeof(command));
            gpio_put(this->dc, 1);
            spi_set_format(this->spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            break;
    }
}

/**
 * @brief Write data to the SPI bus
 * @param spi The SPI instance to use
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void HardwareSPI::spi_write_pixels(const uint16_t* data, size_t length)
{
    switch(hw_interface)
    {
        case(SPI_Interface_t::DMA_HW):
            dma_channel_configure(
                this->dma_tx, // Channel to be configured
                &this->dma_config, // The configuration we just created
                &spi_get_hw(this->spi)->dr, // The write address
                data, // Pointer to the data we want to transmit
                length, // Number of bytes to transmit
                true // Start immediately
            );
            break;
        case(SPI_Interface_t::PIO_HW):
            while(length--)
            {
                pio_spi_program_put_16(this->pio, this->sm, *data++);
            }
            break;
        default:
            // send the data
            spi_write16_blocking(this->spi, data, length);
            break;
    }
}

/**
 * @brief Check if the DMA is busy
 * @return true if busy, false if not
 * @note Always returns false if DMA is not enabled
*/
bool HardwareSPI::dma_busy(void)
{
    if(this->hw_interface != SPI_Interface_t::DMA_HW)
        return false;
    
    return dma_channel_is_busy(this->dma_tx);
}

/**
 * @private
 * @brief Initialize the SPI bus
*/
void HardwareSPI::initSPI(void)
{
    // enable the SPI bus
    spi_init(this->spi, SPI_BAUDRATE);

    // set the pins to SPI function
    gpio_set_function(this->sda, GPIO_FUNC_SPI);
    gpio_set_function(this->scl, GPIO_FUNC_SPI);
    gpio_set_function(this->cs, GPIO_FUNC_SPI);

    gpio_init(this->dc);
    gpio_set_dir(this->dc, GPIO_OUT);
    gpio_put(this->dc, 1);
}

/**
 * @private
 * @brief Initialize the SPI bus with DMA
*/
void HardwareSPI::initDMA(void)
{
    // enable the SPI bus
    this->initSPI();

    // grab some unused DMA channels
    this->dma_tx = dma_claim_unused_channel(true);

    // setup the control channel
    this->dma_config = dma_channel_get_default_config(this->dma_tx);
    channel_config_set_transfer_data_size(&this->dma_config, DMA_SIZE_16); // 16 bit transfers (1/2 word)
    channel_config_set_dreq(&this->dma_config, DREQ_SPI0_TX); // SPI TX DREQ
    channel_config_set_read_increment(&this->dma_config, true); // increment the read address
    channel_config_set_write_increment(&this->dma_config, false); // don't increment the write address
}

/**
 * @private
 * @brief Initialize the SPI bus with PIO
*/
void HardwareSPI::initPIO(void)
{
    // create the first spi state machine
    this->pio = pio0;
    this->sm = 0;
    this->offset = pio_add_program(pio, &pio_spi_program);
    pio_spi_program_init(this->pio, this->sm, this->offset, this->sda, this->scl, SERIAL_CLK_DIV);

    // set the pins to SPI function
    gpio_init(this->cs);
    gpio_init(this->dc);
    gpio_set_dir(this->cs, GPIO_OUT);
    gpio_set_dir(this->dc, GPIO_OUT);
    gpio_put(this->cs, 1);
    gpio_put(this->dc, 1);
}

/**
 * @private
 * @brief Set the data/command pin and/or the chip select pin
 * @param dc The data/command pin
 * @param cs The chip select pin
*/
inline void HardwareSPI::set_dc_cs(bool dc, bool cs)
{
    sleep_us(1);
    gpio_put_masked((1u << this->dc) | (1u << this->cs), !!dc << this->dc | !!cs << this->cs);
    sleep_us(1);
}