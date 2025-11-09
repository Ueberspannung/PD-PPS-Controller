#include <Arduino.h>
#include <Wire.h>
#include "TMP1075.h"

TMP1075_c::TMP1075_c(uint8_t addr, uint8_t alert_pin)
{	// constructor
	i2c_address=addr;
	int_pin = alert_pin;
	tmp1075_type=DETECTION_IDLE;
	int_pin_active_high=false;
}	// constructor

		
bool TMP1075_c::init(void)
{	// i2cProbe + pin init
	bool exists;
	Wire.beginTransmission(i2c_address);
	exists=(!Wire.endTransmission());
	if (int_pin!=0xFF)
	{	// int_pin available
		if (exists)
		{	// chip available, configure int pin
			// first guess pint is active low, enable PullUp
			pinMode(int_pin,INPUT_PULLUP);
		}	// chip available, configure int pin
		else
		{	// no chip -> no pin
			int_pin=-1;
		}	// no chip -> no pin	
	}	// int_pin available
	return exists;
}	// i2cProbe + pin init

bool TMP1075_c::isAlert(void)				// test Alert Pin
{	// isAlert
	bool bAlert=false;
	if (int_pin!=0xFF)
	{	// pin exists
		bAlert=digitalRead(int_pin);
		bAlert=(int_pin_active_high)?(bAlert):(!bAlert);
	}	// pin exists
	return bAlert;
}	// isAlert


int16_t TMP1075_c::getTemp(uint8_t decimals)	// gets temperature in decimal fixed point notation
{	// getTemp(uint8_t decimals)
	return convertToDec(getTemp(),decimals);
}	// getTemp(uint8_t decimals)

int16_t TMP1075_c::getTemp(void)				// gets temperature in binary s7.8 fixed point notiation
{	// getTemp(void)
	return read_word(REG_TEMPERATURE);
}	// getTemp(void)

void TMP1075_c::setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)	// set value in decimal fixed point notation
{	// setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)
	setHighTemperaturLimit(convertToIQ(tempDec,decimals));
}	// setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)

void TMP1075_c::setHighTemperaturLimit(int16_t tempIQ)					// set value in binary s7.8 fixed point notation
{	//	setHighTemperaturLimit(int16_t tempIQ)
	write_word(REG_HIGH_TEMP_LIMIT,tempIQ);
}	//	setHighTemperaturLimit(int16_t tempIQ)

void TMP1075_c::setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)	// set value in decimal fixed point notation
{	// setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)
	setLowTemperaturLimit(convertToIQ(tempDec,decimals));
}	// setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)

void TMP1075_c::setLowTemperaturLimit(int16_t tempIQ)					// set value in binary s7.8 fixed point notation
{	//	setHighTemperaturLimit(int16_t tempIQ)
	write_word(REG_LOW_TEMP_LIMIT,tempIQ);
}	//	setHighTemperaturLimit(int16_t tempIQ)

int16_t TMP1075_c::getHighTemperaturLimit(uint8_t decimals)	// get value in decimal fixed point notation
{	// getHighTemperaturLimit(uint8_t decimals)
	return convertToDec(getHighTemperaturLimit(),decimals);
}	// getHighTemperaturLimit(uint8_t decimals)

int16_t TMP1075_c::getHighTemperaturLimit(void)				// get value in binary s7.8 fixed point notation 
{	// getHighTemperaturLimit(void)
	return read_word(REG_HIGH_TEMP_LIMIT);
}	// getHighTemperaturLimit(void)

int16_t TMP1075_c::getLowTemperaturLimit(uint8_t decimals)		// get value in decimal fixed point notation
{	// getLOWTemperaturLimit(uint8_t decimals)
	return convertToDec(getLowTemperaturLimit(),decimals);
}	// getLOWTemperaturLimit(uint8_t decimals)

int16_t TMP1075_c::getLowTemperaturLimit(void)                 // get value in binary s7.8 fixed point notation
{	// getLowTemperaturLimit(void)
	return read_word(REG_LOW_TEMP_LIMIT);
}	// getLowTemperaturLimit(void)

