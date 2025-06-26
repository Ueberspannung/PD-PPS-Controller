#include <Arduino.h>
#include <Wire.h>
#include "INA232.h"

INA232::INA232(uint8_t addr, uint8_t alert_pin)
{
	i2c_address=addr;
	this->alert_pin=alert_pin;
	bConversionReadyProcessing=false;
	bOverflow=false;
	lastBusVoltage=0;
}

bool INA232::isINA232(void)
{	// isINA232
	return read_word(REG_ID)==INA232_ID;
}	// isINA232


bool INA232::conversion_ready(uint16_t * pBusVoltage_mV)
{	// conversion_ready
	bool bConversionReady;
	if (bConversionReadyProcessing)
	{	// conversion ready by alert
		bConversionReady=digitalRead(alert_pin);
		if (bConversionReady) 
			MaskRegisterValue=read_word(REG_MASK);
	}	// conversion ready by alert
	else
	{	// conversion ready in mask register
		bConversionReady=readMaskFlag(CONVERSION_READY_FLAG);
	}	// conversion ready in mask register
	
	if (bConversionReady)
	{	// conversion ready
		bOverflow=getMaskFlag(OVERFLOW_FLAG);
		lastBusVoltage=read_word(REG_BUS_VOLTAGE);
		lastBusVoltage*=MAX_BUS_VOLTAGE_mV;
		lastBusVoltage>>=BUS_VOLTAGE_BITS;
	}	// conversion ready
	
	*pBusVoltage_mV=lastBusVoltage;
	return bConversionReady;
}	// conversion_ready

void INA232::clear_ready_flag(void)
{	// clear_ready_flag
	MaskRegisterValue=read_word(REG_MASK);
}	// clear_ready_flag

bool INA232::overflow(void)
{	// overrun
	return bOverflow;
}	// overrun

int16_t INA232::getCurrent(void)
{
	/* arithmetic shift right!!!
		save procedure:
		if (x < 0 && n > 0)
			return x >> n | ~(~0U >> n);
		else
			return x >> n;
			
		gcc should provide arithmetic shift
	*/
	return ((int16_t)read_word(REG_CURRENT))>>current_scale;
}

int16_t INA232::getShuntVoltage(void)
{	// getShuntVoltage
	return (int16_t)read_word(REG_SHUNT_VOLTAGE);
}	// getShuntVoltage

uint16_t INA232::getPower(void)
{	//	getPower
	return read_word(REG_POWER);
}	//	getPower

void INA232::setCalibration(uint16_t Cal)
{	// setCalibration
	write_word(REG_CALIBRATION,Cal);
}	// setCalibration

void INA232::setCalibration(uint16_t Rshunt_mR, uint16_t CurrentResolution_mA, uint16_t MaxCurrent_mA)
{	// set calibration
	/*             0,00512            5120      
		CalVal=-----------------=-------------------
               Ilsb_A*Rshunt_Ohm Ilsb_mA*Rshunt_mOhm
     */
	uint16_t CalVal;
	gain_et	ShuntGain;
	ShuntGain=calcSuntGain(Rshunt_mR,MaxCurrent_mA);
	current_scale=0;
	// calculate internal scaler for max. precision
	// ex. 5A with 1mA resolution results in 5000 ticks, leave 3/4 unused
	// setting internal scaler to 2 (=*4) results in 20000 using 2/3 of range
	while (((1<<BUS_VOLTAGE_BITS)-1)>>(current_scale+1)>=MaxCurrent_mA/CurrentResolution_mA)
		current_scale++;
	
	CalVal=Rshunt_mR*CurrentResolution_mA;
	if (ShuntGain==ADC_RANGE_1) CalVal<<=2;
	CalVal=((CAL_BASE_VALUE<<current_scale)+(CalVal>>1))/CalVal;
		
	setCalibration(CalVal);
}	// set calibration

INA232::gain_et INA232::calcSuntGain(uint16_t Rshunt_mR, uint16_t MaxCurrent_mA)
{
	uint32_t maxShuntVoltage_uV;
	gain_et ShuntGain;
	
	maxShuntVoltage_uV=Rshunt_mR;
	maxShuntVoltage_uV*=MaxCurrent_mA;
	if (maxShuntVoltage_uV>MIN_SHUNT_VOLTAGE_uV)
		ShuntGain=ADC_RANGE_0;
	else
		ShuntGain=ADC_RANGE_1;
	return ShuntGain;
}


