#include "VT100.h"
#include "VT100_defs.h"
#include "../ASCII/ASCII_ctrl.h"
#include "Arduino.h"

#ifdef ARDUINO_ARCH_AVR
#define SerialUSB Serial
#endif

const char VT100_c::ESC_RESET_MODE[] PROGMEM					=VT100_ESC_RESET_MODE;			
const char VT100_c::ESC_APPLICATION_MODE[] PROGMEM				=VT100_ESC_APPLICATION_MODE;
const char VT100_c::QUERY_DEVICE_ATTRIBUTES[] PROGMEM			=VT100_QUERY_DEVICE_ATTRIBUTES;	
const char VT100_c::DEVICE_ATTRIBUTES[] PROGMEM					=VT100_DEVICE_ATTRIBUTES;			
const char VT100_c::RESET_TERMINAL[] PROGMEM					=VT100_RESET_TERMINAL;
const char VT100_c::CURSOR_UP[] PROGMEM			   				=VT100_CURSOR_UP;
const char VT100_c::CURSOR_DOWN[] PROGMEM						=VT100_CURSOR_DOWN;				
const char VT100_c::CURSOR_FORWARD[] PROGMEM 					=VT100_CURSOR_FORWARD;			
const char VT100_c::CURSOR_BACK[] PROGMEM						=VT100_CURSOR_BACK;
const char VT100_c::CURSOR_HOME[] PROGMEM						=VT100_CURSOR_HOME;				
const char VT100_c::ERASE_END_OF_LINE[] PROGMEM					=VT100_ERASE_END_OF_LINE;
const char VT100_c::ERASE_START_OF_LINE[] PROGMEM				=VT100_ERASE_START_OF_LINE;		
const char VT100_c::ERASE_LINE[] PROGMEM						=VT100_ERASE_LINE;
const char VT100_c::ERASE_DOWN[] PROGMEM						=VT100_ERASE_DOWN;				
const char VT100_c::ERASE_UP[] PROGMEM							=VT100_ERASE_UP;
const char VT100_c::ERASE_SCREEN[] PROGMEM						=VT100_ERASE_SCREEN;
const char VT100_c::QUERY_CURSOR_POS[] PROGMEM					=VT100_QUERY_CURSOR_POS;
const char VT100_c::SAVE_CURSOR_ATTRIBUTES[] PROGMEM			=VT100_SAVE_CURSOR_ATTRIBUTES;	
const char VT100_c::RESTORE_CURSOR_ATTRIBUTES[] PROGMEM			=VT100_RESTORE_CURSOR_ATTRIBUTES;
const char VT100_c::DOUBLE_HIGHT_UPPER_LINE[] PROGMEM			=VT100_DOUBLE_HIGHT_UPPER_LINE;	
const char VT100_c::DOUBLE_HIGHT_LOWER_LINE[] PROGMEM			=VT100_DOUBLE_HIGHT_LOWER_LINE;	
const char VT100_c::DOUBLE_WIDTH[] PROGMEM						=VT100_DOUBLE_WIDTH;
const char VT100_c::DOUBLE_OFF[] PROGMEM						=VT100_DOUBLE_OFF;				

const char VT100_c::SW_KEY_LOCK_OFF[] PROGMEM					=VT100_KEY_UNLOCK;			// keylock / beep
const char VT100_c::SW_KEY_LOCK_ON[] PROGMEM					=VT100_KEY_LOCK;			// keylock / beep
const char VT100_c::SW_OVERWRITE[] PROGMEM						=VT100_OVERWRITE;			// change insert / overwrite mode
const char VT100_c::SW_INSERT[] PROGMEM							=VT100_INSERT;				// change insert / overwrite mode
const char VT100_c::SW_AUTO_LF_OFF[] PROGMEM					=VT100_AUTO_LF_OFF;			// CR -> CR / LF
const char VT100_c::SW_AUTO_LF_ON[] PROGMEM						=VT100_AUTO_LF_ON;			// CR -> CR / LF
const char VT100_c::SW_CURSOR_MODE_APPLICATION_OFF[] PROGMEM	=VT100_APP_MODE_OFF;		// ESC-Sequenz ESC O, otherwise ESC [
const char VT100_c::SW_CURSOR_MODE_APPLICATION_ON[] PROGMEM		=VT100_APP_MODE_ON;			// ESC-Sequenz ESC O, otherwise ESC [
const char VT100_c::SW_VT52[] PROGMEM							=VT100_MODE_VT52;			// Select emulation VT100 / VT52
const char VT100_c::SW_VT100[] PROGMEM							=VT100_MODE_VT100;			// Select emulation VT100 / VT52
const char VT100_c::SW_COLUMN80[] PROGMEM						=VT100_COLUMN80;			// Select 80 / 132 column mode 
const char VT100_c::SW_COLUMN132[] PROGMEM						=VT100_COLUMN132;			// Select 80 / 132 column mode 
const char VT100_c::SW_SMOOTH_SCROLL_OFF[] PROGMEM				=VT100_LINE_SCROLL;			// soft scroll ( pixel scroll / line scroll)
const char VT100_c::SW_SMOOTH_SCROLL_ON[] PROGMEM				=VT100_SOFT_SCROLL;			// soft scroll ( pixel scroll / line scroll)
const char VT100_c::SW_SCREEN_MODE_REVERSE_OFF[] PROGMEM		=VT100_NORMAL_SCRREN;		// black characters on clored background
const char VT100_c::SW_SCREEN_MODE_REVERSE_ON[] PROGMEM			=VT100_REVERSE_SCREEN;		// black characters on clored background
const char VT100_c::SW_CURSOR_ABSOLUTE[] PROGMEM				=VT100_CURSOR_ABSOLUTE;		// enable relative cursor position
const char VT100_c::SW_CURSOR_RELATIVE[] PROGMEM				=VT100_CURSOR_RELATIVE;		// enable relative cursor position
const char VT100_c::SW_LINE_WRAP_OFF[] PROGMEM					=VT100_LINE_WRAP_OFF;		// enable line wrap
const char VT100_c::SW_LINE_WRAP_ON[] PROGMEM					=VT100_LINE_WRAP_ON;		// enable line wrap
const char VT100_c::SW_AUTO_REPEAT_OFF[] PROGMEM				=VT100_AUTO_REPEAT_OFF;		// enable auto repeat on keyboard
const char VT100_c::SW_AUTO_REPEAT_ON[] PROGMEM					=VT100_AUTO_REPEAT_ON;		// enable auto repeat on keyboard
const char VT100_c::SW_VIDEO_PROGRESSIVE[] PROGMEM				=VT100_PROGRESSIVE_SCAN;	// ??? relict off cathode tube displays 
const char VT100_c::SW_VIDEO_INTRELACED[] PROGMEM				=VT100_INTERLACED;			// ??? relict off cathode tube displays 
const char VT100_c::SW_CURSOR_OFF[] PROGMEM						=VT100_HIDE_CURSOR;			// show cursor
const char VT100_c::SW_CURSOR_ON[] PROGMEM						=VT100_SHOW_CURSOR;			// show cursor
const char VT100_c::SW_DISABLE_GRAPHICS[] PROGMEM				=VT100_NORMAL_CHARSET;		// enable / disable grafics char set
const char VT100_c::SW_ENABLE_GRAPHICS[] PROGMEM				=VT100_GRAPHICS_CHARSET;	// enable / disable grafics char set
const char VT100_c::SW_NUM_MODE_APPLICATION_OFF[] PROGMEM		=VT100_NUMBERIC_KEY_PAD;	// enbale numbe block
const char VT100_c::SW_NUM_MODE_APPLICATION_ON[] PROGMEM		=VT100_ALTERATE_KEY_PAD;	// enbale numbe block
const char VT100_c::SW_CHARACTERSET_G0[] PROGMEM				=VT100_CHARSET_G1;			// enable G0 charset
const char VT100_c::SW_CHARACTERSET_G1[] PROGMEM				=VT100_CHARSET_G0;			// enable G0 charset



