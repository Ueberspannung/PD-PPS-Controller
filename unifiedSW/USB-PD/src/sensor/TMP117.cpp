#include <Arduino.h>
#include <Wire.h>
#include "TMP117.h"

TMP117::TMP117(uint8_t addr, uint8_t alert_pin)
{	// constructor
	i2c_address=addr;
	eeprom_state=EEPROM_START;
	int_pin = alert_pin;
	int_pin_active_high=false;
}	// constructor

bool TMP117::init(void)				// test if chip exists and set int pin
{	// init
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
}	// init

bool TMP117::process_idle(void)
{	// process
	switch (eeprom_state)
	{	// switch eeprom_state
		case EEPROM_IDLE:
			break;
		case EEPROM_WRITE_WAIT:
			// delay required time before proceeding
			if ((uint16_t)(millis()-time)>EEPROM_WRITE_DELAY)
				eeprom_state=EEPROM_WRITE_BUSY;
			break;
		case EEPROM_WRITE_BUSY:
		case EEPROM_START:
			// EEPROM Flag in configuration register will be
			// high during reset and write operation
			configReg.RegisterData=read_word(REG_CONFIGURATION);
			if (!configReg.Flags.EEPROM_Busy)
				eeprom_state=EEPROM_IDLE;
			break;
		default:
			eeprom_state=EEPROM_IDLE;
			break;
	}	// switch eeprom_state
	return eeprom_state==EEPROM_IDLE;
}	// process

bool TMP117::isAlert(void)				// test Alert Pin
{	// isAlert
	bool bAlert=false;
	if (int_pin!=0xFF)
	{	// pin exists
		bAlert=digitalRead(int_pin);
		bAlert=(int_pin_active_high)?(bAlert):(!bAlert);
	}	// pin exists
	else
	{	// read config register
		configReg.RegisterData=read_word(REG_CONFIGURATION);
		bAlert=	(configReg.Flags.AlertMode) ? 
				(configReg.Flags.DataReady) :
				(configReg.Flags.highTempAlert || configReg.Flags.lowTempAlert);
	}	// read config register
	return bAlert;
}	// isAlert


int16_t TMP117::getTemp(uint8_t decimals)	// gets temperature in decimal fixed point notation
{	// getTemp(uint8_t decimals)
	return convertToDec(getTemp(),decimals);
}	// getTemp(uint8_t decimals)

int16_t TMP117::getTemp(void)				// gets temperature in binary s8.7 fixed point notiation
{	// getTemp(void)
	return read_word(REG_TEMPERATURE);
}	// getTemp(void)

void TMP117::setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)	// set value in decimal fixed point notation
{	// setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)
	setHighTemperaturLimit(convertToIQ(tempDec,decimals));
}	// setHighTemperaturLimit(int16_t tempDec, uint8_t decimals)

void TMP117::setHighTemperaturLimit(int16_t tempIQ)					// set value in binary s8.7 fixed point notation
{	//	setHighTemperaturLimit(int16_t tempIQ)
	write_word(REG_HIGH_TEMP_LIMIT,tempIQ);
}	//	setHighTemperaturLimit(int16_t tempIQ)

void TMP117::setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)	// set value in decimal fixed point notation
{	// setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)
	setLowTemperaturLimit(convertToIQ(tempDec,decimals));
}	// setLowTemperaturLimit(int16_t tempDec, uint8_t decimals)

void TMP117::setLowTemperaturLimit(int16_t tempIQ)					// set value in binary s8.7 fixed point notation
{	//	setHighTemperaturLimit(int16_t tempIQ)
	write_word(REG_LOW_TEMP_LIMIT,tempIQ);
}	//	setHighTemperaturLimit(int16_t tempIQ)

int16_t TMP117::getHighTemperaturLimit(uint8_t decimals)	// get value in decimal fixed point notation
{	// getHighTemperaturLimit(uint8_t decimals)
	return convertToDec(getHighTemperaturLimit(),decimals);
}	// getHighTemperaturLimit(uint8_t decimals)

int16_t TMP117::getHighTemperaturLimit(void)				// get value in binary s8.7 fixed point notation 
{	// getHighTemperaturLimit(void)
	return read_word(REG_HIGH_TEMP_LIMIT);
}	// getHighTemperaturLimit(void)

