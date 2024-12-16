#ifndef __lcd__
#define __lcd__

#include <stdint.h>
#include <stdbool.h>
#include "lcdIO.h"


class lcd:lcdIO
{
	public:
	lcd(uint8_t adresse=0xff, uint8_t inverterPin=0xff, uint8_t brightnessPin=0xff, unsigned char Index=' ');
	void init(uint8_t cols, uint8_t rows);	
	void process(void);
	bool isRunning(void);
	
	void putChar(const uint8_t x, const uint8_t y, const char c); 
	void putChar(const uint8_t x, const uint8_t y, const char * c); 
	void putChar(const uint8_t x, const uint8_t y, const __FlashStringHelper * c);
	void putChar_P(const uint8_t x, const uint8_t y, const char * c);

	void putSigned(uint8_t x, uint8_t y, uint8_t size, int16_t val);
	void putUnsigned(uint8_t x, uint8_t y, uint8_t size, uint16_t val);
	void putFixed(uint8_t x, uint8_t y, uint8_t size, uint8_t decimals, uint8_t base, uint16_t val);
	void putFixed(uint8_t x, uint8_t y, uint8_t size, uint8_t decimals, uint8_t base, int16_t val);
	void putHex(uint8_t x, uint8_t y, uint8_t size, uint16_t val);

	void DefineChar(uint8_t char_pos, const uint8_t * char_data);
	void DefineChar(uint8_t char_pos, const __FlashStringHelper * char_data);
	void DefineChar_P(uint8_t char_pos, const uint8_t * char_data);
	
	void Home();
	void Clear();
	void CursorPos(uint8_t x, uint8_t y);
	void CursorOn(bool bOn);
	void BacklightOn(bool bOn);
	void CursorBlink(bool bBlink);
	void DisplayOn(bool bOn);
	void SetContrast(uint8_t Contrast);
	void SetBrightness(uint8_t Brightness);
	uint8_t GetBrightness(void);

	void ProgressBarInit(	uint8_t x, uint8_t y, 	// Position of progress bar
							uint8_t length,			// length of progressbar (1-20)
							uint8_t progress);		// initial state of Progress Bar
	void ProgressSet(uint8_t progress);				// draw new Bar, 0-100
	
	private:

	
		static const uint8_t HD44780_DDRAM_SIZE=80;	// Size of HD44780 DisplayDataRAM
		static const uint8_t HD44780_DDRAM_LINE=40;	// Size of HD44780 DisplayDataRAM
		static const uint8_t HD44780_CGRAM_SIZE=64;	// Size of HD44780 CharacterGeneratorRAM
		static const uint8_t HD44780_CHAR_SIZE_BITS=3;						// Address bits needed fpr 1 user defined char
		static const uint8_t HD44780_CHAR_SIZE=1<<HD44780_CHAR_SIZE_BITS; 	// number of bytes per user char

		static const uint8_t BARICONS=5;			// Size of HD44780 DisplayDataRAM

		static const PROGMEM uint8_t BarIcons[][HD44780_CHAR_SIZE];

		typedef enum:uint8_t { 	HD44780_init_start,						// Start 40ms Timer on power Up
								HD44780_init_FunctionSet8bit_start,		// first (upper) 4bit Function set (0x30)
								HD44780_init_FunctionSet8bit_wait,		// initiate 4,1 ms delay (5-6 ms)
								HD44780_init_FunctionSet4bit,			// full Function set (0010 l0xx) 1 Line l=0, 2/4 Lines l=1
								HD44780_init_DisplayOff,				// turn off Display
								HD44780_init_DisplayClear,				// Clear display buffer 
								HD44780_init_DisplayClear_wait,			// wait for execution 2-3ms
								HD44780_init_EntryModeSet,				// Inkrent bei Puffer Schreiben
								HD44780_init_End,						// placeholder
								HD44780_do_check_DDRAM_Buffer,			// check differences between buffers
								HD44780_do_set_DDRAM_Pointer,			// on difference set corresponding buffer position
								HD44780_do_write_DDRAM_Data,			// and transfer data
								HD44780_do_check_CGRAM_Buffer,			// check differences between buffers
								HD44780_do_set_CGRAM_Pointer,			// on difference set RAM position
								HD44780_do_write_CGRAM_Data,			// transfer 8 bytes
								HD44780_do_update_DisplayControl,		// update display control register
								HD44780_do_update_Contrast,				// update display control register
								HD44780_do_set_Cursor_Pos,				// (restore) cursor Pos
								HD44780_do_ClearDisplay,				// send ClearDisplay CMD
								HD44780_do_Home,						// Send Home CMD
								HD44780_do_startWait,					// update display control register
								HD44780_do_end 							// placeholder
								} lcd_control_state_et;
								

