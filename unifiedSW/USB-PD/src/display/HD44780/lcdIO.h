#ifndef __lcdIO__
#define __lcdIO__

#include <stdint.h>
#include <stdbool.h>
#include "../../LED/cat4004.h"

class lcdIO:cat4004
{
	public:
		typedef enum:uint8_t { 	HD44780_ClearDisplay,
							HD44780_ReturnHome,
							HD44780_EntryModeSet,
							HD44780_DisplayControl,
							HD44780_DisplayCursorShift,
							HD44780_FunctionSet,
							HD44780_SetAddressCGRAM,
							HD44780_SetAddressDDRAM } HD44780_Instruction_et;
							
		/* 
		PCF8574 Pin definition:
			RS	(0)
			RW	(1)
			E	(2)
			BL	(3)
			D4	(4)
			D5	(5)
			D6	(6)
			D7	(7)
		*/
		typedef struct PCF8574_Pin_s { 	uint8_t	rs:1;		// Register Select: 0 = Instruction Register / 1 = RAM
										uint8_t	rw:1;		// Read /Write:	1 = Read / 0 = Write
										uint8_t e:1;		// Enable: 0 = Bus Idles / 1 = Bus active
										uint8_t bl:1;		// Backlight: 0 = Off / 1 = On
										uint8_t	data:4;		// Data: D7-D4
										} PCF8574_Pin_st;
		typedef union PCF8574_Data_u {	uint8_t 		Data;
										PCF8574_Pin_st	Pin;
										} PCF8574_Data_ut;	
		
		typedef struct HD44780_Instruction_ClearDisplay_s	// Clear Display 0000 0001b
									{ 	uint8_t Flag:1;	
										uint8_t	zeros:7;
										} HD44780_Instruction_ClearDisplay_st;
		typedef struct HD44780_Instruction_ReturnHome_s		// Cursor Home 0000 001xb
									{ 	uint8_t dontCare:1;	
										uint8_t Flag:1;
										uint8_t	zeros:6;
										} HD44780_Instruction_ReturnHome_st;
		typedef struct HD44780_Instruction_EntryModeSet_s	// EntryMode set 000 01ISb (performed on read / write)
									{ 	uint8_t Shift:1;	// 1= Shift display, 0=Cursor only
										uint8_t Increment:1;// 1= increment / 0= decrement
										uint8_t Flag:1;
										uint8_t	zeros:5;
										} HD44780_Instruction_EntryModeSet_st;
		typedef struct HD44780_Instruction_DisplayControl_s	// Dswitches on / off
									{ 	uint8_t Blink:1;	// 1= blinking cursor 0=underline
										uint8_t Cursor:1;	// 1= cursor on
										uint8_t Display:1;	// 1= display on
										uint8_t Flag:1;
										uint8_t	zeros:4;
										} HD44780_Instruction_DisplayControl_st;
		typedef struct HD44780_Instruction_DisplayCursorShift_s	// Dswitches on / off
									{ 	uint8_t dontCare:2;	
										uint8_t Left:1;		// 1= blinking cursor 0=underline
										uint8_t Shift:1;	// 1= left shift move / 0= right shift move
										uint8_t Flag:1;     // 1= shift display / 0= move cursor
										uint8_t	zeros:3;
										} HD44780_Instruction_DisplayCursorShift_st;
		typedef struct HD44780_Instruction_FunctionSet_s		// Function select on setup
									{ 	uint8_t dontCare:2;	
										uint8_t Font:1;			// 1= 5x10 / 0= 5x8
										uint8_t Lines:1;	    // 0= 1 Line 1= 2/4 Lines
										uint8_t DataLength:1;   // 1= 8 Bit / 0= 4 Bit
										uint8_t Flag:1;
										uint8_t	zeros:2;
										} HD44780_Instruction_FunctionSet_st;
		typedef struct HD44780_Instruction_SetAddressCGRAM_s	// Set CGRAM Address
									{ 	uint8_t ACG:6;	
										uint8_t Flag:1;
										uint8_t	zeros:1;
										} HD44780_Instruction_SetAddressCGRAM_st;
		typedef struct HD44780_Instruction_SetAddressDDRAM_s	// Set DDRAM Address
									{ 	uint8_t ADD:7;	
										uint8_t Flag:1;
										} HD44780_Instruction_SetAddressDDRAM_st;
		typedef struct HD44780_BusyFlag_s	// Busys Flag / Address Counter
									{ 	uint8_t AddressCounter:7;	// Address Counter
										uint8_t Busy:1;				// 1= Busy, 0=Ready
										} HD44780_BusyFlag_st;
							