VT100_c::VT100_c(HardwareSerial & port,uint16_t RxTimeout_ms):SerialClass(port)
{
	bRxTimeout=false;
	RxTimout=RxTimeout_ms;
	TimeStamp=0;
	
	ProcessState.State=process_start;
	ProcessState.pCmd=(__FlashStringHelper *) QUERY_DEVICE_ATTRIBUTES;
	ProcessState.nRetry=5;

	init();

}
#ifdef HAS_USB_SERIAL
VT100_c::VT100_c(Serial_ & port,uint16_t RxTimeout_ms):SerialClass(port)
{
	bRxTimeout=false;
	RxTimout=RxTimeout_ms;
	TimeStamp=0;
	
	ProcessState.State=process_start;
	ProcessState.pCmd=(__FlashStringHelper *) QUERY_DEVICE_ATTRIBUTES;
	ProcessState.nRetry=5;

	init();

}
#endif


void VT100_c::process(void)
{	// process
	TimeStamp=millis();
	if (dtr())
	{	// connection active
		switch (TerminalMode)
		{	// switch TerminalMode
			case offline:
				RequstFrame();
				switch (ProcessState.State)
				{	// switch ProcessState.State
					case process_timeout:
						TerminalMode=serial;
						SerialOpen();
						break;
					case process_done:
						TerminalOptions=DecodeTerminalOptions((char*)TempBuffer);
						if (TerminalOptions==noTerminal)
						{
							TerminalMode=serial;
							SerialOpen();
						}
						else
						{
							TerminalMode=terminal;
							TerminalOpen();
						}
						EditReset();
						EditKeyReset();
						break;
					default:
						break;
				}	// switch ProcessState.State
				break;
			case serial:
				SerialProcess();
				break;
			case terminal:
				processInput();
				TerminalProcess();
				break;
			default:
				TerminalMode=offline;
				break;
		}	// switch TerminalMode
	}	// connection active
	else
	{	// USB disconnected
		if (TerminalMode!=VT100_c::offline)
		{
			if (TerminalMode==VT100_c::terminal)
				TerminalClose();
			else
				SerialClose();
			TerminalMode=VT100_c::offline;
			bRxTimeout=false;
			ProcessState.State=VT100_c::process_start;
			ProcessState.pCmd=(__FlashStringHelper *)QUERY_DEVICE_ATTRIBUTES;
			ProcessState.nRetry=5;
			TerminalOptions=noTerminal;
		}
	}	// USB disconnected
}	// process

VT100_c::terminal_mode_et VT100_c::GetTerminalMode(void)
{		
	return TerminalMode;
}

void VT100_c::ResetTerminal(void)								// Terminal Initialiserung
{	// ResetTerminal
	print((__FlashStringHelper *)RESET_TERMINAL);
}	// ResetTerminal

void VT100_c::SetKeyLock(bool bLock)				 			// keylock / beep
{	// SetKeyLock                                               
	print((bLock)?                                    
					((__FlashStringHelper *)SW_KEY_LOCK_ON):                           
					((__FlashStringHelper *)SW_KEY_LOCK_OFF));                         
}	// SetKeyLock                                               
																
void VT100_c::SetInsert(bool bInsert)							// change insert / overwrite mode
{	// SetInsert                                                
	print((bInsert)?                                  
					((__FlashStringHelper *)SW_INSERT):                                
					((__FlashStringHelper *)SW_OVERWRITE));                            
}	// SetInsert                                                
																
void VT100_c::SetAutoLF(bool bAutoLF)							// CR -> CR / LF
{	// SetAutoLF                                                
	print((bAutoLF)?                                  
					((__FlashStringHelper *)SW_AUTO_LF_ON):                            
					((__FlashStringHelper *)SW_AUTO_LF_OFF));                          
}	// SetAutoLF                                                
																
void VT100_c::SetCursorModeApplication(bool bModeApplication)	// ESC-Sequenz ESC O, otherwise ESC [
{	// SetCursorModeApplication                             	
	print((bModeApplication)?                     	
					((__FlashStringHelper *)SW_CURSOR_MODE_APPLICATION_ON):        	
					((__FlashStringHelper *)SW_CURSOR_MODE_APPLICATION_OFF));      	
}	// SetCursorModeApplication                             	
																
void VT100_c::SetEmulationVT100(bool bVT100)					// Select emulation VT100 / VT52
{	// SetEmulationVT100                                    	
	print((bVT100)?                               	
					((__FlashStringHelper *)SW_VT100):                             	
					((__FlashStringHelper *)SW_VT52));                             	
}	// SetEmulationVT100                                    	
																
void VT100_c::SetWidth132(bool b132)							// Select 80 / 132 column mode 
{	// SetWidth132                                          	
	print((b132)?                                 	
					((__FlashStringHelper *)SW_COLUMN132):                         	
					((__FlashStringHelper *)SW_COLUMN80));                         	
}	// SetWidth132                                          	
																
