#include "include/Defines.hpp"

// set the display parameters
// the ifdefs are used to prevent the intellisense from complaining about the variables not being defined
Display_Pins displayPins = {
#ifndef __INTELLISENSE__
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.bl = DISP_PIN_BL
#endif
};

Display_Params displayParams = {
#ifndef __INTELLISENSE__
	.hw_interface = SPI_Interface_t::PIO_HW,
	.type = display_type_t::ST7789,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION
#endif
};

// Create the display object
HardwareSPI spi(displayPins, displayParams);
Display display(&spi, &displayPins, &displayParams);
// Create the GFX objects
Print print(display.getFrameBuffer(), displayParams);
Graphics graphics(display.getFrameBuffer(), displayParams);
Gradients gradients(display.getFrameBuffer(), displayParams);
// Create the encoder/decoder object
Encoder encoder;
// Create the PicoGFX object
PicoGFX picoGFX(&display, &print, &graphics, &gradients, &encoder);

#define CHUNK_SIZE 1024 * 10
unsigned char buffer[CHUNK_SIZE] = { 0 };
unsigned char metaDataBuffer[METADATA_BYTES] = { 0 };
unsigned short* framebuffer = picoGFX.getDisplay().getFrameBuffer();
metadata_t metadata, metadatabuffer;
unsigned int totalBytes = 0;
unsigned int bytesRead = 0;
unsigned int bytesToRead = 0;
unsigned int bytesReadNow = 0;

int main()
{
	// setup the microcontroller
	stdio_init_all();

	gpio_init(LEFT_MOSFET_LED);
	gpio_set_dir(LEFT_MOSFET_LED, true);

	picoGFX.getPrint().setFont(&ComicSans24);
	picoGFX.getPrint().setColor(Colors::White);

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
						metadata.type = metaDataBuffer[0];
						metadata.width = metaDataBuffer[1] << 0x08 | metaDataBuffer[2];
						metadata.height = metaDataBuffer[3] << 0x08 | metaDataBuffer[4];
						metadata.totalBytes = metaDataBuffer[5] << 0x18 | metaDataBuffer[6] << 0x10 | metaDataBuffer[7] << 0x08 | metaDataBuffer[8];
						metadatabuffer = metadata;

						// Check if width and height match the expected values
						if (metadata.width == picoGFX.getDisplay().getWidth() && metadata.height == picoGFX.getDisplay().getHeight())
						{
							state = STATE_RECEIVING_DATA;
							framebuffer = picoGFX.getDisplay().getFrameBuffer();
							bytesRead = 0;
							bytesToRead = CHUNK_SIZE; // Initialize bytesToRead
						}
						else
						{
							// Invalid width or height, reset the state machine							
							state = STATE_WAIT_METADATA;
						}
					}
					break;

				case STATE_RECEIVING_DATA:
					bytesToRead = metadata.totalBytes;
					tud_cdc_read(buffer, bytesToRead);
					picoGFX.getEncoder().Decode(&metadata, buffer, framebuffer);
					state = STATE_UPDATE_DISPLAY;
					/*

					// Read the remaining bytes in chunks and process them accordingly
					if (bytesRead + bytesToRead > metadata.totalBytes)
					{
						bytesToRead = metadata.totalBytes - bytesRead;
					}

					// Read the data into the buffer
					bytesReadNow = tud_cdc_read(buffer, bytesToRead);

					if (bytesReadNow > 0)
					{
						// Adjust the metadata totalBytes for the remaining data
						metadatabuffer.totalBytes = bytesReadNow;

						// Process the received data chunk
						picoGFX.getEncoder().Decode(&metadatabuffer, buffer, framebuffer, true);

						// Increment the framebuffer pointer by bytesReadNow
						framebuffer += bytesReadNow;
						bytesRead += bytesReadNow;

						// Check if all bytes have been received
						if (bytesRead >= metadata.totalBytes)
						{
							gpio_put(LEFT_MOSFET_LED, true);
							// Reset the state machine
							state = STATE_UPDATE_DISPLAY;
						}
						else
						{
							gpio_put(LEFT_MOSFET_LED, false);
							// Update the bytesToRead for the next chunk
							bytesToRead = CHUNK_SIZE;
						}
					}*/
					break;

				case STATE_UPDATE_DISPLAY:
					picoGFX.getDisplay().update();
					state = STATE_WAIT_METADATA;
					break;
			}
		}
		else
		{
			state = STATE_WAIT_METADATA;
			totalBytes = 0;
			bytesRead = 0;
			bytesToRead = 0;
		}
	}
}