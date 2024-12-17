#include <Arduino.h>
#include <Wire.h>
#include "lcdIO.h"

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


// define I2c address of PCF8574 0x20-0x27 / 0x38-0x3F (A version)
// 0-7, lower 3 bit autodetect PCF8574 / PCF8574A
// 0xff Autodetect
// valid adress within range, no autodetect
// Autodetect will start with init command, Wire is not available at construction
lcdIO::lcdIO(uint8_t i2c_address, uint8_t inverterPin, uint8_t brightnessPin, unsigned char Index):cat4004{brightnessPin,Index}		
{	// lcdIO
	processing_state.state=lcdIO_idle;
	processing_state.RegisterData=0;
	processing_state.pRegisterData=0;
	processing_state.i2c_address=i2c_address;
	processing_state.inverterPin=inverterPin;
	processing_state.brightnessPin=brightnessPin;
	processing_state.BacklightOn=false;
	processing_state.NibbleMode=false;
	processing_state.i2c_set=false;
	processing_state.Memory=false;
	processing_state.PinData.Data=0xF0;
}	// lcdIO

// init: BusModeNibble=false, 											
// enable Byte-Transfer (in 4bit Mode) BusModeNibble=true,											
// return i2c_address or 0 if no PF8574(A) is found		
uint8_t lcdIO::init(bool BusModeNibble)
{
	if (BusModeNibble==false)
	{	// not yet initialized, check i2c
		if (processing_state.i2c_address==0xFF)
		{	// autodetect
			for (	processing_state.i2c_address=0;
					processing_state.i2c_address<8;
					processing_state.i2c_address++)
			{	// test all offsets
				processing_state.i2c_set=i2cProbe(processing_state.i2c_address+0x20);
				if (processing_state.i2c_set)
				{	// success PCF8574 (0x20+Offset)
					processing_state.i2c_address+=0x20;
				}	// success PCF8574 (0x20+Offset)
				else	
				{	// test PCF8574A (0x38+Offset)
					processing_state.i2c_set=i2cProbe(processing_state.i2c_address+0x38);
					if (processing_state.i2c_set)
					{	// success PCF8574 (0x38+Offset)
						processing_state.i2c_address+=0x38;
					}	// success PCF8574 (0x38+Offset)
				}	// test PCF8574A (0x38+Offset)
				if (processing_state.i2c_set) break;
			}	// test all offsets
		}	// autodetect
		if (processing_state.i2c_address>0x07)
		{	// fixed address
			processing_state.i2c_set=i2cProbe(processing_state.i2c_address);
			if (!processing_state.i2c_set) 
				processing_state.i2c_address=0xFF;
		}	// fixed address
		else 
		{	//	fixed offset
			// test PCF8574 (0x20+Offset) 
			processing_state.i2c_set=i2cProbe(processing_state.i2c_address+0x20);
			if (processing_state.i2c_set)
			{	// success PCF8574 (0x20+Offset)
				processing_state.i2c_address+=0x20;
			}	// success PCF8574 (0x20+Offset)
			else	
			{	// test PCF8574A (0x38+Offset)
				processing_state.i2c_set=i2cProbe(processing_state.i2c_address+0x38);
				if (processing_state.i2c_set)
				{	// success PCF8574 (0x38+Offset)
					processing_state.i2c_address+=0x38;
				}	// success PCF8574 (0x38+Offset)
				else processing_state.i2c_address=0xFF;
			}	// test PCF8574A (0x38+Offset)
		}	//	fixed offset
		
		if (processing_state.i2c_set)
		{	// inital write to PCF8574
			Wire.beginTransmission(processing_state.i2c_address);
			Wire.write(processing_state.PinData.Data);
			Wire.endTransmission();
		}	// inital write to PCF8574
	}	// not yet initialized, check i2c
		
	if (processing_state.inverterPin!=0xFF)
		analogWrite(processing_state.inverterPin,128);

	/* handled in cat4004
	if (processing_state.brightnessPin!=0xFF)
	{
		pinMode(processing_state.brightnessPin,OUTPUT);
		digitalWrite(processing_state.brightnessPin,LOW);
	}
	*/
	
	processing_state.NibbleMode=BusModeNibble;
	
	return processing_state.i2c_address;
}

												 
												
												
void lcdIO::process(void)	// process actions (sequence of miltiple transfers)
{	// 	process
	switch (processing_state.state)		// lcdIO process states for 4bit i2c access
	{	// 	switch state
		case lcdIO_idle:				// wait for request
			break;
		case lcdIO_write_hi:			// write high nibble to bus
			PRINTLN(F("lcdIO::lcdIO_write_hi"));
			writeNibble();
			if (processing_state.NibbleMode)
				processing_state.state=lcdIO_write_lo;
			else
				processing_state.state=lcdIO_idle;
			break;
		case lcdIO_write_lo:			// write low nibble to bus
			PRINTLN(F("lcdIO::lcdIO_write_lo"));
			writeNibble();
			processing_state.state=lcdIO_idle;
			break;
		case lcdIO_read_setup_enable:	// prepare read and enable bus
			PRINT(F("lcdIO::lcdIO_read_setup_enable"));
			readEnable();
			processing_state.state=lcdIO_read_hi;
			break;
		case lcdIO_read_hi:			// read hi nibble
			PRINT(F("lcdIO::lcdIO_read_hi"));
			readNibble();
			processing_state.state=lcdIO_read_next;
			break;
		case lcdIO_read_next:			// low pulse on enable line
			PRINT(F("lcdIO::lcdIO_read_next"));
			readNext();
			processing_state.state=lcdIO_read_lo;
			break;
		case lcdIO_read_lo:			// read low nibble
			PRINT(F("lcdIO::lcdIO_read_lo"));
			readNibble();
			processing_state.state=lcdIO_read_disable;
			break;
		case lcdIO_read_disable: 		// disable bus
			PRINT(F("lcdIO::lcdIO_read_disable"));
			readDisable();
			processing_state.state=lcdIO_idle;
			break;
		default:
			PRINT(F("lcdIO::default"));
			processing_state.state=lcdIO_idle;
			break;
	}	// 	switch state
	
	cat4004::process();
}	// 	process

