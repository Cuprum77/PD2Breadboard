#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    // init the pins
	gpio_init(15);
	gpio_init(25);

	// set the pins to output
	gpio_set_dir(15, GPIO_OUT);
	gpio_set_dir(25, GPIO_OUT);

    while(1)
    {
        gpio_put(15, !gpio_get(15));
        gpio_put(25, !gpio_get(25));
    }
}