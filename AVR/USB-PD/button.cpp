/* **************************************************************************
 * Rotary encoder switch driver
 * Sample implementation on AVR and SAM
 * AVR:
 * all PB Pins are used. AVR is so slow that bouncing ist mostly missed
 * this simple implementation with only one ISR leeds to >95% accuracy
 * SAM:
 * uses different PinChange Interrupts
 * PinChange interrupts are handelt by EIC. The Arduino framework provides
 * several attachInterrupt hooks altough EIC provides only one interrupt vector
 * The analyses of INTFLAG is done in the framework to provide an hook to each
 * external interrupt.
 * 
 * SAM has to be done this way because it gets several but not all bounces 
 * and therefore the AVR implementation will not work
 * If teh threes sginals are distributed among different ports on AVR the SAM
 * implentation can be used. 
 * 
 * 
 * Encoder sequence on SAM
 * 
 * changing from CW to CCW
 * 
 * CW
 * C:0 D:0
 * C:1 D:0
 * C:1 D:1
 * C:0 D:1
 * CCW
 * C:1 D:1
 * C:1 D:0
 * CCW
 * C:0 D:0
 * C:0 D:1
 * C:1 D:1
 * C:1 D:0
 * 
 * 
 * changing form CCW to CW
 * 
 * CCW
 * C:0 D:0
 * C:0 D:1
 * C:1 D:1
 * C:1 D:0
 * CW
 * C:1 D:1
 * C:0 D:1
 * CW
 * C:0 D:0
 * C:1 D:0
 * C:1 D:1
 * C:0 D:1
 * 
 */

#include <Arduino.h>
#include "button.h"

#ifdef ARDUINO_ARCH_AVR
	#define CLOCK_PIN_ARDUINO	(15)	// PB
	#define DATA_PIN_ARDUINO	(14)	// PB
	#define SWITCH_PIN_ARDUINO	(16)	// PB
#else
	#define CLOCK_PIN_ARDUINO	(17)	// PA04 (Pin A3)
	#define DATA_PIN_ARDUINO	(18)	// PA05 (Pin A4)
	#define SWITCH_PIN_ARDUINO	(14)	// PA02 (Pin A0)
#endif

#define SWITCH_PIN_PORT			*portInputRegister(digitalPinToPort(SWITCH_PIN_ARDUINO)) 
#define DATA_PIN_PORT			*portInputRegister(digitalPinToPort(DATA_PIN_ARDUINO)) 
#define CLOCK_PIN_PORT			*portInputRegister(digitalPinToPort(CLOCK_PIN_ARDUINO)) 
#define SWITCH_PIN_MASK			(digitalPinToBitMask(SWITCH_PIN_ARDUINO)) 
#define DATA_PIN_MASK			(digitalPinToBitMask(DATA_PIN_ARDUINO)) 
#define COCK_PIN_MASK			(digitalPinToBitMask(CLOCK_PIN_ARDUINO)) 


#define T_PRELL		(4)
#define T_LONG		(500)


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

static pinState_t SwitchState;
static rotState_t Rotation;

static uint8_t ClockCnt;
static volatile bool	pressed;
static uint16_t			t_prell;
static uint16_t			t_long;

static uint8_t 			lastClockCnt;


// Interrupt Function Prototype für Zero
#ifndef ARDUINO_ARCH_AVR
void SwitchPinHandler(void);
void ClockPinHandler(void);
void DataPinHandler(void);
void InkrementProcess(void);
#endif



