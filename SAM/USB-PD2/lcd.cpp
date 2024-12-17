#include <Arduino.h>
//#include <Wire.h>
#include "lcd.h"


/*
#define PRINTLN(x) 		Serial1.println(x)
#define PRINTLN_HEX(x) 	Serial1.println(x,HEX)
#define PRINT(x)		Serial1.print(x)
#define PRINT_HEX(x)	Serial1.print(x,HEX)
#define WRITE(x)			Serial1.write(x)
*/
#define PRINTLN(x) 	
#define PRINTLN_HEX(x)
#define PRINT(x)	
#define PRINT_HEX(x)
#define WRITE(x)


const PROGMEM uint8_t lcd::BarIcons[][HD44780_CHAR_SIZE]=
		{	//{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 0/5 = Space
			{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10}, // 1/5
			{0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}, // 2/5
			{0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}, // 3/5
			{0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E}, // 4/5
			{0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}}; // 5/5 = A00 Charset 255



/********************************************************/
/*		PUBLIC functions								*/
/********************************************************/
lcd::lcd(uint8_t adresse, uint8_t inverterPin, uint8_t brightnessPin, unsigned char Index):lcdIO{adresse,inverterPin,brightnessPin,Index}
{	// lcd
	for (uint8_t n=0;n<HD44780_DDRAM_SIZE;n++)
	{	// DDRAM-Buffer initialisation
		LcdBuffer.DDRAM[n]=0x20;			// Space =display Clear
		getFlag(LcdBuffer.DDRAM_Flags,n);	// Display Clear done 
											// while init, no action requeire 
	}	// DDRAM-Buffer initialisation
	
	for (uint8_t n=0;n<HD44780_CGRAM_SIZE;n++)
	{	// DDRAM-Buffer initialisation 
		LcdBuffer.CGRAM[n]=0x00;
		setFlag(LcdBuffer.CGRAM_Flags,n);
	}

	LcdBuffer.Status.CursorX=0;
	LcdBuffer.Status.CursorY=0;
	LcdBuffer.Status.BufferPos=0;
	LcdBuffer.Status.Contrast=128;
	LcdBuffer.Status.DisplayFlags.CursorOn		=false;
	LcdBuffer.Status.DisplayFlags.CursorBlink	=false;
	LcdBuffer.Status.DisplayFlags.DisplayOn		=false;
	LcdBuffer.Status.DisplayFlags.BacklightOn	=false;
	LcdBuffer.Status.DisplayFlags.DisplayFlag 	=false;
	LcdBuffer.Status.DisplayFlags.PositionFlag	=false;
	LcdBuffer.Status.DisplayFlags.ContrastFlag	=true;
	LcdBuffer.Status.DisplayFlags.DisplayClear	=false;
	LcdBuffer.Status.DisplayFlags.Home			=false;
	LcdBuffer.Status.DisplayFlags.AddressSet	=true;
	
	ProcessState.waitState.ms_Start=	0;
	ProcessState.waitState.bWaiting=	false;
	ProcessState.waitState.ms_Delay=	0;
	ProcessState.LcdControlState=		HD44780_init_start;
	ProcessState.DDRAM_Pos=				0;		
	ProcessState.CGRAM_Pos=				0;		
	ProcessState.Columns=				0;
	ProcessState.Rows=					0;										
	ProcessState.LineSize=				0;	
	ProcessState.InitCnt=				0;
	
	HD44780_Register.Data=0;
	
	ProgressBar.x=0;
	ProgressBar.y=0;
	ProgressBar.length=0;
	ProgressBar.progress=0;

}	// lcd
	
void lcd::init(uint8_t cols, uint8_t rows)
{	// init
	ProcessState.Columns=cols;
	if (rows>4) rows=4;
	if (rows==3) rows=2;
	ProcessState.Rows	=rows;								
	ProcessState.LineSize=HD44780_DDRAM_SIZE/rows;
	
	lcdIO::init(false);
}	// init

