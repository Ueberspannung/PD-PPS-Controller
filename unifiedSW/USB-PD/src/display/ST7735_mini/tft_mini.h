#ifndef __tft_mini_c_h__
#define __tft_mini_c_h__

#include "../../../config.h"
#include "../../LED/cat4004.h"


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>


/*****************************************************
 * Font is (5+1 x 7+1)
 * 			Display		Performace (ms/char)
 * Size 1 26,7 x 10
 * Size 2 13,3 x 5		2,2
 * Size 3  8,9 x 3,3
 * Size 4  6,7 x 2,5
 *
 * Font				Display		Pixels	line  Offset Perf.
 * FreeMono9pt7b 	14,5 x 4,7	 11x16  19    11     1,7 ms/char
 */



// Color definitions
#define _BLACK    		0x0000
#define _DARK_BLUE  	0x0015
#define _DARK_RED		0xA800
#define _DARK_GREEN		0x0540
#define _DARK_CYAN		0x0555
#define _DARK_MAGENTA	0xA815
#define _DARK_YELLOW	0xAD40
#define _DARK_GREY		0x52AA
#define _GREY			0xAD55
#define _BLUE			0x001F
#define _RED			0xF800
#define _GREEN			0x07E0
#define _CYAN			0x07FF
#define _MAGENTA		0xF81F
#define _YELLOW			0xFFE0
#define _WHITE			0xFFFF

class tft_mini_c:cat4004
{
	public:

        /* *****************************************
         *  display class uses 7bit asci charset,
         *  in order to display icons, add 0x80
         *  to Icon number
         */
		static const char ICON_OFFSET=0x80;
        typedef enum:uint8_t {	ICON_SWITCH	=0,		// char 0x80
                                ICON_RAMP,          // char 0x81
                                ICON_FLASH,         // char 0x82
                                ICON_WRENCH,        // char 0x83
                                ICON_MARK,          // char 0x84
                                ICON_PID,           // char 0x85
                                ICON_SUN,           // char 0x86
                                ICON_TRASH,         // char 0x87
                                ICON_CHECK,         // char 0x88
                                ICON_CANCEL,        // char 0x89
                                ICON_SDCARD,		// char 0x8A
                                ICON_LOG,			// char 0x8B
                                ICON_PROG,			// char 0x8C
                                ICON_FILE,			// char 0x8D 
                                ICON_MENU,			// char 0x8E
                                ICON_STEP,			// char 0x8F
                                MAX_ICON_CNT        
                                } icon_et;

        typedef enum:uint8_t { 	BLACK=0,
                                DARK_BLUE,
                                DARK_GREEN,
                                DARK_CYAN,
                                DARK_RED,
                                DARK_MAGENTA,
                                DARK_YELLOW,
                                GREY,
                                DARK_GREY,
                                BLUE,
                                GREEN,
                                CYAN,
                                RED,
                                MAGENTA,
                                YELLOW,
                                WHITE
                                } colour_et;

        typedef enum:uint8_t	{ 	cursor_off,         // no cursor
                                    cursor_block,       // block cursor, inverse char
                                    cursor_frame,       // framed char
                                    cursor_tray,		// half frame cursor
                                    cursor_line         // underlined char
                                } cursor_mode_et;

        typedef enum:uint8_t	{	font_regular,
                                    font_bold,
                                    font_italic,
                                    font_bold_italic,
                                    font_none
                                } font_face_et;

        typedef enum:uint8_t    {   packed,             // dense packing, required size only
                                    spread,             // if available, increased line spacing
                                    status              // if available, additional spcing to last line
                                    } line_spacing_et;


        tft_mini_c(uint8_t brightnessPin=0xff, unsigned char Index=' ');
        void init(uint8_t cols, uint8_t rows, line_spacing_et LineSpacing);
        void process(void);
        bool isRunning(void);

        void putChar(uint8_t column, uint8_t row, const char c);
        void putChar(uint8_t column, uint8_t row, const char * pC);

        void putHex(uint8_t column, uint8_t row, uint8_t size, uint16_t val);


