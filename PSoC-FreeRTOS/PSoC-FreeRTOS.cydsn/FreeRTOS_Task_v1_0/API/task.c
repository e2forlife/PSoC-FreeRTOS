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
/*
 * this file is a simple extension to a FreeRTOS task that allows the user to
 * use the component interface in PSoC Creator to control task options and
 * parameters.  Also this abstracts the FreeRTOS Task framework within a 
 * PSoC component standard interface.
 */
#include <cytypes.h>
#include "`$INSTANCE_NAME`.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_task.h"
#include "`$FreeRTOS`_semphr.h"

xTaskHandle `$INSTANCE_NAME`_TaskHandle;
uint8 `$INSTANCE_NAME`_initVar = 0u;

#if (`$UseMutex` == 1)
	xSemaphoreHandle `$INSTANCE_NAME`_Mutex;
#endif

/* ------------------------------------------------------------------------ */
/*
 * Place user included headers, defines and task global data in the
 * below merge region section.
 */
/* `#START USER_INCLUDE SECTION */

	/* --> Insert Code Here <-- */
	
/* `#END` */
/* ------------------------------------------------------------------------ */
/*
 * User defined task-local code that is used to process commands, control
 * operations, and/or generrally do stuff to make the taks do something
 * meaningful.
 */
/* `#START USER_TASK_LOCAL_CODE` */

/* --> Insert Code Here <-- */

/* `#END` */
/* ------------------------------------------------------------------------ */
/*
 * This is the main procedure that comprises the task.  Place the code required
 * to preform the desired function within the merge regions of the task procedure
 * to add functionality to the task.
 */
void `$INSTANCE_NAME`_TaskProc( void *pvParameters )
{
	/*
	 * Add and initialize local variables that are allocated on the Task stack
	 * the the section below.
	 */
	/* `#START TASK_VARIABLES` */
	
	/* --> Insert Code Here <-- */
	
	/* `#END` */
	
	/*
	 * Add the task initialzation code in the below merge region to be included
	 * in the task.
	 */
	/* `#START TASK_INIT_CODE` */

	/* --> Insert Code Here <-- */
		
	/* `#END` */
	
	for(;;) {
		/* `#START TASK_LOOP_CODE` */
	
		/* --> Insert Code Here <-- */
	
		/* `#END` */
		
		/* End of the task main loop */
		#if (`$TaskDelay` != 0)
			vTaskDelay(`$TaskDelay`/portTICK_RATE_MS);
		#endif
	}
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start( void )
{
	/*
	 * Insert task global memeory initialization here. Since the OS does not
	 * initialize ANY global memory, execute the initialization here to make
	 * sure that your task data is properly 
	 */
	/* `#START TASK_GLOBAL_INIT` */
	
	/* `#END` */
	
	if (`$INSTANCE_NAME`_initVar != 1) {
		#if (`$UseMutex` == 1)
			`$INSTANCE_NAME`_Mutex = xSemaphoreCreateMutex();
		#endif
	
		/*
	 	* Create the task and then leave. When FreeRTOS starts up the scheduler
	 	* will call the task procedure and start execution of the task.
	 	*/
		xTaskCreate(`$INSTANCE_NAME`_TaskProc,"`$INSTANCE_NAME`",`$StackSize`,NULL,`$Priority`,&`$INSTANCE_NAME`_TaskHandle);
		`$INSTANCE_NAME`_initVar = 1;
	}
}
/* ------------------------------------------------------------------------ */
/* ======================================================================== */
/* [] END OF FILE */