uint16_t TMP1075_c::getDeviceID(void)
{	// getDeviceID
	return read_word(REG_DEVICE_ID);
}	// getDeviceID


void TMP1075_c::setAlertPinPolarity(alert_pin_polarity_et polarity)
{	// setAlertPinPolarity
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AlertPinPolarity=polarity==ALERT_PIN_ACTIVE_HIGH;
	if (int_pin!=0xFF)
	{	// int pin exists
		if (configReg.Flags.AlertPinPolarity)
		{	// active High
			int_pin_active_high=true;
			pinMode(int_pin,INPUT_PULLDOWN);
		}	// active High
		else
		{	// active Low
			int_pin_active_high=false;
			pinMode(int_pin,INPUT_PULLUP);
		}	// active Low
	}	// int pin exists
	write_byte(REG_CONFIGURATION,configReg.RegisterWord.hi);
}	// setAlertPinPolarity

void TMP1075_c::setAlertMode(alert_mode_select_et mode)
{	// setAlertMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AlertMode=mode==ALERT_MODE_INTERRUPT;
	write_byte(REG_CONFIGURATION,configReg.RegisterWord.hi);
}	// setAlertMode

void TMP1075_c::setFaultCount(fault_count_et count)
{	// setAveragingMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.FaultCount=(uint8_t)count;
	write_byte(REG_CONFIGURATION,configReg.RegisterWord.hi);
}	// setAveragingMode

void TMP1075_c::setConversionTime(conversion_time_et time)
{	// setConversionTime
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.ConversionCycleTime=(uint8_t)time;
	write_byte(REG_CONFIGURATION,configReg.RegisterWord.hi);
}	// setConversionTime

void TMP1075_c::setConversionMode(conversion_mode_et mode)
{	// setConversionMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.ShutDown=mode!=MODE_CONTINUOUS;
	configReg.Flags.OneShotMode=mode==MODE_ONE_SHOT;
	write_byte(REG_CONFIGURATION,configReg.RegisterWord.hi);
}	// setConversionMode


TMP1075_c::alert_pin_polarity_et 	TMP1075_c::getAlertPinPolarity(void)
{	// getAlertPinPolarity
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (alert_pin_polarity_et)configReg.Flags.AlertPinPolarity;
}	// getAlertPinPolarity

TMP1075_c::alert_mode_select_et 	TMP1075_c::getAlertMode(void)
{	// getAlertMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (alert_mode_select_et)configReg.Flags.AlertMode;
}	// getAlertMode

TMP1075_c::fault_count_et			TMP1075_c::getFaultCount(void)
{	// getAveragingMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (fault_count_et)configReg.Flags.FaultCount;
}	// getAveragingMode

TMP1075_c::conversion_time_et		TMP1075_c::getConversionTime(void)
{	// getConversionTime
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (conversion_time_et)configReg.Flags.ConversionCycleTime;
}	// getConversionTime

TMP1075_c::conversion_mode_et		TMP1075_c::getConversionMode(void)
{	// getConversionMode
	conversion_mode_et  mode=MODE_SHUTDOWN;
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	
	if (!configReg.Flags.ShutDown)
		mode=MODE_CONTINUOUS;
	else if (configReg.Flags.OneShotMode)
		mode=MODE_ONE_SHOT;
	
	return mode;
}	// getConversionMode

// // only on TMP1075N in one shot mode
bool TMP1075_c::isDataReady(void)
{	// isDataReady
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return configReg.Flags.OneShotMode;
}	// isDataReady

bool TMP1075_c::isTMP1075N(void)	// ceck for N revision
{	// isTMP1075N
	if (tmp1075_type==DETECTION_IDLE)
	{	// not yet cheked
		configReg.RegisterData=read_word(REG_CONFIGURATION);
		tmp1075_type=(configReg.Flags.reserved==0xFF)?(DETECTED_TMP1075):(DETECTED_TMP1075N);
	}	// not yet cheked
	return (tmp1075_type==DETECTED_TMP1075N);
}	// isTMP1075N
		
		
/* *********************************************************************
 * private functions
 * ********************************************************************* */

