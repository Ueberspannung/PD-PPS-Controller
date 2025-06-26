#ifndef __push_button_c_h__
#define __push_button_c_h__

#include <stdint.h>

/* **************************************************
 * simple button class
 * 
 * debouncing needs to be done in HW 
 * 
 * to create an instance the type of switch and the type 
 * of the internal pull-resisort is needed
 * each button pin needs to be located on an seperate
 * interrupt channel
 * 
 * the interface provides three different button states
 * 	- down
 * 		simple button state request
 * 	- pressed
 * 		button has been activated for less than LONG_PRESS_MIN ms
 * 	- long_press
 * 		button is activated and down for more thand LONG_PRES_MIN ms
 *	- released 
 * 		button changes from active to inactive
 * to increase the number of buttons, just add mor instance 
 * pointers and static handlers
 * if serveral buttons share one interrupt this class wil fail
 */
class push_button_c
{
	public:
	
		push_button_c(uint8_t pin, bool normal_open=true, bool pullup=true);
		
		void init(void);
		
		bool pressed(void);
		bool released(void);
		bool long_press(void);
		bool down(void);
	
	private:
		static const uint16_t LONG_PRESS_MIN=500;	
		static const uint8_t  MAX_INSTANCE	=3;

		static uint8_t instanceCnt;
		static push_button_c * pInstance0;
		static push_button_c * pInstance1;
		static push_button_c * pInstance2;

		uint8_t instance;
		
		uint8_t switch_pin;
		bool	switch_polarity;
		bool	pull_up;
		
		volatile bool		button_short;
		volatile bool		button_released;
		volatile bool		button_state;
		volatile uint16_t	button_time;
		
		bool pin_state(void);
		void button_handler(void);
		static void handler0(void);
		static void handler1(void);
		static void handler2(void);
};



#endif // __push_button_c_h__
