#include <Arduino.h>
#include <Wire.h>
#include "eeprom.h"

eeprom::eeprom(eeprom_density_t density_kbit, uint8_t i2c_base, uint8_t devices)
{	// eeprom	
	chip_address=i2c_base;
	chip_cnt=devices;

	switch (density_kbit)
	{
		case eeprom_1kbit:
		case eeprom_2kbit:
			chip_page_size=8;
			break;
		case eeprom_4kbit:
		case eeprom_8kbit:
		case eeprom_16kbit:
			chip_page_size=16;
			break;
		case eeprom_32kbit:
		case eeprom_64kbit:
			chip_page_size=32;
			break;
		case eeprom_128kbit:
		case eeprom_256kbit:
			chip_page_size=64;
			break;
		case eeprom_512kbit:
			chip_page_size=128;
			break;
		default:
			break;
	}

	switch (density_kbit)
	{
		case eeprom_4kbit:
			chip_offset_shift=1;
			break;
		case eeprom_8kbit:
			chip_offset_shift=2;
			break;
		case eeprom_16kbit:
			chip_offset_shift=3;
			break;
		default:
			chip_offset_shift=0;
			break;
	}
	
	
	if (density_kbit>eeprom_16kbit) 
		i2c_addressbytes=2;
	else
		i2c_addressbytes=1;

	chip_address_bits=(uint8_t)density_kbit+7;
	
	if (chip_page_size>(BUFFER_LENGTH>>1)) 
		chip_page_size=BUFFER_LENGTH>>1;
	
	density=density_kbit;
	
	writeDelayMax=7500;	// max 5 ms, sicherjeitshalber 7,5 warten
}	// eeprom

bool eeprom::test()
{	// test
	uint8_t cnt;
	bool bOk=true;
	cnt=chip_cnt;
	while (bOk && (cnt>0))
		bOk=test(--cnt);
	return bOk;
}	// test

bool eeprom::test(uint8_t device)
{	// test()
	return i2cProbe(chip_address+(device<<chip_offset_shift));
}	// test()

bool eeprom::read(uint16_t address, uint8_t * pData, uint16_t nCnt)
{	// read
	bool bOk=true;
	uint8_t nReadCnt;
	
	while (bOk && (nCnt>0))
	{
		nReadCnt=chip_page_size-(address % chip_page_size);
		if (nReadCnt>nCnt) nReadCnt=nCnt;
		bOk=read_chip(getChipAddress(address),getAddressOffset(address),pData,nReadCnt);
		pData+=nReadCnt;
		address+=nReadCnt;
		nCnt-=nReadCnt;
	}
	return bOk;
}	// read

bool eeprom::read(uint16_t address, uint16_t * pData)
{	// read(uint16)
	return read(address,(uint8_t*)pData,sizeof(uint16_t));
}	// read(uint16)


bool eeprom::write(uint16_t address, uint8_t Data)
{	// write single byte
	return write(address,&Data);
}	// write single byte


bool eeprom::write(uint16_t address, uint16_t Data)
{	// write(uint16)
	return write(address,(uint8_t*)&Data,sizeof(Data));
}	// write(uint16)

bool eeprom::write(uint16_t address, uint8_t * pData, uint16_t nCnt)
{	// write
	bool bOk=true;
	uint8_t nWriteCnt;
	
	while (bOk && (nCnt>0))
	{
		nWriteCnt=chip_page_size-(address % chip_page_size);
		if (nWriteCnt>nCnt) nWriteCnt=nCnt;
		bOk=write_page(getChipAddress(address),getAddressOffset(address),pData,nWriteCnt);
		pData+=nWriteCnt;
		nCnt-=nWriteCnt;
		address+=nWriteCnt;
	}
	
	return bOk;
}	// write


bool eeprom::write(	uint16_t address, 	// write byte array
					const __FlashStringHelper * pData, 
					uint16_t nCnt)
{	// write from flash
	return write_P(address,(const char*)pData,16);
}	// write from flash
 

bool eeprom::write_P(	uint16_t address, 	// write byte array
						const char * pData, 
						uint16_t nCnt)
{	// write from flash
	bool bOk=true;
	uint8_t nWriteCnt;
	
	while (bOk && (nCnt>0))
	{
		nWriteCnt=chip_page_size-(address % chip_page_size);
		if (nWriteCnt>nCnt) nWriteCnt=nCnt;
		bOk=write_page_P(getChipAddress(address),getAddressOffset(address),(const uint8_t*)pData,nWriteCnt);
		pData+=nWriteCnt;
		nCnt-=nWriteCnt;
		address+=nWriteCnt;
	}
	
	return bOk;
}	// write from flash


uint16_t eeprom::getAddressSpace(void)
{	// getAddressSpace
	uint16_t result=128;	// = eeprom_1kbit
	result<<=density;
	result*=chip_cnt;
	return result;
}	// getAddressSpace

