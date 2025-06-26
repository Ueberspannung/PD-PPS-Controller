/* *********************************************************************
 * VT100 Host class
 */

#ifndef __VT100_H__
#define __VT100_H__

#include <stdint.h>
#include <Arduino.h>
#include "../tool/SerialClass.h"

class VT100_c:public SerialClass
{
	public:

		typedef enum:uint8_t {	offline,serial,terminal} terminal_mode_et; 
		typedef enum:uint8_t {	noOption	= 0x00, 
								stp			= 0x01,
								avo			= 0x02,
								stp_avo		= 0x03,
								gpo			= 0x04,
								stp_gpo		= 0x05,
								avo_gpo		= 0x06,
								stp_avo_gpo	= 0x07,
								noTerminal	= 0xFF } terminal_options_et;
		typedef enum:uint8_t {	reset		= 0x01,
								bright		= 0x02,
								dim			= 0x04,
								underscore	= 0x08,
								blink		= 0x10,
								reverse		= 0x20,
								hidden		= 0x40 } attributes_et;
		typedef enum:uint8_t {	black		= 0x00,		
		                        red			= 0x01,
		                        green		= 0x02,
		                        yellow		= 0x03,
		                        blue		= 0x04,
		                        magenta		= 0x05,
		                        cyan		= 0x06,
		                        white		= 0x07 } colour_et;
		typedef enum:uint8_t {	normal, 
								double_width,
								double_hight_upper_line,
								double_hight_lower_line } charsize_et;

		/*
		 * subset of control keys limited to 32 (0-31) as they are mapped
		 * to ASCII control characters 0-31
		 */
		typedef enum:uint8_t {	EDIT_CRSR_UP	,	//= 0,		
								EDIT_CRSR_DOWN	,	//= 1,
								EDIT_CRSR_LEFT	,	//= 2,
								EDIT_CRSR_RIGHT	,	//= 3,
								EDIT_CRSR_HOME	,	//= 4,
								EDIT_CRSR_END	,	//= 5,
								EDIT_CRSR_SOL	,	//= 6,
								EDIT_CRSR_EOL	,	//= 7,
								EDIT_BS			,	//= 8,
								EDIT_DEL		,	//= 9,
								EDIT_INS		,	//=10,
								EDIT_PREVIOUS	,	//=11,
								EDIT_NEXT		,	//=12,
								EDIT_PGUP		,	//=13,
								EDIT_PGDN		,	//=14,
								EDIT_ENTER		,	//=15,
								EDIT_ESC		,	//=16,
								EDIT_CANCEL		,	//=17,
								EDIT_END		,	//=18,

								EDIT_F1			,	//=19,
								EDIT_F2			,	//=20,
								EDIT_F3			,	//=21,
								EDIT_F4			,	//=22,
								EDIT_F5			,	//=23,
								EDIT_F6			,	//=24,
								EDIT_F7			,	//=25,
								EDIT_F8			,	//=26,
								EDIT_F9			,	//=27,
								EDIT_F10		,	//=28,
								EDIT_F11		,	//=29,
								EDIT_F12			//=30
								} editCtrlKey_et;

		typedef enum:uint32_t {	FLAG_CRSR_UP	= 1ul<<EDIT_CRSR_UP		,	//=1<< 0,		
								FLAG_CRSR_DOWN	= 1ul<<EDIT_CRSR_DOWN	,	//=1<< 1,
								FLAG_CRSR_LEFT	= 1ul<<EDIT_CRSR_LEFT	,	//=1<< 2,
								FLAG_CRSR_RIGHT	= 1ul<<EDIT_CRSR_RIGHT	,	//=1<< 3,
								FLAG_CRSR_HOME	= 1ul<<EDIT_CRSR_HOME	,	//=1<< 4,
								FLAG_CRSR_END	= 1ul<<EDIT_CRSR_END	,	//=1<< 5,
								FLAG_CRSR_SOL	= 1ul<<EDIT_CRSR_SOL	,	//=1<< 6,
								FLAG_CRSR_EOL	= 1ul<<EDIT_CRSR_EOL	,	//=1<< 7,
								FLAG_BS			= 1ul<<EDIT_BS			,	//=1<< 8,
								FLAG_DEL		= 1ul<<EDIT_DEL			,	//=1<< 9,
								FLAG_INS		= 1ul<<EDIT_INS			,	//=1<<10,
								FLAG_PREVIOUS	= 1ul<<EDIT_PREVIOUS	,	//=1<<11,
								FLAG_NEXT		= 1ul<<EDIT_NEXT		,	//=1<<12,
								FLAG_PGUP		= 1ul<<EDIT_PGUP		,	//=1<<13,
								FLAG_PGDN		= 1ul<<EDIT_PGDN		,	//=1<<14,
								FLAG_ENTER		= 1ul<<EDIT_ENTER		,	//=1<<15,
								FLAG_ESC		= 1ul<<EDIT_ESC			,	//=1<<16,
								FLAG_CANCEL		= 1ul<<EDIT_CANCEL		,	//=1<<17,
								FLAG_END		= 1ul<<EDIT_END			,	//=1<<18,
																			
								FLAG_F1			= 1ul<<EDIT_F1			,	//=1<<19,
								FLAG_F2			= 1ul<<EDIT_F2			,	//=1<<20,
								FLAG_F3			= 1ul<<EDIT_F3			,	//=1<<21,
								FLAG_F4			= 1ul<<EDIT_F4			,	//=1<<22,
								FLAG_F5			= 1ul<<EDIT_F5			,	//=1<<23,
								FLAG_F6			= 1ul<<EDIT_F6			,	//=1<<24,
								FLAG_F7			= 1ul<<EDIT_F7			,	//=1<<25,
								FLAG_F8			= 1ul<<EDIT_F8			,	//=1<<26,
								FLAG_F9			= 1ul<<EDIT_F9			,	//=1<<27,
								FLAG_F10		= 1ul<<EDIT_F10			,	//=1<<28,
								FLAG_F11		= 1ul<<EDIT_F11			,	//=1<<29,
								FLAG_F12		= 1ul<<EDIT_F12				//=1<<30
								} editCtrlKeyFlags_et;
		
