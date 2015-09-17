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
#if !defined( `$INSTANCE_NAME`_H )
	#define `$INSTANCE_NAME`_H
	
#include <cytypes.h>

#define `$INSTANCE_NAME`_DISPLAY_WIDTH           ( 128 )
#define `$INSTANCE_NAME`_DISPLAY_HEIGHT          ( 32 )
#define `$INSTANCE_NAME`_RASTER_SIZE             ( `$INSTANCE_NAME`_DISPLAY_WIDTH * (`$INSTANCE_NAME`_DISPLAY_HEIGHT/8))
#define `$INSTANCE_NAME`_I2C_ADDRESS             ( 0x3C )
#define `$INSTANCE_NAME`_FONT_INCLUDE_CTRL_CHARS ( 1 )	
#define `$INSTANCE_NAME`_FONT_INCLUDE_LOWERCASE  ( 1 )
#define `$INSTANCE_NAME`_FONT_INCLUDE_EXT256     ( 1 )
	
void `$INSTANCE_NAME`_Start( void );
void `$INSTANCE_NAME`_Init( void );
void `$INSTANCE_NAME`_Enable( void );
void `$INSTANCE_NAME`_SendCommand( uint8 command );
void `$INSTANCE_NAME`_Refresh( void );
void `$INSTANCE_NAME`_PutChar( char c );
void `$INSTANCE_NAME`_BigPutChar( char c );
void `$INSTANCE_NAME`_PrintString( char *str );

#endif
/* [] END OF FILE */
