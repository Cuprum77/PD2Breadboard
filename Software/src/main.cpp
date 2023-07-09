#include "include/Defines.hpp"

// set the display parameters
// the ifdefs are used to prevent the intellisense from complaining about the variables not being defined
Display_Pins displayPins = {
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.bl = DISP_PIN_BL
};

Hardware_Params hardwareParams = {
	.hw_interface = SPI_Interface_t::SPI_HW,
	.baudrate = 30000000,
};

Display_Params displayParams = {
	.type = display_type_t::ST7789,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION
};

// Create the display object
HardwareSPI spi(displayPins, hardwareParams, displayParams);
Display display(&spi, &displayPins, &displayParams);
// Create the GFX objects
Print print(display.getFrameBuffer(), displayParams);
Graphics graphics(display.getFrameBuffer(), displayParams);
Gradients gradients(display.getFrameBuffer(), displayParams);
// Create the encoder/decoder object
Encoder encoder;
// Create the PicoGFX object
PicoGFX picoGFX(&display, &print, &graphics, &gradients, &encoder);

#define CHUNK_SIZE 256
unsigned char buffer[CHUNK_SIZE + 1] = { 0 };
unsigned char metaDataBuffer[METADATA_BYTES] = { 0 };
unsigned short* framebuffer = picoGFX.getDisplay().getFrameBuffer();
stream_metadata_t metadata, metadatabuffer;
unsigned int totalBytes = 0;
unsigned int bytesRead = 0;
unsigned int bytesToRead = 0;
unsigned int bytesReadNow = 0;
unsigned int frameBufferIndex = 0;
bool calculatingCore1 = false;

void main1();

int main()
{	
	vreg_set_voltage(VREG_VOLTAGE_MAX);
    set_sys_clock_khz(400000, true);
	
	// setup the microcontroller
	stdio_init_all();

	multicore_launch_core1(main1);

	gpio_init(LEFT_MOSFET_LED);
	gpio_init(RIGHT_MOSFET_LED);
	gpio_set_dir(LEFT_MOSFET_LED, true);
	gpio_set_dir(RIGHT_MOSFET_LED, true);

	picoGFX.getPrint().setFont(&ComicSans24);
	picoGFX.getPrint().setColor(Colors::GreenYellow);

	enum State {
		STATE_WAIT_METADATA,
		STATE_RECEIVING_DATA,
		STATE_UPDATE_DISPLAY
	};

	// Initialize the state machine
	State state = STATE_WAIT_METADATA;

	while (1)
	{
		if (tud_cdc_available())
		{
			switch (state)
			{
				case STATE_WAIT_METADATA:
					// Read the metadata bytes
					if (tud_cdc_read(metaDataBuffer, sizeof(metaDataBuffer)) == sizeof(metaDataBuffer))
					{
						picoGFX.getEncoder().StripMetadata(&metadata, metaDataBuffer);

						// Check if width and height match the expected values
						if (metadata.width == picoGFX.getDisplay().getWidth() && metadata.height == picoGFX.getDisplay().getHeight())
						{
							state = STATE_RECEIVING_DATA;
							metadatabuffer = metadata;
							bytesRead = 0;
							frameBufferIndex = 0;
						}
					}
					break;

				case STATE_RECEIVING_DATA:
					// Read the data in chunks
					if ((bytesRead < metadata.totalBytes))
					{
						// Determine the number of bytes to read in this iteration
						bytesToRead = CHUNK_SIZE;
						if (bytesRead + bytesToRead > metadata.totalBytes)
							bytesToRead = metadata.totalBytes - bytesRead;
						// Read the data into the buffer
						bytesReadNow = tud_cdc_n_read(0, buffer, bytesToRead);

						if (bytesReadNow > 0)
						{
							// Adjust the metadata totalBytes for the remaining data
							metadatabuffer.totalBytes = bytesReadNow;

							// If core 1 is available, use it
							if (!calculatingCore1)
							{
								calculatingCore1 = true;
								multicore_fifo_push_blocking(0x1);
								break;
							}

							// If core 0 is available, use it
							frameBufferIndex += picoGFX.getEncoder().Decode(&metadatabuffer, buffer, framebuffer + frameBufferIndex);
						}
					}
					// Check if all bytes have been received
					if (bytesRead >= metadata.totalBytes)
						// Push the display data to the display
						state = STATE_UPDATE_DISPLAY;

					break;

				case STATE_UPDATE_DISPLAY:
					// signal the other core that it should push the display data
					//pushCores = true;
					picoGFX.getDisplay().update();
					state = STATE_WAIT_METADATA;
					break;
			}
		}
	}
}

void main1()
{
	while(1)
	{
		// wait until we receive data
		while(multicore_fifo_pop_blocking() != 0x1) tight_loop_contents();
		frameBufferIndex += picoGFX.getEncoder().Decode(&metadatabuffer, buffer, framebuffer + frameBufferIndex);
		// Increment the bytesRead counter
		bytesRead += bytesReadNow;
		calculatingCore1 = false;
	}
}