void VT100_c::SetSmoothScroll(bool bSmooth)						// soft scroll ( pixel scroll / line scroll)
{	// SetSmoothScroll                                      	
	print((bSmooth)?                              	
					((__FlashStringHelper *)SW_SMOOTH_SCROLL_ON):                  	
					((__FlashStringHelper *)SW_SMOOTH_SCROLL_OFF));                	
}	// SetSmoothScroll                                      	
																
void VT100_c::SetScreenModeReveres(bool bReverse)				// black characters on clored background
{	// SetScreenModeReveres                                 	
	print((bReverse)?                             	
					((__FlashStringHelper *)SW_SCREEN_MODE_REVERSE_ON):            	
					((__FlashStringHelper *)SW_SCREEN_MODE_REVERSE_OFF));          	
}	// SetScreenModeReveres                                 	
																
void VT100_c::SetCursorRelative(bool bRelative)					// enable relative cursor position
{	// SetCursorRelative                                    	
	print((bRelative)?                            	
					((__FlashStringHelper *)SW_CURSOR_RELATIVE):                   	
					((__FlashStringHelper *)SW_CURSOR_ABSOLUTE));                  	
}	// SetCursorRelative                                    	
																
void VT100_c::SetLineWrap(bool bWrap)							// enable line wrap
{	// SetLineWrap                                          	
	print((bWrap)?                                	
					((__FlashStringHelper *)SW_LINE_WRAP_ON):                      	
					((__FlashStringHelper *)SW_LINE_WRAP_OFF));                    	
}	// SetLineWrap                                          	
																
void VT100_c::SetAutoRepeat(bool bAutoRepeat)					// enable auto repeat on keyboard
{	// SetAutoRepeat                                        	
	print((bAutoRepeat)?                          	
					((__FlashStringHelper *)SW_AUTO_REPEAT_ON):                    	
					((__FlashStringHelper *)SW_AUTO_REPEAT_OFF));                  	
}	// SetAutoRepeat                                        	
																
void VT100_c::SetVideoModeInterlaced(bool bInterlaced)			// ??? relict off cathode tube displays 
{	// SetVideoModeInterlaced                               	
	print((bInterlaced)?                          	
					((__FlashStringHelper *)SW_VIDEO_INTRELACED):                  	
					((__FlashStringHelper *)SW_VIDEO_PROGRESSIVE));                	
}	// SetVideoModeInterlaced                               	
																
void VT100_c::SetCursorOn(bool bOn)								// show cursor
{	// SetCursorOn                                          	
	print((bOn)?
					((__FlashStringHelper *)SW_CURSOR_ON):
					((__FlashStringHelper *)SW_CURSOR_OFF));  	
}	// SetCursorOn                                          	
																
void VT100_c::EnableGraphics(bool bOn)							// enable / disable grafics char set
{	// EnableGraphics                                       	
	print((bOn)?                                  	
					((__FlashStringHelper *)SW_ENABLE_GRAPHICS):                   	
					((__FlashStringHelper *)SW_DISABLE_GRAPHICS));                 	
}	// EnableGraphics                                       	
																
void VT100_c::SetNumModeApplication(bool bOn)					// enbale number block
{	// SetNumModeApplication                                	
	print((bOn)?                                  	
					((__FlashStringHelper *)SW_NUM_MODE_APPLICATION_ON):           	
					((__FlashStringHelper *)SW_NUM_MODE_APPLICATION_OFF));         	
}	// SetNumModeApplication                                	
																
void VT100_c::EnableG0Charset(void)								// enable G0 charset
{	// EnableG0Charset                                      	
	print((__FlashStringHelper *)SW_CHARACTERSET_G0);                    	
}	// EnableG0Charset                                      	
																
void VT100_c::EnableG1Charset(void)                         	
{	// EnableG1Charset                                      	
	print((__FlashStringHelper *)SW_CHARACTERSET_G1);                    	
}	// EnableG1Charset                                      	
																
																
void VT100_c::ClearEOL(void)									// clear form x to eol
{	// ClearEOL                                             	
	print((__FlashStringHelper *)ERASE_END_OF_LINE);                     	
}	// ClearEOL                                             	
																
void VT100_c::ClearSOL(void)									// clear from sol to x
{	// ClearSOL                                             	
	print((__FlashStringHelper *)ERASE_START_OF_LINE);                   	
}	// ClearSOL                                             	
																
void VT100_c::ClearLine(void)									// clear whole line
{	// CleatLine                                            	
	print((__FlashStringHelper *)ERASE_LINE);                            	
}	// CleatLine                                            	
																
void VT100_c::ClearDown(void)									// Clear rest of screen
{	// ClearDown                                            	
	print((__FlashStringHelper *)ERASE_DOWN);                            	
}	// ClearDown                                            	
																
void VT100_c::ClearUp(void)										// screen up to y
{	// ClearUp                                              	
	print((__FlashStringHelper *)ERASE_UP);                              	
}	// ClearUp                                              	
																
void VT100_c::ClearScreen(void)									
{	// ClearScreen                                          	
	print((__FlashStringHelper *)ERASE_SCREEN);                          	
}	// ClearScreen                                          	
																
																
void VT100_c::CursorHome(void)									// = SetCursor(0,0)
{	// CursorHome                                           	
	SetCursor(0,0);                                   	
}	// CursorHome                                           	
																
void VT100_c::SetCursor(uint8_t PosX, uint8_t PosY)				// set cursor pos
{   // SetCursor
	print((__FlashStringHelper *)ESC_RESET_MODE);							// CSI
	print(PosY,DEC);								// Y
	write(';');										// sperator
	print(PosX,DEC);								// X
	write(cVT100_CURSOR_HOME);						// command char
}	// SetCursor

void VT100_c::SetScrollArea(uint8_t TopLine, uint8_t BottomLine)// set area for scrolling 
{	// SetScrollArea
	print((__FlashStringHelper *)ESC_RESET_MODE);							// CSI
	print(TopLine,DEC);								// first parameter
	write(';');										// separtor
	print(BottomLine);								// second Parameter
	write(cVT100_SCROLL_AREA);						// cmd
}	// SetScrollArea



void VT100_c::SaveRestoreCursorAttributes(bool bSave)			// cursor attributes save / restore
{	// SaveRestoreCursorAttributes
	print((bSave)?
					((__FlashStringHelper *)SAVE_CURSOR_ATTRIBUTES):
					((__FlashStringHelper *)RESTORE_CURSOR_ATTRIBUTES) );
}	// SaveRestoreCursorAttributes

