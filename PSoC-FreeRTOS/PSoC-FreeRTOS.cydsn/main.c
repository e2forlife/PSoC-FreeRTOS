/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>

void vMainTask( void *pvParameters);

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
	
	/*
	 * Initialize the Two Thread tasks for the USBUART and the CLI interface
	 * that are both seperate component add-ons to the basic FreeRTOS.
	 */
	USBUART_Start();
	CLI_Start();
	
	/*
	 * Create a simple main task for doing some stuff.
	 */
	xTaskCreate( vMainTask, "MAIN Task", 400, NULL, 3, NULL );

	
	/*
	 * Initialize OS interrupt vectors and start the scheduler. This function
	 * shoud never return because the scheduler is running, so something real
	 * bad occurred if it does.
	 */
	FreeRTOS_Start();
	
	/*
	 * The scheduler shoudl be running, so no code beyond here should ever be
	 * executed.
	 */
    for(;;);
}
/* ======================================================================== */
void vMainTask( void *pvParameters)
{
	int blink;
	char ch;
	
	blink = 70;	
	for(;;) {
		
		vTaskDelay( 1000/portTICK_PERIOD_MS);
	}
}

/* [] END OF FILE */