void lcd::process(void)
{	// process

	lcdIO::process();

	if (ProcessState.waitState.bWaiting)
	{	// time if waiting
		ProcessState.waitState.bWaiting &=
			(uint16_t)(millis()-ProcessState.waitState.ms_Start)<
				ProcessState.waitState.ms_Delay;
	}	// time if waiting
	

	switch (ProcessState.LcdControlState)
	{	// switch (ProcessState.LcdControlState)
		case HD44780_init_start:					// 40ms Timer on power Up#
			if (is_i2c_set())
			{	// start if bus init was succesful
				PRINTLN(F("lcd::HD44780_init_start"));
				startWait(50);
				ProcessState.InitCnt=4;
				ProcessState.LcdControlState=HD44780_init_FunctionSet8bit_start;
			}	// start if bus init was succesful
			break;
		case HD44780_init_FunctionSet8bit_start:	// first (upper) 4bit Function set (0x30)
			if (!ProcessState.waitState.bWaiting)
			{	// not Waiting
				PRINTLN(F("lcd::HD44780_init_FunctionSet8bit_start "));
				PRINTLN(ProcessState.InitCnt);
				HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_FunctionSet);
				HD44780_Register.FunctionSet.DataLength=	// first 3 write 8bit, last 4 bit
					ProcessState.InitCnt!=0;	
				HD44780_Register.FunctionSet.Lines=1;		// 2/4 lines (dontCare)
				HD44780_Register.FunctionSet.Font=0;		// 5x8 Font	(dontCare)
				HD44780_Register.FunctionSet.dontCare=0x03;	 
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_init_FunctionSet8bit_wait;
			}	// not Waiting
			break;
		case HD44780_init_FunctionSet8bit_wait:	// initiate  delay 
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_FunctionSet8bit_wait"));
				ProcessState.LcdControlState=HD44780_init_FunctionSet8bit_start;
				ProcessState.InitCnt--;
				switch (ProcessState.InitCnt)
				{	// select individual delay
					case 3:
						startWait(10);	// second wait is 4.1 ms, set to 10
						break;
					case 2:
						startWait(2);	// third wait is 0.1 ms, set to 2
						break;
					case 1:				// no wait necessary
					case 0:
						break;
					default:
						lcdIO::init(true);			// enable 2x 4bit transfer
						ProcessState.LcdControlState=HD44780_init_FunctionSet4bit;
						LcdBuffer.Status.DisplayFlags.AddressSet=true;
						break;
				}	// select individual delay
			}	// bus operation finished
			break;
		case HD44780_init_FunctionSet4bit:			// full Function set (0010 l0xx) 1Lin l=0 2/4 Lines l=1
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_FunctionSet4bit"));
				HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_FunctionSet);
				HD44780_Register.FunctionSet.DataLength=0;	// set to 4 bit
				HD44780_Register.FunctionSet.Lines=
					ProcessState.Rows!=1;	// 0 = 1 line 1 = 2 / 4 Lines	
				HD44780_Register.FunctionSet.Font=0;		// 5x8 Font	(dontCare)
				HD44780_Register.FunctionSet.dontCare=0x03;	 
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_init_DisplayOff;
			}	// bus operation finished
			break;
		case HD44780_init_DisplayOff:				// turn off Display
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_DisplayOff"));
				HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_DisplayControl);
				HD44780_Register.DisplayControl.Blink=0;		// no blink
				HD44780_Register.DisplayControl.Cursor=0;		// no cursor
				HD44780_Register.DisplayControl.Display=0;		// display off
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_init_DisplayClear;
			}	// bus operation finished
			break;
		case HD44780_init_DisplayClear:				// Clear display buffer 
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_DisplayClear"));
				HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_ClearDisplay);
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_init_DisplayClear_wait;
			}	// bus operation finished
			break;
		case HD44780_init_DisplayClear_wait:		// wait for execution 2-3ms
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_DisplayClear_wait"));
				startWait(3);							// takes 1.52ms, so wee  need at leest 2 
				ProcessState.LcdControlState=HD44780_init_EntryModeSet;
			}	// bus operation finished
			break;
		case HD44780_init_EntryModeSet:				// Inkrent bei Puffer Schreiben
			if (!ProcessState.waitState.bWaiting)
			{	// not Waiting
				PRINTLN(F("lcd::HD44780_init_EntryModeSet"));
				HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_EntryModeSet);
				HD44780_Register.EntryModeSet.Shift=0;		// no display shift	
				HD44780_Register.EntryModeSet.Increment=1;	// increment on write
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_init_End;
			}	// not Waiting
			break;
		case HD44780_init_End:						// placeholder
			if (!isBusy())
			{	// bus operation finished
				PRINTLN(F("lcd::HD44780_init_End"));
				ProcessState.LcdControlState=HD44780_do_check_DDRAM_Buffer;
			}	// bus operation finished
			break;
		case HD44780_do_check_DDRAM_Buffer:			// check differences between buffers
			PRINT(F("lcd::HD44780_do_check_DDRAM_Buffer "));
			PRINTLN(LcdBuffer.Status.BufferPos);
			if (LcdBuffer.Status.DisplayFlags.DisplayClear)
			{	// on request of Display Clear perform Clear CMD immedately
				ProcessState.LcdControlState=HD44780_do_ClearDisplay;
			}	// on request of Display Clear perform Clear CMD immedately
			else
			{	// scan DDRAM Buffer for modifications
				while ( (LcdBuffer.Status.BufferPos<HD44780_DDRAM_SIZE) &&
						(!getFlag(LcdBuffer.DDRAM_Flags,LcdBuffer.Status.BufferPos)) )
				{	// skip non modified sections
					LcdBuffer.Status.DisplayFlags.AddressSet=true;
					LcdBuffer.Status.BufferPos++;
				}	// skip non modified sections
				
				if (LcdBuffer.Status.BufferPos>=HD44780_DDRAM_SIZE)
				{	// one scan finished, continue with CGRAM
					LcdBuffer.Status.DisplayFlags.AddressSet=true;
					LcdBuffer.Status.BufferPos=0;
					ProcessState.LcdControlState=HD44780_do_check_CGRAM_Buffer;
				}	// one scan finished, continue with CGRAM
				else
				{	// not finished, dirty flag found
					clearFlag(LcdBuffer.DDRAM_Flags,LcdBuffer.Status.BufferPos);
					if (LcdBuffer.Status.DisplayFlags.AddressSet)
					{	// first Transfer, set Address
						ProcessState.LcdControlState=HD44780_do_set_DDRAM_Pointer;
					}	// first Transfer, set Address
					else
					{	// not first, continue with Data transfer
						ProcessState.LcdControlState=HD44780_do_write_DDRAM_Data;
					}	// not first, continue with Data transfer
				}	// not finished, dirty flag found
			}	// scan DDRAM Buffer for modifications
			break;
		case HD44780_do_set_DDRAM_Pointer:				// on difference set corresponding buffer position
			if (!isBusy())
			{	// no bus operation pending
				PRINT(F("lcd::HD44780_do_set_DDRAM_Pointer "));
				HD44780_Register.Data=getInstructionRegisterFlag(lcd::HD44780_SetAddressDDRAM);
				HD44780_Register.SetAddressDDRAM.ADD=getDDRAM_Pos(LcdBuffer.Status.BufferPos);
				PRINTLN_HEX(HD44780_Register.SetAddressDDRAM.ADD);
				writeInstructionRegister(HD44780_Register);
				
				ProcessState.LcdControlState=HD44780_do_write_DDRAM_Data;
			}	// no bus operation pending
			break;
		case HD44780_do_write_DDRAM_Data:				// and transfer data
			if (!isBusy())
			{	// no bus operation pending
				PRINT(F("lcd::HD44780_do_write_DDRAM_Data :>"));
				WRITE(LcdBuffer.DDRAM[LcdBuffer.Status.BufferPos]);
				PRINTLN(F("<:"));
				writeRAM(LcdBuffer.DDRAM[LcdBuffer.Status.BufferPos]);
				LcdBuffer.Status.BufferPos++;
				if ( (LcdBuffer.Status.BufferPos==HD44780_DDRAM_LINE) &&
					 (ProcessState.Rows!=1) )
					LcdBuffer.Status.DisplayFlags.AddressSet=true;
				// DDRAM Counter modified restore cursor position
				LcdBuffer.Status.DisplayFlags.PositionFlag=true;

				ProcessState.LcdControlState=HD44780_do_check_DDRAM_Buffer;
			}	// no bus operation pending
			break;
		case HD44780_do_check_CGRAM_Buffer:			// check differences between buffers
			PRINT(F("lcd::HD44780_do_check_CGRAM_Buffer "));
			PRINTLN(LcdBuffer.Status.BufferPos);
			while ( (LcdBuffer.Status.BufferPos<HD44780_CGRAM_SIZE) &&
					(!getFlag(LcdBuffer.CGRAM_Flags,LcdBuffer.Status.BufferPos)) )
			{	// skip non modified sections
				LcdBuffer.Status.DisplayFlags.AddressSet=true;
				LcdBuffer.Status.BufferPos++;
			}	// skip non modified sections
			
			if (LcdBuffer.Status.BufferPos>=HD44780_CGRAM_SIZE)
			{	// one scan finished, continue with Cursor
				LcdBuffer.Status.DisplayFlags.AddressSet=true;
				LcdBuffer.Status.BufferPos=0;
				ProcessState.LcdControlState=HD44780_do_update_DisplayControl;
			}	// one scan finished, continue with Cursor
			else
			{	// not finished, dirty flag found
				clearFlag(LcdBuffer.CGRAM_Flags,LcdBuffer.Status.BufferPos);
				if (LcdBuffer.Status.DisplayFlags.AddressSet)
				{	// first Transfer, set Address
					ProcessState.LcdControlState=HD44780_do_set_CGRAM_Pointer;
				}	// first Transfer, set Address
				else
				{	// not first, continue with Data transfer
					ProcessState.LcdControlState=HD44780_do_write_CGRAM_Data;
				}	// not first, continue with Data transfer
			}	// not finished, dirty flag found
			break;
		case HD44780_do_set_CGRAM_Pointer:					// on difference set transfer pos
			if (!isBusy())
			{	// no bus operation pending
				PRINT(F("lcd::HD44780_do_set_CGRAM_Pointer "));
				HD44780_Register.Data=getInstructionRegisterFlag(lcd::HD44780_SetAddressCGRAM);
				HD44780_Register.SetAddressCGRAM.ACG=LcdBuffer.Status.BufferPos;
				PRINTLN(HD44780_Register.SetAddressCGRAM.ACG);
				writeInstructionRegister(HD44780_Register);
				ProcessState.LcdControlState=HD44780_do_write_CGRAM_Data;
			}	// no bus operation pending
			break;
		case HD44780_do_write_CGRAM_Data:				// transfer 8 bytes
			if (!isBusy())
			{	// no bus operation pending
				PRINT(F("lcd::HD44780_do_write_CGRAM_Data 0x"));
				PRINTLN_HEX(LcdBuffer.CGRAM[LcdBuffer.Status.BufferPos]);
				writeRAM(LcdBuffer.CGRAM[LcdBuffer.Status.BufferPos]);
				LcdBuffer.Status.BufferPos++;
				ProcessState.LcdControlState=HD44780_do_check_CGRAM_Buffer;
				// Address Counter modified restore cursor position
				LcdBuffer.Status.DisplayFlags.PositionFlag=true;
			}	// no bus operation pending
			break;
		case HD44780_do_update_DisplayControl:			// update display control register
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("lcd::HD44780_do_update_DisplayControl"));
				if (LcdBuffer.Status.DisplayFlags.DisplayFlag)
				{	// Display Flag set, send DisplayControl CMD
					lcdIO::BacklightOn(LcdBuffer.Status.DisplayFlags.BacklightOn);
					HD44780_Register.Data=getInstructionRegisterFlag(lcd::HD44780_DisplayControl);
					HD44780_Register.DisplayControl.Blink=	LcdBuffer.Status.DisplayFlags.CursorBlink;
					HD44780_Register.DisplayControl.Cursor=	LcdBuffer.Status.DisplayFlags.CursorOn;		
					HD44780_Register.DisplayControl.Display=LcdBuffer.Status.DisplayFlags.DisplayOn;					
					writeInstructionRegister(HD44780_Register);
				}	// Display Flag set, send DisplayControl CMD
				LcdBuffer.Status.DisplayFlags.DisplayFlag=false;

				ProcessState.LcdControlState=HD44780_do_update_Contrast;
			}	// no bus operation pending
			break;
		case HD44780_do_update_Contrast:				// update display control register
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("lcd::HD44780_do_update_Contrast"));
				if (LcdBuffer.Status.DisplayFlags.ContrastFlag)
				{	// Display Flag set, send DisplayControl CMD
					lcdIO::SetContrast(LcdBuffer.Status.Contrast);
				}	// Display Flag set, send DisplayControl CMD
				LcdBuffer.Status.DisplayFlags.ContrastFlag=false;
				ProcessState.LcdControlState=HD44780_do_set_Cursor_Pos;
			}	// no bus operation pending
			break;
		case HD44780_do_set_Cursor_Pos:				// (restore) cursor Pos
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("HD44780_do_set_Cursor_Pos"));
				if (LcdBuffer.Status.DisplayFlags.PositionFlag)
				{	// Cursor Flag set, set Cursor position
					HD44780_Register.Data=getInstructionRegisterFlag(lcd::HD44780_SetAddressDDRAM);
					HD44780_Register.SetAddressDDRAM.ADD=
						getDDRAM_Pos(getBufferPos(LcdBuffer.Status.CursorX,LcdBuffer.Status.CursorY));
					writeInstructionRegister(HD44780_Register);
				}	// Cursor Flag set, set Cursor position
				LcdBuffer.Status.DisplayFlags.PositionFlag=false;
				ProcessState.LcdControlState=HD44780_do_ClearDisplay;
			}	// no bus operation pending
			break;
		case HD44780_do_ClearDisplay:					// send ClearDisplay CMD
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("lcd::HD44780_do_ClearDisplay"));
				if (LcdBuffer.Status.DisplayFlags.DisplayClear)
				{	// DisplayClear flag set , send CMD	
					HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_ClearDisplay);
					writeInstructionRegister(HD44780_Register);
					LcdBuffer.Status.DisplayFlags.DisplayClear=false;
					ProcessState.LcdControlState=HD44780_do_startWait;
				}	// Cursor Flag set, set Cursor position
				else
				{	// otherwise check Home CMD
					ProcessState.LcdControlState=HD44780_do_Home;
				}	// otherwise check Home CMD
			}	// no bus operation pending
			break;
		case HD44780_do_Home:							// Send Home CMD
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("lcd::HD44780_do_Home"));
				if (LcdBuffer.Status.DisplayFlags.Home)
				{	// DisplayClear flag set , send CMD	
					HD44780_Register.Data=getInstructionRegisterFlag(lcdIO::HD44780_ReturnHome);
					writeInstructionRegister(HD44780_Register);
					LcdBuffer.Status.DisplayFlags.Home=false;
					ProcessState.LcdControlState=HD44780_do_startWait;
				}	// Cursor Flag set, set Cursor position
				else
				{	// otherwise check Home CMD
					ProcessState.LcdControlState=HD44780_do_end;
				}	// otherwise check Home CMD
			}	// no bus operation pending
			break;
		case HD44780_do_startWait:						// update display control register
			if (!isBusy())
			{	// no bus operation pending
				PRINTLN(F("lcd::HD44780_do_startWait"));
				startWait(5);
				ProcessState.LcdControlState=HD44780_do_end;
			}	// no bus operation pending
			break;
		case HD44780_do_end: 							// placeholder
			if (!ProcessState.waitState.bWaiting)
			{	// not Waiting
				PRINTLN(F("lcd::HD44780_do_end"));
				ProcessState.LcdControlState=HD44780_do_check_DDRAM_Buffer;
			}	// not Waiting
			break;
		default:
			break;
	}	// switch (ProcessState.LcdControlState)
}	// process

