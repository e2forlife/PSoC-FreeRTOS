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
	COMIO_Start();
	CLI_Start();
	
	/*
	 * Create a simple main task for doing some stuff.
	 */
	xTaskCreate( vMainTask, "MAIN Task", 200, NULL, 3, NULL );

	
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
	
//	Brightness_Start();
	
//	CLI_RegisterCommand(SetBrightness,"led","Set LED Brightness");
	
	blink = 70;	
	for(;;) {
		
//		Brightness_WritePulse0( blink );
		blink = (blink == 250)?70:(blink+10);
		
		vTaskDelay( 1000/portTICK_PERIOD_MS);
	}
}
/* ======================================================================== */
/**
 */
cystatus cmdSetBrightness( int argc, char **argv )
{
	int arg;
	uint8 usage;
	uint32 channel;
	uint32 value;
	
	channel = 0xFF;
	
	usage = 0;
	for(arg=1;arg<argc;++arg) {
		if (strcmp(argv[arg],"-h") == 0) {
			usage = 0xFF;
		}
		else if (strncmp(argv[arg],"-ch",3) == 0) {
			if (argv[arg][3] == ':') {
				if (strcmpi(&argv[arg][4],"mst") == 0) {
					channel = 0xFF;
				}
				else if ( isdigit( (int)argv[arg][4] ) ) {
					channel = CLI_GetValueParam(&argv[arg][4] );
				}
				else {
					CLI_SystemMsg("{c1}Invalid Channel Identifier following channel tag{c7}.",CLI_ERROR);
					usage = 0xFF;
				}
			}
			else if ((arg+1)<argc) {
				++arg;
				if (strcmpi(argv[arg],"mst") == 0) {
					channel = 0xFF;
				}
				else if ( isdigit((int)argv[arg+1][0]) ) {
					channel = CLI_GetValueParam( argv[arg] );
				}
				else {
					CLI_SystemMsg("{c1}Invalid Channel Identifier folowing a Channel Tag{c7}.",CLI_ERROR);
				}
			}
			else {
				CLI_SystemMsg("{c1}Missing Channel Identifier following channel tag{c7}.", CLI_ERROR);
				usage = 0xFF;
			}
		}
		else {
			CLI_SystemMsg("{c1}Invalid Argument{c7}:{c9} ", CLI_ERROR);
			COMIO_PrintString(argv[arg]);
			usage = 0xFF;
		}
		
	}
	
	if (usage) {
		COMIO_PrintString("\r\n\n{c10}");
		COMIO_PrintString(argv[0]);
		COMIO_PrintString("{c7} -ch{c4}[[{c5}:#{c4}]|{c5}<channel>{c4}] {c3}<Brightness%>");
		COMIO_PrintString("\r\n  {c6}-ch         {c7} :{c6} select channel to update, 1 - 8, mst");
		COMIO_PrintString("\r\n               {c7}:{c14} -ch:# to select channel #=(1-8 or mst)");
		COMIO_PrintString("\r\n               {c7}:{c6} -ch # to select channel.");
		COMIO_PrintString("\r\n  {c14}<Brightness> : The % brightness from 0% to 100% to set");
		COMIO_PrintString("\r\n");
	}
	else {
		
	}
	
	return CYRET_SUCCESS;
}

/* [] END OF FILE */