int16_t TMP117::getLowTemperaturLimit(uint8_t decimals)		// get value in decimal fixed point notation
{	// getLOWTemperaturLimit(uint8_t decimals)
	return convertToDec(getLowTemperaturLimit(),decimals);
}	// getLOWTemperaturLimit(uint8_t decimals)

int16_t TMP117::getLowTemperaturLimit(void)                 // get value in binary s8.7 fixed point notation
{	// getLowTemperaturLimit(void)
	return read_word(REG_LOW_TEMP_LIMIT);
}	// getLowTemperaturLimit(void)

uint16_t TMP117::getDeviceID(void)
{	// getDeviceID
	deviceID_ut deviceID;
	deviceID.RegisterData=read_word(REG_DEVICE_ID);
	return deviceID.Flags.ID;
}	// getDeviceID

uint8_t TMP117::getDeviceRevision(void)
{	// getDeviceRevision
	deviceID_ut deviceID;
	deviceID.RegisterData=read_word(REG_DEVICE_ID);
	return deviceID.Flags.Rev;
}	// getDeviceRevision


void TMP117::reset(void)
{	// reset
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.SoftReset=true;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
	eeprom_state=EEPROM_START;
}	// reset

void TMP117::setAlertPinSource(alert_pin_select_et source)
{	// setAlertPinSource
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AlertPinSelect=source==ALERT_PIN_DATA_READY;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
} 	// setAlertPinSource

void TMP117::setAlertPinPolarity(alert_pin_polarity_et polarity)
{	// setAlertPinPolarity
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AlertPinPolarity=polarity==ALERT_PIN_ACTIVE_HIGH;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
	int_pin_active_high=polarity==ALERT_PIN_ACTIVE_HIGH;
}	// setAlertPinPolarity

void TMP117::setAlertMode(alert_mode_select_et mode)
{	// setAlertMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AlertMode=mode==ALERT_MODE_ALERT;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
}	// setAlertMode

void TMP117::setAveragingMode(averaging_mode_et mode)
{	// setAveragingMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.AveragingMode=(uint8_t)mode;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
}	// setAveragingMode

void TMP117::setConversionTime(conversion_time_et time)
{	// setConversionTime
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.ConversionCycleTime=(uint8_t)time;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
}	// setConversionTime

void TMP117::setConversionMode(conversion_mode_et mode)
{	// setConversionMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	configReg.Flags.ConversionMode=(uint8_t)mode;
	write_word(REG_CONFIGURATION,configReg.RegisterData);
}	// setConversionMode


TMP117::alert_mode_select_et 	TMP117::getAlertPinSource(void)
{	// getAlertPinSource
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (alert_mode_select_et)configReg.Flags.AlertPinSelect;
}	// getAlertPinSource

TMP117::alert_pin_polarity_et 	TMP117::getAlertPinPolarity(void)
{	// getAlertPinPolarity
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (alert_pin_polarity_et)configReg.Flags.AlertPinPolarity;
}	// getAlertPinPolarity

TMP117::alert_mode_select_et 	TMP117::getAlertMode(void)
{	// getAlertMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (alert_mode_select_et)configReg.Flags.AlertMode;
}	// getAlertMode

TMP117::averaging_mode_et		TMP117::getAveragingMode(void)
{	// getAveragingMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (averaging_mode_et)configReg.Flags.AveragingMode;
}	// getAveragingMode

TMP117::conversion_time_et		TMP117::getConversionTime(void)
{	// getConversionTime
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (conversion_time_et)configReg.Flags.ConversionCycleTime;
}	// getConversionTime

TMP117::conversion_mode_et		TMP117::getConversionMode(void)
{	// getConversionMode
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return (conversion_mode_et)configReg.Flags.ConversionMode;
}	// getConversionMode

bool TMP117::isDataReady(void)
{	// isDataReady
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return configReg.Flags.DataReady;
}	// isDataReady

bool TMP117::testHighTemperatureAlert(void)
{	// testHighTemperatureAlert
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return configReg.Flags.highTempAlert;
}	// testHighTemperatureAlert