void VT100_c::SetAttributes(uint8_t Attributes)		   			// set text attributes
{	// SetAttributes
	print((__FlashStringHelper *)ESC_RESET_MODE);
	if (Attributes & (uint8_t)reset)
	{
		write(cVT100_RESET_ATTRIBUTES);
	}
	if (Attributes & (uint8_t)bright)
	{
		write(';');
		write(cVT100_BRIGHT);
	}
	if (Attributes & (uint8_t)dim)
	{
		write(';');
		write(cVT100_DIM);
	}
	if (Attributes & (uint8_t)underscore)
	{
		write(';');
		write(cVT100_UNDERSCORE);
	}
	if (Attributes & (uint8_t)blink)
	{
		write(';');
		write(cVT100_BLINK);
	}
	if (Attributes & (uint8_t)reverse)
	{
		write(';');
		write(cVT100_REVERSE);
	}
	if (Attributes & (uint8_t)hidden)
	{
		write(';');
		write(cVT100_HIDDEN);
	}
	
	write(cVT100_SET_ATTRIBUTES);
}	// SetAttributes


void VT100_c::SetForegroundColour(colour_et Colour)			// set colours (ANSI)
{	// SetForegroundColour
	print((__FlashStringHelper *)ESC_RESET_MODE);
	write('3');
	write('0'+(uint8_t)Colour);
	write(cVT100_SET_ATTRIBUTES);
}	// SetForegroundColour

void VT100_c::SetBackgroundColour(colour_et Colour)			// set colours (ANSI)
{	// SetBackgroundColour
	print((__FlashStringHelper *)ESC_RESET_MODE);
	write('4');
	write('0'+(uint8_t)Colour);
	write(cVT100_SET_ATTRIBUTES);
}	// SetBackgroundColour


void VT100_c::SetColours(	colour_et Foreground,	  			// set colours (ANSI)
							colour_et Background)
{	// SetColours
	print((__FlashStringHelper *)ESC_RESET_MODE);
	write('3');
	write('0'+(uint8_t)Foreground);
	write(';');
	write('4');
	write('0'+(uint8_t)Background);
	write(cVT100_SET_ATTRIBUTES);
}	// SetColours

void VT100_c::SetCharSize(charsize_et Size)					// font size
{	// SetCharSize
	switch (Size)
	{
	case normal					:
		print((__FlashStringHelper *)DOUBLE_OFF);
		break;
	case double_width			:
		print((__FlashStringHelper *)DOUBLE_WIDTH);
		break;
	case double_hight_upper_line:
		print((__FlashStringHelper *)DOUBLE_HIGHT_UPPER_LINE);
		break;
	case double_hight_lower_line:
		print((__FlashStringHelper *)DOUBLE_HIGHT_LOWER_LINE);
		break;
	default:
		break;
	}
}	// SetCharSize


void VT100_c::Print(const char * cString)								// output text
{	// Print
	print(cString);
}	// Print

void VT100_c::Print(const __FlashStringHelper * cFlashString)			// output text
{	// Print
	print(cFlashString);
}	// Print

void VT100_c::PrintAt(	uint8_t PosX, 									// output text
						uint8_t PosY,
						const char * cString)
{	// PrintAt
	SetCursor(PosX,PosY);
	Print(cString);
}	// PrintAt

void VT100_c::PrintAt(	uint8_t PosX, 
						uint8_t PosY, 
						const __FlashStringHelper *cFlashString)		// print at designated pos 
{	// PrintAt flash
	SetCursor(PosX,PosY);
	Print(cFlashString);
}	// PrintAt flash

void VT100_c::PutChar(char cChar)									// output char
{	// 	PutChar
	write(cChar);
}	// PutChar

void VT100_c::PutCharAt(uint8_t PosX, 									// output char
						uint8_t PosY,
						char cChar)
{	// PutCharAt
	SetCursor(PosX,PosY);
	PutChar(cChar);
}	// PutCharAt


bool VT100_c::KeyPressed(void)						// check for input
{
	return TempBufferIn!=TempBufferOut;					
}

bool VT100_c::ScanKey(char * pcKey,bool bSkipCodes)	// Keypressed + Read
{	// check if key has been pressed and return it
	uint8_t cChar;
	bool bChar=KeyPressed();
	if (bChar)
	{	// data available
		cChar=readBuffer();
		if ((cChar>=SPACE) || (!bSkipCodes))
			*pcKey=cChar;	// printable or Control Codes allowed
		else
			bChar=false;	// no control codes
	}	// data available
	return bChar;
}	// check if key has been pressed and return it

bool VT100_c::ScanKeyMask(	char * pcKey,  		// non blocking getKeyMask
							const char * pcKeyMask,
							uint32_t ControlMask)
{	// check if a certain key has been pressed
	uint8_t cTemp;
	bool bKey=ScanKey((char*)&cTemp,ControlMask==0);
	
	if (bKey)
	{ // keypress occured
		bKey=	CheckKeyMask(pcKeyMask,cTemp) || 
				CheckControlKeyMask(ControlMask,cTemp);
		
		if (bKey)
		{ // valid key
			*pcKey=cTemp;
		} // valid key
	} // keypress occured
	
	return bKey;
}	// check if a certain key has been pressed

bool VT100_c::ScanKeyMask(	char * pcKey, 										// Keypressed + Read + KeyMask
							const __FlashStringHelper * pcFlashKeyMask, 		// form program Memory
							uint32_t ControlMask)
{	// check if a certain key has been pressed
	uint8_t cTemp;
	bool bKey=ScanKey((char*)&cTemp,ControlMask==0);
	
	if (bKey)
	{ // keypress occured
		bKey=	CheckKeyMask_P((const char*)pcFlashKeyMask,cTemp) || 
				CheckControlKeyMask(ControlMask,cTemp);
		
		if (bKey)
		{ // valid key
			*pcKey=cTemp;
		} // valid key
	} // keypress occured
	
	return bKey;
}	// check if a certain key has been pressed

char VT100_c::GetKey(bool bEcho, bool bSkipCodes)					// wait for character
{	// GetKey
	uint8_t cTemp;

	while (!ScanKey((char*)&cTemp,bSkipCodes));
	/*
	 * current setup: no echo for control keys
	 */
	if (bEcho && (cTemp>=SPACE))
		write(cTemp);

	return cTemp;
}	// GetKey