		typedef enum:uint8_t {	up, down, left, right, 
								enter, end, cancel,
								editing, idle } editCtrl_et;

		VT100_c(HardwareSerial & port,uint16_t RxTimeout_ms=RECEIVE_TIMEOUT);
		#ifdef HAS_USB_SERIAL
		VT100_c(Serial_ & port,uint16_t RxTimeout_ms=RECEIVE_TIMEOUT);
		#endif
								

		void process(void);
	
		virtual void TerminalOpen(void) {};
		virtual void TerminalProcess(void) {};
		virtual void TerminalClose(void) {};
		
		virtual void SerialOpen(void) {};
		virtual void SerialProcess(void) {};
		virtual void SerialClose(void) {};
	
		terminal_mode_et GetTerminalMode(void);
	
		void ResetTerminal(void);							   				// Terminal Initialiserung

		void SetKeyLock(bool bLock);				 						// keylock / beep
		void SetInsert(bool bInsert);										// change insert / overwrite mode
		void SetAutoLF(bool bAutoLF);										// CR -> CR / LF
		void SetCursorModeApplication(bool bModeApplication);				// ESC-Sequenz ESC O, otherwise ESC [
		void SetEmulationVT100(bool bVT100);								// Select emulation VT100 / VT52
		void SetWidth132(bool b132);										// Select 80 / 132 column mode 
		void SetSmoothScroll(bool bSmooth);									// soft scroll ( pixel scroll / line scroll)
		void SetScreenModeReveres(bool bReverse);							// black characters on clored background
		void SetCursorRelative(bool bRelative);								// enable relative cursor position
		void SetLineWrap(bool bWrap);										// enable line wrap
		void SetAutoRepeat(bool bAutoRepeat);								// enable auto repeat on keyboard
		void SetVideoModeInterlaced(bool bInterlaced);						// ??? relict off cathode tube displays 
		void SetCursorOn(bool bOn);											// show cursor
		void EnableGraphics(bool bOn);										// enable / disable grafics char set
		void SetNumModeApplication(bool bOn);								// enbale numbe block
		void EnableG0Charset(void);											// enable G0 charset
		void EnableG1Charset(void);
								

		void ClearEOL(void);												// clear to end of line
		void ClearSOL(void);												// clear to cursor
		void ClearLine(void);												// clear whole line
		void ClearDown(void);												// clear screen below current line
		void ClearUp(void);													// clear screen above current line
		void ClearScreen(void);												// clear whole screen
		void CursorHome(void);								   				// = SetCursor(0,0)
		void SetCursor(uint8_t PosX, uint8_t PosY);							// set cursor position
		void SetScrollArea(uint8_t TopLine,uint8_t BottomLine); 								 


		void SaveRestoreCursorAttributes(bool bSave);				   		// save / restore cursor attributes
		void SetAttributes(uint8_t Attributes=(uint8_t)reset);		   		// set multiple cursor attributes
		void SetForegroundColour(colour_et Colour);							// set colors
		void SetBackgroundColour(colour_et Colour);							// set colors
		void SetColours(colour_et Foreground, colour_et Background);			// set colors
							