bool TMP117::testLowTemperatureAlert(void)
{	// testLowTemperatureAlert
	configReg.RegisterData=read_word(REG_CONFIGURATION);
	return configReg.Flags.lowTempAlert;
}	// testLowTemperatureAlert

bool TMP117::testEepromBusy(void)
{	// testEepromBusy	
	EEPROM_UnlockReg_ut unlockReg;
	unlockReg.RegisterData=read_word(REG_EEPROM_UNLOCK);
	return unlockReg.Flags.EEPROM_Busy;
}	// testEepromBusy

bool TMP117::testEepromLocked(void)
{	// testEepromLocked
	EEPROM_UnlockReg_ut unlockReg;
	unlockReg.RegisterData=read_word(REG_EEPROM_UNLOCK);
	return unlockReg.Flags.EUN;
}	// testEepromLocked

void TMP117::setEepromLockState(eeprom_lock_mode_et lock)
{	// setEepromLockState
	EEPROM_UnlockReg_ut unlockReg;
	unlockReg.RegisterData=read_word(REG_EEPROM_UNLOCK);
	unlockReg.Flags.EUN=lock==EEPROM_UNLOCK;
	write_word(REG_EEPROM_UNLOCK,unlockReg.RegisterData);
}	// setEepromLockState

void TMP117::writeEeprom(eeprom_pos_et Register, uint16_t val)
{	// writeEeprom(eeprom_pos_et Register, uint16_t val) 
	write_word(REG_EEPROM_1+(uint8_t)Register,val);
	eeprom_state=EEPROM_WRITE_WAIT;
}	// writeEeprom(eeprom_pos_et Register, uint16_t val)

void TMP117::writeTemperatureOffset(int16_t val)
{	// writeTemperatureOffset
	writeEeprom(TEMPERATURE_OFFSET,(uint16_t) val);
}	// writeTemperatureOffset

uint16_t TMP117::readEeprom(eeprom_pos_et Register)
{	// uint16_t readEeprom
	return read_word(Register);
}	// uint16_t readEeprom

int16_t TMP117::readTemperatureOffset(void)
{	// int16_t readEeprom
	return (int16_t)readEeprom(TEMPERATURE_OFFSET);
}	// int16_t readEeprom
		
		
/* *********************************************************************
 * private functions
 * ********************************************************************* */

int16_t TMP117::convertToIQ(int16_t valDec, uint8_t decimals)	// converts a value given as
																// integer part of temperature*10^decimals
																// to IQ 9.7 value
																// 9.5°C given as (95,1) yields in 0x04C0	
{	//	convertToIQ
	/* valIQ=valDec/10^decimals*2^7
	 * 		=valDec/5^decimals*2^(7-decimals)
	 */
	uint8_t shifts=7-decimals;
	bool	overflow=false;

	switch (decimals)
	{	// check if overflow when converting
		case 0:
			overflow|=valDec>255;
			overflow|=valDec<-256;
			break;
		case 1:
			overflow|=valDec>2559;
			overflow|=valDec<-2560;
			break;
		case 2:
			overflow|=valDec>25599;
			overflow|=valDec<-25600;
			break;
		default:
			overflow=true;
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

int16_t TMP117::convertToDec(int16_t valIQ, uint8_t decimals)	// converts a value given as IQ 9.7 value to
																// the integer part of temperature*10^decimals
																// 9,5°C given as IQ 9.7 equals 0x04C0 
																// (0x04C0,1) yields in 95	 
{	//	convertToDec
	/* valDec=valIQ*10^decimals/2^7
	 * 		 =valIQ*5^decimals/2^(7-decimals)
	 */
	uint8_t shifts=7-decimals;
	
	if (decimals>2)	valIQ=INT16_MAX;
		
	if ((valIQ!=INT16_MIN) && (valIQ!=INT16_MAX))
	{	// convert
		while ((shifts>0) || (decimals>0))
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
	
	return valIQ;
}	//	convertToDec

uint16_t TMP117::read_word(uint8_t reg)
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

void TMP117::write_word(uint8_t reg, uint16_t val)
{	// write_word
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.write((val>>8)&0x00FF);
	Wire.write(val&0x00FF);
	Wire.endTransmission();
}	// write_word
