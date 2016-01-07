/* ======================================================================== */
/*
 * Copyright (c) 2016, E2ForLife.com
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
#include "`$INSTANCE_NAME`.h"
#include "`$WIZNET`.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_semphr.h"
#include "`$FreeRTOS`_queue.h"
#include "`$FreeRTOS`_task.h"

xTaskHandle `$INSTANCE_NAME`_task;

/* ======================================================================== */
void `$INSTANCE_NAME`_taskProc( void *pvParameters )
{
	extern xQueueHandle `$STDIN`;
	extern xQueueHandle `$STDOUT`;
	uint8 telnetSocket;
	uint8 buffer[51];
	uint8 status;
	uint16 len;
	int idx;
	uint8 mode;
	
	telnetSocket = 0xFF;
	mode = 0;
	for(;;) {
		if (telnetSocket != 0xFF) {
			status = `$WIZNET`_SocketStatus(telnetSocket);
			if (status == `$WIZNET`_STS_ESTABLISHED) {
				/*
				 * Check for data received from the telnet port.
				 */
				len = `$WIZNET`_SocketReceive(telnetSocket,buffer,50);
				for(idx=0;idx<len;++idx) {
					xQueueSend(`$STDIN`,(void*)&buffer[idx],0);
				}
				/*
				 * check for data waiting to be sent over the telnet port
				 */
				len = uxQueueMessagesWaiting( `$STDOUT` );
				len = (len > 51)?51:len;
				for (idx=0;idx<len;++idx) {
					xQueueReceive(`$STDOUT`,(void*)&buffer[idx],0);
				}
				`$WIZNET`_SocketSend(telnetSocket,buffer,len);
			}
			else if (status != `$WIZNET`_STS_LISTEN) {
				`$WIZNET`_SocketClose(telnetSocket);
				telnetSocket = 0xFF;
			}
		}
		else {
			telnetSocket = `$WIZNET`_TcpOpenServer( `$PORTNUM` );
		}			
		vTaskDelay(`$TASK_DELAY`/portTICK_RATE_MS);
	}
}
/* ======================================================================== */
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start(void)
{
	/*
	 * Create the task for the server, and basically do nothing else.
	 */
	xTaskCreate(`$INSTANCE_NAME`_taskProc,"TELNET",`$STACK`,NULL,`$PRIORITY`,&`$INSTANCE_NAME`_task);
}
/* ======================================================================== */
/* [] END OF FILE */