uint16_t INA232::getCalibration(void)
{	// getCalibration
	return read_word(REG_CALIBRATION);
}	// getCalibration

void INA232::reset(void)
{	// reset
	uint16_t config=read_word(REG_CONFIG);
	config|=CFG_RST_MASK;
	write_word(REG_CONFIG,config);
	bConversionReadyProcessing=false;
	bOverflow=false;
	lastBusVoltage=0;
}	// reset

									
void INA232::setAdcAveraging(uint8_t scale)	// 0-7 = 0-3 2^(2*scale) samples
{	// setAdcAveraging						//		 4-7 2^(3+scale) samoles
	uint16_t config=read_word(REG_CONFIG);
	uint16_t adc_mode=0;
	config&= ~CFG_AVG_MASK;
	adc_mode= scale & ADC_AVERAGE_MASK;
	adc_mode<<=CFG_AVG_SHIFT;
	config|=adc_mode;
	write_word(REG_CONFIG,config);
}	// setAdcAveraging								

void INA232::setAdcConversionTime(	adc_selector_et adc,
									uint8_t time)	// 0-7 ~2^(time+6)us
{	// setAdcConversionTime
	uint16_t config=read_word(REG_CONFIG);
	uint16_t adc_mode=0;
	config&= ~((adc==VOLTAGE)?(CFG_VBUSCT_MASK):(CFG_VSHCT_MASK));
	adc_mode= time & ADC_CONVERSION_MASK;
	adc_mode<<=((adc==VOLTAGE)?(CFG_VBUSCT_SHIFT):(CFG_VSHCT_SHIFT));
	config|=adc_mode;
	write_word(REG_CONFIG,config);
}	// setAdcConversionTime

void INA232::setShuntGain(		gain_et gain)
{	// setShuntGain
	uint16_t config=read_word(REG_CONFIG);
	if (gain==ADC_RANGE_0)
		config&=~CFG_ADCRNG_MASK;
	else
		config|=CFG_ADCRNG_MASK;
	write_word(REG_CONFIG,config);
}	// setShuntGain

void INA232::setOperationMode(	op_mode_et mode)
{	// setOperationMode
	uint16_t config=read_word(REG_CONFIG);
	config&= ~CFG_MODE_MASK;
	config|= ((uint16_t)mode) & CFG_MODE_MASK;
	write_word(REG_CONFIG,config);
	bOverflow=false;
}	// setOperationMode

void INA232::setMaskFlag(event_msk_et Flag, bool bEnable)
{	// setMaskFlag
	MaskRegisterValue=read_word(REG_MASK);
	if (bEnable)
		MaskRegisterValue|=((uint16_t)1<<(uint8_t)Flag);
	else
		MaskRegisterValue&=~((uint16_t)1<<(uint8_t)Flag);
	write_word(REG_MASK,MaskRegisterValue);
	
	// modify ConversionReadyProcessing if necessary
	if (Flag==CONVERSION_READY_ALERT_ENABLE)
		bConversionReadyProcessing&=bEnable;
}	// setMaskFlag

bool INA232::readMaskFlag(event_msk_et Flag)
{	// readMaskFlag
	MaskRegisterValue=read_word(REG_MASK);
	return getMaskFlag(Flag);
}	// readMaskFlag

bool INA232::getMaskFlag(event_msk_et Flag)
{	// getMaskFlag
	return	MaskRegisterValue & ((uint16_t)1<<(uint8_t)Flag);
}	// getMaskFlag

void INA232::enableConversionReadyAlertProcessing(bool bEnable)
{	// enableConversionReadyAlertProcessing
	if ((uint8_t)~alert_pin)
	{	// alert pin has been defined, activate
		pinMode(alert_pin,INPUT_PULLUP);
		
		setMaskFlag(CONVERSION_READY_ALERT_ENABLE,bEnable);
		bConversionReadyProcessing=bEnable;
	}	// alert pin has been defined, activate
	else
	{	// no pin defined, disable
		bConversionReadyProcessing=false;
	}	// no pin defined, disable
}	// enableConversionReadyAlertProcessing

bool INA232::isConversionReadyAlertProcessing(void)
{	// isConversionReadyAlertProcessing
	return bConversionReadyProcessing;
}	// isConversionReadyAlertProcessing



uint16_t INA232::read_word(uint8_t reg)
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

void INA232::write_word(uint8_t reg, uint16_t val)
{	// write_word
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.write((val>>8)&0x00FF);
	Wire.write(val&0x00FF);
	Wire.endTransmission();
}	// write_word
