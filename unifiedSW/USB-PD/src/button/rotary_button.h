#ifndef __rotary_button__
#define __rotary_button__
#include <stdint.h>
#include "../../config.h"

class rotary_button_c
{
	public:
	
		void init(void);
		void process(void);
		void setLong(uint16_t ms);
		int8_t turns(void);
		bool down(void);
		bool pressed(void);
		bool long_press(void);
	
	private:

		static const uint8_t CLOCK_PIN	=HWCFG_ENCODER_CLOCK; 
		static const uint8_t DATA_PIN	=HWCFG_ENCODER_DATA;
		static const uint8_t SWITCH_PIN	=HWCFG_ENCODER_SWITCH; 	



		static const uint16_t T_PRELL	=4;
		static const uint16_t T_LONG	=500;




		typedef struct pinState_s {	uint16_t	ChangeTime;
									uint8_t		State	:1;
									uint8_t		Pin		:1;
									uint8_t		Valid	:1;
									} pinState_t;
									
		typedef struct rotState_s {	uint8_t	Clock:1;
									uint8_t	ClockChange:1;
									uint8_t	Data:1;
									uint8_t DataChange:1;
									uint8_t Left:1;
									uint8_t Right:1;
									} rotState_t;

		pinState_t SwitchState;
		rotState_t Rotation;
		
		uint8_t 		ClockCnt;
		volatile bool	bPressed;
		uint16_t		t_prell;
		uint16_t		t_long;
		
		uint8_t 		lastClockCnt;
	
		static rotary_button_c * pContext;
		
		uint32_t readPortFromPin(uint8_t Pin) 
			{ return * portInputRegister(digitalPinToPort(Pin)); }
		
		static void ClockPinInt(void);
		static void DataPinInt(void);
		static void SwitchPinInt(void);
		
		void SwitchPinHandler(void);
		void ClockPinHandler(void);
		void DataPinHandler(void);
		void InkrementProcess(void);
};
	
#endif //__rotary_button__