bool lcdIO::isBusy(void)	// read / write operation in progress
{	//	isBusys
	return processing_state.state!=lcdIO_idle;
}	//	isBusys

bool lcdIO::is_i2c_set(void)				// Check if first init call was succesful
{	// DisplayFound
	return processing_state.i2c_set;
}	// DisplayFound



uint8_t lcdIO::getInstructionRegisterFlag(HD44780_Instruction_et Instruction)
{	// getInstructionRegisterFlag
	return (1<<Instruction);
}	// getInstructionRegisterFlag

void lcdIO::writeInstructionRegister(HD44780_Rgister_ut InstructionRegister)
{	// writeInstructionRegister
	processing_state.RegisterData=InstructionRegister.Data;
	processing_state.Memory=false;
	processing_state.state=lcdIO_write_hi;
}	// writeInstructionRegister

void lcdIO::readInstructionRegister(HD44780_Rgister_ut * pInstructionRegister)
{	// 	readInstructionRegister
	processing_state.pRegisterData=&pInstructionRegister->Data;
	processing_state.Memory=false;
	processing_state.state=lcdIO_read_setup_enable;
}	// 	readInstructionRegister

void lcdIO::writeRAM(uint8_t Data)
{	// writeRAM
	processing_state.RegisterData=Data;
	processing_state.Memory=true;
	processing_state.state=lcdIO_write_hi;
}	// writeRAM

void lcdIO::readRAM(uint8_t * pData)
{	// readRAM
	processing_state.pRegisterData=pData;
	processing_state.Memory=true;
	processing_state.state=lcdIO_read_setup_enable;
}	// readRAM

