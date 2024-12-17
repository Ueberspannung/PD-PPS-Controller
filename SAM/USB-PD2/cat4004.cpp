#include <Arduino.h>
#include "cat4004.h"

cat4004::cat4004(uint8_t brightnessPin,unsigned char Index)
{	// cat4004
	dimPin=brightnessPin;
	dimCnt=0;
	chipIndex=Index;
	lastBrightness=cat4004_off;
	newBrightness=cat4004_off;
	lastUpdate=0;
	dim_state=dim_init;
}	// cat4004

void cat4004::process(void)
{
	switch (dim_state)
	{	// statemachine for minimum blocking
		case dim_init:	// set configure if dim pin is set
			if (dimPin!=0xFF)
			{	// dim Pin defined, configure
				pinMode(dimPin,OUTPUT);
				digitalWrite(dimPin,LOW);
				dim_state=dim_idle;
			}	// dim Pin defined, configure
			break;
		case dim_idle:	// wait for brightness change
			if (newBrightness!=lastBrightness)
			{	// new Brightness request, process
				uint16_t tempcnt;
				if (newBrightness==cat4004_off)
				{	// disable chip
					dim_state=dim_off;	
				}	// disable chip
				else if (chipIndex!=cat4004_index)
				{	// CAT4004A / CAT4004B
					tempcnt=newBrightness;
					tempcnt*=cat4004A_steps-1;
					tempcnt+=max_brightness>>1;
					tempcnt/=max_brightness;
					dimCnt=(uint8_t)tempcnt;
					dimCnt=cat4004A_steps-1-dimCnt;
					dim_state=dim_reset;
				}	// CAT4004A / CAT4004B
				else
				{	// CAT4004
					/* Mapping of brighness settings
					 * 100% - 76%	:	100%
					 *  75% - 39%	:	 50%
					 *  38% - 20%	:	 25%
					 *  19% - 10%	:	 12,5%
					 *  10% -  6%	:	  6,25%
					 *   6% -  3%	:	  3,125%
					 *   2% -  0%	:     off
					 */
					dimCnt=0;
					tempcnt=max_brightness;
					while((tempcnt-(tempcnt>>2))>newBrightness)
					{
						dimCnt++;
						tempcnt=tempcnt>>1;
					}
					if (dimCnt>=cat4004A_steps-1)
					{	// off
						dimCnt=0;
						dim_state=dim_off;
					}	// off
					else
					{	// set
						dim_state=dim_reset;
					}	// set
				}	// CAT4004
				lastBrightness=newBrightness;
			}	// new Brightness request, process
			break;
		case dim_off:	// disable chip
			digitalWrite(dimPin,LOW);
			dim_state=dim_idle;
			break;
		case dim_reset:	// disable to reset chip
			// turn off to reset chip
			lastUpdate=millis();
			digitalWrite(dimPin,LOW);
			dim_state=dim_enable;
			break;
		case dim_enable:	// wait for reset time and eanble chip
			if ((uint16_t)(millis()-lastUpdate)>min_off_ms)
			{
				lastUpdate=millis();
				digitalWrite(dimPin,HIGH);
				dim_state=dim_pulse;
			}
			break;
		case dim_pulse:		// wait for setup time and send pulses
			// enable / toggle dim pin
			if ((uint16_t)(millis()-lastUpdate)>min_on_ms)
			{
				if (dimCnt>0)
				{	// toggle pin
					digitalWrite(dimPin,LOW);
					digitalWrite(dimPin,HIGH);
					dimCnt--;
				}	// toggle pin
				else
					dim_state=dim_idle;
			}
			break;
		default:
			dim_state=dim_idle;
			break;
	}	// statemachine for minimum blocking
}

void cat4004::set_brightness(uint8_t brightness)
{	// set_brightness
	if (brightness>max_brightness)
		newBrightness=cat4004_off;
	else
		newBrightness=brightness;
}	// set_brightness

uint8_t cat4004::get_brightness(void)
{	// get_brightnes
	if (newBrightness>max_brightness)
		return 0;
	else 
		return newBrightness;
}	// get_brightnes

void cat4004::disable(void)
{
	set_brightness(cat4004_off);
}