        void Home();
        void Clear();
        void CursorPos(uint8_t x, uint8_t y);
        void CursorMode(cursor_mode_et mode);
        void setColour(colour_et textColour);
        void setColour(colour_et textColour,colour_et backgroundColour);
        void setFontFace(font_face_et fontFace);
        void backlightOn(bool bOn);
        void setBrightness(uint8_t Brightness);
        uint8_t getBrightness(void);

        void progressBarInit(	uint8_t column, uint8_t row, 	// Position of progress bar
                                uint8_t length,					// length of progressbar (1-20)
                                uint8_t progress);				// initial state of Progress Bar

        void progressBarInit(	uint8_t column, uint8_t row, 	// Position of progress bar
                                uint8_t length,					// length of progressbar (1-20)
                                uint8_t progress,				// initial state of Progress Bar
                                colour_et colour);


        void progressSet(uint8_t progress);				// draw new Bar, 0-100

        void progressBarDisable(void);

	private:

        static const uint8_t    maxPixelsX=160;
        static const uint8_t    maxPixelsY=80;
        static const uint8_t 	charWidth=11;	// char width of Font Mono9pt
        static const uint8_t	charHight=16;	// char hight of Font Mono9pt
        static const uint8_t	charOffsetY=11;	// text basline Offset of Mono 9pt
        static const uint8_t	iconSize=((charWidth+7)/8*charHight); // Size of one Bitmap icon to fit font
        static const uint8_t	maxColumns=14;
        static const uint8_t	maxRows=5;


        typedef enum:uint8_t	{	INIT,
                                    CLEAR,
                                    SCAN,
                                    BACKGROUND,
                                    CURSOR,
                                    DRAW
                                }   process_state_et;

        #pragma pack(push,1)
        typedef struct displayBuffer_s {	char 	Char;               // character to draw
											uint8_t	textColour  :4;     // selected foreground colouur
											uint8_t	fontFace    :2;     // selected font face
											uint8_t cursor      :1;     // active cursor here
											uint8_t redraw      :1;     // redrwar, changes mode
											} displayBuffer_st;

        typedef union displayBuffer_u {	uint16_t        val;
										displayBuffer_st data;
										} displayBuffer_ut;

        typedef struct cursor_s {	uint8_t			column;
									uint8_t			row;
									} cursor_st;

        typedef struct process_state_s {uint8_t             rows,columns;       // requested number of rows / columns
										uint8_t             yOffset[maxRows];   // startpos ofd each row
										uint8_t             xOffset;            // x-Pos for centered display
										uint8_t				brightness;			// last requested brightness
										colour_et           textColour;
										colour_et           backgroundColour;
										font_face_et		fontFace;
										font_face_et        lastFont;
										cursor_mode_et      cursorMode;
										cursor_st           cursorPos;
										cursor_st           bufferPos;
										displayBuffer_ut    tempBuffer;
										process_state_et 	state;
										} process_state_st;

		typedef struct progress_s	{	cursor_st	pixPos;		// start pos of Progress Bar in pixesl
										cursor_st	charPos;	// start pos of Progress Bar chars
										uint8_t		pixLength;	// length of Progres Bar in pixels
										uint8_t		charLength;	// length of Progres Bar in chars
										uint8_t		progress;	// current state of Progress Bar
										colour_et	barColour;
									}	progress_st;
        #pragma pack(pop)



        static const uint8_t 	iconData[MAX_ICON_CNT][iconSize];

        static const uint16_t 	colourCode[];


        static const uint8_t 	TFT_CS= 	HWCFG_DISPLAY_CS;
        static const uint8_t 	TFT_RST=	HWCFG_DISPLAY_RESET;
        static const uint8_t 	TFT_DC= 	HWCFG_DISPLAY_DC;

        Adafruit_ST7735 tft{TFT_CS, TFT_DC, TFT_RST};

        #pragma pack(push,1)
        displayBuffer_ut    displayBuffer[maxRows][maxColumns];
        process_state_st    processState;
        progress_st			progressBar;
        #pragma pack(pop)

        void init_data(void);
        void recalc_pos(uint8_t & column, uint8_t & row);
        void selectFont(font_face_et font);
};

#endif //__tft_mini_c_h__