void button_init(void)
{
	t_prell=T_PRELL;
	t_long=T_LONG;
	SwitchState.ChangeTime=millis();
	SwitchState.State=!digitalRead(SWITCH_PIN_ARDUINO);
	SwitchState.Pin=SwitchState.State;
	SwitchState.Valid=true;
	
	Rotation.Clock=!digitalRead(CLOCK_PIN_ARDUINO);
	Rotation.Data=!digitalRead(DATA_PIN_ARDUINO);
	Rotation.ClockChange=false;
	Rotation.DataChange=false;
	Rotation.Left=false;
	Rotation.Right=false;
	
	pressed=false;
	ClockCnt=0;
	lastClockCnt=0;
	
	pinMode(CLOCK_PIN_ARDUINO,	INPUT_PULLUP);
	pinMode(DATA_PIN_ARDUINO,	INPUT_PULLUP);
	pinMode(SWITCH_PIN_ARDUINO,	INPUT_PULLUP);
	#ifdef ARDUINO_ARCH_AVR
		*digitalPinToPCMSK(CLOCK_PIN_ARDUINO) 	|= bit (digitalPinToPCMSKbit(CLOCK_PIN_ARDUINO));  	// enable Pin Change Mask 
		*digitalPinToPCMSK(SWITCH_PIN_ARDUINO) 	|= bit (digitalPinToPCMSKbit(SWITCH_PIN_ARDUINO));  	// enable Pin Change Mask
		*digitalPinToPCMSK(DATA_PIN_ARDUINO) 	|= bit (digitalPinToPCMSKbit(DATA_PIN_ARDUINO));  	// enable Pin Change Mask
		PCIFR	|= bit (digitalPinToPCICRbit(CLOCK_PIN_ARDUINO)); // clear any outstanding interrupt
		PCICR 	|= bit (digitalPinToPCICRbit(CLOCK_PIN_ARDUINO)); // enable interrupt for the group 
		PCIFR	|= bit (digitalPinToPCICRbit(DATA_PIN_ARDUINO)); // clear any outstanding interrupt
		PCICR 	|= bit (digitalPinToPCICRbit(DATA_PIN_ARDUINO)); // enable interrupt for the group 
		PCIFR	|= bit (digitalPinToPCICRbit(SWITCH_PIN_ARDUINO)); // clear any outstanding interrupt
		PCICR 	|= bit (digitalPinToPCICRbit(SWITCH_PIN_ARDUINO)); // enable interrupt for the group 
	#else
		attachInterrupt(digitalPinToInterrupt(CLOCK_PIN_ARDUINO), ClockPinHandler, CHANGE);	
		attachInterrupt(digitalPinToInterrupt(DATA_PIN_ARDUINO), DataPinHandler, CHANGE);	
		attachInterrupt(digitalPinToInterrupt(SWITCH_PIN_ARDUINO), SwitchPinHandler, CHANGE);	
	#endif

}

void button_process(void)
{
	uint16_t Time;
	if (!SwitchState.Valid) 
	{
		Time=millis();
		if ((uint16_t)(Time-SwitchState.ChangeTime)>t_prell)
		{
			SwitchState.State=SwitchState.Pin;
			SwitchState.Valid=true;
		}
	}
}

void button_setLong(uint16_t ms)
{
	t_long=ms;
}

int8_t button_turns(void)
{
	uint8_t tempCnt=ClockCnt;
	int8_t	nCnt=tempCnt-lastClockCnt;
	lastClockCnt=tempCnt;

	return nCnt;
}

bool button_down(void)
{
	return SwitchState.State;
}

bool button_pressed(void)
{
	bool bResult;
	bResult=pressed;
	pressed=false;
	return bResult;
}

bool button_held(void)
{
	return 	(SwitchState.State)	&&	
			(SwitchState.Valid) &&
			((uint16_t)(millis()-SwitchState.ChangeTime)>t_long);
}