void lcd::putChar(const uint8_t x, const uint8_t y, const char c)
{	// putchar (char)
	putBuffer(LcdBuffer.DDRAM,LcdBuffer.DDRAM_Flags,c,getBufferPos(x,y));
}	// putchar (char)

void lcd::putChar(const uint8_t x, const uint8_t y, const char * c)
{	// putChar (char*)
	uint8_t pos=getBufferPos(x,y);
	while(*c)
	{
		putBuffer(LcdBuffer.DDRAM,LcdBuffer.DDRAM_Flags,*c,pos);
		pos=nextBufferPos(pos);
		c++;
	}
}	// putChar (char*)

void lcd::putChar(const uint8_t x, const uint8_t y, const __FlashStringHelper * c)
{	// putChar (FlashStringHelper)
	putChar_P(x,y,(const char*)c);
}	// putChar (FlashStringHelper)

void lcd::putChar_P(const uint8_t x, const uint8_t y, const char * c)
{	// putChar_P
	uint8_t pos=getBufferPos(x,y);
	while(pgm_read_byte(c))
	{
		putBuffer(LcdBuffer.DDRAM,LcdBuffer.DDRAM_Flags,pgm_read_byte(c),pos);
		pos=nextBufferPos(pos);
		c++;
	}
}	// putChar_P