char VT100_c::GetKeyMask(	const char *cMask, 							// waits for a dedicatet chacter set
							bool bEcho, 
							uint32_t ControlMask)
{	// GetKeyMask
	uint8_t cTemp;											// Point for charset

	while (!ScanKeyMask((char*)&cTemp,cMask,ControlMask));

	/*
	 * current setup: no echo for control keys
	 */
	if (bEcho && (cTemp>=SPACE))
		write(cTemp);

	return cTemp;
}	// GetKeyMask




/**
 * \brief	EditStart
 * \details	create an edit field at a desired positiom
 * 
 * required Parameters are stored in inter struct editData
 * 
 * \param	[in]		PosX		X-pos of start of edit field  
 * 			[in]		PosY		Y-pos of start of edit field  
 * 			[in,out]	pData		Pointer to Input Data String  
 * 			[in]		Len			Length of Data field (keep '\0' in mind)  
 * 			[in]		pCHarSet	'0'-terminated string of allowed chars  
 * 			[in]		bTrimLeft	trim leading spaces after exit  
 * 			[in]		bTrimRigth	trim trailing spaces after exit  
 * 			[in]		bTermNull	Edit String is '\0' terminated   
 * 			[in]		ExitMask	Control Keys allowd to exit editing  
 * 
 * 
 */
 
 void VT100_c::EditStart(uint8_t PosX, 	uint8_t PosY,
						char *pData,	uint8_t Len,
						const char * cKeyMask,
						bool TrimLeft,
						bool TrimRight,
						bool TermNull,
						uint32_t ExitMask)
{	// EditStart
	uint8_t i;
	EditReset();
	for (i=0;(i<Len) && (i<EDIT_LEN) && pData[i];i++)
		editData.Buffer[i]=pData[i];
	for (;(i<Len) && (i<EDIT_LEN); i++) editData.Buffer[i]=SPACE;
	editData.Buffer[i]=NUL;
	editData.pInput=pData;
	editData.pCharSet=cKeyMask;
	editData.Pos=0;
	editData.PosX=PosX;
	editData.PosY=PosY;
	editData.Len=Len;
	editData.TrimLeft=TrimLeft;
	editData.TrimRight=TrimRight;
	editData.TermNull=TermNull;
	editData.ExitMask=ExitMask;
	/*
	editData.ExitMask|=	FLAG_CRSR_RIGHT |	FLAG_CRSR_LEFT 	| 
						FLAG_CRSR_HOME	|	FLAG_CRSR_END	|
						FLAG_CRSR_SOL	|	FLAG_CRSR_EOL 	|
						FLAG_BS			|	FLAG_DEL		|
						FLAG_END 		|	FLAG_ENTER		|
						FLAG_ESC;
	*/
	SaveRestoreCursorAttributes(true);
	SetCursor(editData.PosX,editData.PosY);
	SetCursorOn(true);
	SetAttributes(reverse);
	editData.State=editing;
	Print(editData.Buffer);
	SetCursor(editData.PosX+editData.Pos,editData.PosY);

}	// EditStart

/**
 * \brief	Edit
 * \details	perform editing
 * \returns exit state or editing
 */
VT100_c::editCtrl_et VT100_c::Edit(void)
{	// Edit
	char cChar;
	uint8_t i;
	
	if (ScanKeyMask(&cChar,editData.pCharSet,-1))
	{	// process if Data available
		if (cChar>=SPACE)
		{ // printable char
			editData.Buffer[editData.Pos]=cChar;
			if (editData.Pos<editData.Len-1) editData.Pos++;
		} // printable char
		else
		{ // ControlCode
			switch (cChar)
			{ // Steuercode selektiern
			case EDIT_CRSR_UP:		
				if (editData.ExitMask & FLAG_CRSR_UP)
				{ 
					editData.State=up;
				}
				break;
			case EDIT_CRSR_DOWN :	
				if (editData.ExitMask & FLAG_CRSR_DOWN)
				{ 
					editData.State=down;
				}
				break;
			case EDIT_CRSR_LEFT :	
				if (editData.Pos>0) 
					editData.Pos--;
				else if (editData.ExitMask & FLAG_CRSR_LEFT)
				{
					editData.State=left;
				}	
				break;
			case EDIT_CRSR_RIGHT:
				editData.Pos++;
				if (editData.Pos>=editData.Len) 
				{	// Ende
					if	(editData.ExitMask & FLAG_CRSR_RIGHT) 
					{	// leaving right -> next
						editData.State=right;
					}	// leaving right -> next
					else
						editData.Pos--;
				}	// Ende
				else	
				{	//	no Cursor if current Char is invalid
					if (!CheckKeyMask(editData.pCharSet,
										editData.Buffer[editData.Pos-1])) 
						editData.Pos--;
				}	//	no Cursor if current Char is invalid
				break;
			case EDIT_CRSR_HOME :	
			case EDIT_CRSR_SOL  :		
				editData.Pos=0;				
				break;
			case EDIT_CRSR_END  :		
			case EDIT_CRSR_EOL  :		
				editData.Pos=editData.Len-1;
				break;
			case EDIT_BS		  :
				if (editData.Pos>0)
				{ // shift to left of cursor
					for (i=editData.Pos-1; i<editData.Len-1; i++)
						editData.Buffer[i]=editData.Buffer[i+1];
					editData.Buffer[i]=SPACE;
					editData.Pos--;
				} // shift to left of cursor
				break;
			case EDIT_DEL		  :	
				// shift left under Cursor
				for (i=editData.Pos; i<editData.Len-1; i++)
					editData.Buffer[i]=editData.Buffer[i+1];
				editData.Buffer[i]=SPACE;
				break;				  
			case EDIT_INS		  :	
				break;
			case EDIT_PREVIOUS  :		
				break;
			case EDIT_NEXT	  :		
				break;
			case EDIT_PGUP	  :		
				break;
			case EDIT_PGDN	  :		
				break;
			case EDIT_ENTER	  :
				editData.State=enter;		
				break;
			case EDIT_ESC		  :	
			case EDIT_CANCEL	  :	
				editData.State=cancel;		
				break;
			case EDIT_END		  :
				editData.State=end;		
				break;
			default:
				break;
			} // Steuercode selektiern
		} // // ControlCode
		PrintAt(editData.PosX,editData.PosY,editData.Buffer);
		SetCursor(editData.PosX+editData.Pos,editData.PosY);
	} // // process if Data available

	if ((editData.State!=cancel) && (editData.State!=editing))
	{	// Exit & not Cancel -> update
		i=0;
		if (editData.TrimLeft) 
			while (editData.Buffer[i]==SPACE) i++;
		editData.Pos=editData.Len-1;
		if (editData.TrimRight) 
			while (	(i<editData.Pos) && 
					(editData.Buffer[editData.Pos]==SPACE)) 
				editData.Pos--;
		for (;i<editData.Pos+1;i++) 
		{ // copy back
			*editData.pInput=editData.Buffer[i];
			editData.pInput++;
		} // copy back
		if (editData.TermNull)
			*editData.pInput=NUL;
		else
			for (;i<editData.Len;i++) 
			{	// if no term, fill with Spaces
				*editData.pInput=SPACE;
				editData.pInput++;
			}	// if no term, fill with Spaces
	}	// Exit & not Cancel -> update
	else if (editData.State==cancel)
	{
		for (i=0;(i<editData.Len) && (i<EDIT_LEN) && editData.pInput[i];i++)
			editData.Buffer[i]=editData.pInput[i];
		for (;(i<editData.Len) && (i<EDIT_LEN); i++) editData.Buffer[i]=SPACE;
		editData.Buffer[i]=NUL;
	}

	if (editData.State!=editing)
	{
		SaveRestoreCursorAttributes(false);
		PrintAt(editData.PosX,editData.PosY,editData.Buffer);
		SetCursorOn(false);
	}
	
	return editData.State;
}	// Edit

