#if !defined(`$INSTANCE_NAME`_H)
	#define `$INSTANCE_NAME`_H
/* ------------------------------------------------------------------------ */
/**
 * \defgroup e2forlife_w5500 W5500 Device Driver for PSoC
 * @{
 */
/**
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
 *     * Neither the name of E2ForLife.com nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file w5500.h
 * \author Chuck Erhardt (chuck@e2forlife.com)
 * 
 * Driver configuration, function prototypes, and chip register defines for
 * accessing the iW5500.
 */
/* ======================================================================== */

#include <cytypes.h>


/* Calculated memory size parameter to allocate more socket mem when using fewer sockets */
#define `$INSTANCE_NAME`_SOCKET_MEM         ( 2 )
/* Chip Configuration. Set to the maximum memory available for socket data */
#define `$INSTANCE_NAME`_CHIP_MEM           ( 16 )
#define `$INSTANCE_NAME`_CHIP_SOCKETS       ( 8 )
/* Chip Configuration (Calculated). This contains the calculated MAX sockets based on available memory */
#define `$INSTANCE_NAME`_MAX_SOCKETS        ( `$INSTANCE_NAME`_CHIP_MEM / `$INSTANCE_NAME`_SOCKET_MEM )
	
#define `$INSTANCE_NAME`_SOCKET_BAD(x)      ( (x >= `$INSTANCE_NAME`_MAX_SOCKETS)||(`$INSTANCE_NAME`_socketStatus[x] == `$INSTANCE_NAME`_SOCKET_AVAILALE) )


/* ------------------------------------------------------------------------ */
#define `$INSTANCE_NAME`_SOCKET_OPEN           ( 1 )
#define `$INSTANCE_NAME`_SOCKET_AVAILALE       ( 0 )

/* ------------------------------------------------------------------------ */
/* Macros & Function Defines */
/*
 * W5500 Register Map
 */
#define `$INSTANCE_NAME`_REG_MODE            ( 0x0000 )
#define `$INSTANCE_NAME`_REG_GAR             ( 0x0001 )
#define `$INSTANCE_NAME`_REG_SUBR            ( 0x0005 )
#define `$INSTANCE_NAME`_REG_SHAR            ( 0x0009 )
#define `$INSTANCE_NAME`_REG_SIPR            ( 0x000F )
#define `$INSTANCE_NAME`_REG_INTLEVEL        ( 0x0013 )
#define `$INSTANCE_NAME`_REG_IR              ( 0x0015 )
#define `$INSTANCE_NAME`_REG_IMR             ( 0x0016 )
#define `$INSTANCE_NAME`_REG_SIR             ( 0x0017 )
#define `$INSTANCE_NAME`_REG_SIMR            ( 0x0018 )
#define `$INSTANCE_NAME`_REG_RTR             ( 0x0019 )
#define `$INSTANCE_NAME`_REG_RCR             ( 0x001B )
#define `$INSTANCE_NAME`_REG_PTIMER          ( 0x001C )
#define `$INSTANCE_NAME`_REG_PMAGIC          ( 0x001D )
#define `$INSTANCE_NAME`_REG_PHAR            ( 0x001E )
#define `$INSTANCE_NAME`_REG_PSID            ( 0x0024 )
#define `$INSTANCE_NAME`_REG_PMRU            ( 0x0026 )
#define `$INSTANCE_NAME`_REG_UIPR            ( 0x0028 )
#define `$INSTANCE_NAME`_REG_UPORTR          ( 0x002C )
#define `$INSTANCE_NAME`_REG_PHYCFGR         ( 0x002E )
#define `$INSTANCE_NAME`_REG_VERSIONR        ( 0x0039 )