void lcd::putSigned(uint8_t x, uint8_t y, uint8_t size, int16_t val)
{	// putSigned
	bool bSign=val<0;
	if (bSign) val=-val;
	do
	{
		size--;
		putChar(x+size,y,'0'+(val % 10));
		val/=10;
	}
	while ((size>1) && (val));
	
	
	if (bSign)
	{
		size--;
		putChar(x+size,y,'-');
	}
	
	while (size)
	{
		size--;
		putChar(x+size,y,' ');
	}
}	// putSigned

void lcd::putUnsigned(uint8_t x, uint8_t y, uint8_t size, uint16_t val)
{	// putUnsigned
	do
	{
		size--;
		putChar(x+size,y,'0'+(val % 10));
		val/=10;
	}
	while ((size) && (val));
	
	while (size)
	{
		size--;
		putChar(x+size,y,' ');
	}
}	// putUnsigned

void lcd::putFixed(uint8_t x, uint8_t y, uint8_t size, uint8_t decimals, uint8_t base, uint16_t val)
{		// putFixed
	
	while (base > decimals)
	{
		val/=10;
		base--;
	}
	
	
	if (decimals)
	{	// print fraction part
		while (decimals > base)
		{
			decimals--;
			size--;
			putChar(x+size,y,'0');
		}

		do
		{
			decimals--;
			size--;
			putChar(x+size,y,'0'+(val % 10));
			val/=10;
		}
		while (decimals);

		size--;
		putChar(x+size,y,'.');
	}	// print fraction part
	
	do
	{
		size--;
		putChar(x+size,y,'0'+(val % 10));
		val/=10;
	}
	while ( (size) && (val));
	
		
	while (size)
	{
		size--;
		putChar(x+size,y,' ');
	}
}	// putFixed

