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
#include <stdio.h>
#include "`$SPI`.h"
#include "`$INSTANCE_NAME`.h"

#include "`$CSN`.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_semphr.h"
#include "`$FreeRTOS`_queue.h"
#include "`$FreeRTOS`_task.h"

/* ======================================================================== */
#if (`$NUM_SOCKETS` == 1)
	const uint8 `$INSTANCE_NAME`_memSize = 16;
#elif (`$NUM_SOCKETS` == 2)
	const uint8 `$INSTANCE_NAME`_memSize = 8;
#elif (`$NUM_SOCKETS` == 3)
	const uint8 `$INSTANCE_NAME`_memSize = 4;
#elif (`$NUM_SOCKETS` == 4)
	const uint8 `$INSTANCE_NAME`_memSize = 4;
#elif (`$NUM_SOCKETS` == 5)
	const uint8 `$INSTANCE_NAME`_memSize = 2;
#elif (`$NUM_SOCKETS` == 6)
	const uint8 `$INSTANCE_NAME`_memSize = 2;
#elif (`$NUM_SOCKETS` == 7)
	const uint8 `$INSTANCE_NAME`_memSize = 2;
#elif (`$NUM_SOCKETS` == 8)
	const uint8 `$INSTANCE_NAME`_memSize = 2;
#else
	#error Unsupported number of sockets selected.
#endif


xSemaphoreHandle `$INSTANCE_NAME`_ControllerMutex;

uint8 `$INSTANCE_NAME`_Sockets[8];
uint8 `$INSTANCE_NAME`_SubnetMask[4];

#define `$INSTANCE_NAME`_DISABLE()      ( `$CSN`_Write(`$CSN`_Read() |   (1<<`$SS_NUM`)) )
#define `$INSTANCE_NAME`_ENABLE()       ( `$CSN`_Write(`$CSN`_Read() & (~(1<<`$SS_NUM`))) )

#define `$INSTANCE_NAME`_TX_BASE(s)     ( (s<`$NUM_SOCKETS`)?(0x8000 + ((s*4*`$INSTANCE_NAME`_memSize)*256)):(0x8000) )
#define `$INSTANCE_NAME`_RX_BASE(s)     ( (s<`$NUM_SOCKETS`)?(0xC000 + ((s*4*`$INSTANCE_NAME`_memSize)*256)):(0xC000) )
#define `$INSTANCE_NAME`_SKT_BASE(s)    (0x4000 | (uint16)(s<<8) )

/* ------------------------------------------------------------------------ */
/* HEX digit conversion tools for MAC Address parsing */
#define `$INSTANCE_NAME`_ISXDIGIT(x) \
( ((x>='0')&&(x<='9'))||((x>='a')&&(x<='f'))||((x>='A')&&(x<='F')) )
/* ------------------------------------------------------------------------ */
#define `$INSTANCE_NAME`_HEX2BIN(x) \
( ((x>='0')&&(x<='9'))? (x-'0') : \
  ((x>='a')&&(x<='f'))? ((x-'a')+10) : \
  ((x>='A')&&(x<='F'))? ((x-'A')+10) : 0 )
/* ------------------------------------------------------------------------ */
#define `$INSTANCE_NAME`_BIN2HEX(x) \
( (x>9)? ((x-10)+'A') : (x + '0'))

/* ======================================================================== */
/* Low-level chip access and protocol functions */

/* ------------------------------------------------------------------------ */
/**
 * \brief Thread-safe enable of the W5200 device.
 * This function attempts to acquire the mutex for the SPI device, then,
 * once aquired, selects the SPI component.
 */
