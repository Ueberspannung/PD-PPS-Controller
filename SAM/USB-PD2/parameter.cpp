#include <Arduino.h>
#include "parameter.h"

/*
#define PRINTLN(x) 		Serial1.println(x)
#define PRINTLN_HEX(x) 	Serial1.println(x,HEX)
#define PRINT(x)		Serial1.print(x)
#define PRINT_HEX(x)	Serial1.print(x,HEX)
*/
#define PRINTLN(x) 	
#define PRINTLN_HEX(x)
#define PRINT(x)	
#define PRINT_HEX(x)

const uint8_t PROGMEM parameter::pgmFF[]={	0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF };

parameter::parameter()
{	// parameter
	setInitData();
	initPos();
	ParameterPos=0;
	Address		=0;
}	// parameter		parameter_st Parameter;


bool parameter::process(void)		// returns true if busy
{	// processe

	switch (paramState)
	{	//	switch (paramState)
		case para_start:
			PRINTLN(F("param_start"));
			uint16_t Flag;
			Eeprom.read(flagAddress,&Flag);
			if (flagVal!=Flag)
			{	// incorrect identifier
				paramState=para_clear;
				Address=0;
			}	// incorrect identifier
			else
			{	// correct identifier
				paramState=para_init;
			}	// correct identifier
			break;
		case para_init:
			PRINTLN(F("para_init"));
			Eeprom.read(calcParamAddress(0),(uint8_t*)&Parameter,sizeof(Parameter));
			bDirFlag=Parameter.dir;
			initPos();
			paramState=para_read;
			break;
		case para_clear:
			PRINT(F("para_clear "));
			PRINT_HEX(Address);
			PRINT(F(" / "));
			PRINT_HEX(Eeprom.getAddressSpace());
			PRINT(F(" : "));
			PRINT(getProgress());
			PRINTLN(F("%"));
			Eeprom.write_P(Address,(const char *)pgmFF,sizeof(pgmFF));
			Address+=sizeof(pgmFF);
			if ((Address==0) || (Address>=Eeprom.getAddressSpace()))
				paramState=para_write_flag;
			break;	
		case para_write_flag:
			PRINTLN(F("para_write_flag"));
			Eeprom.write(flagAddress,flagVal);
			paramState=para_write_first;
			break;
		case para_write_first:
			PRINTLN(F("para_write_first"));
			Parameter.dir=false;
			bDirFlag=false;
			ParameterPos=0;
			Eeprom.write(calcParamAddress(ParameterPos),(uint8_t*)&Parameter,sizeof(Parameter));
			paramState=para_idle;
			break;
		case para_write:
			PRINTLN(F("para_write"));
			if (ParameterPos>=calcParamCnt()-1)
			{	// end Of memory reached
				// inver direction flag and start from 0
				ParameterPos=0;
				bDirFlag=!bDirFlag;
			}	// end Of memory reached
			else
			{	// stil place to go
				ParameterPos++;
			}	// stil place to go
			Parameter.dir=bDirFlag;
			Eeprom.write(calcParamAddress(ParameterPos),(uint8_t*)&Parameter,sizeof(Parameter));
			paramState=para_idle;
			break;
		case para_read:
			PRINT(F("para_read ( "));
			PRINT(lowPos);
			PRINT(F(" / "));
			PRINT(midPos);
			PRINT(F(" / "));
			PRINT(highPos);
			PRINTLN(F(" )"));
			Eeprom.read(calcParamAddress(midPos),(uint8_t*)&Parameter,sizeof(Parameter));
			if (lowPos==highPos)
			{	// last read, ready
				ParameterPos=midPos;
				paramState=para_idle;
			}	// last read, ready
			else if ((midPos==highPos) && (!getDirection()))
			{	// manually set next read
				// this means lowPos is highPos-1
				midPos=lowPos;
				highPos=lowPos;
				// and once more
			}	// manually set next read
			else
			{	// continue search
				nextPos(getDirection());
			}	// continue search
			break;
		case para_idle:
			break;
		default:
			break;
	}	//	switch (paramState)
	return paramState!=para_idle;
}	// processe

uint8_t parameter::getProgress(void)
{	// getProgress
	uint16_t progress=100;
	switch (paramState)
	{	//	switch (paramState)
		case para_start:
			progress=0;
			break;
		case para_init:
			progress=1;
			break;
		case para_clear:
			progress=calcPercentage(Address,Eeprom.getAddressSpace())+1;
			if (progress>98) progress=98;
			break;	
		case para_write_flag:
			progress=99;
			break;
		case para_write_first:
			progress=100;
			break;
		case para_write:
			progress=100;
			break;
		case para_read:
			progress=calcPercentage(calcParamCnt()-(highPos-lowPos),calcParamCnt());
			break;
		case para_idle:
			progress=100;
			break;
		default:
			progress=100;
			break;
	}	//	switch (paramState)

	return (uint8_t) progress;
}	//	getProgress

void parameter::read(void)
{	// read
	paramState=para_init;
}	// read

void parameter::write(void)
{	// write
	paramState=para_write;
}	// write

void parameter::clear(bool bSetInitData)
{	// clear
	if (bSetInitData)
		setInitData();
	Address=0;
	paramState=para_clear;
}	// clear

void parameter::setBrightness(uint8_t brightness)
{	// set brightness value
	Parameter.Brightness=brightness;
}	// set brightness value