void lcdIO::BacklightOn(bool bOn)
{	// BacklightOn
	processing_state.BacklightOn=bOn;
}	// BacklightOn

void lcdIO::SetContrast(uint8_t Contrast)
{
	// not yet defined
}

void lcdIO::SetBrightness(uint8_t Brightness)
{
	cat4004::set_brightness(Brightness);
}

uint8_t lcdIO::GetBrightness(void)
{
	return cat4004::get_brightness();
}

bool lcdIO::i2cProbe(uint8_t i2c_address)
{	// i2cProbe
	Wire.beginTransmission(i2c_address);
	return (!Wire.endTransmission());
}	// i2cProbe

//	Data has to be in upper 4 bits
void lcdIO::setPinDataPCF8574(	uint8_t Data, 
								bool bRS, bool bRW, bool bEnable)
{	//	setPinDataPCF8574
	processing_state.PinData.Data=Data;
	processing_state.PinData.Pin.rs=bRS;
	processing_state.PinData.Pin.rw=bRW;
	processing_state.PinData.Pin.e=bEnable;
	processing_state.PinData.Pin.bl=processing_state.BacklightOn;
}	//	setPinDataPCF8574


void lcdIO::writeNibble(void)
{	// write nibble to display
	Wire.beginTransmission(processing_state.i2c_address);
	if (processing_state.state==lcdIO_write_hi)
		setPinDataPCF8574(	processing_state.RegisterData,
							processing_state.Memory, 
							false, false);
	else
		setPinDataPCF8574(	processing_state.RegisterData<<4,
							processing_state.Memory, 
							false, false);
	Wire.write(processing_state.PinData.Data);
	processing_state.PinData.Pin.e=true;
	Wire.write(processing_state.PinData.Data);
	processing_state.PinData.Pin.e=false;
	Wire.write(processing_state.PinData.Data);
	Wire.endTransmission();
}	// write nibble to display

void lcdIO::readEnable(void)
{	// readEnable
	Wire.beginTransmission(processing_state.i2c_address);
	setPinDataPCF8574(	0xF0,
						processing_state.Memory, 
						true, false);
	processing_state.PinData.Pin.data=0xF;
	Wire.write(processing_state.PinData.Data);
	processing_state.PinData.Pin.e=true;
	Wire.write(processing_state.PinData.Data);
	Wire.endTransmission();
}	// readEnable

void lcdIO::readNext(void)
{	// readNext
	Wire.beginTransmission(processing_state.i2c_address);
	setPinDataPCF8574(	0xF0,
						processing_state.Memory, 
						true, false);
	Wire.write(processing_state.PinData.Data);
	processing_state.PinData.Pin.e=true;
	Wire.write(processing_state.PinData.Data);
	Wire.endTransmission();
}	// readNext

void lcdIO::readDisable(void)
{	// readDisable
	Wire.beginTransmission(processing_state.i2c_address);
	setPinDataPCF8574(	0xF0,
						processing_state.Memory, 
						true, false);
	Wire.write(processing_state.PinData.Data);
	Wire.endTransmission();
}	// readDisable

void lcdIO::readNibble(void)
{	// readNibble
	PCF8574_Data_ut PinData;
	Wire.requestFrom(processing_state.i2c_address,(uint8_t)1);
	*processing_state.pRegisterData=
		*processing_state.pRegisterData & 0xF0;
	PinData.Data=0;
	if (Wire.available())
		PinData.Data=Wire.read();
	if (processing_state.state==lcdIO_read_hi)
		*processing_state.pRegisterData=PinData.Data;
	else
		*processing_state.pRegisterData=
			*processing_state.pRegisterData | PinData.Pin.data;
}	// readNibble


#undef PRITNLN
#undef PRITN
#undef PRITNLN_HEX
#undef PRITN_HEX
