#include <Arduino.h>
#include <Wire.h>
#include "INA219.h"

INA219::INA219(uint8_t addr)
{
	i2c_address=addr;
	bOverflow=false;
}

bool INA219::conversion_ready(uint16_t * pBusVoltage_mV)
{	// conversion_ready
	bool bConversionReady;
	*pBusVoltage_mV=read_word(REG_BUS_VOLTAGE);
	bOverflow=*pBusVoltage_mV & 0x0001;
	*pBusVoltage_mV=*pBusVoltage_mV>>1;
	bConversionReady=*pBusVoltage_mV & 0x0001;
	*pBusVoltage_mV=*pBusVoltage_mV & 0xFFFC;
	return bConversionReady;
}	// conversion_ready

void INA219::clear_ready_flag(void)
{	// clear_ready_flag
	getPower();
}	// clear_ready_flag

bool INA219::overflow(void)
{	// overrun
	return bOverflow;
}	// overrun

int16_t INA219::getCurrent(void)
{
	return (int16_t)read_word(REG_CURRENT);
}

int16_t INA219::getShuntVoltage(void)
{	// getShuntVoltage
	return (int16_t)read_word(REG_SHUNT_VOLTAGE);
}	// getShuntVoltage

uint16_t INA219::getPower(void)
{	//	getPower
	bOverflow=false;
	return read_word(REG_POWER);
}	//	getPower

void INA219::setCalibration(uint16_t Cal)
{	// setCalibration
	write_word(REG_CALIBRATION,Cal);
}	// setCalibration

uint16_t INA219::getCalibration(void)
{	// getCalibration
	return read_word(REG_CALIBRATION);
}	// getCalibration

void INA219::reset(void)
{	// reset
	uint16_t config=read_word(REG_CONFIG);
	config|=CFG_RST_MASK;
	write_word(REG_CONFIG,config);
	bOverflow=false;
}	// reset

void INA219::setBusVoltageRange(bus_voltage_range_et range)
{
	uint16_t config=read_word(REG_CONFIG);
	if (range==BUS_VOLTAGE_16V)
		config&=~CFG_BRNG_MASK;
	else
		config|=CFG_BRNG_MASK;
	write_word(REG_CONFIG,config);
}

void INA219::setAdcResolution(	adc_selector_et adc,
								adc_res_et resolution)
{	// setAdcResolution
	uint16_t config=read_word(REG_CONFIG);
	uint16_t adc_mode;
	config&= ~((adc==CURRENT)?(CFG_SADC_MASK):(CFG_BADC_MASK));
	adc_mode= (uint16_t)resolution;
	adc_mode<<=(adc==CURRENT)?(CFG_SADC_SHIFT):(CFG_BADC_SHIFT);
	config|=adc_mode;
	write_word(REG_CONFIG,config);
}	// setAdcResolution
									
void INA219::setAdcAveraging(	adc_selector_et adc,
								uint8_t scale)	// 0-7 = averaging 2^scale samples
{ // setAdcAveraging
	uint16_t config=read_word(REG_CONFIG);
	uint16_t adc_mode;
	config&= ~((adc==CURRENT)?(CFG_SADC_MASK):(CFG_BADC_MASK));
	adc_mode= (scale & ADC_AVERAGE_MASK) | ADC_AVERAGE_FLAG;
	adc_mode<<=(adc==CURRENT)?(CFG_SADC_SHIFT):(CFG_BADC_SHIFT);
	config|=adc_mode;
	write_word(REG_CONFIG,config);
} // setAdcAveraging								

void INA219::setShuntGain(		gain_et gain)
{	// setShuntGain
	uint16_t config=read_word(REG_CONFIG);
	config&=~CFG_PG_MASK;
	config|=((uint16_t)gain)<<CFG_PG_SHIFT;
	write_word(REG_CONFIG,config);
}	// setShuntGain

void INA219::setOperationMode(	op_mode_et mode)
{	// setOperationMode
	uint16_t config=read_word(REG_CONFIG);
	config&= ~CFG_MODE_MASK;
	config|= ((uint16_t)mode) & CFG_MODE_MASK;
	write_word(REG_CONFIG,config);
	bOverflow=false;
}	// setOperationMode

uint16_t INA219::read_word(uint8_t reg)
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

void INA219::write_word(uint8_t reg, uint16_t val)
{	// write_word
	Wire.beginTransmission(i2c_address);
	Wire.write(reg);
	Wire.write((val>>8)&0x00FF);
	Wire.write(val&0x00FF);
	Wire.endTransmission();
}	// write_word