		void SetCharSize(charsize_et Size);									// set double wiidth / hight
		void Print(const char  * cString);						   			// print at current pos
		void Print(const __FlashStringHelper *cFlashString);
		void PrintAt(uint8_t PosX, uint8_t PosY, const char * cString);		// print at designated pos 
		void PrintAt(uint8_t PosX, uint8_t PosY, const __FlashStringHelper *cFlashString);		// print at designated pos 
				
				
		void PutChar(char c);												// print character
		void PutCharAt(uint8_t PosX, uint8_t PosY, char c);					// print char at designeted pos.
						
		bool KeyPressed(void);												// check key
		char GetKey(bool bEcho=false, bool bSkipCodes=true);				// get a key 
		char GetKeyMask(const char *cMask, 									// get a key from mask;
						bool bEcho=false, 
						uint32_t ControlMask=0);
					
										
		bool ScanKey(char * pcKey,bool bSkipCodes=true);					// Keypressed + Read
		bool ScanKeyMask(char * pcKey, 										// Keypressed + Read + KeyMask
						 const char * pcKeyMask=NULL,
						 uint32_t ControlMask=0);				
		bool ScanKeyMask(char * pcKey, 										// Keypressed + Read + KeyMask
						 const __FlashStringHelper * pcFlashKeyMask=NULL, 			
						 uint32_t ControlMask=0);				


																			// Init Edit with Control
		void EditStart( uint8_t PosX, 	uint8_t PosY,
						char *pData,	uint8_t Len,
						const char * cKeyMask=NULL,
						bool TrimLeft=false,
						bool TrimRight=false,
						bool TermNull=true,
						uint32_t ExitMask=0);

		editCtrl_et Edit(void);												// process Edit 
	
		editCtrl_et EditGetState(void) {return editData.State;}
		
	private:
	
								


		static const char ESC_RESET_MODE[] PROGMEM;				
		static const char ESC_APPLICATION_MODE[] PROGMEM;		
		static const char QUERY_DEVICE_ATTRIBUTES[] PROGMEM;		
		static const char DEVICE_ATTRIBUTES[] PROGMEM;				
		static const char RESET_TERMINAL[] PROGMEM; 				
		static const char CURSOR_UP[] PROGMEM;			   		
		static const char CURSOR_DOWN[] PROGMEM;					
		static const char CURSOR_FORWARD[] PROGMEM;				
		static const char CURSOR_BACK[] PROGMEM;					
		static const char CURSOR_HOME[] PROGMEM;					
		static const char ERASE_END_OF_LINE[] PROGMEM;			
		static const char ERASE_START_OF_LINE[] PROGMEM;			
		static const char ERASE_LINE[] PROGMEM;					
		static const char ERASE_DOWN[] PROGMEM;					
		static const char ERASE_UP[] PROGMEM;					
		static const char ERASE_SCREEN[] PROGMEM;				
		static const char QUERY_CURSOR_POS[] PROGMEM;			
		static const char SAVE_CURSOR_ATTRIBUTES[] PROGMEM;		
		static const char RESTORE_CURSOR_ATTRIBUTES[] PROGMEM;	
		static const char DOUBLE_HIGHT_UPPER_LINE[] PROGMEM;		
		static const char DOUBLE_HIGHT_LOWER_LINE[] PROGMEM;		
		static const char DOUBLE_WIDTH[] PROGMEM;				
		static const char DOUBLE_OFF[] PROGMEM;					