int16_t TMP1075_c::convertToIQ(int16_t valDec, uint8_t decimals)	// converts a value given as
																// integer part of temperature*10^decimals
																// to IQ 9.7 value
																// 9.5°C given as (95,1) yields in 0x04C0	
{	//	convertToIQ
	/* valIQ=valDec/10^decimals*2^8
	 * 		=valDec/5^decimals*2^(8-decimals)
	 */
	uint8_t shifts=8-decimals;
	bool	overflow=false;

	switch (decimals)
	{	// check if overflow when converting
		case 0:
			overflow|=valDec>127;
			overflow|=valDec<-128;
			break;
		case 1:
			overflow|=valDec>1279;
			overflow|=valDec<-1280;
			break;
		case 2:
			overflow|=valDec>12799;
			overflow|=valDec<-12800;
			break;
		default:
			overflow=false;
	}	// check if overflow when converting
	
	if (!overflow)
	{	// convert
		while ((shifts>0) || (decimals>0))
		{	// interchange leftshift and division to guarantee max precision
			while ((valDec<INT16_MAX>>1) && (valDec>INT16_MIN>>1) && (shifts>0))
			{	// shift while result ist less than Max
				shifts--;
				valDec<<=1;
			}	// shift while result ist less than Max
			if (decimals>0)
			{	// divide
				decimals--;
				valDec/=5;
			}	// divide
		}	// interchange leftshift and division to guarantee max precision
	}	// convert
	else
	{	// saturate
		if (valDec>0)
			valDec=INT16_MAX;
		else
			valDec=INT16_MIN;
	}	// saturate
	
	return valDec;
}	//	convertToIQ

int16_t TMP1075_c::convertToDec(int16_t valIQ, uint8_t decimals)	// converts a value given as IQ 8.8 value to
																	// the integer part of temperature*10^decimals
																	// 9,5°C given as IQ 8.8 equals 0x0980 
																	// (0x0980,1) yields in 95	 
{	//	convertToDec
	/* valDec=valIQ*10^decimals/2^8
	 * 		 =valIQ*5^decimals/2^(8-decimals)
	 */
	uint8_t shifts=8-decimals;
	
	//if (decimals>2)	valIQ=INT16_MAX;
		
	if ((valIQ!=INT16_MIN) && (valIQ!=INT16_MAX))
	{	// convert
		while (	(shifts>0) ||   
				( 	(decimals>0) && 
					(valIQ<INT16_MAX/5) && 
					(valIQ>INT16_MIN/5)	)	)
		{	// interchange rightshift and multiplication to guarantee max precision
			while ((valIQ<INT16_MAX/5) && (valIQ>INT16_MIN/5) && (decimals>0))
			{	//  multiply while result ist less than Max
				decimals--;
				valIQ*=5;
			}	// multiply  while result ist less than Max
			if (shifts>0)
			{	// shift, needs to be implemented als arithmetc shift
				shifts--;
				valIQ>>=1;
			}	// shift, needs to be implemented als arithmetc shift
		}	// interchange rightshift and multiplication to guarantee max precision
	}	// convert
	
	if (decimals!=0) 
	{	// overflow?
		if (valIQ>0) 
			valIQ=INT16_MAX;
		else
			valIQ=INT16_MIN;
	}	// overflow?
	
	return valIQ;
}	//	convertToDec

uint16_t TMP1075_c::read_word(uint8_t reg)
{	// read_word
	uint16_t temp=0;
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(i2c_address,2);
	temp=(uint8_t)Wire.read();
	temp<<=8;
	temp|=(uint8_t)Wire.read();
	return temp;
}	// read_word

void TMP1075_c::write_word(uint8_t reg, uint16_t val)
{	// write_word
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.write((val>>8)&0x00FF);
	Wire.write(val&0x00FF);
	Wire.endTransmission();
}	// write_word

void TMP1075_c::write_byte(uint8_t reg, uint8_t val)
{	// write_word
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission();
}	// write_word