cystatus `$INSTANCE_NAME`_HW_EnableDevice( void )
{
	extern xSemaphoreHandle `$SPI_MUTEX`;
	
	if (xSemaphoreTake(`$SPI_MUTEX`, portMAX_DELAY) == pdPASS) {
		/*
		 * Wait for the SPI resource to be IDLE state or DONE state
		 */
		while ( (`$SPI`_ReadTxStatus() & 0x11) == 0 );
		/* Enable the W5200 device chip select to prepare for data transfer */
		`$INSTANCE_NAME`_ENABLE();
		return(CYRET_SUCCESS);
	}

	return(CYRET_TIMEOUT);
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Disable the W5200 and release the mutex to free the SPI resource
 * This function deselects the W5200 device by raising the chip select line
 * high, then releases the Mutex held on the SPI port.
 */
cystatus `$INSTANCE_NAME`_HW_DisableDevice( void )
{
	extern xSemaphoreHandle `$SPI_MUTEX`;
	/*
	 * Wait for the transmission to complete before disabling the W5200 chip
	 * select.  This prevents the last byte from becoming clipped and causing
	 * errors in data block transfers.
	 */
	while ( (`$SPI`_ReadTxStatus() & 0x11) == 0 );
	/*
	 * Disable the chip select, then return the MUTEX to allow access to the
	 * SPI port from other tasks.
	 */
	`$INSTANCE_NAME`_DISABLE();
	if (xSemaphoreGive(`$SPI_MUTEX`) == pdPASS) {
		return(CYRET_SUCCESS);
	}

	return(CYRET_TIMEOUT);
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_HW_ChipAccess( uint16 adr, uint8 *buffer, uint16 len, uint8 write)
{
	uint32 count;
	uint8 status;
	
	/* fault checking. Quit if bad data, parametes, or voids are passed */
	if (len == 0) return CYRET_BAD_PARAM;
	if (len > 0x7FFF) return CYRET_BAD_PARAM;
	if (buffer == NULL) return CYRET_BAD_PARAM;
	if (`$INSTANCE_NAME`_HW_EnableDevice() != CYRET_SUCCESS) return CYRET_TIMEOUT;

	`$SPI`_ClearFIFO();
	
	/*  Send the address to the W5200 */
	`$SPI`_WriteTxData( HI8(adr) );
	`$SPI`_WriteTxData( LO8(adr) );
	/* Send the opcode to the W5200 */
	if (write) {
		`$SPI`_WriteTxData( 0x80 | HI8(len) );
	}
	else {
		`$SPI`_WriteTxData( HI8(len) & 0x7F);
	}
	`$SPI`_WriteTxData( LO8(len) );
	
	/*
	 * Now the header has been transmitted to the device, perform the data block
	 * writes.
	 */
	/* Clear the header from the data buffer */
	count = 0;
	do {
		status = `$SPI`_GetRxBufferSize();
		if (status > 0) {
			`$SPI`_ReadRxData();
			++count;
		}
	}
	while (count < 4);

	for (count = 0; count < len;++count) {
		/* wait for room in the tx buffer */
		do {
			status = `$SPI`_GetTxBufferSize();
		}
		while (status >= 4);
		
		`$SPI`_WriteTxData( (write!=0) ?buffer[count] : 0xFF );

		/* wait for response data */
		do {
			status = `$SPI`_GetRxBufferSize();
		}
		while (status == 0);
		status = `$SPI`_ReadRxData();
		if( write==0) {
			buffer[count] = status;
		}
	}
	/*
	 * Received all of the bytes we should have, so clear the FIFOs
	 */
	`$SPI`_ClearFIFO();
	/* Free Mutex and disable chip */
	return `$INSTANCE_NAME`_HW_DisableDevice();
}
/* ======================================================================== */
cystatus `$INSTANCE_NAME`_utlIPAddress(const char *ipString, uint32 *ip )
{
	/*
	 * Parse a human readable string in to a IP address usable by the hardare
	 */
	int index;
	char digit[5];
	uint8 value[4];
	int counter;
	int dindex;
	
	index = 0;
	dindex = 0;
	counter = 0;
	
	while ( (counter < 4) && ((unsigned int)index < strlen(ipString) ) ) {
		if ( (ipString[index] >= '0' ) && (ipString[index] <= '9') ) {
			if (dindex > 3) {
				*ip = 0xFFFFFFFF;
				return CYRET_BAD_PARAM;
			}
			digit[dindex++] = ipString[index];
			
		}
		
		if ( (ipString[index] == '.') || (ipString[index+1] == 0) ) {
			digit[dindex] = 0;
			dindex = 0;
			/* convert the value and store in the buffer */
			value[counter] = 0;
			while (digit[dindex] != 0) {
				value[counter] = (value[counter]*10) + (digit[dindex]-'0');
				++dindex;
			}
			/* reset the digit index to start accumulating digits again*/
			dindex = 0;
			/* move to the next address byte */
			++counter;
		}
		++index;
	}
	
	if (counter != 4) {
		*ip = 0xFFFFFFFF;
		return CYRET_BAD_DATA;
	}
	else {
		*ip = *((uint32*)&value[0]);
		return CYRET_SUCCESS;
	}
}
/* ------------------------------------------------------------------------ */
/**
 * /brief Parse a MAC Address string in to a 6-byte mac address
 * /param *macString Pointer to the ASCII-Z String containing the MAC address
 * /param *mac Pointer to the 6-byte array to hold the output mac addres
 */
cystatus `$INSTANCE_NAME`_utlMACAddress(const char *macString, uint8 *mac)
{
	/* 
	 * a mac address is specified as a string of 6 hex bytes with
	 * dashes ('-') seperating the bytes.  An invalidly formed
	 * address will only process the values up the error and return BAD_DATA
	 * otherwise, SUCESS is returned.
	 */
	int digit;
	int index;
	cystatus result;
	
	result = (macString[0]!=0)?CYRET_SUCCESS:CYRET_BAD_DATA;
	index = 0;
	for(digit = 0;(digit<6) && (result == CYRET_SUCCESS)&&(macString[index] != 0);++digit) {
		// process the first nibble
		if (`$INSTANCE_NAME`_ISXDIGIT(macString[index]) ) {
			mac[digit] = `$INSTANCE_NAME`_HEX2BIN(macString[index]);
			++index;
			mac[digit] <<= 4;
			if (`$INSTANCE_NAME`_ISXDIGIT(macString[index])) {
				mac[digit] += `$INSTANCE_NAME`_HEX2BIN(macString[index]);
				++index;
				/*
				 * now for digits other than digit 5 (the last one) look for
				 * the dash seperator.  If there is no dash, return bad data
				 */
				if (digit<5) {
					if ((macString[index]!=':')&&(macString[index]!='-')) {
						result = CYRET_BAD_DATA;
					}
					++index; // move conversion pointer to the next value
				}
			}
			else {
				result = CYRET_BAD_DATA;
			}
		}
		else {
			result = CYRET_BAD_DATA;
		}
	}
	return( result );
}
/* ------------------------------------------------------------------------ */
/**
 * \brief REturn the status of the Link adn PHY
 */
uint8 `$INSTANCE_NAME`_utlLinkStatus( void )
{
	uint8 sts;
	
	
	if (`$INSTANCE_NAME`_HW_ChipAccess(0x35,&sts,1,0) != CYRET_SUCCESS) {
		sts = 0;
	}
	return sts&0x28; /* return the status bits with the reserved bit zeroed */
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Set the PHY power down mode
 * \param pwrdwn non-zero value writes the PHY in to power-dowm mode
 */
cystatus `$INSTANCE_NAME`_utlPHYPowerMode( uint8 pwrdwn )
{
	pwrdwn = (pwrdwn == 0)?0:0x08;
	return `$INSTANCE_NAME`_HW_ChipAccess(0x35,&pwrdwn,1,1);
}
/* ======================================================================== */
cystatus `$INSTANCE_NAME`_InitializeSocket(uint8 sock)
{
	uint8 cfg[2];
	uint8 verify[2];
	uint16 adr;
	uint16 sockBase;
	uint8 ir;
	
	adr = 0x401E | (uint16)(sock<<8);
	if (sock < `$NUM_SOCKETS`) {
		/* Compute the memory length from table lookup */
		cfg[0] = `$INSTANCE_NAME`_memSize;
		cfg[1] = `$INSTANCE_NAME`_memSize;
		`$INSTANCE_NAME`_Sockets[sock] = 0;
	}
	else {
		/* Socket is invalid, so set the mem ptrs to remove it from the map */
		cfg[0] = 0;
		cfg[1] = 0;
		`$INSTANCE_NAME`_Sockets[sock] = 0xFF;
	}
		
	`$INSTANCE_NAME`_HW_ChipAccess(adr,&cfg[0], 2, 1);
	`$INSTANCE_NAME`_HW_ChipAccess(adr,&verify[0],2,0);

	/* Initialize memory pointers */
	sockBase = `$INSTANCE_NAME`_TX_BASE(sock);
	sockBase = CYSWAP_ENDIAN16(sockBase);
	`$INSTANCE_NAME`_HW_ChipAccess(adr+4,(uint8*)&sockBase,2,1);
	`$INSTANCE_NAME`_HW_ChipAccess(adr+6,(uint8*)&sockBase,2,1);
	sockBase = `$INSTANCE_NAME`_RX_BASE(sock);
	sockBase = CYSWAP_ENDIAN16(sockBase);
	`$INSTANCE_NAME`_HW_ChipAccess(adr+10,(uint8*)&sockBase,2,1);
	`$INSTANCE_NAME`_HW_ChipAccess(adr+12,(uint8*)&sockBase,2,1);
	/* Clear all pending interrupts */
	ir = 0xFF;
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(sock)+2,&ir,1,1);
	/* 
	 * Setup the interrupt mask for the socket. this is used to determine that 
	 * send data has completed, and verifying that a connection is still valid.
	 */
	ir = `$INSTANCE_NAME`_IR_SENDOK | `$INSTANCE_NAME`_IR_TIMEOUT | `$INSTANCE_NAME`_IR_DISCON;
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(sock)+0x002C,&ir,1,1);
	
	return ((cfg[0]==verify[0])&&(cfg[1]==verify[1]))?CYRET_SUCCESS : CYRET_BAD_DATA;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_Initialize( void )
{
	uint8 cfg[18];
	uint8 rsp[18];
	int idx;
	
	if (`$INSTANCE_NAME`_utlIPAddress("`$GATEWAY`",(uint32*)&cfg[0]) != CYRET_SUCCESS) return CYRET_BAD_DATA;
	if (`$INSTANCE_NAME`_utlIPAddress("`$SUBNET`",(uint32*)&`$INSTANCE_NAME`_SubnetMask) != CYRET_SUCCESS) return CYRET_BAD_DATA;
	if (`$INSTANCE_NAME`_utlMACAddress("`$MAC`",&cfg[8]) != CYRET_SUCCESS) return CYRET_BAD_DATA;
	if (`$INSTANCE_NAME`_utlIPAddress("`$IP`",(uint32*)&cfg[14])  != CYRET_SUCCESS) return CYRET_BAD_DATA;
	
	/* assign default subnet mask as EVERYTHING! (errata) */
	cfg[4] = 0;
	cfg[5] = 0;
	cfg[6] = 0;
	cfg[7] = 0;
	
	/* Write the common configuration registers */
	`$INSTANCE_NAME`_HW_ChipAccess(1,&cfg[0],18,1);
	/* Execute a confirmation readback to verify register contents */
	`$INSTANCE_NAME`_HW_ChipAccess(1,&rsp[0],18,0);
	
	idx = 0;
	for (idx=0;((idx<19) && (cfg[idx] == rsp[idx]));++idx);
	if (idx<19) {
		return CYRET_BAD_DATA;
	}
	
	/* Socket Initialization */
	for(idx=0;idx<8;++idx) {
		`$INSTANCE_NAME`_InitializeSocket(idx);
	}
	
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_Start( void )
{
	/*
	 * The network driver requires that the SPI hardware has been initialized
	 * and running, so when the SPI port has not yet been activated, enable the
	 * SPI port.
	 */
	if (`$SPI`_initVar != 1u) {
		`$SPI`_Start();
	}
	/* create a mutex for access control to MAC data area */
	`$INSTANCE_NAME`_ControllerMutex = xSemaphoreCreateMutex();
	
	return `$INSTANCE_NAME`_Initialize();
}
/* ======================================================================== */
/* Basic Socket Functionality */
uint8 `$INSTANCE_NAME`_SocketOpen(uint16 port, uint8 socketParam)
{
	uint16 adr;
	uint8 skt;
	uint8 cfg[2];
	uint32 timeout;
	
	/* Aquire the Mutex */
	if (xSemaphoreTake(`$INSTANCE_NAME`_ControllerMutex,portMAX_DELAY) == pdPASS) {
		for (skt=0;(skt<`$NUM_SOCKETS`)&&(`$INSTANCE_NAME`_Sockets[skt] != 0);++skt);
		if (skt < `$NUM_SOCKETS`) {
			adr = 0x4000 + (skt*256);
			`$INSTANCE_NAME`_InitializeSocket(skt);
			cfg[0] = HI8(port);
			cfg[1] = LO8(port);
			`$INSTANCE_NAME`_HW_ChipAccess(adr+4,&cfg[0],2,1);
			cfg[0] = socketParam;
			cfg[1] = `$INSTANCE_NAME`_CMD_OPEN;
			`$INSTANCE_NAME`_HW_ChipAccess(adr,&cfg[0],2,1);
			for(timeout=0;(timeout<1000)&&(cfg[1]!=0);++timeout) {
				`$INSTANCE_NAME`_HW_ChipAccess(adr+1,&cfg[1],1,0);
				if (cfg[1] != 0) {
					vTaskDelay(1/portTICK_RATE_MS);
				}
			}
			if (timeout < 1000) {
				`$INSTANCE_NAME`_Sockets[skt] = socketParam;
			}
			else {
				skt = 0xFF;
			}
		}
		else {
			skt = 0xFF;
		}
		xSemaphoreGive(`$INSTANCE_NAME`_ControllerMutex);
	}
	else {
		skt = 0xFF;
	}
	return skt;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_SocketClose(uint8 skt)
{
	cystatus result;
	uint8 cmd;
	uint16 base;
	uint32 timeout;
	
	if (skt >= `$NUM_SOCKETS`) return CYRET_BAD_PARAM;
	
	base = 0x4000 + (skt*256);
	result = CYRET_SUCCESS;
	/* Aquire mutex to access socket table */
	if (xSemaphoreTake(`$INSTANCE_NAME`_ControllerMutex,portMAX_DELAY) == pdPASS) {
		if (`$INSTANCE_NAME`_Sockets[skt] != 0) {
			`$INSTANCE_NAME`_HW_ChipAccess(base+3,&cmd,1,0);
			if ((cmd = `$INSTANCE_NAME`_STS_ESTABLISHED)&&((`$INSTANCE_NAME`_Sockets[skt]&0x0F) == 0x01) ) {
				cmd = `$INSTANCE_NAME`_CMD_DISCON;
			}
			else {
				cmd = `$INSTANCE_NAME`_CMD_CLOSE;
			}
			`$INSTANCE_NAME`_HW_ChipAccess(base+1,&cmd,1,1);
			for(timeout=0;(timeout<1000)&&(cmd!=0);++timeout) {
				`$INSTANCE_NAME`_HW_ChipAccess(base+1,&cmd,1,0);
				if (cmd != 0) {
					vTaskDelay(1/portTICK_RATE_MS);
				}
			}
			if (timeout < 1000) {
				do {
					`$INSTANCE_NAME`_HW_ChipAccess(base+3,&cmd,1,0);
					if (cmd != 0) {
						vTaskDelay(1/portTICK_RATE_MS);
					}
				}
				while (cmd != 0);
				`$INSTANCE_NAME`_Sockets[skt] = 0;
			}
			else {
				result = CYRET_TIMEOUT;
			}
			
		}
		else {
			result = CYRET_BAD_PARAM;
		}
	}
	else {
		return CYRET_BAD_DATA;
	}
	xSemaphoreGive(`$INSTANCE_NAME`_ControllerMutex);
	return result;
}
/* ------------------------------------------------------------------------ */
uint8 `$INSTANCE_NAME`_SocketStatus( uint8 skt )
{
	uint8 result;
	
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+3,&result,1,0);
	return result;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_SocketCommand(uint8 skt, uint8 cmd)
{
	uint32 timeout;
	uint8 result;
	uint16 adr;
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 1;
	timeout = 0;
	`$INSTANCE_NAME`_HW_ChipAccess(adr,&cmd,1,1);
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,&result,1,0);
		if (result != 0) {
			++timeout;
			vTaskDelay(1/portTICK_RATE_MS);
		}
	}
	while ( (timeout < 1000) && (result != 0) );
	
	return (timeout>=1000)?CYRET_TIMEOUT:CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
uint8 `$INSTANCE_NAME`_TcpOpenServer(uint16 port)
{
	uint8 skt;
	
	skt = `$INSTANCE_NAME`_SocketOpen(port, `$INSTANCE_NAME`_SOCK_TCP);
	if (skt != 0xFF) {
		if (`$INSTANCE_NAME`_SocketStatus(skt) == `$INSTANCE_NAME`_STS_INIT) {
			`$INSTANCE_NAME`_SocketCommand(skt,`$INSTANCE_NAME`_CMD_LISTEN);
		}
		else {
			`$INSTANCE_NAME`_SocketClose(skt);
			skt = 0xFF;
		}
	}
	return skt;
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketDataWaiting(uint8 skt)
{
	uint16 adr;
	uint16 result[2];
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 0x0026;
	
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[0],2,0);
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[1],2,0);
	}
	while (result[0] != result[1]);
	
	return CYSWAP_ENDIAN16(result[1]);
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketTxFree(uint8 skt)
{
	uint16 adr;
	uint16 result[2];
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 0x0020;
	
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[0],2,0);
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[1],2,0);
	}
	while (result[0] != result[1]);
	
	return CYSWAP_ENDIAN16(result[1]);
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketGetTxWritePtr(uint8 skt)
{
	uint16 adr;
	uint16 result[2];
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 0x0024;
	
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[0],2,0);
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[1],2,0);
	}
	while (result[0] != result[1]);
	
	return CYSWAP_ENDIAN16(result[1]);
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketGetTxReadPtr(uint8 skt)
{
	uint16 adr;
	uint16 result[2];
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 0x0022;
	
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[0],2,0);
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[1],2,0);
	}
	while (result[0] != result[1]);
	
	return CYSWAP_ENDIAN16(result[1]);
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketGetRxReadPtr(uint8 skt)
{
	uint16 adr;
	uint16 result[2];
	
	adr = `$INSTANCE_NAME`_SKT_BASE(skt) + 0x0028;
	
	do {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[0],2,0);
		`$INSTANCE_NAME`_HW_ChipAccess(adr,(uint8*)&result[1],2,0);
	}
	while (result[0] != result[1]);
	
	return CYSWAP_ENDIAN16(result[1]);
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_SocketTxProcessing(uint8 skt, uint8 *packet, uint16 len)
{
	uint16 base, max;
	uint16 ptr;
	uint16 adr;
	uint16 free;
	uint8 status;
	
	if (skt >= `$NUM_SOCKETS`) return CYRET_BAD_PARAM;
	/* 
	 * compute the max buffer length, and verify that the packet length does not
	 * exceed the limits.
	 */
	max = (uint16)(`$INSTANCE_NAME`_memSize*1024);
	if (len >= max) return CYRET_BAD_PARAM;
	/*
	 * compute the base pointer for the buffer memory, and the max offset address
	 * of the memory buffer.  This allows for the detection of poiner wrap so
	 * the buffer can be managed corectly.
	 */
	base = `$INSTANCE_NAME`_TX_BASE(skt);
	/*
	 * read the free space available within the buffer to determine if there is
	 * enough room available to write the complete packet. when there is not
	 * enough free space, delay until more room becomes available.
	 */
	free = `$INSTANCE_NAME`_SocketTxFree(skt);
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+3,&status,1,0);
	while ( (free < len) && (status == `$INSTANCE_NAME`_STS_ESTABLISHED) ) {
		vTaskDelay(1/portTICK_RATE_MS);
		free = `$INSTANCE_NAME`_SocketTxFree(skt);
		`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+3,&status,1,0);	
	}
	
	/*
	 * read the buffer offset pointer value from the W5200 to determine where
	 * to begin writing data to the device.  Since this is read as BIG endian,
	 * swap the bytes to make the value usable.  Also calculate the write
	 * address in to the W5200 memory buffers.
	 */
	ptr = `$INSTANCE_NAME`_SocketGetTxWritePtr(skt);
	adr = base + (ptr & (max-1));
	/* calculate the number bytes exceeding the memory range */
	free = max - (ptr&(max-1));
	/*
	 * after calculating the amount of data to be written, write the block of
	 * data in to the transmit buffer.
	 */
	if (len > free) {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,packet,free,1);
		`$INSTANCE_NAME`_HW_ChipAccess(base,&packet[free],len-free,1);
	}
	else {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,packet,len,1);
	}
	/*
	 * update TX pointer in memory and issue a SEND command to transmit the
	 * socket data through the Ethernet protocol.
	 */
	ptr = ptr+len;
	ptr = CYSWAP_ENDIAN16(ptr);
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+0x24,(uint8*)&ptr,2,1);

	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_SocketSend(uint8 skt, uint8 *packet, uint16 len)
{
	uint8 sts[2];
	uint8 errata[4];
	cystatus result = 0;
	
	if (skt > 7) return CYRET_BAD_PARAM;

	/*
	 * Allocate Semaphore to prevent other threads from accessing the chip (and
	 * otherwise changing registers) during the data transmission to the device
	 * buffer.
	 */
	if (xSemaphoreTake(`$INSTANCE_NAME`_ControllerMutex,portMAX_DELAY) == pdPASS) {
	
		errata[0] = 0;
		errata[1] = 0;
		errata[2] = 0;
		errata[3] = 0;
		result = `$INSTANCE_NAME`_SocketTxProcessing(skt,packet,len);
		/* Clear pending flags in the IR */
		sts[0] = `$INSTANCE_NAME`_IR_SENDOK | `$INSTANCE_NAME`_IR_TIMEOUT;
		result = `$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+2,&sts[0],1,1);
		/* set subnet mask (errata) */
		`$INSTANCE_NAME`_HW_ChipAccess(5,`$INSTANCE_NAME`_SubnetMask,4,1);
		
		`$INSTANCE_NAME`_SocketCommand(skt,`$INSTANCE_NAME`_CMD_SEND);
		result = `$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+2,sts,2,0);
		
		result = (sts[0]&`$INSTANCE_NAME`_IR_TIMEOUT)? CYRET_TIMEOUT :
			(sts[0]&`$INSTANCE_NAME`_IR_DISCON)?CYRET_TIMEOUT :
			(sts[0]&`$INSTANCE_NAME`_IR_SENDOK)?CYRET_SUCCESS :
			(sts[1] != `$INSTANCE_NAME`_STS_ESTABLISHED) ? CYRET_TIMEOUT : CYRET_STARTED;
		
		while ( result == CYRET_STARTED) {
			vTaskDelay(1/portTICK_RATE_MS);
			`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+2,sts,2,0);
			result = (sts[0]&`$INSTANCE_NAME`_IR_TIMEOUT)? CYRET_TIMEOUT :
				(sts[0]&`$INSTANCE_NAME`_IR_DISCON)?CYRET_TIMEOUT :
				(sts[0]&`$INSTANCE_NAME`_IR_SENDOK)?CYRET_SUCCESS :
				(sts[1] != `$INSTANCE_NAME`_STS_ESTABLISHED) ? CYRET_TIMEOUT : CYRET_STARTED;
		}

		/* Clear pending flags in the IR */
		sts[0] = 0x1F;
		`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+2,&sts[0],1,1);
		
		/* reset subnet mask to all zeros * (errata) */
		`$INSTANCE_NAME`_HW_ChipAccess(5, errata,4,1);
	}
	xSemaphoreGive( `$INSTANCE_NAME`_ControllerMutex );
	return result;
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketRxProcessing(uint8 skt, uint8 *packet, uint16 len)
{
	uint16 base, max;
	uint16 ptr;
	uint16 adr;
	uint16 free;
//	uint8 status;
	
	if (skt >= `$NUM_SOCKETS`) return CYRET_BAD_PARAM;
	/* 
	 * compute the max buffer length, and verify that the packet length does not
	 * exceed the limits.
	 */
	max = (uint16)(`$INSTANCE_NAME`_memSize*1024);
	if (len >= max) return CYRET_BAD_PARAM;
	/*
	 * compute the base pointer for the buffer memory, and the max offset address
	 * of the memory buffer.  This allows for the detection of poiner wrap so
	 * the buffer can be managed corectly.
	 */
	base = `$INSTANCE_NAME`_RX_BASE(skt);
	
	/*
	 * read the buffer offset pointer value from the W5200 to determine where
	 * to begin writing data to the device.  Since this is read as BIG endian,
	 * swap the bytes to make the value usable.  Also calculate the write
	 * address in to the W5200 memory buffers.
	 */
	ptr = `$INSTANCE_NAME`_SocketGetRxReadPtr(skt);
	adr = base + (ptr & (max-1));
	/* calculate the number bytes exceeding the memory range */
	free = max - (ptr&(max-1));
	/*
	 * after calculating the amount of data to be written, write the block of
	 * data in to the transmit buffer.
	 */
	if (len > free) {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,packet,free,0);
		`$INSTANCE_NAME`_HW_ChipAccess(base,&packet[free],len-free,0);
	}
	else {
		`$INSTANCE_NAME`_HW_ChipAccess(adr,packet,len,0);
	}
	/*
	 * update TX pointer in memory and issue a SEND command to transmit the
	 * socket data through the Ethernet protocol.
	 */
	ptr = ptr+len;
	ptr = CYSWAP_ENDIAN16(ptr);
	`$INSTANCE_NAME`_HW_ChipAccess(`$INSTANCE_NAME`_SKT_BASE(skt)+0x28,(uint8*)&ptr,2,1);

	return CYRET_SUCCESS;
	
}
/* ------------------------------------------------------------------------ */
uint16 `$INSTANCE_NAME`_SocketReceive(uint8 skt, uint8 *packet, uint16 len)
{
	uint16 RxSize = 0;

	/*
	 * Return immediately upon the detection of a socket that is invalid
	 */
	if (skt >= `$NUM_SOCKETS`) return 0;

	/*
	 * Allocate Semaphore to prevent other threads from accessing the chip (and
	 * otherwise changing registers) during the data transmission to the device
	 * buffer.
	 */
	if (xSemaphoreTake(`$INSTANCE_NAME`_ControllerMutex,portMAX_DELAY) == pdPASS) {
		/*
		 * read the number of waiting bytes in the buffer memory
		 * but, clip the length of data read to the requested
		 * length of data.
		 */
		RxSize = `$INSTANCE_NAME`_SocketDataWaiting( skt );
		RxSize = (RxSize > len) ? len : RxSize;
		/* If there was waiting data, read it from the buffer */
		if (RxSize > 0) {
			`$INSTANCE_NAME`_SocketRxProcessing( skt, packet, RxSize);
			/* 
			 * after reading the buffer data, send the receive command
			 * to the socket so that the W5100 completes the read
			 */
			`$INSTANCE_NAME`_SocketCommand(skt,`$INSTANCE_NAME`_CMD_RECV);
		}
	}
	xSemaphoreGive( `$INSTANCE_NAME`_ControllerMutex );
	/* return the number of read bytes from the buffer memory */
	return RxSize;
	
}
/* ======================================================================== */
/* [] END OF FILE */