		static const char SW_KEY_LOCK_ON[] PROGMEM;		 			// keylock / beep
		static const char SW_KEY_LOCK_OFF[] PROGMEM;		 		// keylock / beep
		static const char SW_INSERT[] PROGMEM;						// change insert / overwrite mode
		static const char SW_OVERWRITE[] PROGMEM;					// change insert / overwrite mode
		static const char SW_AUTO_LF_ON[] PROGMEM;					// CR -> CR / LF
		static const char SW_AUTO_LF_OFF[] PROGMEM;					// CR -> CR / LF
		static const char SW_CURSOR_MODE_APPLICATION_ON[] PROGMEM;	// ESC-Sequenz ESC O, otherwise ESC [
		static const char SW_CURSOR_MODE_APPLICATION_OFF[] PROGMEM;	// ESC-Sequenz ESC O, otherwise ESC [
		static const char SW_VT100[] PROGMEM;						// Select emulation VT100 / VT52
		static const char SW_VT52[] PROGMEM;						// Select emulation VT100 / VT52
		static const char SW_COLUMN132[] PROGMEM;					// Select 80 / 132 column mode 
		static const char SW_COLUMN80[] PROGMEM;						// Select 80 / 132 column mode 
		static const char SW_SMOOTH_SCROLL_ON[] PROGMEM;			// soft scroll ( pixel scroll / line scroll)
		static const char SW_SMOOTH_SCROLL_OFF[] PROGMEM;			// soft scroll ( pixel scroll / line scroll)
		static const char SW_SCREEN_MODE_REVERSE_ON[] PROGMEM;		// black characters on clored background
		static const char SW_SCREEN_MODE_REVERSE_OFF[] PROGMEM;		// black characters on clored background
		static const char SW_CURSOR_RELATIVE[] PROGMEM;				// enable relative cursor position
		static const char SW_CURSOR_ABSOLUTE[] PROGMEM;				// enable relative cursor position
		static const char SW_LINE_WRAP_ON[] PROGMEM;				// enable line wrap
		static const char SW_LINE_WRAP_OFF[] PROGMEM;				// enable line wrap
		static const char SW_AUTO_REPEAT_ON[] PROGMEM;				// enable auto repeat on keyboard
		static const char SW_AUTO_REPEAT_OFF[] PROGMEM;				// enable auto repeat on keyboard
		static const char SW_VIDEO_INTRELACED[] PROGMEM;			// ??? relict off cathode tube displays 
		static const char SW_VIDEO_PROGRESSIVE[] PROGMEM;			// ??? relict off cathode tube displays 
		static const char SW_CURSOR_ON[] PROGMEM;					// show cursor
		static const char SW_CURSOR_OFF[] PROGMEM;					// show cursor
		static const char SW_ENABLE_GRAPHICS[] PROGMEM;				// enable / disable grafics char set
		static const char SW_DISABLE_GRAPHICS[] PROGMEM;			// enable / disable grafics char set
		static const char SW_NUM_MODE_APPLICATION_ON[] PROGMEM;		// enbale numbe block
		static const char SW_NUM_MODE_APPLICATION_OFF[] PROGMEM;	// enbale numbe block
		static const char SW_CHARACTERSET_G0[] PROGMEM;				// enable G0 charset
		static const char SW_CHARACTERSET_G1[] PROGMEM;				// enable G0 charset

		static const uint16_t 	TERMINAL_TIMEOUT	=300;	// Hyperterm has a round trip of 90!ms @1,2kBd
		static const uint16_t 	RECEIVE_TIMEOUT 	=100;	// 100ms default

		
		static const uint8_t	EDIT_LEN	   		=40;	// 40 max edit field 

		static const uint8_t 	TEMP_BUFFER_SIZE 	=16;	// needs to be 2^n

		typedef enum:uint8_t {	process_start,
								process_wait,
								process_wait_done,
								process_wait_timeout,
								process_timeout,
								process_done,
							} process_state_et;
		
		
		typedef struct editKey_s { 	char 	cData;
									uint8_t	Number;
									uint8_t EscFlag:1;
									uint8_t ControlFlag:1;
									uint8_t UnknownFlag:1;
									uint8_t NumberFlag:1;
									uint8_t Ok:1;
									uint8_t	Data:1;
									uint8_t TimeOut:1;
									uint16_t LastRx;
								}	editKey_st;


		typedef struct process_state_s { __FlashStringHelper * pCmd;
										 uint16_t 			TimeStamp;
										 uint8_t 			nRetry;
										 process_state_et 	State;
										 uint8_t			RxCnt;
									 } process_state_st;


		typedef struct edit_s { char 		Buffer[EDIT_LEN+1];
								char *		pInput;
								const char*	pCharSet;
								editCtrl_et	State;
								uint8_t		Pos, PosX,PosY,Len;
								uint8_t		TrimLeft:1;
								uint8_t		TrimRight:1;
								uint8_t		TermNull:1;
								uint32_t	ExitMask;
							} edit_st;


		uint8_t 	TempBuffer[TEMP_BUFFER_SIZE];
		uint8_t 	TempBufferIn,TempBufferOut;
		editKey_st 	editKeyData;
		edit_st		editData;

		terminal_mode_et 	TerminalMode;
		terminal_options_et	TerminalOptions;
		process_state_st 	ProcessState;
		
		uint16_t TimeStamp;
		bool bRxTimeout;
		uint16_t RxTimout;

		void init(void);
		void ResetRequest(void);
		void RequstFrame(void);

		bool GetFrame(char * cData, uint8_t MaxCount);				// get CSI 
											
		char GetCode(const char  * cData);							// Get CMD Code of CSI
		uint16_t GetParam(const char* cData,uint8_t unParam); 		// get Parameter #unParam, 0=Count
									
		bool StrCmp(const char * StringA,const char * StringB);
		bool StrCmp(const char * StringA,const __FlashStringHelper * StringB);

		void WaitForInput(void);

		void processInput(void);
		
		uint8_t readBuffer(void);
		
		bool CheckKeyMask(const char* pcKeyMask, uint8_t cKey);
		bool CheckKeyMask_P(const char* pcKeyMask, uint8_t cKey);
		bool CheckControlKeyMask(uint32_t ControlMask, uint8_t cKey);

		void EditReset(void);
		void EditKeyReset(void);
		
		terminal_options_et DecodeTerminalOptions(const char * buffer);
};




#endif //__VT100_H__
