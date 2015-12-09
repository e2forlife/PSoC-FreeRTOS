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
#if !defined(`$INSTANCE_NAME`_H)
	#define `$INSTANCE_NAME`_H
	
#include <cytypes.h>
	
#define `$INSTANCE_NAME`_LCDHEIGHT     ( 64 )
#define `$INSTANCE_NAME`_LCDWIDTH      ( 128 )

#define `$INSTANCE_NAME`_CMD_SETCOLADR              ( 0x15 )
#define `$INSTANCE_NAME`_CMD_SETROWADR              ( 0x75 )
#define `$INSTANCE_NAME`_CMD_SETCONTRAST            ( 0x81 )
#define `$INSTANCE_NAME`_CMD_SETCURRENT             ( 0x84 )

#define `$INSTANCE_NAME`_CMD_SETREMAP               ( 0xA0 )
#define `$INSTANCE_NAME`_CMD_SETSTARTLINE           ( 0xA1 )
#define `$INSTANCE_NAME`_CMD_SETOFFSET              ( 0xA2 )
#define `$INSTANCE_NAME`_CMD_NORMALDISPLAY          ( 0xA4 )
#define `$INSTANCE_NAME`_CMD_DISPLAYALLON           ( 0xA5 )
#define `$INSTANCE_NAME`_CMD_DISPLAYALLOFF          ( 0xA6 )
#define `$INSTANCE_NAME`_CMD_INVERTDISPLAY          ( 0xA7 )
#define `$INSTANCE_NAME`_CMD_SETMULTIPLEX           ( 0xA8 )
#define `$INSTANCE_NAME`_CMD_MASTERCONFIG           ( 0xAD )
#define `$INSTANCE_NAME`_CMD_DISPLAYOFF             ( 0xAE )
#define `$INSTANCE_NAME`_CMD_DISPLAYON              ( 0xAF )

#define `$INSTANCE_NAME`_CMD_SETPRECHARGECOMPENABLE ( 0xB0 )
#define `$INSTANCE_NAME`_CMD_SETPHASELEN            ( 0xB1 )
#define `$INSTANCE_NAME`_CMD_SETROWPERIOD           ( 0xB2 )
#define `$INSTANCE_NAME`_CMD_SETCLOCK               ( 0xB3 )
#define `$INSTANCE_NAME`_CMD_SETPRECHARGECOMP       ( 0xB4 )
#define `$INSTANCE_NAME`_CMD_SETGRAYTABLE           ( 0xB8 )
#define `$INSTANCE_NAME`_CMD_SETPRECHARGEVOLTAGE    ( 0xBC )
#define `$INSTANCE_NAME`_CMD_SETVCOMLEVEL           ( 0xBE )
#define `$INSTANCE_NAME`_CMD_SETVSL                 ( 0xBF )

#define `$INSTANCE_NAME`_CMD_GFXACCEL               ( 0x23 )
#define `$INSTANCE_NAME`_CMD_DRAWRECT               ( 0x24 )
#define `$INSTANCE_NAME`_CMD_COPY                   ( 0x25 )

void `$INSTANCE_NAME`_Init( void );
void `$INSTANCE_NAME`_Start( void );
void `$INSTANCE_NAME`_DrawPixel( uint16 x, uint16 y, uint16 color );
void `$INSTANCE_NAME`_Command( uint8 cmd );
void `$INSTANCE_NAME`_Data( uint8 color );
void `$INSTANCE_NAME`_Refresh( void );
void `$INSTANCE_NAME`_ClearDisplay( void );
void `$INSTANCE_NAME`_ClearLine( uint8 mode );
cystatus `$INSTANCE_NAME`_PutChar( char c );
cystatus `$INSTANCE_NAME`_PrintString( const char *str );
void `$INSTANCE_NAME`_Send( uint8 value );

#endif

/* [] END OF FILE */
