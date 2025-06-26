#ifndef __VT100_defs__
#define __VT100_defs__

/* *********************************************************************
 * extract from VT100 User Guide (EK-VT1OO-UG-002 / Jan. 1979)
 */

#define cVT100_RESET_ATTRIBUTES	'0'
#define cVT100_BRIGHT			'1'
#define cVT100_DIM 				'2'
#define cVT100_UNDERSCORE		'4'
#define cVT100_BLINK			'5'
#define cVT100_REVERSE			'7'
#define cVT100_HIDDEN			'8'

#define VT100_CHAR_SIZE_NORMAL					0	
#define VT100_CHAR_SIZE_DOUBLE_WIDTH			1	
#define VT100_CHAR_SIZE_DOUBLE_HIGHT_UPPER_LINE	2	
#define VT100_CHAR_SIZE_DOUBLE_HIGHT_LOWER_LINE	3	

#define VT100_ESC_RESET_MODE			"\x1b["			//
#define VT100_ESC_APPLICATION_MODE		"\x1bO"			// 
#define VT100_QUERY_DEVICE_ATTRIBUTES	"\x1b[c"		// QDA: ESC [ Pn c -> RDA	
#define VT100_DECID						"\x1bZ"			// DECID: ESC Z same as QDA (private)
#define VT100_DEVICE_ATTRIBUTES			"\x1b[?"		// RDA: ESC [ ? 1; Pn c
#define VT100_RESET_TERMINAL 			"\x1b\x63"		// RIS: ESC c
#define VT100_CURSOR_UP					"\x1b[A"		// CUU: ESC [ A (Cursor Key)
#define VT100_CURSOR_DOWN				"\x1b[B"		// CUD: ESC [ B (Cursor Key)
#define VT100_CURSOR_FORWARD			"\x1b[C"		// CUF: ESC [ C (Cursor Key)
#define VT100_CURSOR_BACK				"\x1b[D"		// CUB: ESC [ D (Cursor Key)
#define VT100_CURSOR_HOME				"\x1b[H"		// CUP: ESC [ H (Cursor Key)
#define VT100_ERASE_END_OF_LINE			"\x1b[K"		// EL : ESC [Ps K  (erase to end of line / right)
#define VT100_ERASE_START_OF_LINE		"\x1b[1K"		// EL : ESC [Ps K  (erase to start of line / left) 
#define VT100_ERASE_LINE				"\x1b[2K"		// EL : ESC [Ps K  (erase line)
#define VT100_ERASE_DOWN				"\x1b[J"		// ED : ESC [ Pn J (erase to end of display / down)
#define VT100_ERASE_UP					"\x1b[1J"		// ED : ESC [ Pn J (erase to top of display / up)
#define VT100_ERASE_SCREEN				"\x1b[2J"		// ED : ESC [ Pn J (Display)
#define VT100_QUERY_CURSOR_POS			"\x1b[6n"		// DSR: ESC [ Pn n (6 = CPR)
#define VT100_SAVE_CURSOR_ATTRIBUTES	"\x1b\x37"		// DECSC: ESC 7 (private, alt. ESC [ s)
#define VT100_RESTORE_CURSOR_ATTRIBUTES	"\x1b\x38"		// DECRC: ESC 8 (private, alt. ESC [ u)
#define VT100_DOUBLE_HIGHT_UPPER_LINE	"\x1b#3"		// DECDHL_U: ESC # 3
#define VT100_DOUBLE_HIGHT_LOWER_LINE	"\x1b#4"		// DECDHL_L: ESC # 4
#define VT100_DOUBLE_WIDTH				"\x1b#6"		// DECDWL  : ESC # 6
#define VT100_DOUBLE_OFF				"\x1b#5"		// DECSWL  : ESC # 5
#define VT100_SCREEN_ALIGNMENT_DIPLAY	"\x1b#8"		// DECALN  : ESC # 8