void lcd::putFixed(uint8_t x, uint8_t y, uint8_t size, uint8_t decimals, uint8_t base, int16_t val)
{		// putFixed
	bool bSign=val<0;
	if (bSign) val=-val;
	
	while (base > decimals)
	{
		val/=10;
		base--;
	}
	
	if (decimals)
	{	// print fraction part
		while (decimals > base)
		{
			decimals--;
			size--;
			putChar(x+size,y,'0');
		}

		do
		{
			decimals--;
			size--;
			putChar(x+size,y,'0'+(val % 10));
			val/=10;
		}
		while (decimals);

		size--;
		putChar(x+size,y,'.');
	}	// print fraction part
	
	do
	{
		size--;
		putChar(x+size,y,'0'+(val % 10));
		val/=10;
	}
	while ( ( ((size) && (!bSign)) || ((size>1) && bSign) ) && (val));
	
	if (bSign)
	{
		size--;
		putChar(x+size,y,'-');
	}
	
	while (size)
	{
		size--;
		putChar(x+size,y,' ');
	}
}	// putFixed

void lcd::putHex(uint8_t x, uint8_t y, uint8_t size, uint16_t val)
{	// putHex
	while (size)
	{
		size--;
		if ((val & 0x0F) < 10)
			putChar(x+size,y,'0'+(val & 0x0F));
		else
			putChar(x+size,y,'A'-10+(val & 0x0F));
		val>>=4;
	}
}	// putHex

