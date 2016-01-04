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

xSemaphoreHandle SPIM_Mutex;

/* ------------------------------------------------------------------------ */
cystatus cmdEth( int argc, char **argv )
{
	int arg;
	int mode;
	uint16 adr;
	uint16 len;
	uint8  buffer[32];
	char obuf[21];
	
	mode = 0;
	adr = 0;
	len = 0;
	/* process CLI command parameters */
	for (arg = 1; arg < argc; ++arg ) {
		if (strcmp(argv[arg],"-h") == 0) {
			mode = -1; /* HELP! */
		}
		else if (strcmp(argv[arg],"-w") == 0){
			mode = 1;
		}
		else if (strncmp(argv[arg],"-adr",4) == 0) {
			if (argv[arg][4] == ':') {
				adr = CLI_GetValueParam(&argv[arg][5]);
			}
			else {
				CLI_SystemMsg("{c1}Missing {c9}:{c1}.",CLI_ERROR);
				mode = -1;
			}
		}
		else if (strncmp(argv[arg],"-len",4) == 0) {
			if (argv[arg][4] == ':') {
				len = CLI_GetValueParam(&argv[arg][5]);
				len = (len >=32)?32:len;
			}
			else {
				CLI_SystemMsg("{c1}Missing {c9}:{c1}.",CLI_ERROR);
				mode = -1;
			}
		}
		else if (isdigit((int)argv[arg][0]) ) {
			buffer[len++] = CLI_GetValueParam(argv[arg]);
		}
		else {
			mode = -1;
			CLI_SystemMsg("{c1}Unknown command line option {c15}",CLI_ERROR);
			CLI_PrintString(argv[arg]);
			CLI_PrintString("{c1}. Please use -h for help.\r\n");
		}
	}
	/*
	 * Process command + selected options
	 */
	switch(mode) {
		case -1: /* HELP! */
			break;
		case 1:
			ETH_HW_ChipAccess(adr,&buffer[0],len,1);
		case 0: /* Read Register */
			ETH_HW_ChipAccess(adr,&buffer[0],len,0);
			sprintf(obuf,"%04X",adr);
			CLI_PrintString("\r\n{c4}[{c14}ADR: 0x");
			CLI_PrintString(obuf);
			CLI_PrintString("{c4}]");
			for (adr = 0;adr<len;++adr) {
				if ((adr&0x07)==0) {
					CLI_PrintString("\r\n{c4}[{c15}DAT{c4}]{c7}: {c6}");
				}
				sprintf(obuf,"%02X ",buffer[adr]);
				CLI_PrintString(obuf);
			}
			break;
		default:
			CLI_SystemMsg("{c5}Unknown Mode",CLI_ERROR);
			break;
	}
	CLI_PrintString("{c7;b0}\r\n\n");
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

	ETHIO_Write(1); /* set reset and disable chip */
	
	qIn = xQueueCreate( 512, sizeof(char) );
	qOut = xQueueCreate( 512, sizeof(char) );
	SPIM_Mutex = xSemaphoreCreateMutex();
	
	CLI_RegisterCommand(cmdEth,"eth","Control or test Ethernet Device.");
	
	SPIM_Start();
	ETHIO_Write(3); /* bring chip out of reset while not selected */
	CyDelay(150);   /* wait for Ethernet PLL to lock (fixed max delay from datasheet) */
	
	ETH_Start(); /* initialize the MAC and Protocol engine */
	
	/*
	 * Initialize the Two Thread tasks for the USBUART and the CLI interface
	 * that are both seperate component add-ons to the basic FreeRTOS.
	 */
	USB_Start();
	CLI_Start();
	
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
/* [] END OF FILE */