/* Socket Registers */
#define `$INSTANCE_NAME`_SREG_MR             ( 0x0000 )
#define `$INSTANCE_NAME`_SREG_CR             ( 0x0001 )
#define `$INSTANCE_NAME`_SREG_IR             ( 0x0002 )
#define `$INSTANCE_NAME`_SREG_SR             ( 0x0003 )
#define `$INSTANCE_NAME`_SREG_PORT           ( 0x0004 )
#define `$INSTANCE_NAME`_SREG_DHAR           ( 0x0006 )
#define `$INSTANCE_NAME`_SREG_DIPR           ( 0x000C )
#define `$INSTANCE_NAME`_SREG_DPORT          ( 0x0010 )
#define `$INSTANCE_NAME`_SREG_MSSR           ( 0x0012 )
#define `$INSTANCE_NAME`_SREG_TOS            ( 0x0015 )
#define `$INSTANCE_NAME`_SREG_TTL            ( 0x0016 )
#define `$INSTANCE_NAME`_SREG_RXBUF_SIZE     ( 0x001E )
#define `$INSTANCE_NAME`_SREG_TXBUF_SIZE     ( 0x001F )
#define `$INSTANCE_NAME`_SREG_TX_FSR         ( 0x0020 )
#define `$INSTANCE_NAME`_SREG_TX_RD          ( 0x0022 )
#define `$INSTANCE_NAME`_SREG_TX_WR          ( 0x0024 )
#define `$INSTANCE_NAME`_SREG_RX_RSR         ( 0x0026 ) 
#define `$INSTANCE_NAME`_SREG_RX_RD          ( 0x0028 )
#define `$INSTANCE_NAME`_SREG_RX_WR          ( 0x002A )
#define `$INSTANCE_NAME`_SREG_IMR            ( 0x002C )
#define `$INSTANCE_NAME`_FRAG                ( 0x002D )
#define `$INSTANCE_NAME`_KPALVTR             ( 0x002F )

/*
 * W5500 Commands
 */
#define `$INSTANCE_NAME`_CR_OPEN            ( 0x01 )
#define `$INSTANCE_NAME`_CR_LISTEN          ( 0x02 )
#define `$INSTANCE_NAME`_CR_CONNECT         ( 0x04 )
#define `$INSTANCE_NAME`_CR_DISCON          ( 0x08 )
#define `$INSTANCE_NAME`_CR_CLOSE           ( 0x10 )
#define `$INSTANCE_NAME`_CR_SEND            ( 0x20 )
#define `$INSTANCE_NAME`_CR_SEND_MAC        ( 0x21 )
#define `$INSTANCE_NAME`_CR_SEND_KEEP       ( 0x22 )
#define `$INSTANCE_NAME`_CR_RECV            ( 0x40 )

/*
 * W5500 Interrupt Flags
 */
#define `$INSTANCE_NAME`_IR_CON              ( 0x01 )
#define `$INSTANCE_NAME`_IR_DISCON           ( 0x02 )
#define `$INSTANCE_NAME`_IR_RECV             ( 0x04 )
#define `$INSTANCE_NAME`_IR_TIMEOUT          ( 0x08 )
#define `$INSTANCE_NAME`_IR_SEND_OK          ( 0x10 )

/*
 * Socket Status
 */
#define `$INSTANCE_NAME`_SR_CLOSED         ( 0x00 )
#define `$INSTANCE_NAME`_SR_INIT           ( 0x13 )
#define `$INSTANCE_NAME`_SR_LISTEN         ( 0x14 )
#define `$INSTANCE_NAME`_SR_ESTABLISHED    ( 0x17 )
#define `$INSTANCE_NAME`_SR_CLOSE_WAIT     ( 0x1C )
#define `$INSTANCE_NAME`_SR_UDP            ( 0x22 )
#define `$INSTANCE_NAME`_SR_MACRAW         ( 0x42 )

/*
 * timeout used to prevent command acceptance deadlock
 */
#define `$INSTANCE_NAME`_CMD_TIMEOUT         ( 125 )