void lcd::DefineChar(uint8_t char_pos, const uint8_t * char_data)
{	// DefineChar(char *)
	char_pos &= HD44780_CHAR_SIZE-1;
	char_pos <<=HD44780_CHAR_SIZE_BITS;
	for (uint8_t n=0; n<HD44780_CHAR_SIZE;n++)
	{
		putBuffer(	LcdBuffer.CGRAM,
					LcdBuffer.CGRAM_Flags,
					char_data[n],
					char_pos+n);
	}
}	// DefineChar(char *)

void lcd::DefineChar(uint8_t char_pos, const __FlashStringHelper * char_data)
{	// DefineChar(FlashStringHelper*)
	char_pos &= HD44780_CHAR_SIZE-1;
	char_pos <<=HD44780_CHAR_SIZE_BITS;
	for (uint8_t n=0; n<HD44780_CHAR_SIZE;n++)
	{
		putBuffer(	LcdBuffer.CGRAM,
					LcdBuffer.CGRAM_Flags,
					pgm_read_byte((&((const uint8_t*)char_data)[n])),
					char_pos+n);
	}
}	// DefineChar(FlashStringHelper*)

void lcd::DefineChar_P(uint8_t char_pos, const uint8_t * char_data)
{	// DefineChar_P
	char_pos &= HD44780_CHAR_SIZE-1;
	char_pos <<=HD44780_CHAR_SIZE_BITS;
	for (uint8_t n=0; n<HD44780_CHAR_SIZE;n++)
	{
		putBuffer(	LcdBuffer.CGRAM,
					LcdBuffer.CGRAM_Flags,
					pgm_read_byte(char_data),
					char_pos+n);
		char_data++;
	}
}	// DefineChar_P


void lcd::Home()
{	// Cursor Home = Set Cursor to DDRAM 0,0
	CursorPos(0,0);
	LcdBuffer.Status.DisplayFlags.Home=true;
}	// Cursor Home = Set Cursor to DDRAM 0,0	


void lcd::Clear()
{	// Clear Clear Buffer and perform Clear CMD
	for (uint8_t n=0;n<HD44780_DDRAM_SIZE;n++)
	{
		LcdBuffer.DDRAM[n]=0x20;
		clearFlag(LcdBuffer.DDRAM_Flags,n);	
	}
	CursorPos(0,0);
	LcdBuffer.Status.DisplayFlags.DisplayClear=true;
}	// Clear

