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
#if !defined(`$INSTANCE_NAME`_H)
	#define `$INSTANCE_NAME`_H

#include <cytypes.h>
	
/* ======================================================================== */
#define `$INSTANCE_NAME`_SOCK_MULTICAST      ( 0x80 )
#define `$INSTANCE_NAME`_SOCK_MACFILT        ( 0x40 )
#define `$INSTANCE_NAME`_SOCK_NDMC           ( 0x20 )
#define `$INSTANCE_NAME`_SOCK_TCP            ( 0x01 )
#define `$INSTANCE_NAME`_SOCK_UDP            ( 0x02 )
#define `$INSTANCE_NAME`_SOCK_IP             ( 0x03 )
#define `$INSTANCE_NAME`_SOCK_MAC            ( 0x04 )
	
#define `$INSTANCE_NAME`_CMD_OPEN            ( 0x01 )
#define `$INSTANCE_NAME`_CMD_LISTEN          ( 0x02 )
#define `$INSTANCE_NAME`_CMD_CONNECT         ( 0x04 )
#define `$INSTANCE_NAME`_CMD_DISCON          ( 0x08 )
#define `$INSTANCE_NAME`_CMD_CLOSE           ( 0x10 )
#define `$INSTANCE_NAME`_CMD_SEND            ( 0x20 )
#define `$INSTANCE_NAME`_CMD_SEND_MAC        ( 0x21 )
#define `$INSTANCE_NAME`_CMD_SEND_KEEP       ( 0x22 )
#define `$INSTANCE_NAME`_CMD_RECV            ( 0x40 )
	
#define `$INSTANCE_NAME`_STS_CLOSED          ( 0x00 )
#define `$INSTANCE_NAME`_STS_INIT            ( 0x13 )
#define `$INSTANCE_NAME`_STS_LISTEN          ( 0x14 )
#define `$INSTANCE_NAME`_STS_ESTABLISHED     ( 0x17 )
#define `$INSTANCE_NAME`_STS_CLOSE_WAIT      ( 0x1C )
#define `$INSTANCE_NAME`_STS_UDP             ( 0x22 )
#define `$INSTANCE_NAME`_STS_IPRAW           ( 0x32 )
#define `$INSTANCE_NAME`_STS_MACRAW          ( 0x42 )
	
#define `$INSTANCE_NAME`_IR_CONNECT          ( 0x01 )
#define `$INSTANCE_NAME`_IR_DISCON           ( 0x02 )
#define `$INSTANCE_NAME`_IR_RECV             ( 0x04 )
#define `$INSTANCE_NAME`_IR_TIMEOUT          ( 0x08 )
#define `$INSTANCE_NAME`_IR_SENDOK           ( 0x10 )

/* ======================================================================== */
/* Hardware access */
cystatus `$INSTANCE_NAME`_HW_ChipAccess( uint16 adr, uint8 *buffer, uint16 len, uint8 write);
/* ======================================================================== */
/* Utility Functions */
cystatus `$INSTANCE_NAME`_utlIPAddress( const char *ipString, uint32 *ip );
cystatus `$INSTANCE_NAME`_utlMACAddress(const char *macString, uint8 *mac);
uint8 `$INSTANCE_NAME`_utlLinkStatus( void );
cystatus `$INSTANCE_NAME`_utlPHYPowerMode( uint8 pwrdwn );

/* ======================================================================== */
uint8 `$INSTANCE_NAME`_SocketOpen(uint16 port, uint8 socketParam);
cystatus `$INSTANCE_NAME`_SocketClose(uint8 skt );
uint8 `$INSTANCE_NAME`_SocketStatus( uint8 skt );
cystatus `$INSTANCE_NAME`_SocketCommand(uint8 skt, uint8 cmd);
uint16 `$INSTANCE_NAME`_SocketDataWaiting(uint8 skt);
uint16 `$INSTANCE_NAME`_SocketTxFree(uint8 skt);

cystatus `$INSTANCE_NAME`_SocketSend(uint8 skt, uint8 *packet, uint16 len);

uint8 `$INSTANCE_NAME`_TcpOpenServer(uint16 port);

/* ======================================================================== */
/* Initialization and common code */
cystatus `$INSTANCE_NAME`_InitializeSocket(uint8 sock);
cystatus `$INSTANCE_NAME`_Initialize( void );
cystatus `$INSTANCE_NAME`_Start( void );

/* ======================================================================== */
#endif
/* [] END OF FILE */
