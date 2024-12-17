#include "rgb_led.h"
#include <Arduino.h>


rgb_led_c::rgb_led_c(	uint8_t red_led_pin, 
						uint8_t green_led_pin, 
						uint8_t blue_led_pin, 
						bool led_off_state)
{	// rgb_led_c
	this->red_led_pin	=red_led_pin;
	this->green_led_pin	=green_led_pin;
	this->blue_led_pin	=blue_led_pin;
	this->led_off_state	=led_off_state;
}	// rgb_led_c


void rgb_led_c::init()
{	// init
	digitalWrite(red_led_pin,led_off_state);
	digitalWrite(green_led_pin,led_off_state);
	digitalWrite(blue_led_pin,led_off_state);
	
	pinMode(red_led_pin,OUTPUT);
	pinMode(green_led_pin,OUTPUT);
	pinMode(blue_led_pin,OUTPUT);

	digitalWrite(red_led_pin,led_off_state);
	digitalWrite(green_led_pin,led_off_state);
	digitalWrite(blue_led_pin,led_off_state);
}	// init

void rgb_led_c::set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{	// set_rgb
	if (led_off_state)
	{	// high = off, invert PWM
		red=~red;
		green=~green;
		blue=~blue;
	}	// high = off, invert PWM
	analogWrite(red_led_pin,red);
	analogWrite(green_led_pin,green);
	analogWrite(blue_led_pin,blue);
}	// set_rgb

void rgb_led_c::set_led_color(uint8_t color)
{	// set_led_color
	if (led_off_state) color=~color;
	digitalWrite(red_led_pin,color & RED_LED_MASK);
	digitalWrite(green_led_pin,color & GREEN_LED_MASK);
	digitalWrite(blue_led_pin,color & BLUE_LED_MASK);
}	// set_led_color
