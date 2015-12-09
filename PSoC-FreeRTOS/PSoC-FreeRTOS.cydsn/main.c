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
#include <stdio.h>

void vMainTask( void *pvParameters);

extern xQueueHandle OLED_Queue;

xQueueHandle qIn;
xQueueHandle qOut;

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

	
	qIn = xQueueCreate( 512, sizeof(char) );
	qOut = xQueueCreate( 512 sizeof(char) );
	
//	OLED_RST_Write( 0 );
//	
//	OLED_RST_Write( 1 );
//	CyDelay( 500 );
//	
//	SSD1306_Start();
//	SSD1306_PrintString("{row;col;mv;clr}Welcome to FreeRTOS ");
//	SSD1306_PrintString("{row3;col1;mv;big}v8.2.2");
//	SSD1306_Refresh();
	
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
	char c;
	
	c = 0;
	for(;;) {
//		SSD1306_PutChar( c++ );
		vTaskDelay( 1000/portTICK_RATE_MS);
	}
}
/* [] END OF FILE */
