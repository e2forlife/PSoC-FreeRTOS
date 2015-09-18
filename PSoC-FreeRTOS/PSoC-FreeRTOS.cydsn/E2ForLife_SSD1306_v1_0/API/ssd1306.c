/*
 * Copyright (c) 2015, E2ForLife
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the E2ForLife nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL E2FORLIFE BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ------------------------------------------------------------------------ */
#include <cytypes.h>
#include <cylib.h>
#include <stdio.h>

#include "cyfitter.h"
#include "cypins.h"
#include "`$INSTANCE_NAME`.h"

#if (`$Mode` == 1)

#else
	#error Only I2C is presently supported
#endif

/* ------------------------------------------------------------------------ */
#if (`$VBLANK_MODE` == 3)
	void `$INSTANCE_NAME`_HW_Delay(uint32 time)
	{
		for (int i=0;i<time;++i);
	}
#else
	void `$INSTANCE_NAME`_HW_Delay(uint32 time)
	{
		CyDelayUs( time );
	}
#endif

void `$INSTANCE_NAME`_HW_Start( void )
{
	CyPins_SetPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_SetPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_ClearPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_ClearPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
}

void `$INSTANCE_NAME`_HW_Stop( void )
{
	CyPins_ClearPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_ClearPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_SetPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_SetPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
}

