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

xTaskHandle redTask;
xTaskHandle blueTask;
xTaskHandle greenTask;

void vRedTask( void* pvParameters);
void vGreenTask( void* pvParameters);
void vBlueTask( void* pvParameters);

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
	
	xTaskCreate(vRedTask,"red",80,NULL,3,&redTask);
	xTaskCreate(vGreenTask,"green",80,NULL,3,&greenTask);
//	xTaskCreate(vBlueTask,"blue",80,NULL,3,&blueTask);

	FreeRTOS_Start();
	
    for(;;);
}

void vRedTask( void* pvParameters)
{
	for(;;) {
		vTaskDelay( 125/portTICK_RATE_MS );
		RED_Write( ~RED_Read() );
	}
}

void vGreenTask( void* pvParameters)
{
	for(;;) {
		vTaskDelay( 219/portTICK_RATE_MS );
		GREEN_Write( ~GREEN_Read() );
	}
}

void vBlueTask( void* pvParameters)
{
}

/* [] END OF FILE */
