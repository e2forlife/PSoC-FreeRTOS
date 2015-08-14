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
#if !defined (`$INSTANCE_NAME`_H)
	#define `$INSTANCE_NAME`_H

#include <cytypes.h>
	
#define `$INSTANCE_NAME`_YES            ( 1 )
#define `$INSTANCE_NAME`_NO             ( 0 )

#define `$INSTANCE_NAME`_BLOCKING_SEND   ( `$USE_BLOCKING_SEND` )
#define `$INSTANCE_NAME`_BLOCKING_GETS   ( `$INSTANCE_NAME`_NO )
#define `$INSTANCE_NAME`_USB_MODE        ( `$USING_USB` )
	
void `$INSTANCE_NAME`_Start( void );
void `$INSTANCE_NAME`_Init( void );
void `$INSTANCE_NAME`_Enable( void );

void `$INSTANCE_NAME`_Task( void *pvParameters );



char `$INSTANCE_NAME`_GetChar( void );
cystatus `$INSTANCE_NAME`_PutChar( char ch );
cystatus `$INSTANCE_NAME`_PrintString( const char *str );
cystatus `$INSTANCE_NAME`_SetColor( uint8 fg, uint8 bg );
cystatus `$INSTANCE_NAME`_ClearLine(uint8 mode);
cystatus `$INSTANCE_NAME`_Position(uint8 row, uint8 col);
//cystatus `$INSTANCE_NAME`_PrintStringColor(const char *str, uint8 fg, uint8 bg);
cystatus `$INSTANCE_NAME`_GetString( char *str );
uint16 `$INSTANCE_NAME`_ScanKey( void );

#define `$INSTANCE_NAME`_HideCursor        ( `$INSTANCE_NAME`_PutString("\x1b[?25l") )
#define `$INSTANCE_NAME`_ShowCursor        ( `$INSTANCE_NAME`_PutString("\x1b[?25h") )
#define `$INSTANCE_NAME`_SaveCursor        ( `$INSTANCE_NAME`_PutString("\x1b[s") )
#define `$INSTANCE_NAME`_RestoreCursor     ( `$INSTANCE_NAME`_PutString("\x1b[u") )

/* The size of the buffer is equal to maximum packet size of the 
*  IN and OUT bulk endpoints. 
*/
#define `$INSTANCE_NAME`_BUFFER_LEN   ( 64u )
#define `$INSTANCE_NAME`_RX_SIZE      ( `$RX_SIZE` )
#define `$INSTANCE_NAME`_TX_SIZE      ( `$TX_SIZE` )

/* ------------------------------------------------------------------------ */
#define `$INSTANCE_NAME`_KEY_UP         ( 'A' )
#define `$INSTANCE_NAME`_KEY_DOWN       ( 'B' )
#define `$INSTANCE_NAME`_KEY_LEFT       ( 'D' )
#define `$INSTANCE_NAME`_KEY_RIGHT      ( 'C' )

#define `$INSTANCE_NAME`_KEY_CSI       ( 0x0100 )
#define `$INSTANCE_NAME`_KEY_CTRL      ( 0x0200 )
#define `$INSTANCE_NAME`_KEY_SHFT      ( 0x0300 )

/* ------------------------------------------------------------------------ */

#endif
/* [] END OF FILE */
