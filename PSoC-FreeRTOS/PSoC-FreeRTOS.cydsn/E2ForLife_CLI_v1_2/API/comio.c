/* ======================================================================== */
/*
 * Copyright (c) 2015, E2ForLife.com
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the E2ForLife.com nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL E2FORLIFE.COM BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ======================================================================== */
#include <cytypes.h>
#include <cylib.h>
#include <stdio.h>

#include "`$INSTANCE_NAME`.h"

/* ======================================================================== */
extern uint8 `$INSTANCE_NAME`_initVar;
/* ======================================================================== */
cystatus `$INSTANCE_NAME`_GenericPutChar(char ch, xQueueHandle h)
{
	if (h== NULL) return CYRET_BAD_PARAM;
	
	xQueueSendToBack(h, (void*)&ch, portMAX_DELAY);
	return CYRET_SUCCESS;
}	
/* ------------------------------------------------------------------------ */
char `$INSTANCE_NAME`_GetChar( void )
{
	char value;
	
	if (`$InputQueue` == NULL) return 0;
	
	/* Quit when not yet initialized */
	if (`$INSTANCE_NAME`_initVar == 0) return 0;
	
	xQueueReceive(`$InputQueue`,(void*)&value,portMAX_DELAY);
	
	return value;
}
/* ------------------------------------------------------------------------ */
#if (`$EnableEscapeSequence` == 1)
int `$INSTANCE_NAME`_ProcessEscapeSequence( const char *str, xQueueHandle h)
{
	int idx;
	char argv[25];
	uint32 row, col;
	uint32 value;
	char out[21];
	int oi;
	
	if (h == NULL) return 0;
	
	/* trap an error that shouldn't happen, but might make print code easier */
	if ( str[0] != '{') return 0;
	
	row = 1;
	col = 1;
	idx = 0;
	
	while ( (str[idx] != '}') && (str[idx] != 0) ) {
		++idx;
		/*
		 * strip leading whitespace from the tag
		 */
		while (isspace((int)str[idx]) ) {
			++idx;
		}
		/*
		 * copy the tag in to the holding buffer
		 */
		oi = 0;
		while( (str[idx]!=';') && (str[idx]!='}') && (oi<20) && (str[idx] != 0) ) {
			argv[oi++] = str[idx++];
			argv[oi] = 0;
		}
		
		if (strncmp(argv, "left",4) == 0) {
			if (argv[4] != 0) {
				sscanf(&argv[4],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luD",value);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (strncmp(argv, "right",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luC",value);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (strcmp(argv,"cls") == 0) {
			sprintf(out, "\x1b[2J");
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (strncmp(argv,"cl",2) == 0) {
			if (argv[2] != 0) {
				sscanf(&argv[2],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luK", value);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
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
			sprintf(out,"\x1b[%lu;%luH",row,col);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (strcmp(argv, "hide") == 0) {
			sprintf(out,"\x1b[?25l");
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (strcmp(argv, "show") == 0) {
			sprintf(out,"\x1b[?25h");
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (tolower((int)argv[0]) == 'c') {
			/* Set Foreground Color */
			if (argv[1] != 0) {
				sscanf(&argv[1],"%lu",&value);
			}
			else {
				value = 7;
			}
			value = (value > 7) ? (90 + (value&0x07)):(value + 30);
			sprintf( out, "\x1b[%lum",value);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (tolower((int)argv[0]) == 'b') {
			/* Set background color */
			if (argv[1] != 0) {
				sscanf(&argv[1],"%lu",&value);
			}
			else {
				value = 0;
			}
			value = (value > 7) ? (100 + (value&0x07)):(value + 40);
			sprintf( out, "\x1b[%lum",value);
			oi = 0;
			while (out[oi] != 0) {
				`$INSTANCE_NAME`_GenericPutChar( out[oi++], h );
			}
		}
		else if (argv[0] == '{') {
				`$INSTANCE_NAME`_GenericPutChar( '{', h );
		}
	}
	
	return idx+1;
}
#endif
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_GenericPrintString( const char *str, xQueueHandle h )
{
	int idx;
	cystatus result;
	
	if (h == NULL) return CYRET_BAD_PARAM;
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
		#if (`$EnableEscapeSequence` == 1)
		if ( str[idx] == '{') {
			idx += `$INSTANCE_NAME`_ProcessEscapeSequence( &str[idx], h );
		}
		else {
			/* insert character in to the send fifo */
			result = `$INSTANCE_NAME`_GenericPutChar(str[idx++], h);
		}
		#else
			result = `$INSTANCE_NAME`_GenericPutChar(str[idx++],h);
		#endif
	}
	
	return result;
}
/* ------------------------------------------------------------------------- */
cystatus `$INSTANCE_NAME`_GenericGetString(char *str)
{
	cystatus result;
	char ch;
	int idx;
	
	result = CYRET_STARTED;
	idx = 0;

	if (`$InputQueue` == NULL) return CYRET_BAD_PARAM;
	
	ch = `$INSTANCE_NAME`_GetChar();
	/* 
	 * While there are no EOL character read, read the data fro mthe queue,
	 * and take a peek at the next data in the buffer.
	 */
	while ( (ch != '\r') && (ch != '\n') ) {
		/*
		 * When a backspace has been read, remove the last read character
		 * from the input buffer, and output a destructive backspace character
		 * so that the terminal will clear the character from the display.
		 */
		if ( (ch == '\b') || (ch == 127) ) {
			str[idx] = 0;
			if (idx>0) {
				idx--;
				#if (`$EchoInput`==1)
				`$INSTANCE_NAME`_PutChar('\b');
				`$INSTANCE_NAME`_PutChar(' ');
				`$INSTANCE_NAME`_PutChar('\b');
				#endif
			}
			ch = `$INSTANCE_NAME`_GetChar();
		}
		/*
		 * When a newline is read, push the character back in to the
		 * queue, since we don't want it, and don't read anymore data
		 * from the queue.
		 * v1.1.1 : Addd a conition to check for 0 values, since they shouldn't
		 *          
		 */
		else if ((ch!='\r')&&(ch!='\n')) {
			if (ch != 0) {
				str[idx++] = ch;
				#if (`$EchoInput`==1)
				`$INSTANCE_NAME`_PutChar( (uint8) ch );
				#endif
			}
			ch = `$INSTANCE_NAME`_GetChar();
		}
		str[idx] = 0;
	}
	
	return result;
}

/* ------------------------------------------------------------------------- */
/*
 * scan key, and process escape sequences for command keys.
 */
uint16 `$INSTANCE_NAME`_ScanKey( void )
{
	uint16 result;
	char ch;
	
	if (`$InputQueue` == NULL) return 0;
	
	result = 0xFF00;
	if (uxQueueMessagesWaiting(`$InputQueue`) > 0) {
		ch = `$INSTANCE_NAME`_GetChar();
		if (ch == '\x1b') {
			ch = `$INSTANCE_NAME`_GetChar();
			if (ch == '[') {
				do {
					ch = `$INSTANCE_NAME`_GetChar();
				}
				while ( !isalpha((int)ch) );
				result = (uint16) ch;
				result |= `$INSTANCE_NAME`_KEY_CSI;
			}
			else {
				xQueueSendToFront(`$InputQueue`,(void*)&ch,portMAX_DELAY);
				result = `$INSTANCE_NAME`_KEY_ESC;
			}
		}
		else if ( ch < 32 ) {
			/*
			 * In situations where the control key was pressed (CTRL-C)
			 * 
			 */
			result = (ch+32) | `$INSTANCE_NAME`_KEY_CTRL;
		}
		else if ( ((ch>='A')&&(ch <='Z'))||((ch>='!')&&(ch<='+'))||(ch=='^')||(ch=='_')||(ch==':')||(ch=='<')||(ch=='>')||(ch=='?') ) {
			result = ch | `$INSTANCE_NAME`_KEY_SHFT;
		}
		else {
			result = ch;
		}
	}
	return result;
}
/* ------------------------------------------------------------------------- */
/* [] END OF FILE */