/* Block select regions of the W5500 */
#define `$INSTANCE_NAME`_BLOCK_COMMON        (0x00)
#define `$INSTANCE_NAME`_BLOCK_S0_REG        (0x08)
#define `$INSTANCE_NAME`_BLOCK_S0_TXB        (0x10)
#define `$INSTANCE_NAME`_BLOCK_S0_RXB        (0x18)
#define `$INSTANCE_NAME`_BLOCK_S1_REG        (0x28)
#define `$INSTANCE_NAME`_BLOCK_S1_TXB        (0x30)
#define `$INSTANCE_NAME`_BLOCK_S1_RXB        (0x38)
#define `$INSTANCE_NAME`_BLOCK_S2_REG        (0x48)
#define `$INSTANCE_NAME`_BLOCK_S2_TXB        (0x50)
#define `$INSTANCE_NAME`_BLOCK_S2_RXB        (0x58)
#define `$INSTANCE_NAME`_BLOCK_S3_REG        (0x68)
#define `$INSTANCE_NAME`_BLOCK_S3_TXB        (0x70)
#define `$INSTANCE_NAME`_BLOCK_S3_RXB        (0x78)
#define `$INSTANCE_NAME`_BLOCK_S4_REG        (0x88)
#define `$INSTANCE_NAME`_BLOCK_S4_TXB        (0x90)
#define `$INSTANCE_NAME`_BLOCK_S4_RXB        (0x98)
#define `$INSTANCE_NAME`_BLOCK_S5_REG        (0xA8)
#define `$INSTANCE_NAME`_BLOCK_S5_TXB        (0xB0)
#define `$INSTANCE_NAME`_BLOCK_S5_RXB        (0xB8)
#define `$INSTANCE_NAME`_BLOCK_S6_REG        (0xC8)
#define `$INSTANCE_NAME`_BLOCK_S6_TXB        (0xD0)
#define `$INSTANCE_NAME`_BLOCK_S6_RXB        (0xD8)
#define `$INSTANCE_NAME`_BLOCK_S7_REG        (0xE8)
#define `$INSTANCE_NAME`_BLOCK_S7_TXB        (0xF0)
#define `$INSTANCE_NAME`_BLOCK_S7_RXB        (0xF8)

/* 
 * MACRO functions to calculate the socket region block selects
 * from the socket number.  These are helpful when working with
 * the socket based commands.
 */
#define `$INSTANCE_NAME`_SOCKET_BASE(s)                     (( (s*4) + 1 )<<3)
#define `$INSTANCE_NAME`_TX_BASE(s)                         (( (s*4) + 2 )<<3)
#define `$INSTANCE_NAME`_RX_BASE(s)                         (( (s*4) + 3 )<<3)

/* Socket Configuration FLags */
#define `$INSTANCE_NAME`_FLG_SKT_UDP_MULTICAST_ENABLE       ( 0x80 )
#define `$INSTANCE_NAME`_FLG_SKT_MACRAW_MAC_FILT_ENABLE     ( 0x80 )
#define `$INSTANCE_NAME`_FLG_SKT_BLOCK_BROADCAST            ( 0x40 )
#define `$INSTANCE_NAME`_FLG_SKT_ND_ACK                     ( 0x20 )
#define `$INSTANCE_NAME`_FLG_SKT_UDP_IGMP_V1                ( 0x20 )
#define `$INSTANCE_NAME`_FLG_SKT_MACRAW_MULTICAST_BLOCK     ( 0x20 )
#define `$INSTANCE_NAME`_FLG_SKT_UDP_BLOCK_UNICAST          ( 0x10 )
#define `$INSTANCE_NAME`_FLG_SKT_MACRAW_IPV6_BLOCKING       ( 0x10 )
/* Socket protocol configurations */
#define `$INSTANCE_NAME`_PROTO_CLOSED                       ( 0x00 )
#define `$INSTANCE_NAME`_PROTO_TCP                          ( 0x01 )
#define `$INSTANCE_NAME`_PROTO_UDP                          ( 0x02 )
#define `$INSTANCE_NAME`_PROTO_MACRAW                       ( 0x04 )

#define `$INSTANCE_NAME`_TXRX_FLG_WAIT                      ( 0x01 )

