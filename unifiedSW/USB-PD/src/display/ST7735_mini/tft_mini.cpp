#include "tft_mini.h"
#include "../../ASCII/ASCII_ctrl.h"

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

const uint8_t tft_mini_c::iconData[MAX_ICON_CNT][iconSize] = {	{	// Switch Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000100, 0b00000000,		//  1
																	0b00001110, 0b00000000,		//  2
																	0b00001110, 0b00000000,		//  3
																	0b00001110, 0b00000000,		//  4
																	0b00101110, 0b10000000,		//  5
																	0b01101110, 0b11000000,		//  6
																	0b11001110, 0b01100000,		//  7
																	0b11001110, 0b01100000,		//  8
																	0b11000100, 0b01100000,		//  9
																	0b11000000, 0b01100000,		// 10
																	0b11000000, 0b01100000,		// 11
																	0b01100000, 0b11000000,		// 12
																	0b00110001, 0b10000000,		// 13
																	0b00011111, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// Ramp Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000000, 0b00000000,		//  1
																	0b00000000, 0b00000000,		//  2
																	0b00000000, 0b01000000,		//  3
																	0b00000000, 0b11000000,		//  4
																	0b00000001, 0b11000000,		//  5
																	0b00000011, 0b11000000,		//  6
																	0b00000111, 0b11000000,		//  7
																	0b00001111, 0b11000000,		//  8
																	0b00011111, 0b11000000,		//  9
																	0b00111111, 0b11000000,		// 10
																	0b01111111, 0b11000000,		// 11
																	0b11111111, 0b11000000,		// 12
																	0b00000000, 0b00000000,		// 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// Flash Icon
																	0b00000000, 0b11000000,		//  0
																	0b00000001, 0b10000000,		//  1
																	0b00000011, 0b00000000,		//  2
																	0b00000110, 0b00000000,		//  3
																	0b00001100, 0b00000000,		//  4
																	0b00011000, 0b00000000,		//  5
																	0b00110001, 0b11000000,		//  6
																	0b01111111, 0b10000000,		//  7
																	0b11111111, 0b00000000,		//  8
																	0b11100110, 0b00000000,		//  9
																	0b00001100, 0b00000000,		// 10
																	0b00011000, 0b00000000,		// 11
																	0b00110000, 0b00000000,		// 12
																	0b01100000, 0b00000000,		// 13
																	0b11000000, 0b00000000,		// 14
																	0b10000000, 0b00000000},	// 15
																{	// Wrench Icon
																	0b01000000, 0b10000000,		//  0
																	0b11000000, 0b11000000,		//  1
																	0b11000000, 0b11000000,		//  2
																	0b11000000, 0b11000000,		//  3
																	0b11110011, 0b11000000,		//  4
																	0b01111111, 0b10000000,		//  5
																	0b00011110, 0b00000000,		//  6
																	0b00011110, 0b00000000,		//  7
																	0b00011110, 0b00000000,		//  8
																	0b00011110, 0b00000000,		//  9
																	0b00011110, 0b00000000,		// 10
																	0b00011110, 0b00000000,		// 11
																	0b00011110, 0b00000000,		// 12
																	0b00011110, 0b00000000,		// 13
																	0b00011110, 0b00000000,		// 14
																	0b00001100, 0b00000000},	// 15
																{	// Mark Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000100, 0b00000000,		//  1
																	0b00000100, 0b00000000,		//  2
																	0b00001110, 0b00000000,		//  3
																	0b00010101, 0b00000000,		//  4
																	0b00100000, 0b10000000,		//  5
																	0b01000000, 0b01000000,		//  6
																	0b11100100, 0b11100000,		//  7
																	0b01000000, 0b01000000,		//  8
																	0b00100000, 0b10000000,		//  9
																	0b00010101, 0b00000000,		// 10
																	0b00001110, 0b00000000,		// 11
																	0b00000100, 0b00000000,		// 12
																	0b00000100, 0b00000000,		// 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// PID Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000000, 0b00000000,		//  1
																	0b10000000, 0b00000000,		//  2
																	0b10000000, 0b00000000,		//  3
																	0b10000000, 0b01000000,		//  4
																	0b11110000, 0b10000000,		//  5
																	0b10010001, 0b00000000,		//  6
																	0b10010010, 0b00000000,		//  7
																	0b10010100, 0b00000000,		//  8
																	0b10011000, 0b00000000,		//  9
																	0b10000000, 0b00000000,		// 10
																	0b10000000, 0b00000000,		// 11
																	0b10000000, 0b00000000,		// 12
																	0b11111111, 0b11000000,		// 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 18
																{	// SUN Icom
																	0b00000000, 0b00000000,		//  0
																	0b00000100, 0b00000000,		//  1
																	0b00000100, 0b00000000,		//  2
																	0b01000000, 0b01000000,		//  3
																	0b00100000, 0b10000000,		//  4
																	0b00001110, 0b00000000,		//  5
																	0b00010001, 0b00000000,		//  6
																	0b10010001, 0b00100000,		//  7
																	0b00010001, 0b00000000,		//  8
																	0b00001110, 0b00000000,		//  9
																	0b00100000, 0b10000000,		// 10
																	0b01000000, 0b01000000,		// 11
																	0b00000100, 0b00000000,		// 12
																	0b00000100, 0b00000000,     // 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// Trash Icon
																	0b00111111, 0b00000000,		//  0
																	0b00111111, 0b00000000,		//  1
																	0b11111111, 0b11000000,		//  2
																	0b00000000, 0b00000000,		//  3
																	0b00000000, 0b00000000,		//  4
																	0b11111111, 0b11000000,		//  5
																	0b10000000, 0b01000000,		//  6
																	0b10010010, 0b01000000,		//  7
																	0b10010010, 0b01000000,		//  8
																	0b10010010, 0b01000000,		//  9
																	0b10010010, 0b01000000,		// 10
																	0b10010010, 0b01000000,		// 11
																	0b10010010, 0b01000000,		// 12
																	0b11000000, 0b11000000,		// 13
																	0b01100001, 0b10000000,		// 14
																	0b00111111, 0b00000000},	// 15
																{	// Check Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000000, 0b00000000,		//  1
																	0b00000000, 0b00000000,		//  2
																	0b00000000, 0b00000000,		//  3
																	0b00000000, 0b00000000,		//  4
																	0b00000000, 0b01000000,		//  5
																	0b00000000, 0b11000000,		//  6
																	0b00000001, 0b10000000,		//  7
																	0b10000011, 0b00000000,		//  8
																	0b11000110, 0b00000000,		//  9
																	0b01101100, 0b00000000,		// 10
																	0b00111000, 0b00000000,		// 11
																	0b00010000, 0b00000000,		// 12
																	0b00000000, 0b00000000,		// 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// Cancel Icon
																	0b00000000, 0b00000000,		//  0
																	0b00000000, 0b00000000,		//  1
																	0b10000000, 0b01000000,		//  2
																	0b11000000, 0b11000000,		//  3
																	0b01100001, 0b10000000,		//  4
																	0b00110011, 0b00000000,		//  5
																	0b00011110, 0b00000000,		//  6
																	0b00001100, 0b00000000,		//  7
																	0b00011110, 0b00000000,		//  8
																	0b00110011, 0b00000000,		//  9
																	0b01100001, 0b10000000,		// 10
																	0b11000000, 0b11000000,		// 11
																	0b10000000, 0b01000000,		// 12
																	0b00000000, 0b00000000,		// 13
																	0b00000000, 0b00000000,		// 14
																	0b00000000, 0b00000000},	// 15
																{	// SD-Card Icon
																	0b00011111, 0b11100000,		//  0
																	0b00100000, 0b00100000,		//  1
																	0b01001010, 0b10100000,		//  2
																	0b10001010, 0b10100000,		//  3
																	0b10101010, 0b10100000,		//  4
																	0b10100000, 0b00100000,		//  5
																	0b10100000, 0b00100000,		//  6
																	0b10000000, 0b00100000,		//  7
																	0b10011011, 0b00100000,		//  8
																	0b10100010, 0b10100000,		//  9
																	0b10010010, 0b10100000,		// 10
																	0b10001010, 0b10100000,		// 11
																	0b10110011, 0b00100000,		// 12
																	0b10000000, 0b00100000,		// 13
																	0b11111111, 0b11100000,		// 14
																	0b00000000, 0b00000000}	};	// 15

const uint16_t tft_mini_c::colourCode[]={ 	_BLACK,
										_DARK_BLUE,
										_DARK_GREEN,
										_DARK_CYAN,
										_DARK_RED,
										_DARK_MAGENTA,
										_DARK_YELLOW,
										_GREY,
										_DARK_GREY,
										_BLUE,
										_GREEN,
										_CYAN,
										_RED,
										_MAGENTA,
										_YELLOW,
										_WHITE
										};



/********************************************************/
/*		PUBLIC functions								*/
/********************************************************/
tft_mini_c::tft_mini_c(uint8_t brightnessPin, unsigned char Index):cat4004{brightnessPin,Index}
{	// display
	init_data();
    Clear();

}	// lcd

void tft_mini_c::init(uint8_t cols, uint8_t rows, line_spacing_et LineSpacing)
{	// init

    if (cols>=maxColumns) cols=maxColumns;
    if (rows>=maxRows)  rows=maxRows;
	processState.columns=cols;
	processState.rows	=rows;
	switch (LineSpacing)
	{	// switch LineSpancing
		case packed:
			for (uint8_t n=0; n<rows; n++)
				processState.yOffset[n]=n*charHight;
			break;
		case spread:
			for (uint8_t n=0; n<rows; n++)
				processState.yOffset[n]=n*(maxPixelsY/rows)+
										((maxPixelsY/rows)-charHight)/2;
			break;
		case status:
			for (uint8_t n=0; n<rows; n++)
				processState.yOffset[n]=n*charHight;
			processState.yOffset[rows-1]=maxPixelsY-charHight;
			break;
	}	// switch LineSpancing
	processState.xOffset=(maxPixelsX-cols*charWidth)/2;
}	// init

void tft_mini_c::process(void)
{	// process

	cat4004::process();

	switch (processState.state)
	{	// switch (ProcessState.LcdControlState)
		case INIT:	        // setup
            set_brightness(50);                  // brightness
            tft.initR(INITR_MINI160x80_PLUGIN);  // Init ST7735S mini display
            tft.setRotation(3);                     //  oriientation
            processState.state=CLEAR;
            break;
		case CLEAR:	        // initial clear
		    tft.fillScreen(processState.backgroundColour);
		    processState.state=SCAN;
			break;
		case SCAN:	        // scan buffer for modifications
		    do
            {   // scan complete buffer until redraw
                processState.tempBuffer.val=displayBuffer[processState.bufferPos.row][processState.bufferPos.column].val;
                if (processState.tempBuffer.data.redraw)
                {   // changes have been made, redraw
                    processState.state=BACKGROUND;
                }   // changes have been made, redraw
                else
                {   // no changes, next
                    processState.bufferPos.column++;
                    recalc_pos(processState.bufferPos.column,processState.bufferPos.row);
                }   // no changes, next
            }   // scan complete buffer until redraw
            while (processState.state==SCAN && (processState.bufferPos.column || processState.bufferPos.row));
			break;
		case BACKGROUND:    // draw background
            if (!processState.tempBuffer.data.cursor || processState.cursorMode!=cursor_block)
            {   // erase with background color
				tft.fillRect(	processState.xOffset+processState.bufferPos.column*charWidth,
								processState.yOffset[processState.bufferPos.row],
								charWidth,
								charHight,
								colourCode[(uint8_t)processState.backgroundColour]);
				processState.state=(processState.tempBuffer.data.cursor)?(CURSOR):(DRAW);
            }   // erase with background color
            else
            {   // inverse, fill with textcolor
				tft.fillRect(	processState.xOffset+processState.bufferPos.column*charWidth,
								processState.yOffset[processState.bufferPos.row],
								charWidth,
								charHight,
								colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
                processState.state=DRAW;
            }   // inverse, fill with textcolor
			break;
		case CURSOR:		// draw cursor frame / line
		    switch (processState.cursorMode)
		    {   // draw box or line
                case cursor_frame:
					tft.drawRect(	processState.xOffset+processState.bufferPos.column*charWidth,
									processState.yOffset[processState.bufferPos.row],
									charWidth,
									charHight,
									colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
                    break;
                case cursor_tray:
					tft.drawFastVLine(	processState.xOffset+(processState.bufferPos.column+1)*charWidth-1,
										processState.yOffset[processState.bufferPos.row]+charHight-(charWidth>>1),
										charWidth>>1,
										colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
					tft.drawFastVLine(	processState.xOffset+processState.bufferPos.column*charWidth,
										processState.yOffset[processState.bufferPos.row]+charHight-(charWidth>>1),
										charWidth>>1,
										colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
					tft.drawFastHLine(	processState.xOffset+processState.bufferPos.column*charWidth,
										processState.yOffset[processState.bufferPos.row]+charHight-1,
										charWidth,
										colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
                    break;
                case cursor_line:
					tft.drawFastHLine(	processState.xOffset+processState.bufferPos.column*charWidth,
										processState.yOffset[processState.bufferPos.row]+charHight-1,
										charWidth,
										colourCode[(uint8_t)processState.tempBuffer.data.textColour]);
                    break;
                default:
                    break;
		    }   // draw box or line
            processState.state=DRAW;
			break;
		case DRAW:			// draw char
			uint8_t textColour, backgroundColour;

			if (processState.tempBuffer.data.cursor && (processState.cursorMode==cursor_block))
			{	// block cursor use background colour
				textColour=processState.backgroundColour;
				backgroundColour=processState.tempBuffer.data.textColour;
			}	// block cursor use background colour
			else
			{	// Use text colour
				textColour=processState.tempBuffer.data.textColour;
				backgroundColour=processState.backgroundColour;
			}	// Use text colour

			if (processState.tempBuffer.data.Char & 0x80)
			{	// icon -> draw bitmap
				tft.drawBitmap(	processState.xOffset+processState.bufferPos.column*charWidth,
								processState.yOffset[processState.bufferPos.row],
								iconData[processState.tempBuffer.data.Char & ~ICON_OFFSET],
								charWidth,
								charHight,
								colourCode[textColour]);
			}	// icon -> draw bitmap
			else
			{	// text -> draw char
                selectFont((font_face_et)processState.tempBuffer.data.fontFace);
				tft.drawChar(	processState.xOffset+processState.bufferPos.column*charWidth,
								processState.yOffset[processState.bufferPos.row]+charOffsetY,
								processState.tempBuffer.data.Char,
								colourCode[textColour],
								colourCode[backgroundColour],
								1);
			}	// text -> draw char
			if (processState.tempBuffer.val==
				displayBuffer[processState.bufferPos.row][processState.bufferPos.column].val)
			{	// no changes , reset
				processState.tempBuffer.data.redraw=false;
				displayBuffer[processState.bufferPos.row][processState.bufferPos.column].val
					=processState.tempBuffer.val;
			}	// no changes , reset
            processState.state=SCAN;
		    break;
		default:
			break;
	}	// switch (ProcessState.LcdControlState)
}	// process

void tft_mini_c::putChar(uint8_t column, uint8_t row, const char c)
{	// putchar (char)
	displayBuffer_ut temp;

	recalc_pos(column, row);
	temp.val=displayBuffer[row][column].val;

	temp.data.Char=c;
	temp.data.textColour=processState.textColour;
	temp.data.fontFace=processState.fontFace;

	if (temp.val!=displayBuffer[row][column].val)
	{	// displayBuffer modified, redraw
		temp.data.redraw=true;
		displayBuffer[row][column].val=temp.val;
	}	// displayBuffer modified, redraw
}	// putchar (char)

void tft_mini_c::putChar(uint8_t column, uint8_t row, const char * c)
{	// putChar (char*)
	while(*c)
	{
		recalc_pos(column,row);
		putChar(column,row,*c);
		column++;
		c++;
	}
}	// putChar (char*)



void tft_mini_c::putHex(uint8_t column, uint8_t row, uint8_t size, uint16_t val)
{	// putHex
	while (size)
	{
		size--;
		if ((val & 0x0F) < 10)
			putChar(column+size,row,'0'+(val & 0x0F));
		else
			putChar(column+size,row,'A'-10+(val & 0x0F));
		val>>=4;
	}
}	// putHex

void tft_mini_c::Home()
{	// Cursor Home = Set Cursor to DDRAM 0,0
	CursorPos(0,0);
}	// Cursor Home = Set Cursor to DDRAM 0,0


void tft_mini_c::Clear(void)
{	// Clear Clear Buffer and perform Clear CMD
    uint8_t row,column;
	for (row=0;row<maxRows;row++)
        for(column=0;column<maxColumns;column++)
	{
		displayBuffer[row][column].data.Char=SPACE;
		displayBuffer[row][column].data.textColour=processState.textColour;
		displayBuffer[row][column].data.fontFace=processState.fontFace;
        displayBuffer[row][column].data.redraw=true;
	}
	CursorPos(0,0);
}	// Clear

void tft_mini_c::CursorPos(uint8_t column, uint8_t row)
{	// CursorPos
	recalc_pos(column,row);

	if ((processState.cursorPos.column!=column) ||
		(processState.cursorPos.row!=row))
	{	// cursor pos has change

		displayBuffer_ut temp;

		// clear last cursor
		temp.val=displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val;
		temp.data.cursor=false;
		if (temp.val!=displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val)
		{	// displaybuffer modified, redraw
			temp.data.redraw=true;
			displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val=temp.val;
		}	// displaybuffer modified, redraw
		// set new cursor
		temp.val=displayBuffer[row][column].val;
		temp.data.cursor=(processState.cursorMode!=cursor_off);
		if (temp.val!=displayBuffer[row][column].val)
		{	// displaybuffer modified, redraw
			temp.data.redraw=true;
			displayBuffer[row][column].val=temp.val;
		}	// displaybuffer modified, redraw
		processState.cursorPos.column=column;
		processState.cursorPos.row=row;
	}	// cursor pos has changed
}	// CursorPos

void tft_mini_c::CursorMode(cursor_mode_et mode)
{	// CursorOn
	if (mode!=processState.cursorMode)
	{	// mode changed
		displayBuffer_ut temp;

		temp.val=displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val;
		temp.data.cursor=mode!=cursor_off;
		if ((temp.val!=displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val) ||
			(processState.cursorMode!=mode))
		{	// displaybuffer modified, redraw
			temp.data.redraw=true;
			displayBuffer[processState.cursorPos.row][processState.cursorPos.column].val=temp.val;
		}	// displaybuffer modified, redraw

		processState.cursorMode=mode;
	}	// mode changed
}	// CursorOn

void tft_mini_c::setColour(colour_et textColour)
{	// new textColour
	processState.textColour=textColour;
}	// new textColour

void tft_mini_c::setColour(colour_et textColour,colour_et backgroundColour)
{	// text and background
	processState.textColour=textColour;
	processState.backgroundColour=backgroundColour;
}	// text and background

void tft_mini_c::setFontFace(font_face_et fontFace)
{	// setFontFace
	processState.fontFace=fontFace;
}	// setFontFace

void tft_mini_c::backlightOn(bool bOn)
{	// backlight on / Off
	if (bOn)
		cat4004::set_brightness(processState.brightness);
	else
		cat4004::set_brightness(0);
}	// backlight on / Off

void tft_mini_c::setBrightness(uint8_t Brightness)
{	// SetBrightness
    cat4004::set_brightness(Brightness);
    processState.brightness=cat4004::get_brightness();
}	// SetBrightness

uint8_t tft_mini_c::getBrightness(void)
{	// SetBrightness
	return cat4004::get_brightness();
}	// SetBright

void tft_mini_c::progressBarInit(uint8_t column, uint8_t row, 	// Position of progress bar
								uint8_t length,					// length of progressbar (1-20) (0=disable)
								uint8_t progress)				// initial state of Progress Bar
{	// ProgressBarInit
	progressBarInit(column,row,length,progress,processState.textColour);
}	// ProgressBarInit

void tft_mini_c::progressBarInit(uint8_t column, uint8_t row, 	// Position of progress bar
                                uint8_t length,					// length of progressbar (1-20)
                                uint8_t progress,				// initial state of Progress Bar
                                colour_et colour)
{	// ProgressBarInit
	if (length!=0)
	{	// Init Progress Bar
		recalc_pos(column,row);
		progressBar.pixLength=		(length-1)*charWidth;
		progressBar.pixPos.column=	column*charWidth +
									processState.xOffset +
									(charWidth>>1);
		progressBar.pixPos.row=		processState.yOffset[row] +
									(charHight>>2);
		progressBar.barColour=		colour;
		progressBar.progress=255;

		progressBar.charPos.column=	column;
		progressBar.charPos.row=	row;
		progressBar.charLength=		length;

		// prevent redraw
		for (uint8_t n=0; n<progressBar.charLength;n++)
			displayBuffer[progressBar.charPos.row][progressBar.charPos.column+n].data.redraw=false;

		// clear area
		tft.fillRect(	progressBar.pixPos.column-(charWidth>>1),
						progressBar.pixPos.row-(charHight>>2),
						progressBar.pixLength+charWidth,
						charHight,
						colourCode[(uint8_t)processState.backgroundColour]);
		progressSet(progress);
	}	// Init Progress Bar
}	// ProgressBarInit

void tft_mini_c::progressSet(uint8_t progress)				// draw new Bar, 0-100
{
    if (progress>100) progress = 100;
    if (progressBar.progress!=progress)
    {   // change, redraw
        uint16_t l;
        progressBar.progress=progress;
        l=progressBar.pixLength-2;
        l*=progress;
        l/=100;
        // outer frame
        tft.drawRect(	progressBar.pixPos.column,progressBar.pixPos.row,
						progressBar.pixLength,charHight>>1,
						colourCode[(uint8_t)progressBar.barColour]);
		// filled area
        tft.fillRect(	progressBar.pixPos.column+1,progressBar.pixPos.row+1,
						l,(charHight>>1)-2,
						colourCode[(uint8_t)progressBar.barColour]);
		// unfilled area
        tft.fillRect(	progressBar.pixPos.column+1+l,progressBar.pixPos.row+1,
						progressBar.pixLength-2-l,(charHight>>1)-2,
						colourCode[(uint8_t)processState.backgroundColour]);
    }   // change, redraw
}

void tft_mini_c::progressBarDisable(void)
{
	for (uint8_t n=0; n<progressBar.charLength;n++)
		displayBuffer[progressBar.charPos.row][progressBar.charPos.column+n].data.redraw=true;
}

/********************************************************/
/*		private functions								*/
/********************************************************/
void tft_mini_c::init_data(void)
{	// init data
    processState.rows   			=maxRows;
    processState.columns			=maxColumns;
    for (uint8_t n=0; n<maxRows;n++)
        processState.yOffset[n]		=n*charHight;              		// startpos ofd each row
    processState.xOffset			=(maxPixelsX-maxColumns*charWidth)/2;	// x-Pos for centered display

	processState.brightness			=50;
    processState.textColour			=WHITE;
    processState.backgroundColour	=BLACK;
    processState.cursorMode			=cursor_off;
    processState.fontFace			=font_regular;
    processState.lastFont           =font_none;

    processState.cursorPos.column	=0;
    processState.cursorPos.row		=0;
    processState.bufferPos.column	=0;
    processState.bufferPos.row		=0;
    processState.state=INIT;

	progressBar.pixPos.column 	=0;
	progressBar.pixPos.row 		=0;
	progressBar.pixLength		=0;
	progressBar.charPos.column 	=0;
	progressBar.charPos.row 	=0;
	progressBar.charLength		=0;
	progressBar.barColour		=WHITE;
	progressBar.progress		=0;
}	// init data

void tft_mini_c::recalc_pos(uint8_t & column, uint8_t & row)
{	// recalc_pos
	if (column>=processState.columns)
	{	// correct columns
		// increment rows
		row=row+column/processState.columns;
		// correct columns
		column=column%processState.columns;
	}	// correct columns
	if (row>=processState.rows)
	{	// correct rows
		row=row%processState.rows;
	}	// correct rows
}	// recalc_pos


void tft_mini_c::selectFont(font_face_et font)
{   // selectFont
    if (processState.lastFont!=font)
    {   // new fontFace
        processState.lastFont=font;
        switch (font)
        {
            case font_regular:
                tft.setFont(&FreeMono9pt7b);
                break;
            case font_bold:
                tft.setFont(&FreeMonoBold9pt7b);
                break;
            case font_italic:
                tft.setFont(&FreeMonoOblique9pt7b);
                break;
            case font_bold_italic:
                tft.setFont(&FreeMonoBoldOblique9pt7b);
                break;
            default:
                break;
        }
    }   // new fontFace
}   // selectFont