#define VT100_KEY_UNLOCK				"\x1b[2l"
#define VT100_KEY_LOCK					"\x1b[2h"
#define VT100_OVERWRITE					"\x1b[4l"
#define VT100_INSERT					"\x1b[4h"
#define VT100_AUTO_LF_OFF				"\x1b[20l"
#define VT100_AUTO_LF_ON				"\x1b[20h"
#define VT100_APP_MODE_OFF				"\x1b[?1l"
#define VT100_APP_MODE_ON				"\x1b[?1h"
#define VT100_MODE_VT52					"\x1b[?2l"
#define VT100_MODE_VT100				"\x1b[22h"
#define VT100_COLUMN80					"\x1b[?3l"
#define VT100_COLUMN132					"\x1b[?3h"
#define VT100_LINE_SCROLL				"\x1b[?4l"
#define VT100_SOFT_SCROLL				"\x1b[?4h"
#define VT100_NORMAL_SCRREN				"\x1b[?5l"
#define VT100_REVERSE_SCREEN			"\x1b[?5h"
#define VT100_CURSOR_ABSOLUTE			"\x1b[?6l"
#define VT100_CURSOR_RELATIVE			"\x1b[?6h"
#define VT100_LINE_WRAP_OFF				"\x1b[?7l"
#define VT100_LINE_WRAP_ON				"\x1b[?7h"
#define VT100_AUTO_REPEAT_OFF			"\x1b[?8l"
#define VT100_AUTO_REPEAT_ON			"\x1b[?8h"
#define VT100_PROGRESSIVE_SCAN			"\x1b[?9l"
#define VT100_INTERLACED				"\x1b[?9h"
#define VT100_HIDE_CURSOR				"\x1b[?25l"
#define VT100_SHOW_CURSOR				"\x1b[?25h"
#define VT100_NORMAL_CHARSET			"\x1b\x32"
#define VT100_GRAPHICS_CHARSET			"\x1b\x31"
#define VT100_NUMBERIC_KEY_PAD			"\x1b>"			// DECKPNM; ESC > (Keypad Numeric Mode, private)
#define VT100_ALTERATE_KEY_PAD			"\x1b="			// DECKPAM: ESC = (Keypad Application Mode, private) 
#define VT100_CHARSET_G1				"\x0e"
#define VT100_CHARSET_G0				"\x0f"



/* *********************************************************************
* last letter of several control codes
*/
#define cVT100_CURSOR_UP			'A'		// CUU: ESC [Pn A  ( bidirectional )
#define cVT100_CURSOR_DOWN			'B'		// CUD: ESC [Pn B  ( bidirectional )
#define cVT100_CURSOR_FORWARD		'C'		// CUF: ESC [Pn C  ( bidirectional )
#define cVT100_CURSOR_BACK			'D'		// CUB: ESC [Pn D  ( bidirectional )
#define cVT100_CURSOR_HOME			'H'		// CUP: ESC [Pn; PnH (->HVP)
#define cVT100_CURSOR_HOME_ANSI		'J'		// ED : ESC [Ps J  ( erase 0: to end of disply, 1: to top of display, 2: clear screem)
#define cVT100_CURSOR_END			'K'		// EL : ESC [Ps K  ( erase 0: to end of line, 1: to start of line, 2: complete line) 
#define cVT100_CURSOR_REPORT		'R'		// CPR: ESC [Pn; Pn R ( antwort auf DSR )
#define cVT100_DEVICE_ATTRIBUTES	'c'		// RDA: ESC [?1; Pn c ( anwort auf QDA  )
#define CVT100_HORIZONTAL_VERTICAL	'f'		// HVP: ESC [Pn; Pn f ( Harizontal / Vertical Cursour position ->CUP )
#define cVT100_SET_ATTRIBUTES		'm'		// SGR: ESC [ Ps; ... ; Ps m ( 0:off  1:bold  4:underscore 5:blink 7:inverse)
#define CVT100_LOAD_LEDS			'q'		// DECLL: ESC [ Ps q ( switch LED, private)
#define cVT100_SCROLL_AREA			'r'		// DECSTBM: ESC [ Pn; Pn; r ( private )
#define cVT100_FORGROUND			'3'
#define cVT100_BACKGROUND			'4'
#define cVT100_SWITCH_ON			'h'
#define cVT100_SWITCH_OFF			'l'
#define cVT100_RESET_MODE_ESC		'['
#define cVT100_APPLICATION_MODE_ESC	'O'
#define cVT100_NUM_CODE				'~'

#define cVT100_APPLICATION_MODE_F1	'P'
#define cVT100_APPLICATION_MODE_F2	'Q'
#define cVT100_APPLICATION_MODE_F3	'R'
#define cVT100_APPLICATION_MODE_F4	'S'

// numerical code Sequences ending with '~'	
// Keyboard codes ESC [ Pn ~
#define VT100_HOME					(1)
#define VT100_INS					(2)
#define VT100_DEL					(3)
#define VT100_END					(4)
#define VT100_PGUP					(5)
#define VT100_PGDN					(6)
#define VT100_F1					(11)
#define VT100_F2					(12)
#define VT100_F3					(13)
#define VT100_F4					(14)
#define VT100_F5					(15)
#define VT100_F6					(17)
#define VT100_F7					(18)
#define VT100_F8					(19)
#define VT100_F9					(20)
#define VT100_F10					(21)
#define VT100_F11					(23)
#define VT100_F12					(24)


#endif
