#include "push_button.h"
#include <Arduino.h>

uint8_t push_button_c::instanceCnt=0;
push_button_c * push_button_c::pInstance0;
push_button_c * push_button_c::pInstance1;
push_button_c * push_button_c::pInstance2;


push_button_c::push_button_c(uint8_t pin, bool normal_open, bool pullup)
{	// button_c
	switch_pin=pin;
	/// Switch polarity true = active high
	switch_polarity=(normal_open && !pullup) || 
					(!normal_open && pullup);
	pull_up=pullup;
	
	this->instance=instanceCnt;
	instanceCnt++;
}	// button_c

void push_button_c::init(void)
{	// init
	// configure pin & interrupts
	pinMode(switch_pin,(pull_up)?(INPUT_PULLUP):(INPUT_PULLDOWN));
	button_short=false;      
	button_released=false;
	button_state=pin_state();	
	button_time=millis();	
	switch(instance)
	{
		case 0:
			attachInterrupt(digitalPinToInterrupt(switch_pin), handler0, CHANGE);	
			pInstance0= this;
			break;
		case 1:
			attachInterrupt(digitalPinToInterrupt(switch_pin), handler1, CHANGE);	
			pInstance1= this;
			break;
		case 2:
			attachInterrupt(digitalPinToInterrupt(switch_pin), handler2, CHANGE);	
			pInstance2= this;
			break;
		default:
			break;
	}
}	// init


bool push_button_c::pressed(void)
{
	if (button_short)
	{
		button_short=false;
		button_released=false;
		return true;
	}
	else
		return false;
}

bool push_button_c::released(void)
{

	if (button_released)
	{
		button_released=false;
		return true;
	}
	else
		return false;
}

bool push_button_c::long_press(void)
{
	return 	(button_state) && 
			((uint16_t)(millis()-button_time)>LONG_PRESS_MIN);
}

bool push_button_c::down(void)
{
	return button_state;
}

bool push_button_c::pin_state(void)
{
	return (digitalRead(switch_pin)==switch_polarity);
}

void push_button_c::button_handler(void)
{
	bool 		tempState=pin_state();
	uint16_t	tempTime=millis();
	if (button_state!=tempState)
	{	// change, source was pin
		if (button_state && !tempState)
		{ // button has been released
			button_short=(uint16_t)(tempTime-button_time)<LONG_PRESS_MIN;
			button_released=true;
		} // button has been released
		else
			button_released=button_released && !tempState;
		button_state=tempState;
		button_time=tempTime;
	}	// change, source was pin
}

void push_button_c::handler0(void)
{	// handler0
	pInstance0->button_handler();
}	// handler0
void push_button_c::handler1(void)
{	// handler1
	pInstance1->button_handler();
}	// handler1
void push_button_c::handler2(void)
{	// handler2
	pInstance2->button_handler();
}	// handler2