/* ******************************************************* 
 * private funktions
 */


void VT100_c::init(void)
{
	TerminalOptions=noTerminal;
	TerminalMode=offline;
	TempBufferIn=0;
	TempBufferOut=0;
	TimeStamp=0;
	
	EditKeyReset();
	EditReset();
}

void VT100_c::ResetRequest(void)
{
	TempBufferIn=0;
	TempBufferOut=0;
	
	EditKeyReset();
	EditReset();
}

// Requests a Terminal Frame nonblocking;
// numer of Retries and cmd to be defined in ProcessState 
void VT100_c::RequstFrame(void)
{	// Request Frame
	bool bStop=false;
	switch (ProcessState.State)
	{	// switch ProcessState
		case VT100_c::process_start:
			ResetRequest();
			print(ProcessState.pCmd);
			ProcessState.TimeStamp=TimeStamp;
			ProcessState.State=process_wait;
			ProcessState.RxCnt=0;
			break;
		case VT100_c::process_wait:
			while (available() && (!bStop))
			{	// get Data from Serial
				ProcessState.TimeStamp=TimeStamp;
				TempBuffer[ProcessState.RxCnt]=read();
				switch (ProcessState.RxCnt) 
				{	// switch RxCnt
					case 0:	// wait for start of sequence
						if (TempBuffer[0]==ESC)
						{	// valid Sequence Start detected
							 ProcessState.RxCnt++;
						}	// valid Sequence Start detected
						break;
					case 1: // for valid command start
						if ((TempBuffer[1]==cVT100_RESET_MODE_ESC) ||		// Reset-Mode ESC-Sequenz oder
							(TempBuffer[1]==cVT100_APPLICATION_MODE_ESC))	// Apllication Mode ESC-Sequenz 
						{	// valid sequence start found
							ProcessState.RxCnt++;
						}	// valid sequence start found
						else
						{	// no volid sequence start found
							ProcessState.RxCnt=0;
						} 	// no volid sequence start found
						break;
					default:
						ProcessState.RxCnt++;
						if (TempBuffer[ProcessState.RxCnt-1]>='@')
						{	// last character
							ProcessState.State=process_wait_done;
							bStop=true;
						}	// last character
						break;
				}	// switch RxCnt
				
				bStop|=ProcessState.RxCnt<TEMP_BUFFER_SIZE-2;
			}	// get Data from Serial
			TempBuffer[ProcessState.RxCnt]='\0';
			if (ProcessState.State==process_wait)
			{	// check timeout
				if ((uint16_t)(TimeStamp-ProcessState.TimeStamp)>RECEIVE_TIMEOUT)
				{
					if (ProcessState.nRetry--)
						ProcessState.State=VT100_c::process_start;
					else
						ProcessState.State=VT100_c::process_timeout;
				}
			}	// check timeout
			break;
		case VT100_c::process_wait_done:
			/* fall through */
		case VT100_c::process_wait_timeout:
			if (read()!=-1) ProcessState.TimeStamp=TimeStamp;
			if ((uint16_t)(TimeStamp-ProcessState.TimeStamp)>RECEIVE_TIMEOUT)
			{
				if (ProcessState.State==process_wait_done)
					ProcessState.State=VT100_c::process_done;
				else
					ProcessState.State=VT100_c::process_timeout;
			}
			break;
		case VT100_c::process_timeout:
			ProcessState.RxCnt=0;
			/* fall through */
		case VT100_c::process_done:
			// nothing to do
			break;
	}	// switch ProcessState
}	// Request Frame

bool VT100_c::GetFrame(char * cData, uint8_t MaxCount)				// Wait for CSI
{	// GetFrame
	bool bResult=false;
	char * In=cData;
	uint8_t Count;
	setTimeout(RECEIVE_TIMEOUT);
	Count=readBytes(In,MaxCount);
	if (Count)
	{	// Antwort erhalten
		while ((Count) && (*In!=ESC))
		{
			In++;
			Count--;
		}
		*cData=*In;
				
		if  ((Count) && (*cData==ESC))
		{	// Steuersequenz wurde gefunden & noch zeichen vorhanden
			*++cData=*++In;
			MaxCount-=2;
			if ((Count) ||										// noch Daten vorhanden
				(*cData==cVT100_RESET_MODE_ESC) ||				// Reset-Mode ESC-Sequenz oder
				(*cData==cVT100_APPLICATION_MODE_ESC))			// Apllication Mode ESC-Sequenz 
			{
				do
				{	// Daten bis letztes Zeichen lesen	
					Count--;
					MaxCount--;
					*++cData=*++In;
				}	// Daten bis letztes Zeichen lesen
				while (	(Count) && 			// noch zeichen vorhanden
						(*cData<'@') && 						// letztes zeichen noch nicht erreicht
						(MaxCount!=0));						// noch platz im Puffer
			}
		}	// Steuersequenz wurde gefunden & noch zeichen vorhanden
		bResult=!(*cData<'@');
		if (MaxCount!=0) *++cData=0;
	}	// Antwort erhalten
	return ((bResult) && (MaxCount!=0));
}	// GetFrame

char VT100_c::GetCode(const char * cData)					// Get CMD Code of CSI
{	// GetCode
	while (*cData!=0) cData++;								// skipt to end
	cData--;												// get last character
	return *cData;											
}	// GetCode

