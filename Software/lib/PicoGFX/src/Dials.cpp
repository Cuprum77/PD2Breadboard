#include "Dials.hpp"

Dials::Dials(Print* print, Graphics* graphics, unsigned short* frameBuffer, Display_Params params)
{
	this->print = print;
	this->gfx = graphics;
	this->frameBuffer = frameBuffer;
	this->width = params.width;
	this->height = params.height;
	this->centerX = this->width / 2;
	this->centerY = this->height / 2;
	this->totalPixels = params.width * params.height;
}

void Dials::drawGauge(Gauge* gauge)
{
	gauge->gaugeBuffer = new unsigned short[gauge->radius * gauge->radius];
}

void Dials::updateGauge(Gauge* gauge, int newValue)
{
	if (gauge->currentValue != newValue)
	{

	}

	// Copy the gauge buffer over the frame buffer at the gauge's location
	for (int i = 0; i < (gauge->radius * gauge->radius); i++)
	{
	}
}