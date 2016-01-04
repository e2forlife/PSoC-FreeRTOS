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
#include "`$USB`.h"
#include "`$USB`_cdc.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_task.h"
#include "`$FreeRTOS`_queue.h"
#include "`$FreeRTOS`_semphr.h"

/* ======================================================================== */

uint8 `$INSTANCE_NAME`_initVar;
xTaskHandle `$INSTANCE_NAME`_TaskHandle;

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
	
	if (`$USB`_initVar == 0) {
    	/* Start USBFS Operation with 3V operation */
		#if (CYDEV_VDDIO1_MV < 5000)
    		`$USB`_Start(0u, `$USB`_3V_OPERATION);
		#else
			`$USB`_Start(0u, `$USB`_5V_OPERATION);
		#endif
	}
		
	xTaskCreate( `$INSTANCE_NAME`_Task,"`$INSTANCE_NAME` Task", 400, NULL, `$USB_PRIORITY`, &`$INSTANCE_NAME`_TaskHandle);
	
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
	extern xQueueHandle `$ReceiveDataQueue`;
	extern xQueueHandle `$TransmitDataQueue`;
    uint16 count;
    uint8 buffer[`$INSTANCE_NAME`_BUFFER_LEN];
	uint16 idx;
	
	for (;;) {
		/* Handle enumeration of USB port */
    	if(`$USB`_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
    	{
        	if(`$USB`_GetConfiguration() != 0u)   /* Init IN endpoints when device configured */
        	{
            	/* Enumeration is done, enable OUT endpoint for receive data from Host */
            	`$USB`_CDC_Init();
        	}
    	}
		/*
		 *
		 */
	    if(`$USB`_GetConfiguration() != 0u)
	    {
			/*
			 * Process received data from the USB, and store it in to the
			 * receiver message Q.
			 */
	        if(`$USB`_DataIsReady() != 0u)
	        {   
	            count = `$USB`_GetAll(buffer);
	            if(count != 0u)
	            {
					/* insert data in to Receive FIFO */
					for(idx=0;idx<count;++idx) {
						xQueueSend( `$ReceiveDataQueue`, (void*)&buffer[idx],0);
					}
				}
			}
			/*
			 * Send a block of data ack through the USB port to the PC,
			 * by checkig to see if there is data to send, then sending
			 * up to the BUFFER_LEN of data (64 bytes)
			 */
			count = uxQueueMessagesWaiting( `$TransmitDataQueue` );
			count = (count > `$INSTANCE_NAME`_BUFFER_LEN)? `$INSTANCE_NAME`_BUFFER_LEN:count;
			
			/* When component is ready to send more data to the PC */			
            if ( (`$USB`_CDCIsReady() != 0u) && (count > 0) ) {
				/*
				 * Read the data from the transmit queue and buffer it
				 * locally so that the data can be utilized.
				 */
				for (idx = 0; idx < count; ++idx) {
					xQueueReceive( `$TransmitDataQueue`,&buffer[idx],0);
				}
				/* Send data back to host */
    	        `$USB`_PutData(buffer, count);
				
				/* If the last sent packet is exactly maximum packet size, 
            	 *  it shall be followed by a zero-length packet to assure the
             	 *  end of segment is properly identified by the terminal.
             	 */
            	if(count == `$INSTANCE_NAME`_BUFFER_LEN){
					/* Wait till component is ready to send more data to the PC */
                	while(`$USB`_CDCIsReady() == 0u) {
						vTaskDelay( `$USB_SCAN_RATE` / portTICK_RATE_MS );
					}
                	`$USB`_PutData(NULL, 0u);         /* Send zero-length packet to PC */
            	} 
			}
		}
		
		vTaskDelay(`$USB_SCAN_RATE`/portTICK_PERIOD_MS);
	}
}
/* ======================================================================== */
/* [] END OF FILE */