void lcd::CursorPos(uint8_t x, uint8_t y)
{	// CursorPos
	if (x>=ProcessState.LineSize) 
	   x=ProcessState.LineSize-1;
	if (y>=ProcessState.Rows) 
	   y=ProcessState.Rows-1;
	LcdBuffer.Status.DisplayFlags.PositionFlag|=
		(LcdBuffer.Status.CursorX!=x) ||
		(LcdBuffer.Status.CursorY!=y);
	LcdBuffer.Status.CursorX=x;
	LcdBuffer.Status.CursorY=y;
}	// CursorPos

void lcd::CursorOn(bool bOn)
{	// CursorOn
	LcdBuffer.Status.DisplayFlags.DisplayFlag|=
		LcdBuffer.Status.DisplayFlags.CursorOn!=bOn;
	LcdBuffer.Status.DisplayFlags.CursorOn=bOn;
}	// CursorOn

void lcd::BacklightOn(bool bOn)
{	// BacklightOn
	LcdBuffer.Status.DisplayFlags.DisplayFlag|=
		LcdBuffer.Status.DisplayFlags.BacklightOn!=bOn;
	LcdBuffer.Status.DisplayFlags.BacklightOn=bOn;
	if (LcdBuffer.Status.DisplayFlags.DisplayFlag)
		lcdIO::SetBrightness((bOn)?(100):(0));
}	// BacklightOn

void lcd::CursorBlink(bool bBlink)
{
	LcdBuffer.Status.DisplayFlags.DisplayFlag|=
		LcdBuffer.Status.DisplayFlags.CursorBlink!=bBlink;
	LcdBuffer.Status.DisplayFlags.CursorBlink=bBlink;
}

void lcd::DisplayOn(bool bOn)
{	// DisplayOn
	LcdBuffer.Status.DisplayFlags.DisplayFlag|=
		LcdBuffer.Status.DisplayFlags.DisplayOn!=bOn;
	LcdBuffer.Status.DisplayFlags.DisplayOn=bOn;
	
}	// DisplayOn

void lcd::SetContrast(uint8_t Contrast)
{	// SetContrast
	LcdBuffer.Status.DisplayFlags.ContrastFlag|=
		LcdBuffer.Status.Contrast!=Contrast;
	LcdBuffer.Status.Contrast=Contrast;
}	// SetContrast

void lcd::SetBrightness(uint8_t Brightness)
{	// SetBrightness
	BacklightOn(Brightness!=0);
	lcdIO::SetBrightness(Brightness);
}	// SetBrightness

uint8_t lcd::GetBrightness(void)
{	// SetBrightness
	return lcdIO::GetBrightness();
}	// SetBright

void lcd::ProgressBarInit(	uint8_t x, uint8_t y, 	// Position of progress bar
						uint8_t length,			// length of progressbar (1-20) (0=disable)
						uint8_t progress)		// initial state of Progress Bar	
{	// ProgressBarInit
	if (length!=0)
	{	// Init Progress Bar
		if (x>=ProcessState.Columns) x=ProcessState.Columns-1;
		if (y>=ProcessState.Rows) y=ProcessState.Rows-1;
		ProgressBar.x=x;
		ProgressBar.y=y;
		if (length+x>ProcessState.Columns) length=ProcessState.Columns-x;
		ProgressBar.length=length;
		ProgressBar.progress=0;
		DefineChar_P(0,BarIcons[4]);	// all dots set
		ProgressSet(progress);
	}	// Init Progress Bar
	else
	{	// Disable Progress bar
		ProgressSet(0);
		ProgressBar.length=0;
	}	// Disable Progress bar	
}	// ProgressBarInit

void lcd::ProgressSet(uint8_t progress)				// draw new Bar, 0-100
{
	uint8_t nCnt;
	uint16_t Dots;
	if (progress>100) progress=100;
	Dots=ProgressBar.length*5;
	Dots*=progress;
	Dots/=100;
	progress=(uint8_t)Dots;
	for (nCnt=0;nCnt<progress/5;nCnt++)
		putChar(ProgressBar.x+nCnt,ProgressBar.y,'\x00');
	if (progress%5)
	{
		if ((ProgressBar.progress%5)!=(progress%5))
			DefineChar_P(1, BarIcons[progress%5-1]);
		putChar(ProgressBar.x+progress/5,ProgressBar.y,'\x01');
		nCnt++;
	}
	for (;nCnt<ProgressBar.length;nCnt++)
		putChar(ProgressBar.x+nCnt,ProgressBar.y,'\x20');
	ProgressBar.progress=progress;
}

/********************************************************/
/*		private functions								*/
/********************************************************/
	// now it gets a bit complicated:
	// on 2 Line displays the second line starts at pos 0x40
	// on 4 line displays the second line starts at pos 0x40
	// on 4 line displays the third lins starts at pos 0x14
	// on 4 line displays the fourth line starts at pos 0x54
	// so the order in DDRAM is 0-2-1-3
	// Data in internal buffer is oriented in the same way
	// without the gap between 0x28 and 0x40
