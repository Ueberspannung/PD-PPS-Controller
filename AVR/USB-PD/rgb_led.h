#ifndef __RGB_LED_C_H__
#define __RGB_LED_C_H__

#include <stdint.h>

class rgb_led_c 
{
	public:
		static const uint8_t RGB_LED_OFF	=0;		// all LEDs off
		static const uint8_t RGB_LED_RED	=1;		// red LED
		static const uint8_t RGB_LED_YELLOW	=3;		// red LED + green LED 
		static const uint8_t RGB_LED_GREEN	=2;		// green LED
		static const uint8_t RGB_LED_CYAN	=6;		// green LED + blue + LED
		static const uint8_t RGB_LED_BLUE	=4;		// blue LED
		static const uint8_t RGB_LED_PURPLE	=5;		// blue LED + red LED
		static const uint8_t RGB_LED_WHITE	=7;		// blue LED + red LED + green LED
													
		rgb_led_c(	uint8_t red_led_pin, 
					uint8_t green_led_pin, 
					uint8_t blue_led_pin, 
					bool led_off_state=false);
		void init();
		void set_rgb(uint8_t red, uint8_t green, uint8_t blue);
		void set_led_color(uint8_t color);
		
	private:
		
	static const uint8_t RED_LED_MASK	=0x01;
	static const uint8_t GREEN_LED_MASK	=0x02;
	static const uint8_t BLUE_LED_MASK	=0x04;
	
	uint8_t red_led_pin;
	uint8_t green_led_pin;
	uint8_t blue_led_pin;
	bool led_off_state;
};

#endif // __RGB_LED_C_H__