uint16_t VT100_c::GetParam(const char* cData,uint8_t Param) // get CSI parameter
{	// GetParam
	uint8_t 	Count=0;
	uint16_t	Temp=0;

	while (	(*cData==ESC) || 								// skip CSI
			(*cData==cVT100_RESET_MODE_ESC) ||
			(*cData==cVT100_APPLICATION_MODE_ESC) ) 
		cData++;		

	while (	(*cData) && 									// search first number
			( (*cData<='0') || 
			  (*cData>='9') ) )
		cData++;		

	if (Param==0)											// #0 -> just count
	{	// count parameters
		Count=0;
		if ((*cData>='0') && (*cData<='9')) Count++;		// digit -> 1st parameter
		while (*cData!=0)									// search til end of string
		{
			if (*cData==';') Count++;						// ';' -> new parameter
			cData++;
		}
		Temp=Count;
	}	// count parameters
	else													// Parameter Isolieren
	{	// extract parameter
		while (--Param>0)									// skip leading parameters
		{
			while ((*cData!=0) && (*cData!=';')) cData++;	//  search for next  ';' 
			if (*cData==';') cData++;
		}

		while ((*cData) && (*cData==' ')) cData++;			// skip spaces
		Temp=0;
		
		while ((*cData) && (*cData>='0') && (*cData<='9')) 	// convert Number
		{
			Temp*=10;
			Temp+=*cData-'0';
			cData++;
		}
	}	// extract parameter

	return Temp;
}	// GetParam


bool VT100_c::StrCmp(const char * StringA, const char * StringB)
{	// StrCmp
	while ((*StringA) && (*StringB) && (*StringA==*StringB))
	{
		StringA++;
		StringB++;
	}

	return (!(*StringA)) || (!(*StringB));
}	// StrCmp

bool VT100_c::StrCmp(const char * StringA,const __FlashStringHelper * StringB)
{
	const char * StringF=(const char*)StringB;
	while ((*StringA) && (pgm_read_byte(StringF)) && (*StringA==pgm_read_byte(StringF)))
	{
		StringA++;
		StringF++;
	}

	return (!(*StringA)) || (!pgm_read_byte(StringF));
}

void VT100_c::processInput(void)
{	// processInput

		
		
		if (available())
		{ // process date
			editKeyData.TimeOut=false;
			editKeyData.Data=true;
			editKeyData.LastRx=TimeStamp;
			editKeyData.cData=read();
			editKeyData.UnknownFlag|= 	editKeyData.NumberFlag && 
									(editKeyData.cData!=cVT100_NUM_CODE) &&
									( (editKeyData.cData<'0') || (editKeyData.cData>'9') );

			if	(!editKeyData.UnknownFlag)
			{ 	// no error yet
				if (editKeyData.EscFlag)
				{ // ESC already processes
					if (editKeyData.ControlFlag)
					{	// CSI or application detected
						switch (editKeyData.cData)
						{	// analyse controll characters
						case cVT100_CURSOR_UP:
							editKeyData.cData=EDIT_CRSR_UP;
							editKeyData.Ok=true;
							break;
						case cVT100_CURSOR_DOWN:
							editKeyData.cData=EDIT_CRSR_DOWN;
							editKeyData.Ok=true;
							break;
						case cVT100_CURSOR_FORWARD:
							editKeyData.cData=EDIT_CRSR_RIGHT;
							editKeyData.Ok=true;
							break;
						case cVT100_CURSOR_BACK:
							editKeyData.cData=EDIT_CRSR_LEFT;
							editKeyData.Ok=true;
							break;
						case cVT100_CURSOR_HOME:
							editKeyData.cData=EDIT_CRSR_HOME;
							editKeyData.Ok=true;
							break;
						case cVT100_CURSOR_END:
							editKeyData.cData=EDIT_CRSR_END;
							editKeyData.Ok=true;
							break;
						case cVT100_APPLICATION_MODE_F1:
							editKeyData.cData=EDIT_F1;
							editKeyData.Ok=true;
							break;
						case cVT100_APPLICATION_MODE_F2:
							editKeyData.cData=EDIT_F2;
							editKeyData.Ok=true;
							break;
						case cVT100_APPLICATION_MODE_F3:
							editKeyData.cData=EDIT_F3;
							editKeyData.Ok=true;
							break;
						case cVT100_APPLICATION_MODE_F4:
							editKeyData.cData=EDIT_F4;
							editKeyData.Ok=true;
							break;
						case cVT100_NUM_CODE:
							switch (editKeyData.Number)
							{ // ESC [ Pn ~ -> evaluate Pn
							case VT100_HOME:	 
								editKeyData.cData=EDIT_CRSR_HOME;
								break;
							case VT100_INS:	
								editKeyData.cData=EDIT_INS;
								break;
							case VT100_DEL:	
								editKeyData.cData=EDIT_DEL;
								break;
							case VT100_END:	
								editKeyData.cData=EDIT_CRSR_END;
								break;
							case VT100_PGUP:
								editKeyData.cData=EDIT_PGUP;
								break;
							case VT100_PGDN:
								editKeyData.cData=EDIT_PGDN;
								break;
							case VT100_F1:	
								editKeyData.cData=EDIT_F1;
								break;
							case VT100_F2:	
								editKeyData.cData=EDIT_F2;
								break;
							case VT100_F3:	
								editKeyData.cData=EDIT_F3;
								break;
							case VT100_F4:	
								editKeyData.cData=EDIT_F4;
								break;
							case VT100_F5:	
								editKeyData.cData=EDIT_F5;
								break;
							case VT100_F6:	
								editKeyData.cData=EDIT_F6;
								break;
							case VT100_F7:	
								editKeyData.cData=EDIT_F7;
								break;
							case VT100_F8:	
								editKeyData.cData=EDIT_F8;
								break;
							case VT100_F9:	
								editKeyData.cData=EDIT_F9;
								break;
							case VT100_F10:	
								editKeyData.cData=EDIT_F10;
								break;
							case VT100_F11:	
								editKeyData.cData=EDIT_F11;
								break;
							case VT100_F12:	
								editKeyData.cData=EDIT_F12;
								break;
							default:
								editKeyData.UnknownFlag=true;
								break; 
							} // ESC [ Pn ~ -> evaluate Pn
							editKeyData.Ok=true;
							break;
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							editKeyData.NumberFlag=true;
							editKeyData.Number=editKeyData.Number*10+editKeyData.cData-'0';
							break;
						default:
							editKeyData.UnknownFlag=true;
							break;
						}	// // analyse controll characters
						editKeyData.Ok&=!editKeyData.UnknownFlag;
					}	// CSI or application detected
					else
					{	// ESC but no control Sequence yet
						editKeyData.ControlFlag|=(editKeyData.cData==cVT100_RESET_MODE_ESC) ||	// CSI
										(editKeyData.cData==cVT100_APPLICATION_MODE_ESC);		// or application mode
						editKeyData.UnknownFlag|=!editKeyData.ControlFlag;						// no CSI -> error
					}	// ESC but no control Sequence yet
				} // ESC already processes
				else
				{ // wait for ESC
					editKeyData.UnknownFlag|=(editKeyData.cData==ESC) && editKeyData.EscFlag; 	// ESC ESC not yet implemented
					editKeyData.EscFlag|=(editKeyData.cData==ESC);							// ESC -> Start of control sequence
				} // wait for ESC
			} 	// no error yet
		} // process data
		else
		{ // check Timeout
			editKeyData.TimeOut=(editKeyData.EscFlag | editKeyData.ControlFlag) && 
								((uint16_t)(TimeStamp-editKeyData.LastRx)>RxTimout);
		} // check Timeout

		if ((editKeyData.Data	&&			// Key Data present
			 !editKeyData.ControlFlag && 	// no Control Sequence yet
			 !editKeyData.EscFlag) || 		// no ESC detected
			editKeyData.TimeOut)			// or timeout occured
		{	// no control sequence detected
			if ((editKeyData.cData<' ') || (editKeyData.cData==DEL))
			{ // is control code
				editKeyData.Ok=true;
				editKeyData.ControlFlag=true;
				switch (editKeyData.cData)
				{	// check control code
				case ESC:
					editKeyData.cData=EDIT_ESC;
					break;
				case CR:
					editKeyData.cData=EDIT_ENTER;
					break;
				case CAN:
					editKeyData.cData=EDIT_CANCEL;
					break;
				case CPM_EOF:
					editKeyData.cData=EDIT_END;
					break;
				case DEL:
					editKeyData.cData=EDIT_DEL;
					break;
				case BS:
					editKeyData.cData=EDIT_BS;
					break;
				default: // invalid or currently not implemented
					editKeyData.Ok=false;
					break;
				}	// check control code
			} // is control code
			else 
			{ // is printable char
				editKeyData.Ok=true;
			} // is printable char
		}	// no control sequence detected

		if (editKeyData.Ok)
		{	// valid character or control (sequence) 
			
			if (((TempBufferIn+2) & (TEMP_BUFFER_SIZE-1)) != TempBufferOut)
			{	//
				TempBuffer[TempBufferIn]=editKeyData.cData;
				TempBufferIn=(TempBufferIn+1) & (TEMP_BUFFER_SIZE-1); 
			}	//
		}	// valid character or control (sequence)
		
		if (editKeyData.Ok || editKeyData.UnknownFlag || editKeyData.TimeOut)
		{	// clear result
			EditKeyReset();
		}  // clear result
}	// processInput