		typedef	struct	Nibble_s	{	uint8_t	low:4;
										uint8_t high:4;
										} Nibble_st;
		
		typedef union 	HD44780_Register_u
									{	uint8_t		Data;
										Nibble_st	Nibble;
										HD44780_Instruction_ClearDisplay_st			ClearDisplay;
										HD44780_Instruction_ReturnHome_st			ReturnHome;
										HD44780_Instruction_EntryModeSet_st			EntryModeSet;
										HD44780_Instruction_DisplayControl_st		DisplayControl;
										HD44780_Instruction_DisplayCursorShift_st	DisplayCursorShift;
										HD44780_Instruction_FunctionSet_st			FunctionSet;
										HD44780_Instruction_SetAddressCGRAM_st		SetAddressCGRAM;
										HD44780_Instruction_SetAddressDDRAM_st		SetAddressDDRAM;
									} HD44780_Rgister_ut;

		lcdIO(uint8_t i2c_address, uint8_t inverterPin, uint8_t brightnessPin, unsigned char Index);				
												// define I2c address of PCF8574 0x20-0x27 / 0x38-0x3F (A version)
												// i2c_address=0xFF: Autotdetect 
												// i2c_address=0x00...0x07: autodetect Base address PCF8574 (A) 0x20/0x38
												// i2c_address=0x20...0x27,0x38...0x3F no Autodetect, just test
												// Autodetect will start with init command, Wire is not available a construction
												// inverterPin: Arduino pin wchi Voltage Inverter is connected to (Contrast Voltage)
												// 0xFF if unused
												// brightnessPin: Arduino pin which is connected to LED Driver Chip
												// index of CAT4004, either ' ' or 'A'
		uint8_t init(bool BusModeNibble=false);	// init: BusModeNibble=false, 
												// enable Byte-Transfer (in 4bit Mode) BusModeNibble=true, 
												// return i2c_address or 0 if no PF8574(A) is found
												
		void process(void);						// process actions (sequence of miltiple transfers)
		bool isBusy(void);						// read / write operation in progress
		bool is_i2c_set(void);				// Check if first init call was succesful
		uint8_t	getInstructionRegisterFlag(HD44780_Instruction_et Instruction);
		void writeInstructionRegister(HD44780_Rgister_ut InstructionRegister);
		void readInstructionRegister(HD44780_Rgister_ut * pInstructionRegister);
		void writeRAM(uint8_t Data);
		void readRAM(uint8_t * pData);
		void BacklightOn(bool bOn);
		void SetContrast(uint8_t Contrast);
		void SetBrightness(uint8_t Brightness);
		uint8_t GetBrightness(void);
		
	private:
		
		typedef enum:uint8_t {	lcdIO_idle,				// lcdIO process states for 4bit i2c access
								lcdIO_write_hi,			// write high nibble to bus
								lcdIO_write_lo,			// write low nibble to bus
								lcdIO_read_setup_enable,// prepare read and enable bus
								lcdIO_read_hi,			// read hi nibble
								lcdIO_read_next,		// low pulse on enable line
								lcdIO_read_lo,			// read low nibble
								lcdIO_read_disable 		// disable bus
								} lcdIO_state_et;
		
		typedef struct lcdIO_state_s {	lcdIO_state_et	state;
										uint8_t *		pRegisterData;
										uint8_t			RegisterData;
										uint8_t			i2c_address;
										uint8_t			inverterPin;
										uint8_t			brightnessPin;
										PCF8574_Data_ut	PinData;
										uint8_t			BacklightOn:1;
										uint8_t			NibbleMode:1;
										uint8_t			i2c_set:1;
										uint8_t			Memory:1;		// RS, false: Instruction=register, true=DDRAM/CGRAM
										} lcdIO_state_st;
		
		lcdIO_state_st	processing_state;
		
		bool i2cProbe(uint8_t i2c_address);
		
		// bRS: false = Instruction, true=Memory
		// bRW: false = read, true=write
		// bEnable: fale =inactive 1, 1=active
		// Data: uses upper 4 bit for Data Transfer
		void setPinDataPCF8574(	uint8_t Data, 
								bool bRS, 
								bool bRW, 
								bool bEnable);		
		void writeNibble(void);
		void readEnable(void);
		void readNext(void);
		void readDisable(void);
		void readNibble(void);
};
#endif // __lcdIO__
