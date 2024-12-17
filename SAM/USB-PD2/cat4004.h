#ifndef __cat4004_h__
#define __cat4004_h__

/* ********************************************************************************
 * controls brighness using CAT4004, used by LCDDIO
 * 
 * Constructor needs to be called witn IO Pin number and Chip index
 * CAT4004 uses 6 Steps with 100%, 50%, 25%, 12,5%, 6,25% and 3,125% Imax
 * CAT4004A & B uses 32 linear Steps form 31/31 to 0/31 Imax
 * 
 * the setup procedure is non blockking
 * -> process has to be called cyclic in order to proceed the setup procedure
 * 3 Steps:
 * 1 -> Disable chip for >2ms to reset brightness
 * 2 -> Enable Chip for >10us to create Setup time
 * 3 -> Pulse Enable low n times for at least 200ns
 * in Arduino framework a digitalWrite high-low-high creates a ~1.5us pulse
 */  

#include <stdint.h>
#include <stdbool.h>

class cat4004
{
	public:
		cat4004(uint8_t brightnessPin=0xFF,unsigned char Index=' ');
		
		void process(void);
		
		void set_brightness(uint8_t brightness);
		uint8_t get_brightness(void);
		void disable(void);
		

	private:
		static const uint8_t min_off_ms		= 6;	// at least 5ms low time (spec: 1.5ms)
		static const uint8_t min_on_us		=15;	// at least 15us on time before brightnes setup (spec: 10us)
		static const uint8_t min_on_ms		= 2;	// at least 15us on time before brightnes setup (spec: 10us)
		static const uint8_t pulse_delay_us	= 1;	// at least 1us pulse on / off time	(spec: min 0.2us max 100us)	
		
		static const uint8_t cat4004_steps	=6;		// 6 steps (0 - 5) for CAT4004 I=Imax * 2^(-n)
		static const uint8_t cat4004A_steps	=32;	// 32 steps (0 - 31) for CAT4004 I=Imax * (31-n)/31
		static const unsigned char cat4004_index=' ';// no index

		static const uint8_t max_brightness	=100;	// set 0-100%
		static const uint8_t cat4004_off	=255;	// set brigthness to 255 to turn off (internal)
		
		typedef enum:uint8_t {	dim_init,
								dim_idle,
								dim_off,
								dim_reset,
								dim_enable,
								dim_pulse } dim_state_et;
		dim_state_et dim_state;
		uint8_t lastBrightness;
		uint8_t newBrightness;
		uint8_t	dimPin;
		uint8_t	dimCnt;
		unsigned char chipIndex;
		uint16_t lastUpdate;
};

#endif // __cat4004_h__