//#define `$INSTANCE_NAME`_IPADDRESS(x1,x2,x3,x4)   ( (uint32)(x1&0x000000FF) + (uint32)((x2<<8)&0x0000FF00) + (uint32)((x3<<16)&0x00FF0000) + ((uint32)(x4<<24)&0xFF000000 ))
	
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Send(uint16 offset, uint8 block_select, uint8 write, uint8 *buffer, uint16 len);

cystatus `$INSTANCE_NAME`_Start( void );
cystatus `$INSTANCE_NAME`_StartEx( const char *gateway, const char *subnet, const char *mac, const char *ip );
cystatus `$INSTANCE_NAME`_Init( uint8* gateway, uint8* subnet, uint8 *mac, uint8* ip );
void `$INSTANCE_NAME`_GetMac(uint8* mac);
uint32 `$INSTANCE_NAME`_GetIp( void );
uint16 `$INSTANCE_NAME`_GetTxLength(uint8 socket, uint16 len, uint8 flags);
cystatus `$INSTANCE_NAME`_WriteTxData(uint8 socket, uint8 *buffer, uint16 tx_length, uint8 flags);

uint16 `$INSTANCE_NAME`_RxDataReady( uint8 socket );
uint16 `$INSTANCE_NAME`_TxBufferFree( uint8 socket );
uint8 `$INSTANCE_NAME`_SocketOpen( uint16 port, uint8 flags );
cystatus `$INSTANCE_NAME`_SocketClose( uint8 sock, uint8 discon );
cystatus `$INSTANCE_NAME`_SocketDisconnect( uint8 sock );
cystatus `$INSTANCE_NAME`_SocketSendComplete( uint8 socket );
cystatus `$INSTANCE_NAME`_ExecuteSocketCommand(uint8 socket, uint8 cmd );

cystatus `$INSTANCE_NAME`_TcpConnected( uint8 sock );
uint8 `$INSTANCE_NAME`_TcpOpenClient( uint16 port, uint32 remote_ip, uint16 remote_port );
uint8 `$INSTANCE_NAME`_TcpOpenServer(uint16 port);
cystatus `$INSTANCE_NAME`_TcpWaitForConnection( uint8 socket );
uint16 `$INSTANCE_NAME`_TcpSend( uint8 socket, uint8* buffer, uint16 len, uint8 flags);
void `$INSTANCE_NAME`_TcpPrint(uint8 socket, const char *string );
uint16 `$INSTANCE_NAME`_TcpReceive(uint8 socket, uint8* buffer, uint16 len, uint8 flags);
char `$INSTANCE_NAME`_TcpGetChar( uint8 socket );
int `$INSTANCE_NAME`_TcpGetLine( uint8 socket, char *buffer );

uint8 `$INSTANCE_NAME`_UdpOpen( uint16 port );
uint16 `$INSTANCE_NAME`_UdpSend(uint8 socket, uint32 ip, uint16 port, uint8 *buffer, uint16 len, uint8 flags);
uint16 `$INSTANCE_NAME`_UdpReceive(uint8 socket, uint8 *header, uint8 *buffer, uint16 len, uint8 flags);


uint32 `$INSTANCE_NAME`_ParseIP( const char* ipString );
cystatus `$INSTANCE_NAME`_ParseMAC(const char *macString, uint8 *mac);
void `$INSTANCE_NAME`_StringMAC(uint8 *mac, char *macString);
void `$INSTANCE_NAME`_StringIP( uint32 ip, char *ipString );
int `$INSTANCE_NAME`_Base64Encode(const void* data_buf, int dataLength, char* result, int resultSize);
int `$INSTANCE_NAME`_Base64Decode (char *in, int inLen, uint8 *out, int *outLen);
uint32 `$INSTANCE_NAME`_IPADDRESS(uint8 x1, uint8 x2, uint8 x3, uint8 x4 );

#endif
/** @} */
/* [] END OF FILE */
