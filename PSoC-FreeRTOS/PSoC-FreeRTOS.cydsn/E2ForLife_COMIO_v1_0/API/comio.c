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

#include "`$COM_INSTANCE`.h"
#include "`$COM_INSTANCE`_cdc.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_task.h"
#include "`$FreeRTOS`_queue.h"
#include "`$FreeRTOS`_semphr.h"

/* ======================================================================== */

xQueueHandle `$INSTANCE_NAME`_RxQ;
xQueueHandle `$INSTANCE_NAME`_TxQ;
xSemaphoreHandle `$INSTANCE_NAME`_Mutex;

uint8 `$INSTANCE_NAME`_initVar;

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
	/*  Initialize the USB COM port */
	
	if (`$COM_INSTANCE`_initVar == 0) {
    	/* Start USBFS Operation with 3V operation */
		#if (CYDEV_VDDIO1_MV < 5000)
    		`$COM_INSTANCE`_Start(0u, `$COM_INSTANCE`_3V_OPERATION);
		#else
			`$COM_INSTANCE`_Start(0u, `$COM_INSTANCE`_5V_OPERATION);
		#endif
	}
	
	/* Initialize and create the semaphore */
	`$INSTANCE_NAME`_Mutex = xSemaphoreCreateMutex();
	
	/* Initialize USB Buffers */
	`$INSTANCE_NAME`_TxQ = xQueueCreate( `$TX_SIZE`, 1 );
	`$INSTANCE_NAME`_RxQ = xQueueCreate( `$RX_SIZE`, 1 );
	
	xTaskCreate( `$INSTANCE_NAME`_Task,"`$INSTANCE_NAME` Task", 400, NULL, `$USB_PRIORITY`, NULL);
	
	`$INSTANCE_NAME`_initVar = 1;
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
void `$INSTANCE_NAME`_Task( void *pvParameters )
{
    uint16 count;
    uint8 buffer[`$INSTANCE_NAME`_BUFFER_LEN];
	uint16 idx;
	
	for (;;) {
		/* Handle enumeration of USB port */
    	if(`$COM_INSTANCE`_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
    	{
        	if(`$COM_INSTANCE`_GetConfiguration() != 0u)   /* Init IN endpoints when device configured */
        	{
            	/* Enumeration is done, enable OUT endpoint for receive data from Host */
            	`$COM_INSTANCE`_CDC_Init();
        	}
    	}
		/*
		 *
		 */
	    if(`$COM_INSTANCE`_GetConfiguration() != 0u)
	    {
			/*
			 * Process received data from the USB, and store it in to the
			 * receiver message Q.
			 */
	        if(`$COM_INSTANCE`_DataIsReady() != 0u)
	        {   
	            count = `$COM_INSTANCE`_GetAll(buffer);
	            if(count != 0u)
	            {
					/* insert data in to Receive FIFO */
					for(idx=0;idx<count;++idx) {
						xQueueSend( `$INSTANCE_NAME`_RxQ, (void*)&buffer[idx],0);
						#if (`$DEBUG_USB_RX_ECHO` == 1 )
							xQueueSend( `$INSTANCE_NAME`_TxQ, (void*)&buffer[idx],0);
						#endif
					}
				}
			}
			/*
			 * Send a block of data ack through the USB port to the PC,
			 * by checkig to see if there is data to send, then sending
			 * up to the BUFFER_LEN of data (64 bytes)
			 */
			count = uxQueueMessagesWaiting( `$INSTANCE_NAME`_TxQ );
			count = (count > `$INSTANCE_NAME`_BUFFER_LEN)? `$INSTANCE_NAME`_BUFFER_LEN:count;
			
			/* When component is ready to send more data to the PC */			
            if ( (`$COM_INSTANCE`_CDCIsReady() != 0u) && (count > 0) ) {
				/*
				 * Read the data from the transmit queue and buffer it
				 * locally so that the data can be utilized.
				 */
				for (idx = 0; idx < count; ++idx) {
					xQueueReceive( `$INSTANCE_NAME`_TxQ,&buffer[idx],0);
				}
				/* Send data back to host */
    	        `$COM_INSTANCE`_PutData(buffer, count);
				
				/* If the last sent packet is exactly maximum packet size, 
            	 *  it shall be followed by a zero-length packet to assure the
             	 *  end of segment is properly identified by the terminal.
             	 */
            	if(count == `$INSTANCE_NAME`_BUFFER_LEN){
					/* Wait till component is ready to send more data to the PC */
                	while(`$COM_INSTANCE`_CDCIsReady() == 0u) {
						vTaskDelay( `$USB_SCAN_RATE` / portTICK_RATE_MS );
					}
                	`$COM_INSTANCE`_PutData(NULL, 0u);         /* Send zero-length packet to PC */
            	} 
			}
		}
		
		vTaskDelay(`$USB_SCAN_RATE`/portTICK_PERIOD_MS);
	}
}

/* ======================================================================== */
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
char `$INSTANCE_NAME`_GetChar( void )
{
	char value;
	portBASE_TYPE xStatus;
	
	/* wait for data to become available */
	xStatus = xQueueReceive( `$INSTANCE_NAME`_RxQ, &value, portMAX_DELAY);
	
	if (xStatus != pdPASS) {
		value = 255;
	}
	
	return value;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_PutChar( char ch )
{
	
	portBASE_TYPE xStatus;
	
	xStatus = xQueueSend( `$INSTANCE_NAME`_TxQ, &ch, portMAX_DELAY);
	
	return (xStatus == pdPASS)?CYRET_SUCCESS:CYRET_MEMORY;
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
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
		}
		else if (strcmp(argv,"cls") == 0) {
			sprintf(out, "\x1b[2J");
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
		}
		else if (strcmp(argv, "hide") == 0) {
			sprintf(out,"\x1b[?25l");
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
		}
		else if (strcmp(argv, "show") == 0) {
			sprintf(out,"\x1b[?25h");
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
			oi = 0;
			while (out[oi] != 0) `$INSTANCE_NAME`_PutChar(out[oi++]);
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
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_SetColor( uint8 fg, uint8 bg )
{
	char buffer[21];
	
	fg = (fg > 7) ?	(90 + (fg&0x07)):(fg + 30);
	bg = (bg > 7) ? (100+(bg&7)):(bg + 40);
	sprintf(buffer,"\x1b[%d;%dm",fg,bg);
	return `$INSTANCE_NAME`_PrintString(buffer);
}
/* ------------------------------------------------------------------------- */
cystatus `$INSTANCE_NAME`_ClearLine(uint8 mode)
{
	char buffer[15];
	
	sprintf(buffer,"\x1b[%dK",mode);
	return `$INSTANCE_NAME`_PrintString(buffer);
}
/* ------------------------------------------------------------------------- */
cystatus `$INSTANCE_NAME`_Position(uint8 row, uint8 col)
{
	char buffer[21];
	
	sprintf(buffer,"\x1b[%d;%dH",row+1,col+1);
	return `$INSTANCE_NAME`_PrintString(buffer);
}
/* ------------------------------------------------------------------------- */
//cystatus `$INSTANCE_NAME`_PrintStringColor(const char *str, uint8 fg, uint8 bg)
//{
//	cystatus result;
//	int idx;
//	
//	result = `$INSTANCE_NAME`_SetColor(fg,bg);
//	idx = 0;
//	while ( (str[idx] != 0) && (result == CYRET_SUCCESS) ) {
//		if ( ( (str[idx] == '[') || (str[idx] == ']') || (str[idx] == '(') || (str[idx] == ')') ) && (bg!=4) ) {
//			result = `$INSTANCE_NAME`_SetColor(4,bg);
//			`$INSTANCE_NAME`_PutChar( str[idx] );
//			result = `$INSTANCE_NAME`_SetColor(fg,bg);
//		}
//		else {
//			`$INSTANCE_NAME`_PutChar( str[idx] );
//		}
//		++idx;
//	}
//	`$INSTANCE_NAME`_SetColor(7,0);
//	
//	return result;
//}
/* ------------------------------------------------------------------------- */
cystatus `$INSTANCE_NAME`_GetString(char *str)
{
	cystatus result;
	char ch;
	int idx;
	
	result = CYRET_STARTED;
	idx = 0;
	
	xSemaphoreTake( `$INSTANCE_NAME`_Mutex, portMAX_DELAY);
	{
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
				}
				`$INSTANCE_NAME`_PutChar('\b');
				`$INSTANCE_NAME`_PutChar(' ');
				`$INSTANCE_NAME`_PutChar('\b');
				ch = `$INSTANCE_NAME`_GetChar();
			}
			/*
			 * When a newline is read, push the character back in to the
			 * queue, since we don't want it, and don't read anymore data
			 * from the queue.
			 */
			else if ((ch!='\r')&&(ch!='\n')) {
				str[idx++] = ch;
				`$INSTANCE_NAME`_PutChar( ch );
				ch = `$INSTANCE_NAME`_GetChar();
			}
			str[idx] = 0;
		}
		
		/*
		 * Remove the lingering EOL characters from the queue to prepare reading
		 * of the next string.
		 */
		if ( (ch == '\r') || (ch == '\n') ) {
			while( ((ch == '\r') || (ch == '\n')) && (uxQueueMessagesWaiting(`$INSTANCE_NAME`_RxQ) > 0) )
			{
				ch = `$INSTANCE_NAME`_GetChar(); /* Remove the EOL character from buffer */
				if ( (ch != '\r') && (ch != '\n') ) {
					xQueueSendToFront( `$INSTANCE_NAME`_RxQ, &ch, 0);
				}
			}
			result = CYRET_FINISHED;
		}
	}
	xSemaphoreGive( `$INSTANCE_NAME`_Mutex );
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
	
	result = 0xFF00;
	if (uxQueueMessagesWaiting( `$INSTANCE_NAME`_RxQ ) > 0) {
		ch = `$INSTANCE_NAME`_GetChar();
		if (ch == '\x1b') {
			ch = `$INSTANCE_NAME`_GetChar(); /* wait for bracket */
			if (ch == '[') {
				do {
					ch = `$INSTANCE_NAME`_GetChar(); /* Get command */
				}
				while ( !isalpha((int)ch) );
				result = (uint16) ch;
				result |= `$INSTANCE_NAME`_KEY_CSI;
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
