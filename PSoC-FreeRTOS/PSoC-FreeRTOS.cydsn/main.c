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

xQueueHandle OLED_Queue;

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
	
	/*
	 * Initialize the Two Thread tasks for the USBUART and the CLI interface
	 * that are both seperate component add-ons to the basic FreeRTOS.
	 */
	OLED_RST_Write( 0 );
	
	I2C_Start();
	OLED_RST_Write( 1 );
	CyDelay( 500 );

	SSD1306_Start();
	CyDelay( 500 );
	SSD1306_PrintString("{row;col;mv;clr}Welcome to FreeRTOS ");
	SSD1306_PrintString("{row3;col1;mv;big}v8.2.2");
	SSD1306_Refresh();
//	CyDelay(10);
	SSD1306_Refresh();
	CyDelay(1000);
	
	/* Create a Queue for the OLED output device */
	OLED_Queue = xQueueCreate( 512, 1 );
	
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
	uint8 csi;
	char c;
	
	SSD1306_PrintString("{row;col;clr;big}    PSoC\n \x0E Rocks \x0E");
	csi = 0;
	
	for(;;) {
		SSD1306_Refresh();
		vTaskDelay(20/portTICK_RATE_MS);
		while (uxQueueMessagesWaiting( OLED_Queue) > 0 ) {
			xQueueReceive( OLED_Queue, (void*)&c, portMAX_DELAY);
			if (csi) {
				if (isalpha((int)c) ) {
					csi = 0;
				}
			}
			else if (c == '\x1b') {
				csi = 1;
			}
			else {
				SSD1306_PutChar( c );
			}
		}
	}
}
/* [] END OF FILE */
