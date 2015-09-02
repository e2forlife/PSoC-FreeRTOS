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
#include "`$COM_DEVICE`.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_semphr.h"

/* ======================================================================== */
uint8 `$INSTANCE_NAME`_initVar;

xSemaphoreHandle `$INSTANCE_NAME`_Mutex;

extern uint8 `$COM_DEVICE`_initVar;

/* ======================================================================== */
void `$INSTANCE_NAME`_Start( void )
{
	if (`$INSTANCE_NAME`_initVar != 1) {
		`$INSTANCE_NAME`_Init();
	}
	
	/* Check for enumeration after initialization */
	`$INSTANCE_NAME`_Enable();
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Init( void )
{
	/*
	 * If the I/O Driver has not yet been started, initialize and start the
	 * device by callin gthe system API.
	 */
	if (`$COM_DEVICE`_initVar == 0) {
		`$COM_DEVICE`_Start();
	}
	
	/*  Initialize the I/O library */
	`$INSTANCE_NAME`_Mutex = xSemaphoreCreateMutex();
	if ( `$INSTANCE_NAME`_Mutex != NULL ) {
		`$INSTANCE_NAME`_initVar = 1;
	}
	else {
		`$INSTANCE_NAME`_initVar = 0;
	}
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Enable( void )
{
	if (`$INSTANCE_NAME`_initVar != 0) {
		/*
		 * COMIO was initialized, and now is bing enabled for use.
		 * Enter user extension enables within the merge region below.
		 */
		/* `#START COMIO_ENABLE` */
		
		/* `#END` */
	}
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
char `$INSTANCE_NAME`_GetChar( void )
{
	char value;
	
	/* Quit when not yet initialized */
	if (`$INSTANCE_NAME`_initVar == 0) return 0;
	
	`$COM_DEVICE`_ReadByte( (uint8*) &value);
	
	return value;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_PutChar( char ch )
{
	return `$COM_DEVICE`_WriteByte( (uint8) ch );
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
	char out[21];
	int oi;
	
	
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
		
		if (strncmp(argv, "left",4) == 0) {
			if (argv[4] != 0) {
				sscanf(&argv[4],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luD",value);
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (strncmp(argv, "right",5) == 0) {
			if (argv[5] != 0) {
				sscanf(&argv[5],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luC",value);
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (strcmp(argv,"cls") == 0) {
			sprintf(out, "\x1b[2J");
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (strncmp(argv,"cl",2) == 0) {
			if (argv[2] != 0) {
				sscanf(&argv[2],"%lu",&value);
			}
			else {
				value = 1;
			}
			sprintf(out, "\x1b[%luK", value);
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
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
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (strcmp(argv, "hide") == 0) {
			sprintf(out,"\x1b[?25l");
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
		}
		else if (strcmp(argv, "show") == 0) {
			sprintf(out,"\x1b[?25h");
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (argv[0] == 'c') {
			/* Set Foreground Color */
			if (argv[1] != 0) {
				sscanf(&argv[1],"%lu",&value);
			}
			else {
				value = 7;
			}
			value = (value > 7) ? (90 + (value&0x07)):(value + 30);
			sprintf( out, "\x1b[%lum",value);
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
		}
		else if (argv[0] == 'b') {
			/* Set background color */
			if (argv[1] != 0) {
				sscanf(&argv[1],"%lu",&value);
			}
			else {
				value = 0;
			}
			value = (value > 7) ? (100 + (value&0x07)):(value + 40);
			sprintf( out, "\x1b[%lum",value);
			`$COM_DEVICE`_Write((uint8*)out,strlen(out));
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
	xSemaphoreTake(`$INSTANCE_NAME`_Mutex, portMAX_DELAY);
	{
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
	}
	xSemaphoreGive( `$INSTANCE_NAME`_Mutex );
	
	return result;
}
/* ------------------------------------------------------------------------- */
cystatus `$INSTANCE_NAME`_GenericGetString(char *str, uint8 quiet)
{
	cystatus result;
	char ch;
	int idx;
	
	result = CYRET_STARTED;
	idx = 0;
	
	xSemaphoreTake( `$INSTANCE_NAME`_Mutex, portMAX_DELAY);
	{
		`$COM_DEVICE`_ReadByte((uint8*)&ch);
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
					if (quiet == 0) `$COM_DEVICE`_Write((uint8*)"\b \b",3);
				}
				`$COM_DEVICE`_ReadByte((uint8*)&ch);
			}
			/*
			 * When a newline is read, push the character back in to the
			 * queue, since we don't want it, and don't read anymore data
			 * from the queue.
			 */
			else if ((ch!='\r')&&(ch!='\n')) {
				str[idx++] = ch;
				if (quiet == 0) `$COM_DEVICE`_WriteByte( (uint8) ch );
				`$COM_DEVICE`_ReadByte( (uint8*) &ch );
			}
			str[idx] = 0;
		}
		
		/*
		 * Remove the lingering EOL characters from the queue to prepare reading
		 * of the next string.
		 */
		if ( (ch == '\r') || (ch == '\n') ) {
			while( ((ch == '\r') || (ch == '\n')) && (`$COM_DEVICE`_DataWaiting() > 0) )
			{
				`$COM_DEVICE`_ReadByte((uint8*)&ch);
				if ( (ch != '\r') && (ch != '\n') ) {
					`$COM_DEVICE`_UnRead((uint8*)&ch,1);
				}
			}
			result = CYRET_FINISHED;
		}
	}
	xSemaphoreGive( `$INSTANCE_NAME`_Mutex );
	return result;
}

cystatus `$INSTANCE_NAME`_GetString(char *str)
{
	return `$INSTANCE_NAME`_GenericGetString(str, 0);
}

/* ------------------------------------------------------------------------- */
/*
 * scan key, and process escape sequences for command keys.
 */
uint16 `$INSTANCE_NAME`_ScanKey( void )
{
	uint16 result;
	char ch;
	
	result = 0xFF00;
	if (`$COM_DEVICE`_DataWaiting() > 0) {
		`$COM_DEVICE`_ReadByte( (uint8*)&ch );
		if (ch == '\x1b') {
			`$COM_DEVICE`_ReadByte( (uint8*) &ch );
			if (ch == '[') {
				do {
					`$COM_DEVICE`_ReadByte((uint8*)&ch);
				}
				while ( !isalpha((int)ch) );
				result = (uint16) ch;
				result |= `$INSTANCE_NAME`_KEY_CSI;
			}
			else {
				`$COM_DEVICE`_UnRead((uint8*)&ch,1);
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