		typedef struct DisplayFlags_s {	uint8_t	CursorOn	:1;	// Cursor State
										uint8_t	CursorBlink	:1;	// Cursor Mode
										uint8_t DisplayOn	:1;	// Display Mode
										uint8_t	BacklightOn	:1;	// Backlight State
										uint8_t DisplayFlag :1; // one of above modified
										uint8_t	PositionFlag:1;	// cursor position modified
										uint8_t	ContrastFlag:1;	// contrast modified
										uint8_t	DisplayClear:1;	// Display Clear requested
										uint8_t	Home		:1;	// Return Home requested
										uint8_t AddressSet	:1;	// Setting of RAM address required
										} DisplayFlags_st;
		
		typedef struct LcsStatus_s { 	uint8_t 		CursorX;
										uint8_t 		CursorY;
										uint8_t 		Contrast;
										uint8_t			BufferPos;
										DisplayFlags_st DisplayFlags;
										} LcdStatus_st;

		typedef struct LCdBuffer_s	{	LcdStatus_st	Status;
										uint8_t			DDRAM[HD44780_DDRAM_SIZE];
										uint8_t			DDRAM_Flags[HD44780_DDRAM_SIZE>>3];
										uint8_t			CGRAM[HD44780_CGRAM_SIZE];
										uint8_t			CGRAM_Flags[HD44780_CGRAM_SIZE>>3];
										} LcdBuffer_st;

		typedef struct waitState_s { 	uint16_t	ms_Start;	// ms on call of start wait
										bool		bWaiting;	// running flag
										uint8_t		ms_Delay;	// ms to wait
										}	waitState_st;

		typedef struct processState_s {	lcd_control_state_et	LcdControlState; 	// state variable
										uint8_t					Columns;			// Display dimiensions
										uint8_t					Rows;				// Display dimiensions
										uint8_t					LineSize;			// Buffersize per Line
										uint8_t					DDRAM_Pos;			// current processing position in DDRAM buffer
										uint8_t					CGRAM_Pos;			// current processing position in CGRAM buffer
										uint8_t					InitCnt;			// used for Bus Initialiszation
										waitState_st			waitState;
										} 	processState_st; 

		typedef struct progress_s	{	uint8_t x;			// start Column of Progress Bar
										uint8_t y;			// row of Progress Bar
										uint8_t length;		// length of Progres Bar
										uint8_t progress;	// current state of Progress Bar
									}	progress_st;

										
		LcdBuffer_st			LcdBuffer;
		processState_st			ProcessState;
		lcd::HD44780_Rgister_ut HD44780_Register;
		progress_st				ProgressBar;
		
		uint8_t temp;
		
		void startWait(uint8_t ms);
		bool isWaiting(void);

		uint8_t getBufferPos(uint8_t x, uint8_t y);
		uint8_t	nextBufferPos(uint8_t BufferPos);
		uint8_t	getColumn(uint8_t BufferPos);
		uint8_t	getRow(uint8_t BufferPos);
		uint8_t	getDDRAM_Pos(uint8_t BufferPos);
		
		void putBuffer(uint8_t *pBuffer, uint8_t *pFlags, uint8_t Data, uint8_t Pos);
		bool getFlag(uint8_t * pFlags, uint8_t Pos);
		void setFlag(uint8_t * pFlags, uint8_t Pos);
		void clearFlag(uint8_t * pFlags, uint8_t Pos);
};
#endif // __lcd__