void VT100_c::WaitForInput(void)
{	// WaitForInput
	while (!KeyPressed()) __asm__ __volatile__ ("nop\n\t");			// auf Zeichen warten
}	// WaitForInput

uint8_t VT100_c::readBuffer(void)
{	// read Key from buffer
	uint8_t cTemp=0;

	if (TempBufferIn!=TempBufferOut)
	{
		cTemp=TempBuffer[TempBufferOut];
		TempBufferOut=(TempBufferOut + 1) & (TEMP_BUFFER_SIZE-1);
	}
	
	return cTemp;
}	// read Key from buffer


bool VT100_c::CheckKeyMask(const char* pcKeyMask, uint8_t cKey)
{	// CheckKeyMask
	if (pcKeyMask!=NULL)
	{	// key mask given, test
		while ((*pcKeyMask) && (*pcKeyMask!=cKey))	// scan mask string untill end
			pcKeyMask++;	
		return (*pcKeyMask) && (cKey>=SPACE);
	}	// key mask given, test
	else
		return true;
}	// CheckKeyMask

bool VT100_c::CheckKeyMask_P(const char* pcKeyMask, uint8_t cKey)
{	// CheckKeyMask_P (from Program Memory)
	if (pcKeyMask!=NULL)
	{	// key mask given, test
		while ( (pgm_read_byte(pcKeyMask)) && 
				(pgm_read_byte(pcKeyMask)!=cKey) )
			pcKeyMask++;
		return (pgm_read_byte(pcKeyMask)) && (cKey>=SPACE);
	}	// key mask given, test
	else
		return true;
}	// CheckKeyMask_P (from Program Memory)

bool  VT100_c::CheckControlKeyMask(uint32_t ControlMask, uint8_t cKey)
{	// CheckControkKeyMask
	if (cKey<SPACE)
		return ControlMask & (1ul<<cKey);
	else
		return false;
}	// CheckControkKeyMask



void VT100_c::EditReset(void)
{	// EditReset
	uint8_t n;
	for(n=0;n<EDIT_LEN;n++) 
		editData.Buffer[n]=NUL;
	editData.State=idle;
	editData.ExitMask=0;
	editData.Len=0;
	editData.Pos=0;
	editData.PosX=0;
	editData.PosY=0;
	editData.TermNull=0;
	editData.TrimLeft=0;
	editData.TrimRight=0;
	editData.pCharSet=NULL;
	editData.pInput=NULL;
}	// EditReset

void VT100_c::EditKeyReset(void)
{	// EditKeyReset
	editKeyData.Ok=false;
	editKeyData.EscFlag=false;
	editKeyData.ControlFlag=false;
	editKeyData.UnknownFlag=false;
	editKeyData.NumberFlag=false;
	editKeyData.Data=false;
	editKeyData.TimeOut=false;
	editKeyData.Number=0;
	editKeyData.LastRx=TimeStamp;
}	// EditKeyReset


VT100_c::terminal_options_et VT100_c::DecodeTerminalOptions(const char * buffer)		// try to get terminal caps
{
	terminal_options_et Options=noTerminal;
		
	if ( (GetCode(buffer)==cVT100_DEVICE_ATTRIBUTES) &&
		 StrCmp(buffer,(__FlashStringHelper*)DEVICE_ATTRIBUTES) ) 
	{	// richtige Antwort
		Options=(terminal_options_et)GetParam(buffer,GetParam(buffer,0));
	}	// richtige Antwort
	return Options;
}