void parameter::setAutoSet(bool bAuto)
{	// setAutoSet
	Parameter.autoSet=bAuto;
}	// setAutoSet

void parameter::setAutoOn(bool bAuto)
{	//	setAutoOn
	Parameter.autoOn=bAuto;
}	//	setAutoOn

void parameter::setModePPS(bool bPPS)
{	// 	setModePPS
	Parameter.PPS=bPPS;
}	//	setModePPS

void parameter::setCVCC(controller_c::operating_mode_et mode)
{	// setCVCC
	Parameter.CVCC=(uint8_t)mode;
}	// setCVCC

void parameter::setCalU(uint16_t Cal, uint16_t Ref)
{	// setCalU
	Parameter.CalU=Cal;
	Parameter.RefU=Ref;
}	// setCalU

void parameter::setCalI(uint16_t Cal, uint16_t Ref)
{	// setCalI
	Parameter.CalI=Cal;
	Parameter.RefI=Ref;
}	// setCalI


void parameter::setVoltage_mV(uint16_t VoltageStep)
{	//	setVoltageStep
	Parameter.setU=VoltageStep;
}	//	setVoltageStep

void parameter::setCurrent_mA(uint16_t CurrentStep)
{	//	setVoltageStep
	Parameter.setI=CurrentStep;
}	//	setVoltageStep

uint8_t parameter::getBrightness(void)
{	// get Brightness
	return Parameter.Brightness;
}	// get Brightness

bool parameter::getAutoSet(void)
{	// 	getAutoSet
	return Parameter.autoSet;
}	// 	getAutoSet

bool parameter::getAutoOn(void)
{	// getAutoOn
	return Parameter.autoOn;
}	// getAutoOn

bool parameter::getModePPS(void)
{	// getModePPS
	return Parameter.PPS;
}	// getModePPS

controller_c::operating_mode_et parameter::getCVCC(void)
{	// getCVCC
	return (controller_c::operating_mode_et)Parameter.CVCC;
}	// getCVCC

void parameter::getCalU(uint16_t *pCal, uint16_t *pRef)
{	// get CalU
	*pCal=Parameter.CalU;
	*pRef=Parameter.RefU;
}	// get CalU

void parameter::getCalI(uint16_t *pCal, uint16_t *pRef)
{	// get CalI
	*pCal=Parameter.CalI;
	*pRef=Parameter.RefI;
}	// get CalI

uint16_t parameter::getVoltage_mV(void)
{	// getVoltageStep
	return Parameter.setU;
}	// getVoltageStep

uint16_t parameter::getCurrent_mA(void)
{	// getCurrentStep
	return Parameter.setI;
}	// getCurrentStep

/* ************************************************
 * privta funkctions
 * ************************************************
 */
	
uint16_t parameter::calcParamCnt(void)
{	//	calcParamCnt
	return (Eeprom.getAddressSpace()-DataOffset)/sizeof(parameter_st);
}	//	calcParamCnt

uint16_t parameter::calcParamAddress(uint16_t ParameterPos)
{	//	calcParamAddress
	return DataOffset+ParameterPos*sizeof(parameter_st);
}	//	calcParamAddress

void parameter::nextPos(bool bUp)
{	//	nextPos
	if (bUp)
	{	// use upper half
		lowPos=midPos;
		midPos= (lowPos+highPos+1)>>1;
	}	// use upper half
	else
	{	// use lower half
		highPos=midPos;
		midPos= (lowPos+highPos)>>1;
	}	// use lower half
}	// 	nextPos

void parameter::initPos(void)
{	// initPos
	lowPos=0;
	highPos=calcParamCnt()-1;;
	midPos=0;
	nextPos(true);
}	// initPos


bool parameter::getDirection(void)	// true =up
{	// getDirection
	// !Parameter.dir converts binary data to boolean
	return bDirFlag!=(!Parameter.dir);
}	// getDirection

uint16_t parameter::calcPercentage(uint16_t part, uint16_t total)
{	// try to calculate percentage as exact as possible
	if (part<=655)
	{	// *100 /1
		part*=100;
	}	// *100 /1
	else if (part<=6553) 
	{	// *10 /10
		part*=10;
		total/=10;
	}	// *10 /10
	else
	{	// *1 /100
		total/=100;
	}	// *1 /100
	
	part=part/total;
	if (part>100) part =100;

	return part;
}	// try to calculate percentage as exact as possible

void parameter::setInitData(void)
{	//	setInitData
	Parameter.CalU		=20000;	// Calibration for Output Voltage	
	Parameter.RefU		=20000;	// internal measurement for Calibration
	Parameter.CalI		=5000;	// Calibration for output current
	Parameter.RefI		=5000;	// internal measurement for calibration
	Parameter.setU		=5000;	// start value voltage mV
	Parameter.setI		=1000;	// start value current mA
	Parameter.Brightness=50; 	// backlight 50%
	Parameter.PPS		=false;	// slect fix or pps profile
	Parameter.autoSet	=false;	// Flag for automatic setup to last settings on startup
	Parameter.autoOn	=false;	// implies autoSet but switches output on startup
	Parameter.CVCC		=controller_c::CONTROLLER_MODE_OFF;
	Parameter.dir		=false;	// indicator for rotation memory usage	
	bDirFlag			=false;	// initial diriction not swapped
}	// 	setInitData

#undef PRITNLN
#undef PRITN
#undef PRITNLN_HEX
#undef PRITN_HEX
