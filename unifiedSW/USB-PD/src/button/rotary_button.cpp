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
#include "rotary_button.h"


rotary_button_c * rotary_button_c::pContext;

void rotary_button_c::init(void)
{
	t_prell=T_PRELL;
	t_long=T_LONG;
	SwitchState.ChangeTime=millis();
	SwitchState.State=!digitalRead(SWITCH_PIN);
	SwitchState.Pin=SwitchState.State;
	SwitchState.Valid=true;
	
	Rotation.Clock=!digitalRead(CLOCK_PIN);
	Rotation.Data=!digitalRead(DATA_PIN);
	Rotation.ClockChange=false;
	Rotation.DataChange=false;
	Rotation.Left=false;
	Rotation.Right=false;
	
	bPressed=false;
	ClockCnt=0;
	lastClockCnt=0;
	
	pinMode(CLOCK_PIN,	INPUT_PULLUP);
	pinMode(DATA_PIN,	INPUT_PULLUP);
	pinMode(SWITCH_PIN,	INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CLOCK_PIN), ClockPinInt, CHANGE);	
	attachInterrupt(digitalPinToInterrupt(DATA_PIN), DataPinInt, CHANGE);	
	attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), SwitchPinInt, CHANGE);	
	pContext=this;
}

void rotary_button_c::process(void)
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

void rotary_button_c::setLong(uint16_t ms)
{
	t_long=ms;
}

int8_t rotary_button_c::turns(void)
{
	uint8_t tempCnt=ClockCnt;
	int8_t	nCnt=tempCnt-lastClockCnt;
	lastClockCnt=tempCnt;

	return nCnt;
}

bool rotary_button_c::down(void)
{
	return SwitchState.State;
}

bool rotary_button_c::pressed(void)
{
	bool bResult;
	bResult=bPressed;
	bPressed=false;
	return bResult;
}

bool rotary_button_c::long_press(void)
{
	return 	(SwitchState.State)	&&	
			(SwitchState.Valid) &&
			((uint16_t)(millis()-SwitchState.ChangeTime)>t_long);
}




// ISR for ARM (SAMD21)
// using ARDUINO EIC ISR Handler to seperate different PCINTs
// each line of rotary switch gets ist own handler
//	-> switch, data clock
void rotary_button_c::SwitchPinHandler(void) 
{	// switch pin interupt handler
	bool bSwitch;
	uint16_t t=millis();

	bSwitch=(*portInputRegister(digitalPinToPort(SWITCH_PIN)) & (digitalPinToBitMask(SWITCH_PIN)))==0;
		
	if (SwitchState.Valid)
	{
		if ((!bSwitch) && (SwitchState.State))
			bPressed=(uint16_t)(t-SwitchState.ChangeTime)<t_long;
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

void rotary_button_c::ClockPinHandler(void)
{	// SAMD21 EIC ClockPinInterrupt
	if (!Rotation.ClockChange)
	{	// filter filter bouncing
		Rotation.ClockChange=true;
		Rotation.DataChange=false;
		InkrementProcess();
	}	// filter filter bouncing
}	// SAMD21 EIC ClockPinInterrupt

void rotary_button_c::DataPinHandler(void)
{	// SAMD21 EIC ClockPinInterrupt
	if (!Rotation.DataChange)
	{	// filter filter bouncing
		Rotation.ClockChange=false;
		Rotation.DataChange=true;
		InkrementProcess();
	}	// filter filter bouncing
}	// SAMD21 EIC ClockPinInterrupt

void rotary_button_c::InkrementProcess(void)
{	// process Rotation afte pin change
	bool bClock,bData;
	
	bClock=Rotation.Clock;	
	bData=Rotation.Data;

	if (Rotation.ClockChange)
	{	// ClockChange
		// Clock instable, use last clock
		// Data stable, use pin Data
		// bClock=Rotation.Clock;	
		bData=(*portInputRegister(digitalPinToPort(DATA_PIN)) & (digitalPinToBitMask(DATA_PIN)))==0;
	}	// ClockChange

	if (Rotation.DataChange)
	{	// DataChange
		// Data instable, use last clock
		// Clock stable, use pin Data
		bClock=(*portInputRegister(digitalPinToPort(CLOCK_PIN)) & (digitalPinToBitMask(CLOCK_PIN)))==0;
		// bData=Rotation.Data;
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

void rotary_button_c::ClockPinInt(void)
{	// Clock pin interrupt functon
	pContext->ClockPinHandler(); // call Handler
}	// Clock pin interrupt functon


void rotary_button_c::DataPinInt(void)
{	// Data Pin Interrupt function
	pContext->DataPinHandler();	// call handler
}	// Data Pin Interrupt function
	
void rotary_button_c::SwitchPinInt(void)
{	// Switch Pin Interruptfunction 
	pContext->SwitchPinHandler(); // call Handler
}	// Switch Pin Interruptfunction 