uint8 `$INSTANCE_NAME`_HW_Write( uint8 value )
{
	uint8 index;

	for(index = 0; index < 8; index++)
	{
		if (value&0x80) {
			CyPins_SetPin(`$INSTANCE_NAME`_SDO__0__PC);
		}
		else {
			CyPins_ClearPin(`$INSTANCE_NAME`_SDO__0__PC);
		}
		value <<= 1;            		// Shift the byte by one bit
		`$INSTANCE_NAME`_HW_Delay(2);
		CyPins_SetPin(`$INSTANCE_NAME`_SCL__0__PC);
		`$INSTANCE_NAME`_HW_Delay(2);
		CyPins_ClearPin(`$INSTANCE_NAME`_SCL__0__PC);
		`$INSTANCE_NAME`_HW_Delay(2);
	}

	CyPins_SetPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_SetPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	index = CyPins_ReadPin(`$INSTANCE_NAME`_SDO__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	CyPins_ClearPin(`$INSTANCE_NAME`_SCL__0__PC);
	`$INSTANCE_NAME`_HW_Delay(2);
	
	return(index!=0)?1:0;
}
/* ------------------------------------------------------------------------ */

#if (`$VBLANK_MODE` == 2)
	/*
	 * We are going to use FreeRTOS to execute automatic VBlanks to refresh
	 * the display.
	 */
	#include "FreeRTOS.h"
	#include "FreeRTOS_task.h"
	#include "FreeRTOS_semphr.h"

	xTaskHandle `$INSTANCE_NAME`_task;
	xSemaphoreHandle `$INSTANCE_NAME`_mutex;
	
	void `$INSTANCE_NAME`_vblankTask( void *pvParameters )
	{
		for(;;) {
			/*
			 * Wait for a VBLANK to occur.
			 */
			vTaskDelay(`$VBLANK_RATE`/portTICK_RATE_MS);
			xSemaphoreTake( `$INSTANCE_NAME`_mutex, portMAX_DELAY );
			{
				/* render the display by sending the data to the controller */
				SSD1306_Refresh();
			}
			xSemaphoreGive( `$INSTANCE_NAME`_mutex );
			/*
			 * Call user VBLANK Callback
			 */
			/* `#START VBLANK_TASK` */
			
			/* `#END` */
		}
		
	}
	
#elif (`$VBLANK_MODE` == 3)
	/*
	 * Using local interrupt driven vblanks, executed on a timer base
	 */
	#include "`$INSTANCE_NAME`_VBLANK_TIMER.h"
	#include "`$INSTANCE_NAME`_eventVBlank.h"
	
	CY_ISR( `$INSTANCE_NAME`_eventVBLANK )
	{
		uint8 status;
		
		status = CyEnterCriticalSection();
		`$INSTANCE_NAME`_Refresh();
		CyExitCriticalSection(status);
		
		/*
		 * Call user VBLANK Callback
		 */
		/* `#START VBLANK_TASK` */
		
		/* `#END` */
		status = `$INSTANCE_NAME`_VBLANK_TIMER_STATUS;
		/* this line does nothing but eliminate a bogus warning */
		status = status;
	}
#endif

/* ------------------------------------------------------------------------ */
/* Global Component Data */
uint8 `$INSTANCE_NAME`_initVar = 0u;
//extern uint8 I2C_initVar;

/* The display raster buffer used to refresh the display during "vblank" */
uint8 `$INSTANCE_NAME`_Raster[ `$INSTANCE_NAME`_RASTER_SIZE + 1];
/*
 * When using Text output, these variables define the present row and column
 * of the display output. These can be manipulated by the PrintString function
 * escape sequences, and are updated after writing a character to the display
 */
uint8 `$INSTANCE_NAME`_CursorX;
uint8 `$INSTANCE_NAME`_CursorY;

/* the font bitmap, from `$INSTANCE_NAME`_font.c */
extern const uint8 `$INSTANCE_NAME`_Font[];

const uint8 `$INSTANCE_NAME`_DisplayInit[] =
{
	0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14,
	0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x02, 0x81, 0x8F, 0xD9, 0xF1,
	0xDB, 0x40, 0xA4, 0xA6
};
/* 
 * the number command elements to be transmitted to the display controller
 * in order to initialize the display hardware.
 */
#define `$INSTANCE_NAME`_INIT_SIZE     ( 24 )
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Init( void )
{
	int idx;
	
	/*
	 * Clear the Raster buffer when starting up (essentially, erase
	 * the display contents, and reset the cursor to the top left position.
	 */
	memset( (void*)&`$INSTANCE_NAME`_Raster[1],0,`$INSTANCE_NAME`_RASTER_SIZE );
	/*
	 * initialize the starting position of the character output cursor
	 * so that characters are drawn in nice rows adn columns
	 */
	`$INSTANCE_NAME`_CursorX = 0;
	`$INSTANCE_NAME`_CursorY = 0;
	
	/*
	 * Send Initialization table to the display
	 */
	for (idx = 0; idx < `$INSTANCE_NAME`_INIT_SIZE;++idx) {
		`$INSTANCE_NAME`_SendCommand( `$INSTANCE_NAME`_DisplayInit[idx] );
	}
	
	`$INSTANCE_NAME`_initVar = 1;
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Enable( void )
{
	/*
	 * Just turn on the display
	 */
	`$INSTANCE_NAME`_SendCommand( 0xAF );
	/* and then send the default raster to the display */
	`$INSTANCE_NAME`_Refresh();
	
	/*
	 * Setup the vblank calls, if any, based upon the configuration selections
	 * made in the dialog window.
	 */
	#if (`$VBLANK_MODE` == 2)
		/* 
		 * FreeRTOS mode:
		 * OS mode uses tasks to render the character raster, also, if there is
		 * data sitting in the queue, it will be read and proecssed to render
		 * the font bitmaps to the screen.
		 */
		xTaskCreate(`$INSTANCE_NAME`_vblankTask,"VBLANK",800,NULL,`$VBLANK_PRIORITY`, &`$INSTANCE_NAME`_task);
		`$INSTANCE_NAME`_mutex = xSemaphoreCreateMutex();
		
	#elif (`$VBLANK_MODE` == 3)
		/* 
		 * IRQ Mode:
		 * When using the IRQ mode, all that needs to be done is to start the
		 * timer, then enable the IRQ event handler to redraw the screen.
		 */
		`$INSTANCE_NAME`_VBLANK_TIMER_Start();
		/* 
		 * After the timer is started, set the vblank period, since this can't
		 * be done through the configuration dialog.
		 */
		`$INSTANCE_NAME`_VBLANK_TIMER_WritePeriod( `$VBLANK_RATE` );
		`$INSTANCE_NAME`_eventVBlank_StartEx( `$INSTANCE_NAME`_eventVBLANK );
		
	#endif
	/* 
	 * Manual mode (and undefined modes) we do nothing since the main software
	 * is taking the responsibility of refreshing the display
	 */
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start( void )
{
	if (`$INSTANCE_NAME`_initVar != 1) {
		`$INSTANCE_NAME`_Init();
	}
	`$INSTANCE_NAME`_Enable();
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_SendCommand( uint8 command )
{
	`$INSTANCE_NAME`_HW_Start();
	`$INSTANCE_NAME`_HW_Write(`$INSTANCE_NAME`_I2C_ADDRESS<<1);
	`$INSTANCE_NAME`_HW_Write( 0 );
	`$INSTANCE_NAME`_HW_Write( command );
	`$INSTANCE_NAME`_HW_Stop();
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Refresh( void )
{
	int idx;
	/*
	 * redraw the screen using the I2C interface to send the buffer to the
	 * display VRAM.
	 */
//	`$INSTANCE_NAME`_SendCommand(0x21);
//	`$INSTANCE_NAME`_SendCommand(0);
//	`$INSTANCE_NAME`_SendCommand(127);
//	`$INSTANCE_NAME`_SendCommand(0x22);
//	`$INSTANCE_NAME`_SendCommand(0);
//	`$INSTANCE_NAME`_SendCommand( 3 );

	`$INSTANCE_NAME`_Raster[0] = 0x40;
	
	`$INSTANCE_NAME`_HW_Start();
	`$INSTANCE_NAME`_HW_Write(`$INSTANCE_NAME`_I2C_ADDRESS<<1);
	for(idx = 0;idx<`$INSTANCE_NAME`_RASTER_SIZE+1;++idx) {
		`$INSTANCE_NAME`_HW_Write( `$INSTANCE_NAME`_Raster[idx] );
	}
	`$INSTANCE_NAME`_HW_Stop();
	
}

/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_ScrollUp(int lines)
{
	int cnt;
	
	for(cnt=0;cnt<lines;++cnt) {	
		memcpy((void*)&`$INSTANCE_NAME`_Raster[1], 
			(void*)&`$INSTANCE_NAME`_Raster[`$INSTANCE_NAME`_DISPLAY_WIDTH+1],
			`$INSTANCE_NAME`_RASTER_SIZE-`$INSTANCE_NAME`_DISPLAY_WIDTH);
		
		memset((void*)&`$INSTANCE_NAME`_Raster[(((`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)-1)*`$INSTANCE_NAME`_DISPLAY_WIDTH)+1],0,`$INSTANCE_NAME`_DISPLAY_WIDTH);
	}
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_PutChar( char c )
{
	int xpos;
	int ypos;
	
	#if (`$VBLANK_MODE` == 2)
	xSemaphoreTake( `$INSTANCE_NAME`_mutex, portMAX_DELAY);
	{
	#endif
	/*
	 * Render a character on the display, then update the cursor location
	 * to the next available space.
	 */
	if (`$INSTANCE_NAME`_CursorY >= (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)) {
		/*
		 * The new character is giong to display below the bottom of the
		 * display, so, scroll the display up one line, and clear the bottom
		 * row. Lastly, set the cursor y location to the bottom row.
		 */
		`$INSTANCE_NAME`_ScrollUp(1);
		`$INSTANCE_NAME`_CursorY = (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)-1;
	}
	/* 
	 * Check for CR and NL characters. These are non-printable (even though
	 * there is a font icon for them), and control the cursor location.
	 */
	if (c == '\n') {
		`$INSTANCE_NAME`_CursorY++;
		`$INSTANCE_NAME`_CursorX = 0;
	}
	else if (c == '\r') {
		`$INSTANCE_NAME`_CursorX = 0;
	}
	else {
		/*
		 * Otherwise, this is a real character that needs to be displayed.
		 * Fortunately, the font is buit in FLASH with an identical column
		 * structure to copy the data in to the raster without modifying
		 * the data.
		 */
		#if (`$INSTANCE_NAME`_FONT_INCLUDE_LOWERCASE == 0)
			c = toupper((int)c);
		#endif
		if ( (c < 127) || ((c>126)&&(`$INSTANCE_NAME`_FONT_INCLUDE_EXT256!=0)) ) {
			#if (`$INSTANCE_NAME`_FONT_INCLUDE_CTRL_CHARS == 0)
				/* Fix braces, bar, and tilde */
				#if (`$INSTANCE_NAME`_FONT_INCLUDE_LOWERCASE == 0)
				if (c > 122) {
					/* 
					 * this puts the open brace at the ASCII position of
					 * lowercase 'a', and maps it to the propoer font
					 * location when using the reduced memory versions
					 */
					c -= 26;
				}
				#endif
				/*
				 * align the space character at ASCII 0, since the control
				 * chars are omitted from the font
				 */
				c = c - ' ';
			#endif
			/* 
			 * There are 5 columns to each font, so, the start index is a
			 * multiple of 5.
			 */
			xpos = c * 5;
			/*
			 * Now, calculate the x,y position of the pixel start, or, more
			 * accurately, the index in to the raster array to which the font
			 * bitmap will be copied. We add one to this value to account
			 * for the command byte stored at index 0.
			 */
			ypos = ((`$INSTANCE_NAME`_CursorX*6) + (`$INSTANCE_NAME`_CursorY*128))+1;
			memcpy((void*)&`$INSTANCE_NAME`_Raster[ypos], (void*)&`$INSTANCE_NAME`_Font[xpos], 5 );
			`$INSTANCE_NAME`_Raster[ypos+5] = 0;			
		}
		/* move the cursor to the next location */
		`$INSTANCE_NAME`_CursorX++;
		if (`$INSTANCE_NAME`_CursorX >= (`$INSTANCE_NAME`_DISPLAY_WIDTH/6)) {
			`$INSTANCE_NAME`_CursorX = 0;
			`$INSTANCE_NAME`_CursorY++;
		}
	}
	#if (`$VBLANK_MODE` == 2)
	}
	xSemaphoreGive( `$INSTANCE_NAME`_mutex );
	#endif
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_SizedPutChar(char c, uint8 xsize, uint8 ysize)
{
	int xpos;
	int ypos;
	int idx;
	uint8 mask;
	uint8 pixel;
	uint8 bmp[24];
	
	/*
	 * Render a character on the display, then update the cursor location
	 * to the next available space.
	 */
	if (`$INSTANCE_NAME`_CursorY >= (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)) {
		/*
		 * The new character is giong to display below the bottom of the
		 * display, so, scroll the display up one line, and clear the bottom
		 * row. Lastly, set the cursor y location to the bottom row.
		 */
		`$INSTANCE_NAME`_ScrollUp((ysize!=0)?2:1);
		`$INSTANCE_NAME`_CursorY = (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)- ((ysize!=0)?2:1);
	}
	/* 
	 * Check for CR and NL characters. These are non-printable (even though
	 * there is a font icon for them), and control the cursor location.
	 */
	if (c == '\n') {
		`$INSTANCE_NAME`_CursorY += (ysize!=0)?2:1;
		`$INSTANCE_NAME`_CursorX = 0;
	}
	else if (c == '\r') {
		`$INSTANCE_NAME`_CursorX = 0;
	}
	else {
		/*
		 * Otherwise, this is a real character that needs to be displayed.
		 * Fortunately, the font is buit in FLASH with an identical column
		 * structure to copy the data in to the raster without modifying
		 * the data.
		 */
		#if (`$INSTANCE_NAME`_FONT_INCLUDE_LOWERCASE == 0)
			c = toupper((int)c);
		#endif
		if ( (c < 127) || ((c>126)&&(`$INSTANCE_NAME`_FONT_INCLUDE_EXT256!=0)) ) {
			#if (`$INSTANCE_NAME`_FONT_INCLUDE_CTRL_CHARS == 0)
				/* Fix braces, bar, and tilde */
				if (c > 122) {
					/* 
					 * this puts the open brace at the ASCII position of
					 * lowercase 'a', and maps it to the propoer font
					 * location when using the reduced memory versions
					 */
					c -= 26;
				}
				/*
				 * align the space character at ASCII 0, since the control
				 * chars are omitted from the font
				 */
				c = c - ' ';
			#endif
			/* 
			 * There are 5 columns to each font, so, the start index is a
			 * multiple of 5.
			 */
			xpos = c * 5;
			/*
			 * Expand the font in to the bitmap that is to be copied to the
			 * raster buffer.
			 * The first thing to do is to copy the column data to make the
			 * font wider.
			 */
			memset((void*)&bmp[0],0,24);
			if (xsize != 0) {
				for (idx=0;idx<5;idx++) {
					bmp[(idx<<1)] = `$INSTANCE_NAME`_Font[xpos+idx];
					bmp[(idx<<1)+1] = `$INSTANCE_NAME`_Font[xpos+idx];
				}
			}
			else {
				for (idx=0;idx<5;idx++) {
					bmp[idx] = `$INSTANCE_NAME`_Font[xpos+idx];
				}
			}				
			/*
			 * now that the font was stretched in the X direction, stretch
			 * it in the Y direction as well.  this is a little harder since
			 * we now have to expand the bits in to the upper portion of the
			 * bitmap.
			 */
			if (ysize != 0) {
				for(idx=0;idx<12;++idx) {
					pixel = 0xC0;
					for (mask=0x80;mask!=0x08;mask>>=1) {
						if (bmp[idx]&mask) {
							bmp[idx+12] |= pixel;
						}
						pixel >>= 2;
					}
				}
				/*
				 * and now that the lower portion of the letter was expanded,
				 * spread the upper bits in the same manner
				 */
				for(idx=0;idx<12;++idx) {
					pixel = 0xC0;
					ypos = bmp[idx];
					for(mask=0x08;mask!=0;mask>>=1) {
						ypos &= ~pixel;
						if (bmp[idx]&mask) {
							ypos |= pixel;
						}
						pixel >>= 2;
					}
					bmp[idx] = ypos;
				}
			}
			/*
			 * Now, calculate the x,y position of the pixel start, or, more
			 * accurately, the index in to the raster array to which the font
			 * bitmap will be copied. We add one to this value to account
			 * for the command byte stored at index 0.
			 */
			ypos = ((`$INSTANCE_NAME`_CursorX*6) + (`$INSTANCE_NAME`_CursorY*128))+1;
			memcpy((void*)&`$INSTANCE_NAME`_Raster[ypos], (void*)&bmp[0], (xsize!=0)?12:6 );
			if (ysize != 0) {
				memcpy((void*)&`$INSTANCE_NAME`_Raster[ypos+128], (void*)&bmp[12],(xsize!=0)?12:6);
			}
		}
		
		/* move the cursor to the next location */
		`$INSTANCE_NAME`_CursorX += (xsize!=0)?2:1;
		if (`$INSTANCE_NAME`_CursorX >= (`$INSTANCE_NAME`_DISPLAY_WIDTH/6)) {
			`$INSTANCE_NAME`_CursorX = 0;
			`$INSTANCE_NAME`_CursorY += (ysize!=0)?2:1;
		}
	}
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_PrintString( char *str )
{
	int idx = 0;
	int cmdix = 0;
	uint8 mode;
	uint8 expandX,expandY;
	char cmd[21];
	int value;
	
	/* default the mode to "Normal output" */
	mode = 0;
	expandX = 0; /* default to normal sized fonts */
	expandY = 0;
	
	/*
	 * Loop through the string and process commands or, output the character
	 * until the end of string is detected.
	 */
	while (str[idx] != 0) {
		/*
		 * When an escape sequence is detected, process the command, or, 
		 * buffer the data.
		 */
		if (mode != 0) {
			if ((str[idx] == ';')||(str[idx]=='}')) {
				/*
				 * the end of the command was detected, so process the
				 * command sequence.
				 */
				if (strncmp(cmd,"up",2) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
					}
					else {
						value = 1;
					}
					for (cmdix=0;cmdix<value;++cmdix) {
						if (`$INSTANCE_NAME`_CursorY > 0) {
							`$INSTANCE_NAME`_CursorY--;
						}
					}
				}
				else if (strncmp(cmd,"down",4) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
					}
					else {
						value = 1;
					}
					for (cmdix=0;cmdix<value;++cmdix) {
						if (`$INSTANCE_NAME`_CursorY < (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)) {
							`$INSTANCE_NAME`_CursorY++;
						}
					}
				}
				else if (strncmp(cmd,"right",5) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
					}
					else {
						value = 1;
					}
					for (cmdix=0;cmdix<value;++cmdix) {
						if (`$INSTANCE_NAME`_CursorX < (`$INSTANCE_NAME`_DISPLAY_WIDTH/6)) {
							`$INSTANCE_NAME`_CursorX++;
						}
					}
				}
				else if (strncmp(cmd,"left",4) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
					}
					else {
						value = 1;
					}
					for (cmdix=0;cmdix<value;++cmdix) {
						if (`$INSTANCE_NAME`_CursorX > 0) {
							`$INSTANCE_NAME`_CursorX--;
						}
					}
				}
				else if (strncmp(cmd,"clr",3) == 0) {
					if (cmd[3] == '1') {
						/* up to the current row */
						value = (`$INSTANCE_NAME`_CursorY*128);
						memset((void*)&`$INSTANCE_NAME`_Raster[1],0,value);
					}
					else if (cmd[3] == '2') {
						/* from current row down */
						value = (`$INSTANCE_NAME`_CursorY*128);
						memset((void*)&`$INSTANCE_NAME`_Raster[value+1],0,`$INSTANCE_NAME`_RASTER_SIZE-value);
					}
					else if (cmd[3] == '3') {
						/* clear current row */
						value = `$INSTANCE_NAME`_CursorY;
					}
					else if (cmd[3] == '4') {
						/* clear from current pos to end */
						value = ((`$INSTANCE_NAME`_CursorX*6) + (`$INSTANCE_NAME`_CursorY*128)) + 1;
						memset((void*)&`$INSTANCE_NAME`_Raster[value+1],0,128-(`$INSTANCE_NAME`_CursorX*6));
					}
					else if (cmd[3] == '5') {
						/* clear from current position to beginning */
						value = (`$INSTANCE_NAME`_CursorY*128);
						memset((void*)&`$INSTANCE_NAME`_Raster[value+1],0,`$INSTANCE_NAME`_CursorX*6);
					}
					else {
						/* clear whole display */
						memset((void*)&`$INSTANCE_NAME`_Raster[1],0,`$INSTANCE_NAME`_RASTER_SIZE);
					}

				}
				else if (strncmp(cmd,"row",3) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
						if (value > (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8)) {
							value = `$INSTANCE_NAME`_DISPLAY_HEIGHT/8 - 1;
						}
						else if (value < 1) {
							value = 1;
						}
					}
					else {
						value = 1;
					}
					`$INSTANCE_NAME`_CursorY = value - 1;
				}
				else if (strncmp(cmd,"col",3) == 0) {
					if (isdigit((int)cmd[3]) ) {
						sscanf(&cmd[3],"%d",&value);
						if (value > (`$INSTANCE_NAME`_DISPLAY_WIDTH/6)) {
							value = `$INSTANCE_NAME`_DISPLAY_WIDTH/6 - 1;
						}
						else if (value < 1) {
							value = 1;
						}
					}
					else {
						value = 1;
					}
					`$INSTANCE_NAME`_CursorX = value - 1;
				}
				else if (strncmp(cmd,"big",5) == 0) {
					expandX = 1;
					expandY = 1;
				}
				else if (strncmp(cmd,"wide",4) == 0) {
					expandX = 1;
					expandY = 0;
				}
				else if (strncmp(cmd,"tall",4) == 0) {
					expandX = 0;
					expandY = 1;
				}
				else if (strncmp(cmd,"normal",6) == 0) {
					expandX = 0;
					expandY = 0;
				}
				cmdix = 0;
				memset((void*)&cmd[0],0,21);
				if (str[idx] == '}') {
					mode = 0;
				}
			}
			else {
				if (cmdix < 20) {
					cmd[cmdix++] = tolower((int)str[idx]);
					cmd[cmdix] = 0;
				}
			}
		}
		else if (str[idx] == '{') {
			cmdix = 0;
			mode = 1;
		}
		else {
			if (( expandX != 0)||(expandY != 0) ) {
				`$INSTANCE_NAME`_SizedPutChar( str[idx],expandX,expandY );
			}
			else {
				`$INSTANCE_NAME`_PutChar( str[idx] );
			}
		}
		++idx;
	}
}

/* ------------------------------------------------------------------------ */
/* [] END OF FILE */
