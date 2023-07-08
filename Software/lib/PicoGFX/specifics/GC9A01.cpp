#include "Display.hpp"

void Display::GC9A01_Init()
{
	// constants for this driver
	this->BGR = true;
	this->maxHeight = 240;
	this->maxWidth = 240;

	// initialize the display
    this->GC9A01_HardReset();
    sleep_ms(100);

    this->GC9A01_HardReset();
    sleep_ms(100);

    this->writeData(0xef, (const uint8_t *) NULL, 0);
	this->writeData(0xeb, (const uint8_t *) "\x14", 1);
    this->writeData(0xfe, (const uint8_t *) NULL, 0);
	this->writeData(0xef, (const uint8_t *) NULL, 0);
	this->writeData(0xeb, (const uint8_t *) "\x14", 1);
	this->writeData(0x84, (const uint8_t *) "\x40", 1);
	this->writeData(0x85, (const uint8_t *) "\xFF", 1);
	this->writeData(0x86, (const uint8_t *) "\xFF", 1);
	this->writeData(0x87, (const uint8_t *) "\xFF", 1);
	this->writeData(0x88, (const uint8_t *) "\x0A", 1);
	this->writeData(0x89, (const uint8_t *) "\x21", 1);
	this->writeData(0x8a, (const uint8_t *) "\x00", 1);
	this->writeData(0x8b, (const uint8_t *) "\x80", 1);
	this->writeData(0x8c, (const uint8_t *) "\x01", 1);
	this->writeData(0x8d, (const uint8_t *) "\x01", 1);
	this->writeData(0x8e, (const uint8_t *) "\xFF", 1);
	this->writeData(0x8f, (const uint8_t *) "\xFF", 1);
	this->writeData(0xb6, (const uint8_t *) "\x00\x00", 2);
	this->GC9A01_SetRotation((displayRotation_t)this->params->rotation);
	this->writeData(0x3a, (const uint8_t *) "\x55", 1);
	this->writeData(0x90, (const uint8_t *) "\x08\x08\x08\x08", 4);
	this->writeData(0xbd, (const uint8_t *) "\x06", 1);
	this->writeData(0xbc, (const uint8_t *) "\x00", 1);
	this->writeData(0xff, (const uint8_t *) "\x60\x01\x04", 3);
	this->writeData(0xc3, (const uint8_t *) "\x13", 1);
	this->writeData(0xc4, (const uint8_t *) "\x13", 1);
	this->writeData(0xc9, (const uint8_t *) "\x22", 1);
	this->writeData(0xbe, (const uint8_t *) "\x11", 1);
	this->writeData(0xe1, (const uint8_t *) "\x10\x0E", 2);
	this->writeData(0xdf, (const uint8_t *) "\x21\x0c\x02", 3);
	this->writeData(0xf0, (const uint8_t *) "\x45\x09\x08\x08\x26\x2A", 6);
 	this->writeData(0xf1, (const uint8_t *) "\x43\x70\x72\x36\x37\x6F", 6);
 	this->writeData(0xf2, (const uint8_t *) "\x45\x09\x08\x08\x26\x2A", 6);
 	this->writeData(0xf3, (const uint8_t *) "\x43\x70\x72\x36\x37\x6F", 6);
	this->writeData(0xed, (const uint8_t *) "\x1B\x0B", 2);
	this->writeData(0xae, (const uint8_t *) "\x77", 1);
	this->writeData(0xcd, (const uint8_t *) "\x63", 1);
	this->writeData(0x70, (const uint8_t *) "\x07\x07\x04\x0E\x0F\x09\x07\x08\x03", 9);
	this->writeData(0xe8, (const uint8_t *) "\x34", 1);
	this->writeData(0x62, (const uint8_t *) "\x18\x0D\x71\xED\x70\x70\x18\x0F\x71\xEF\x70\x70", 12);
	this->writeData(0x63, (const uint8_t *) "\x18\x11\x71\xF1\x70\x70\x18\x13\x71\xF3\x70\x70", 12);
	this->writeData(0x64, (const uint8_t *) "\x28\x29\xF1\x01\xF1\x00\x07", 7);
	this->writeData(0x66, (const uint8_t *) "\x3C\x00\xCD\x67\x45\x45\x10\x00\x00\x00", 10);
	this->writeData(0x67, (const uint8_t *) "\x00\x3C\x00\x00\x00\x01\x54\x10\x32\x98", 10);
	this->writeData(0x74, (const uint8_t *) "\x10\x85\x80\x00\x00\x4E\x00", 7);
    this->writeData(0x98, (const uint8_t *) "\x3e\x07", 2);
	this->writeData(0x35, (const uint8_t *) NULL, 0);
	this->writeData(0x21, (const uint8_t *) NULL, 0);
	this->writeData(0x11, (const uint8_t *) NULL, 0);
    sleep_ms(120);

	this->writeData(0x29, (const uint8_t *) NULL, 0);
	sleep_ms(20);
}

void Display::GC9A01_SoftReset()
{
	this->writeData(SWRESET, (const uint8_t *) NULL, 0);
	sleep_ms(120);
}

void Display::GC9A01_HardReset()
{
	// Hardware reset
	gpio_put(this->pins->rst, 1);
	sleep_ms(50);
	gpio_put(this->pins->rst, 0);
	sleep_ms(50);
	gpio_put(this->pins->rst, 1);
}

void Display::GC9A01_SetRotation(displayRotation_t rotation)
{
	unsigned int width = this->params->width;
	unsigned int height = this->params->height;
	unsigned int maxHeight = this->maxHeight;
	unsigned int maxWidth = this->maxWidth;

	switch(rotation)
    {
        case displayRotation_t::DEG_90:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MX | Display_MADCTL::MV | Display_MADCTL::RGB));
			this->params->height = width;
			this->params->width = height;
			this->maxHeight = maxWidth;
			this->maxWidth = maxHeight;
            break;
        case displayRotation_t::DEG_180:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MY | Display_MADCTL::BGR));
			this->params->height = height;
			this->params->width = width;
			this->maxHeight = maxHeight;
			this->maxWidth = maxWidth;
            break;
        case displayRotation_t::DEG_270:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MV | Display_MADCTL::MY | Display_MADCTL::BGR));
			this->params->height = width;
			this->params->width = height;
			this->maxHeight = maxWidth;
			this->maxWidth = maxHeight;
            break;
        case displayRotation_t::DEG_0:
        default:
            this->writeData(MADCTL, (uchar)Display_MADCTL::BGR | Display_MADCTL::MX);
			this->params->height = height;
			this->params->width = width;
			this->maxHeight = maxHeight;
			this->maxWidth = maxWidth;
            break;
    }
}