/* ========================================
 * Copyright (c) 2015, E2ForLife
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */
#include <cytypes.h>
#include <cylib.h>
#include <stdio.h>
#include <string.h>

#include "`$INSTANCE_NAME`.h"

#include "`$INSTANCE_NAME`_CS.h"
#include "`$INSTANCE_NAME`_RST.h"
#include "`$INSTANCE_NAME`_DC.h"
#include "`$INSTANCE_NAME`_SCK.h"
#include "`$INSTANCE_NAME`_SDO.h"


int `$INSTANCE_NAME`_ProcessString( const char *buffer, char *argv );
int `$INSTANCE_NAME`_ProcessEscapeSequence( const char *str);

extern const uint8 `$INSTANCE_NAME`_font[];

uint8 `$INSTANCE_NAME`_initVar;

uint8  `$INSTANCE_NAME`_FrameBuffer[ `$INSTANCE_NAME`_LCDHEIGHT * `$INSTANCE_NAME`_LCDWIDTH / 8];
uint16 `$INSTANCE_NAME`_CursorX;
uint16 `$INSTANCE_NAME`_CursorY;

const uint8 `$INSTANCE_NAME`_Initialization[] = 
{
	`$INSTANCE_NAME`_CMD_DISPLAYALLOFF,
	`$INSTANCE_NAME`_CMD_SETCLOCK,               0xF1,
	`$INSTANCE_NAME`_CMD_SETMULTIPLEX,           0x3F,
	`$INSTANCE_NAME`_CMD_SETOFFSET,              0x4C, 
	`$INSTANCE_NAME`_CMD_SETSTARTLINE,           0x00,
	`$INSTANCE_NAME`_CMD_MASTERCONFIG,           0x02, /* ADAFruit usees 0x02 */
	`$INSTANCE_NAME`_CMD_SETREMAP,               0x56,
	`$INSTANCE_NAME`_CMD_SETCURRENT + 2, 
	`$INSTANCE_NAME`_CMD_SETGRAYTABLE,           0x01, 0x11, 0x22, 0x32, 0x43, 0x54, 0x65, 0x76,
	`$INSTANCE_NAME`_CMD_SETCONTRAST,            0x7F,
	`$INSTANCE_NAME`_CMD_SETROWPERIOD,           0x51,
	`$INSTANCE_NAME`_CMD_SETPHASELEN,            0x55,
	`$INSTANCE_NAME`_CMD_SETPRECHARGECOMP,       0x02,
	`$INSTANCE_NAME`_CMD_SETPRECHARGECOMPENABLE, 0x28,
	`$INSTANCE_NAME`_CMD_SETVCOMLEVEL,           0x1C,
	`$INSTANCE_NAME`_CMD_SETVSL,                 0x0D | 0x02,
	`$INSTANCE_NAME`_CMD_NORMALDISPLAY
};
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Send( uint8 value )
{
	uint8 cnt;
	
	for( cnt=0x80;cnt;cnt >>= 1) {
		`$INSTANCE_NAME`_SDO_Write( (value&cnt)? 1: 0);
		`$INSTANCE_NAME`_SCK_Write( 0 );
		`$INSTANCE_NAME`_SCK_Write( 1 );
	}
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Init( void )
{
	`$INSTANCE_NAME`_initVar = 1;
		
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_DISPLAYOFF);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETCLOCK);
	`$INSTANCE_NAME`_Command(0x91);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETMULTIPLEX);
	`$INSTANCE_NAME`_Command( 0x3F );

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETOFFSET);
	`$INSTANCE_NAME`_Command(0x4C);

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETSTARTLINE);
	`$INSTANCE_NAME`_Command(0x00);

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_MASTERCONFIG);
	`$INSTANCE_NAME`_Command(0x02); /* ADAFruit usees 0x02 */
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETREMAP);
	`$INSTANCE_NAME`_Command(0x50);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETCURRENT + 2);

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETGRAYTABLE);
	`$INSTANCE_NAME`_Command(0x01);
	`$INSTANCE_NAME`_Command(0x11); 
	`$INSTANCE_NAME`_Command(0x22); 
	`$INSTANCE_NAME`_Command(0x32); 
	`$INSTANCE_NAME`_Command(0x43); 
	`$INSTANCE_NAME`_Command(0x54); 
	`$INSTANCE_NAME`_Command(0x65);
	`$INSTANCE_NAME`_Command(0x76);

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETCONTRAST);
	`$INSTANCE_NAME`_Command(0x3F);

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETROWPERIOD);
	`$INSTANCE_NAME`_Command(0x51);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETPHASELEN);
	`$INSTANCE_NAME`_Command(0x55);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETPRECHARGECOMP);
	`$INSTANCE_NAME`_Command(0x03); // 0x03

	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETPRECHARGECOMPENABLE);
	`$INSTANCE_NAME`_Command(0x28);  // 0x28
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETVCOMLEVEL);
	`$INSTANCE_NAME`_Command(0x1C);
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_SETVSL);
	`$INSTANCE_NAME`_Command(0x0D );//| 0x02 );
	
	`$INSTANCE_NAME`_Command(`$INSTANCE_NAME`_CMD_NORMALDISPLAY);
	
	`$INSTANCE_NAME`_ClearDisplay();
	`$INSTANCE_NAME`_Refresh();
	
	`$INSTANCE_NAME`_Command( `$INSTANCE_NAME`_CMD_DISPLAYON );
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start( void )
{
	`$INSTANCE_NAME`_RST_Write( 0 );
	`$INSTANCE_NAME`_CS_Write( 1 );
	CyDelay(10);
	`$INSTANCE_NAME`_RST_Write( 1 );
	`$INSTANCE_NAME`_CS_Write(0);
	CyDelay( 500 );
	
	`$INSTANCE_NAME`_CursorX = 0;
	`$INSTANCE_NAME`_CursorY = 0;
	
	if (`$INSTANCE_NAME`_initVar != 1) {
		`$INSTANCE_NAME`_Init();
	}
	`$INSTANCE_NAME`_Refresh();
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_DrawPixel( uint16 x, uint16 y, uint16 color )
{
	uint32 pixel;
	uint8 value;
	uint8 mask;
	
	/*
	 * The pixel ID = (Y * (width/8)) + (X/8)
	 */
	if (x >= `$INSTANCE_NAME`_LCDWIDTH) return;
	if (y >= `$INSTANCE_NAME`_LCDHEIGHT) return;
	
	pixel =  (y * (`$INSTANCE_NAME`_LCDWIDTH>>3) ) + (x>>3);
	value = `$INSTANCE_NAME`_FrameBuffer[pixel];
	mask = 1<<(x&0x07);
	
	value = (color != 0)? value | mask : value &(~mask);
	`$INSTANCE_NAME`_FrameBuffer[pixel] = value;
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Command( uint8 cmd )
{
	`$INSTANCE_NAME`_DC_Write( 0 );
//	`$INSTANCE_NAME`_CS_Write( 0 );
	`$INSTANCE_NAME`_Send( cmd );
//	`$INSTANCE_NAME`_CS_Write( 1 );
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Refresh( void )
{
	int   row, col;
	uint8   pixel;
	uint8 lft, rht;
	int   idx;
	uint8 value;
	uint8 mask;
	
	`$INSTANCE_NAME`_Command( `$INSTANCE_NAME`_CMD_SETCOLADR );
	`$INSTANCE_NAME`_Command( 0x00 );
	`$INSTANCE_NAME`_Command( 0x3F );
	
	`$INSTANCE_NAME`_Command( `$INSTANCE_NAME`_CMD_SETROWADR );
	`$INSTANCE_NAME`_Command( 0x00 );
	`$INSTANCE_NAME`_Command( 0x3F );
	
	`$INSTANCE_NAME`_DC_Write( 1 );
//	`$INSTANCE_NAME`_CS_Write( 0 );
	
	idx = 0;
	
	for (row=0;row<`$INSTANCE_NAME`_LCDHEIGHT;++row) {
		for (col = 0;col<(`$INSTANCE_NAME`_LCDWIDTH/8);++col) {
			pixel = 0x01;
			while ( pixel != 0 ) {
				
				value = ((`$INSTANCE_NAME`_FrameBuffer[idx] & pixel)!=0) ? 0x0F : 0x00;
				pixel <<= 1;
				value |= ((`$INSTANCE_NAME`_FrameBuffer[idx] & pixel)!=0) ? 0xF0 : 0x00;
				pixel <<= 1;
				
				`$INSTANCE_NAME`_Send( value );			
			}
			++idx;
		}
	}
//	`$INSTANCE_NAME`_CS_Write( 1 );	
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_ClearDisplay( void )
{
	memset( (void*)&`$INSTANCE_NAME`_FrameBuffer[0], 0, `$INSTANCE_NAME`_LCDHEIGHT * `$INSTANCE_NAME`_LCDWIDTH / 8 );
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_ClearLine( uint8 mode )
{
	int lineStart;
	int lineEnd;
	int idx;
	
	if (mode > 2) return;
	
	
	lineStart = `$INSTANCE_NAME`_CursorY * `$INSTANCE_NAME`_LCDWIDTH;

	switch ( mode ) {
		case 0:
			/* Clear to end of line */
			/* Set the line end to the last pixel in the row */
			lineEnd = lineStart + 15; 
			lineStart = lineStart + (`$INSTANCE_NAME`_CursorX*6);
			break;
		case 1:
			/* Clear to start of line */
			lineEnd = lineStart + (`$INSTANCE_NAME`_CursorX*6);
			break;
		case 2:
			/* clear entire line */
			lineEnd = lineStart + 15; 
			break;
		default:
			lineEnd = lineStart + (`$INSTANCE_NAME`_CursorX*6);
			break;
	}
	
	for (idx=0;idx<8;++idx) {
		memset( (void*)&`$INSTANCE_NAME`_FrameBuffer[ lineStart ], 0, lineEnd - lineStart );
		lineStart += 16;
		lineEnd += 16;
	}	
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_ScrollUp( uint8 lines )
{
	while (lines > 0) {
		/* Scroll display up */
		memcpy((void*)&`$INSTANCE_NAME`_FrameBuffer[0],(void*)&`$INSTANCE_NAME`_FrameBuffer[8*(`$INSTANCE_NAME`_LCDWIDTH/8) ], (56 * (`$INSTANCE_NAME`_LCDWIDTH/8)) );
		memset((void*)&`$INSTANCE_NAME`_FrameBuffer[(56 * (`$INSTANCE_NAME`_LCDWIDTH/8))], 0, 128);
		--lines;
	}
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_PutChar( char c )
{
	uint8 bmp[5];
	uint8 mask;
	uint32 x,y;
	uint8 line;
	
	if (`$INSTANCE_NAME`_CursorY >= (`$INSTANCE_NAME`_LCDHEIGHT/8)) {
		`$INSTANCE_NAME`_ScrollUp( 1 );
		`$INSTANCE_NAME`_CursorY = (`$INSTANCE_NAME`_LCDHEIGHT/8) -1;
	}

	if (c == '\r') {
		`$INSTANCE_NAME`_CursorX = 0;
	}
	else if (c == '\n') {
		`$INSTANCE_NAME`_CursorX = 0;
		`$INSTANCE_NAME`_CursorY++;
	}
	else {
		/* obtain the start of the character map for the font */
		x = c*5;
		for(y=0;y<5;++y) {
			bmp[y] = `$INSTANCE_NAME`_font[x++];
		}
		x = `$INSTANCE_NAME`_CursorX * 6;
		y = `$INSTANCE_NAME`_CursorY * 8;
		
		mask = 0x01;
		for (line=0;line<8;line++) {
			`$INSTANCE_NAME`_DrawPixel(x,y,   (bmp[0]&mask)?1:0 );
			`$INSTANCE_NAME`_DrawPixel(x+1,y, (bmp[1]&mask)?1:0 );
			`$INSTANCE_NAME`_DrawPixel(x+2,y, (bmp[2]&mask)?1:0 );
			`$INSTANCE_NAME`_DrawPixel(x+3,y, (bmp[3]&mask)?1:0 );
			`$INSTANCE_NAME`_DrawPixel(x+4,y, (bmp[4]&mask)?1:0 );
			`$INSTANCE_NAME`_DrawPixel(x+5,y, 0 );
			++y;
			mask <<= 1;
		}
		
		`$INSTANCE_NAME`_CursorX++;
		if (`$INSTANCE_NAME`_CursorX >= (`$INSTANCE_NAME`_LCDWIDTH/6)) {
			`$INSTANCE_NAME`_CursorX = 0;
			`$INSTANCE_NAME`_CursorY++;
		}
	}
	
	
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
int `$INSTANCE_NAME`_ProcessString( const char *buffer, char *argv )
{
	int idx;
	int ptr;
	cystatus result;
	
	result = CYRET_STARTED;
	idx = 0;
	ptr = 0;
	while ( (buffer[idx] != 0) && (result == CYRET_STARTED) ) {
		/*
		 * drop leading whitespace, and set all spaces to NULL to
		 * prevent later confusion.
		 */
		while ( (buffer[idx] != 0) && isspace((int)buffer[idx]) ) {
			++idx;
		}
		/*
		 * now, we know that the index is pointing to a non-space character,
		 * so process the character based upon it's state.
		 */
		if (buffer[idx] == ';') {
			/* 
			 * The end of a command can be the end of the buffer, or, a
			 * semicolon can be used for the creation of compound
			 * statements.
			 * A compound statement seperator was detected, so, clear it to
			 * form a terminator for the last argument, and then, set the
			 * result to finished to let the processor know that the arguments
			 * of the current command are now fully split.
			 */
			argv[ptr] = 0;
			idx++;
			result = CYRET_FINISHED;
		}
		else if (buffer[idx] == '}') {
			/* End of sequence was detected, just return */
			idx++;
			result = CYRET_FINISHED;
		}
		/*
		 * Otherwise, store the character location, and move the index pointer
		 * to the next break in the input, or the end of the buffer
		 */
		else {
			argv[ptr++] = buffer[idx++];
			while ( (!isspace((int)buffer[idx])) && (buffer[idx] != 0) && (buffer[idx] != ';') && (buffer[idx] != '}') ) {
				argv[ptr++] = buffer[idx++];
			}
			argv[ptr] = 0;
		}
	}
	
	return idx;
}
/* ------------------------------------------------------------------------ */
int `$INSTANCE_NAME`_ProcessEscapeSequence( const char *str)
{
	int idx;
	char argv[25];
	int done;
	uint32 row, col;
	uint32 value;	
	
	/* trap an error that shouldn't happen, but might make print code easier */
	if ( str[0] != '{') return 0;
	
	row = 1;
	col = 1;
	done = 0;
	idx = 1;
	
	while (done == 0) {
		idx += `$INSTANCE_NAME`_ProcessString( &str[idx], argv );
		if (str[idx-1] == '}') {
			done = 1;
		}		
		else if (strncmp(argv, "right",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			`$INSTANCE_NAME`_CursorX += value;
			if (`$INSTANCE_NAME`_CursorX >= (`$INSTANCE_NAME`_LCDWIDTH/6)) {
				`$INSTANCE_NAME`_CursorX = (`$INSTANCE_NAME`_LCDWIDTH/6) - 1;
			}
		}
		else if (strncmp(argv, "up",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			if (`$INSTANCE_NAME`_CursorY >= value) {
				`$INSTANCE_NAME`_CursorY -= value;
			}
			else {
				`$INSTANCE_NAME`_CursorY = 0;
			}
		}
		else if (strncmp(argv, "down",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			
			`$INSTANCE_NAME`_CursorY += value;
			if (`$INSTANCE_NAME`_CursorY >= (`$INSTANCE_NAME`_LCDHEIGHT/8)) {
				`$INSTANCE_NAME`_CursorX = (`$INSTANCE_NAME`_LCDHEIGHT/8)-1;
			}
		}
		else if (strncmp(argv, "left",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			if (`$INSTANCE_NAME`_CursorX >= value) {
				`$INSTANCE_NAME`_CursorX -= value;
			}
			else {
				`$INSTANCE_NAME`_CursorX = 0;
			}
		}
		else if (strcmp(argv,"cls") == 0) {
			/* clear screen */
			`$INSTANCE_NAME`_ClearDisplay();
		}
		else if (strncmp(argv,"cl",2) == 0) {
			/* Clear line */
			if (argv[2] != 0) {
				sscanf(&argv[2],"%lu",&value);
			}
			else {
				value = 0;
			}
			`$INSTANCE_NAME`_ClearLine(value);
		}
		else if (strncmp(argv,"row",3) == 0) {
			if (argv[3] != 0) {
				sscanf(&argv[3],"%lu",&row);
			}
			else {
				row = 1;
			}
		}
		else if (strncmp(argv,"col",3) == 0) {
			if (argv[3] != 0) {
				sscanf(&argv[3],"%lu",&col);
			}
			else {
				col = 1;
			}
		}
		else if (strcmp(argv,"mv") == 0) {
			`$INSTANCE_NAME`_CursorY = row - 1;
			`$INSTANCE_NAME`_CursorX = col - 1;
		}
		else if (strcmp(argv, "hide") == 0) {
		}
		else if (strcmp(argv, "show") == 0) {
		}
		else if (argv[0] == 'c') {
		}
		else if (argv[0] == 'b') {
		}
		else if (argv[0] == '{') {
			`$INSTANCE_NAME`_PutChar('{');
		}
	}
	
	return idx;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_PrintString( const char *str )
{
	int idx;
	cystatus result;
	
	/*
	 * Inline escape sequences are contained within braces. To output an
	 * open brace, use a double brace {{}.
	 * {c#}        : Set Foreground Color
	 * {b#}        : Set background Color
	 * {left#}     : Cursor Left
	 * {right#}    : Cursor Right
	 * {cls}       : Clear Screen
	 * {cl#}       : Clear Line
	 * {row#}      : Set Row
	 * {col#}      : Set Column
	 * {mv}        : move cursor to row column
	 * {hide}      : hide cursor
	 * {show}      : show cursor
	 */
	result = CYRET_SUCCESS;
	idx = 0;
	while ( (str[idx] != 0) && (result == CYRET_SUCCESS) ) {
		if ( str[idx] == '{') {
			idx += `$INSTANCE_NAME`_ProcessEscapeSequence( &str[idx] );
		}
		else {
			/* insert character in to the send fifo */
			result = `$INSTANCE_NAME`_PutChar(str[idx++]);
		}
	}
	
	return result;
}
/* ------------------------------------------------------------------------ */
/* [] END OF FILE */