/************/
/* Private  */										
/************/
		
bool eeprom::i2cProbe(uint8_t i2c_address)
{	// i2cProbe
	//Serial1.print(F("eeprom::i2cProbe:"));
	//Serial1.println(i2c_address,HEX);
	Wire.beginTransmission(i2c_address);
	return (!Wire.endTransmission());
}	// i2cProbe

bool eeprom::read_chip(uint8_t i2c_address, uint16_t address, uint8_t * pData, uint16_t nCnt)
{
	bool bOk=true;
	
	//Serial1.print(F("eeprom::read_chip: i2c="));
	//Serial1.print(i2c_address,HEX);
	//Serial1.print(F(", a="));
	//Serial1.print(address,HEX);
	//Serial1.print(F(", n="));
	//Serial1.print(nCnt);
	//Serial1.print(F(", p="));
	//Serial1.print((uint16_t)pData,HEX);

	Wire.beginTransmission(i2c_address);
	if (i2c_addressbytes==2)
		Wire.write((uint8_t)(address>>8));
	Wire.write((uint8_t)(address & 0x00FF));
	bOk=Wire.endTransmission(false)==0;
	//if (bOk)
	//	Serial1.print(F(", i2c ok"));
	//else
	//	Serial1.print(F(", i2c fail"));

	bOk=bOk && (Wire.requestFrom(i2c_address,nCnt)==nCnt);
	//if (bOk)
	//	Serial1.print(F(", read ok"));
	//else
	//	Serial1.print(F(", read fail"));
	//Serial1.println();

	while (Wire.available()) *pData++=Wire.read();
	
	return bOk;
}

bool eeprom::write_byte(uint8_t i2c_address, uint16_t address, uint8_t Data)
{
	return write_page(i2c_address,address,&Data);
}

bool eeprom::write_page(uint8_t i2c_address, uint16_t address, uint8_t *pData, uint16_t nCnt)
{
	bool bOk;

	//Serial1.print(F("eeprom::write_page: i2c="));
	//Serial1.print(i2c_address,HEX);
	//Serial1.print(F(", a="));
	//Serial1.print(address,HEX);
	//Serial1.print(F(", n="));
	//Serial1.print(nCnt);
	//Serial1.print(F(", p="));
	//Serial1.print((uint16_t)pData,HEX);
	Wire.beginTransmission(i2c_address);
	if (i2c_addressbytes==2)
		Wire.write((uint8_t)(address>>8));
	Wire.write((uint8_t)(address & 0x00FF));
	Wire.write(pData,nCnt);
	bOk=Wire.endTransmission()==0;
	//if (bOk)
	//	Serial1.print(F(", i2c ok"));
	//else
	//	Serial1.print(F(", i2c fail"));
	bOk=bOk && acknowledgePolling(getChipAddress(address));
	//if (bOk)
	//	Serial1.print(F(", write ok"));
	//else
	//	Serial1.print(F(", write fail"));
	//Serial1.println();
	//while (nCnt>0) 
	//{
	//	Serial1.print(*pData++,HEX);
	//	Serial1.print(F(" "));
	//	nCnt--;
	//}
	//Serial1.println();
	return bOk;
}

bool eeprom::write_page_P(	uint8_t i2c_address, 	// write a page page size max 1/2 Wire Buffer form Flash
							uint16_t address, 
							const uint8_t *pData, 
							uint16_t nCnt)
{	// write_page_P
	bool bOk;

	Wire.beginTransmission(i2c_address);
	if (i2c_addressbytes==2)
		Wire.write((uint8_t)(address>>8));
	Wire.write((uint8_t)(address & 0x00FF));
	while(nCnt)
	{
		Wire.write(pgm_read_byte(pData));
		nCnt--;
		pData++;
	}
	bOk=Wire.endTransmission()==0;
	bOk=bOk && acknowledgePolling(getChipAddress(address));
	return bOk;
}	//write_page_P

bool eeprom::acknowledgePolling(uint8_t i2c_address)
{	//	acknowledgePolling
	uint16_t cnt=0;

	// max 5ms Schribzyklus, 9 bit+Start / Stop @100kHz ~0,1ms @400kHz ~0,025ms
	// -> max 50 Aufrufe @100kHz, zur Sicherheit 400
	while (!i2cProbe(i2c_address) && (cnt<400)) cnt++;
	
	return i2cProbe(i2c_address);
}	//	acknowledgePolling


uint8_t eeprom::getChipAddress(uint16_t address)
{	// getChipAddress
	uint8_t address_high;
	
	address_high=(uint8_t)(address>>chip_address_bits);
	address_high&=0x07;
	return chip_address+address_high;
}	// getChipAddress

uint16_t eeprom::getAddressOffset(uint16_t address)
{
	address&=(1<<chip_address_bits)-1;
	return address;
}