uint8_t lcd::getBufferPos(uint8_t x, uint8_t y)
{	// getBufferPos
	y=( (y & 0x01) << 1)  | ( (y & 0x02) >> 1);
	if (x>=ProcessState.Columns) x=ProcessState.Columns-1;
	if (y>=ProcessState.Rows) y=ProcessState.Rows-1;
	return y*ProcessState.LineSize+x; 
}	// getBufferPos

uint8_t	lcd::nextBufferPos(uint8_t BufferPos)
{	// nextBufferPos
	PRINT(F("lcd::nextBufferPos: "));
	PRINT(BufferPos);
	BufferPos=(BufferPos+1)%HD44780_DDRAM_SIZE;
	if ((BufferPos%ProcessState.LineSize)==0)
	{	// switch lines 
		uint8_t row=BufferPos/ProcessState.LineSize;
		row=( (row & 0x02) >> 1) | 
			( ( ( (row & 0x02) >> 1) ^ (row & 0x01) ) << 1 );
		if (row>=ProcessState.Rows) row=ProcessState.Rows-1;
		BufferPos%=ProcessState.LineSize;
		BufferPos+=row*ProcessState.LineSize;
	}	// switch lines 
	PRINT(F(" -> +1 -> "));
	PRINTLN(BufferPos);
	return BufferPos;
}	// nextBufferPos

uint8_t	lcd::getColumn(uint8_t BufferPos)
{	// getColumn
	return BufferPos%ProcessState.LineSize;
}	// getColumn

uint8_t	lcd::getRow(uint8_t BufferPos)
{	// getRow
	uint8_t row=BufferPos/ProcessState.LineSize;
	row=( (row & 0x01) << 1)  | ( (row & 0x02) >> 1);
	if (row>=ProcessState.Rows) row=ProcessState.Rows-1;
	return row;
}	// getRow

uint8_t	lcd::getDDRAM_Pos(uint8_t BufferPos)
{	// getDDRAM_Pos
	if (ProcessState.Rows>1)
	{	// multi Line
		// two sections 0x00-0x27 and 0x40-0x6F
		// two line display:
		// 0: 0x00-0x27 / 1: 0x40-0x67
		// four line display:
		// 0: 0x00-0x13 / 1: 0x40-0x53
		// 2: 0x14-0x27 / 3: 0x53-0x67
		// lines in LcdBufff.DDRAM are already in correct order
		// only addresoffset has to be calculated
		if (BufferPos>=HD44780_DDRAM_LINE)
			BufferPos=(BufferPos%HD44780_DDRAM_LINE) | 0x40;
	}	// multi Line
	/*
	else
	{	// single line
		// continuous address space 0x00 - 0x4F
		// nothing to do
	}	// single line
	*/
	return BufferPos;
}	// getDDRAM_Pos

void lcd::startWait(uint8_t ms)
{	// startWait
	ProcessState.waitState.ms_Delay=ms;
	ProcessState.waitState.ms_Start=millis();
	ProcessState.waitState.bWaiting=true;
}	// startWait


void lcd::putBuffer(uint8_t *pBuffer, uint8_t *pFlags, uint8_t Data, uint8_t Pos)
{	// putBuffer
	if (pBuffer[Pos]!=Data)
	{	// new data, save and set Flag
		pBuffer[Pos]=Data;
		setFlag(pFlags,Pos);
	}	// new data, save and set Flag
}	// putBuffer

bool lcd::getFlag(uint8_t * pFlags, uint8_t Pos)
{	// getFlag
	uint8_t BytePos=Pos>>3;
	uint8_t BitMask=1<<(Pos & 0x07);
	return 	pFlags[BytePos] & BitMask;
}	// getFlag

void lcd::setFlag(uint8_t * pFlags, uint8_t Pos)
{	// setFlag
	uint8_t BytePos=Pos>>3;
	uint8_t BitMask=1<<(Pos & 0x07);
	pFlags[BytePos]|=BitMask;
}	// setFlag

void lcd::clearFlag(uint8_t * pFlags, uint8_t Pos)
{	// clearFlag
	uint8_t BytePos=Pos>>3;
	uint8_t BitMask=1<<(Pos & 0x07);
	pFlags[BytePos]&=~BitMask;
}	// clearFlag


#undef PRITNLN
#undef PRITN
#undef PRITNLN_HEX
#undef PRITN_HEX
#undef WRITE