#ifdef ARDUINO_ARCH_AVR
	// ISR for AVR acchitecture
	// currently using only PCINT0
	ISR (PCINT0_vect) // handle pin change interrupt for D0 to D7 here
	{	// AVR PCINT0
		bool bClock,bData,bSwitch;
		uint16_t t=millis();

		bClock=(CLOCK_PIN_PORT & (digitalPinToBitMask(CLOCK_PIN_ARDUINO)))==0;
		bData=(DATA_PIN_PORT & (digitalPinToBitMask(DATA_PIN_ARDUINO)))==0;
		bSwitch=(SWITCH_PIN_PORT & (digitalPinToBitMask(SWITCH_PIN_ARDUINO)))==0;
		
			
		if (SwitchState.Valid)
		{
			if ((!bSwitch) && (SwitchState.State))
				pressed=(uint16_t)(t-SwitchState.ChangeTime)<t_long;
			if (bSwitch !=SwitchState.State)
				SwitchState.Valid=false;
		}
		
		if (!SwitchState.Valid)
		{
			SwitchState.ChangeTime=t;
			SwitchState.Pin=bSwitch;
			SwitchState.Valid=false;
		}
		if (!(Rotation.Left && Rotation.Right))
		{	// Rotation state is valid
			
			if ((((bool)Rotation.Clock)!=bClock) && (((bool)Rotation.Data)!=bData))
			{	// single transition missed, rotation state invalid
				Rotation.Left=true;
				Rotation.Right=true; 
			}	// single transition missed, rotation state invalid
			else if (((bool)Rotation.Clock!=bClock) || ((bool)Rotation.Data!=bData))
			{	// single transition happend, proceed
				if ((bClock && bData) && (!(Rotation.Left || Rotation.Right)))
				{	// Mid State and no direction yet
					// previous state is either 
					// Clock=1 && Data=0 => Right or
					// Clock=0 && Data=1 => Left
					if (Rotation.Clock)
					{	// Right turn
						ClockCnt++;
						Rotation.Right=true;
					}	// Right turn
					else
					{	// Left turn
						ClockCnt--;
						Rotation.Left=true;
					}	// Left turn
				}	// Mid State and no direction yet
				Rotation.Clock=bClock;
				Rotation.Data=bData;
				if (!(Rotation.Clock || Rotation.Data))
				{	// Zero Position, reset
					Rotation.Left=false;
					Rotation.Right=false;
				}	// Zero Position, reset
			}	// single transition happend, proceed
		}	// Rotation state is valid
		else
		{	// Rotation state is invalid
			// Reset on zero position
			if ((!bClock) && (!bData))
			{	// Zero Position, reset
				Rotation.Left=false;
				Rotation.Right=false;
				Rotation.Clock=false;
				Rotation.Data=false;
			}	// Zero Position, reset
		}	// Rotation state is invalid  
	}	// AVR PCINT0
#else
	// ISR for ARM (SAMD21)
	// using ARDUINO EIC ISR Handler to seperate different PCINTs
	// each line of rotary switch gets ist own handler
	//	-> switch, data clock
	void SwitchPinHandler(void) 
	{	// switch pin interupt handler
		bool bSwitch;
		uint16_t t=millis();

		bSwitch=(SWITCH_PIN_PORT & (digitalPinToBitMask(SWITCH_PIN_ARDUINO)))==0;
			
		if (SwitchState.Valid)
		{
			if ((!bSwitch) && (SwitchState.State))
				pressed=(uint16_t)(t-SwitchState.ChangeTime)<t_long;
			if (bSwitch !=SwitchState.State)
				SwitchState.Valid=false;
		}
		
		if (!SwitchState.Valid)
		{
			SwitchState.ChangeTime=t;
			SwitchState.Pin=bSwitch;
			SwitchState.Valid=false;
		}
	}	// switch pin intterupt handler

	void ClockPinHandler(void)
	{	// SAMD21 EIC ClockPinInterrupt
		if (!Rotation.ClockChange)
		{	// filter filter bouncing
			Rotation.ClockChange=true;
			Rotation.DataChange=false;
			InkrementProcess();
		}	// filter filter bouncing
	}	// SAMD21 EIC ClockPinInterrupt

	void DataPinHandler(void)
	{	// SAMD21 EIC ClockPinInterrupt
		if (!Rotation.DataChange)
		{	// filter filter bouncing
			Rotation.ClockChange=false;
			Rotation.DataChange=true;
			InkrementProcess();
		}	// filter filter bouncing
	}	// SAMD21 EIC ClockPinInterrupt

	void InkrementProcess(void)
	{	// process Rotation afte pin change
		bool bClock,bData;

		if (Rotation.ClockChange)
		{	// ClockChange
			// Clock instable, use last clock
			// Data stable, use pin Data
			bClock=Rotation.Clock;	
			bData=(DATA_PIN_PORT & (digitalPinToBitMask(DATA_PIN_ARDUINO)))==0;
		}	// ClockChange

		if (Rotation.DataChange)
		{	// DataChange
			// Data instable, use last clock
			// Clock stable, use pin Data
			bClock=(CLOCK_PIN_PORT & (digitalPinToBitMask(CLOCK_PIN_ARDUINO)))==0;
			bData=Rotation.Data;
		}	// DataChange
		
		if (bClock!=Rotation.Clock)
		{	// Clock has Changed
			// Clock 1->0, Data==1 =>++
			if (!bClock && bData) 
				ClockCnt++;
			Rotation.Clock=bClock;
		}	// Clock has Changed

		if (bData!=Rotation.Data)
		{	// Data has Changed
			// Data 1->0, Clock==1 =>--
			if (bClock && !bData)
				ClockCnt--;
			Rotation.Data=bData;
		}	// Data has Changed
	}	// process Rotation afte pin change
#endif
