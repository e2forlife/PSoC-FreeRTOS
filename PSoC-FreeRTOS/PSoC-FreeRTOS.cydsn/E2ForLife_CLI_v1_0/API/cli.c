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
#include <string.h>
#include <ctype.h>
	
#include "`$INSTANCE_NAME`.h"
#include "`$COM_INSTANCE`.h"

#include "`$FreeRTOS`.h"
#include "`$FreeRTOS`_task.h"
#include "`$FreeRTOS`_queue.h"
	
/* ------------------------------------------------------------------------ */
	
`$INSTANCE_NAME`_CLI_COMMAND `$INSTANCE_NAME`_CommandTable[ `$CLI_COMMANDS`+1 ];

/* ------------------------------------------------------------------------ */

int `$INSTANCE_NAME`_CLIrefresh;
uint8 `$INSTANCE_NAME`_CLIinitVar;

/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start( void )
{
	/*
	 * Register commands for help and clear
	 */
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_CliHelp,"help","Display command table descriptions");
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_CliClearScreen,"cls","Clear the screen");
	
	#if (configUSE_TRACE_FACILITY != 0)
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_TaskList,"taskmgr","Display task information");
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_KillTask,"kill","Force delete a task.");	
	#endif
	
	xTaskCreate( `$INSTANCE_NAME`_vCliTask, "Cli Task", 600, (void*)&`$INSTANCE_NAME`_CommandTable[0], `$CLI_PRIORITY`, NULL);
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_SystemMsg(const char *str, uint8 level)
{
	switch(level) {
		case `$INSTANCE_NAME`_NOTE:
			`$COM_INSTANCE`_PrintString("\r\n{c4}[{c10}NOTE{c4}]{c7}:");
			`$COM_INSTANCE`_PrintString(str);
			break;
		case `$INSTANCE_NAME`_WARN:
			`$COM_INSTANCE`_PrintString("\r\n{c4}[{c11}WARNING{c4}]{c7}: ");
			`$COM_INSTANCE`_PrintString(str);
			break;
		case `$INSTANCE_NAME`_ERROR:
			`$COM_INSTANCE`_PrintString("\r\n{c4}[{c9}ERROR{c4}]{c7}: ");
			`$COM_INSTANCE`_PrintString(str);
			break;
		case `$INSTANCE_NAME`_FATAL:
			`$COM_INSTANCE`_PrintString("\r\n{c4}[{c9}FATAL{c4}]{c7}: {c9}");
			`$COM_INSTANCE`_PrintString(str);
			break;
		default:
			`$COM_INSTANCE`_PrintString("\r\n{c4}[{c15}????{c4}]{c7}: ");
			`$COM_INSTANCE`_PrintString(str);
			break;
	}
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_CliHelp( int argc, char **argv )
{
	int idx;
	char bfr[51];
	
	`$COM_INSTANCE`_PrintString("{row1;col1;mv;cls}");
	
	idx = 0;
	while ( strlen(`$INSTANCE_NAME`_CommandTable[idx].name) != 0) {
		if ( strlen(`$INSTANCE_NAME`_CommandTable[idx].desc) > 0 ) {
			sprintf(bfr,"\r\n{c4}[{c15}%10s{c4}]{c7} : ",`$INSTANCE_NAME`_CommandTable[idx].name);
			`$COM_INSTANCE`_PrintString(bfr);
			sprintf(bfr,"{c%d}",((idx&0x01)?10:2));
			`$COM_INSTANCE`_PrintString(bfr);
			`$COM_INSTANCE`_PrintString(`$INSTANCE_NAME`_CommandTable[idx].desc);
		}
		++idx;
	}
	`$COM_INSTANCE`_PrintString("\r\n\n");
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_CliClearScreen( int argc, char **argv )
{
	argc = argc;
	argv = argv;
	
	`$COM_INSTANCE`_PrintString("{cls}");
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_CliShowPrompt( char *lineBuffer )
{
	`$COM_INSTANCE`_PrintString("\r\n");
	`$COM_INSTANCE`_PrintString("`$UserMessageString`");
	`$COM_INSTANCE`_PrintString("`$UserPromptString`");
	`$COM_INSTANCE`_PrintString( lineBuffer);
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_RegisterCommand( `$INSTANCE_NAME`_CLIfunc fn, char *cmd, char *description)
{
	int idx;
	
	cystatus result;
	
	result = CYRET_UNKNOWN;
	
	if ( fn != NULL )
	{
		/* look for the first available command slot in the table */
		for( idx = 0;(idx<`$CLI_COMMANDS`)&&(strlen( `$INSTANCE_NAME`_CommandTable[idx].name) >0); ++idx);
		if (idx <`$CLI_COMMANDS`) {
			/*
			 * There was a slot available in the command table, so allocate the
			 * position, and store the command information in to the table.
			 */
			`$INSTANCE_NAME`_CommandTable[idx].fn = fn;
			memcpy((void*)&`$INSTANCE_NAME`_CommandTable[idx].name[0], cmd, strlen(cmd) );
			memcpy((void*)&`$INSTANCE_NAME`_CommandTable[idx].desc[0], description, strlen(description));
			result = CYRET_SUCCESS;
		}
		
	}
	else {
		result = CYRET_BAD_PARAM;
	}
	
	return result;
}

/* ------------------------------------------------------------------------ */
int `$INSTANCE_NAME`_CliGetArguments( char *buffer, int *argc, char **argv )
{
	int idx;
	cystatus result;
	
	result = CYRET_STARTED;
	idx = 0;
	*argc = 0;
	while ( (buffer[idx] != 0) && (result == CYRET_STARTED) ) {
		/*
		 * drop leading whitespace, and set all spaces to NULL to
		 * prevent later confusion.
		 */
		while ( (buffer[idx] != 0) && isspace((int)buffer[idx]) ) {
			buffer[idx] = 0;
			++idx;
		}
		/*
		 * now, we know that the index is pointing to a non-space character,
		 * so process the character based upon it's state.
		 */
		if (buffer[idx] == ';') {
			/* 
			 * The end of a command can be the end of the buffer, or, a
			 * semicolon can be used for the creation of compound
			 * statements.
			 * A compound statement seperator was detected, so, clear it to
			 * form a terminator for the last argument, and then, set the
			 * result to finished to let the processor know that the arguments
			 * of the current command are now fully split.
			 */
			buffer[idx++] = 0;
			result = CYRET_FINISHED;
		}
		/*
		 * Otherwise, store the character location, and move the index pointer
		 * to the next break in the input, or the end of the buffer
		 */
		else {
			argv[*argc] = &buffer[idx];
			*argc = *argc + 1;
			while ( (!isspace((int)buffer[idx])) && (buffer[idx] != 0) && (buffer[idx] != ';') ) {
				++idx;
			}
			/*
			 * Set a terminator, if a space was detected, and update the index
			 * pointer to the next value.
			 */
			if ( isspace((int)buffer[idx]) ) {
				buffer[idx++] = 0;
			}
		}
	}
	
	return idx;
}
/* ------------------------------------------------------------------------ */
/**
 * \brief Process a token from the line buffer as a read or write operation
 * \param *buffer pointer to the buffer (with the read operator removed)
 * \param read identifier for read operation or write operations
 *
 * process token will pull the next token (has to be a label) from the input
 * buffer, and process the line to completion.  If there is a syntax error
 * the buffer is dumped and the user is notified. otherwise, the processor
 * function is called.
 */
cystatus `$INSTANCE_NAME`_CliProcessCommand(const `$INSTANCE_NAME`_CLI_COMMAND *tbl, int argc, char **argv)
{
	int idx;
	`$INSTANCE_NAME`_CLIfunc fn;
	
	static char outBuffer[`$MAX_CLI_OUTPUT_BUFFER`];
	
	cystatus result;
	
	if (tbl == NULL) {
		`$INSTANCE_NAME`_SystemMsg("Invalid command table",`$INSTANCE_NAME`_FATAL);
		return CYRET_UNKNOWN;
	}
	
	result = CYRET_UNKNOWN;
	fn = NULL;
	if (argc > 0) {
		/* look for the processed command */
		idx = 0;
		while ( ( strlen(tbl[idx].name ) > 0) && (idx < `$CLI_COMMANDS`) ) {
			if ( strcmp(tbl[idx].name, argv[0]) == 0 ) {
				fn = tbl[idx].fn;
				if (fn != NULL) {
					result = fn(argc,argv);
				}
				else {
					result = CYRET_INVALID_OBJECT;
					sprintf(outBuffer,"\"%s\" has not yet been implemented.",argv[0]);
					`$INSTANCE_NAME`_SystemMsg(outBuffer,`$INSTANCE_NAME`_WARN);
				}
			}
			++idx;
		}
		
		if (result == CYRET_UNKNOWN) {
			sprintf(outBuffer,"Unknown Command \"%s\"",argv[0]);
			`$INSTANCE_NAME`_SystemMsg(outBuffer, `$INSTANCE_NAME`_ERROR);
		}
	}
	return result;
}
/* ======================================================================== */
	
void `$INSTANCE_NAME`_vCliTask( void *pvParameters )
{
	/* Buffer to hold received user input on the line */
	static char lineBuffer[`$MAX_CLI_INPUT_BUFFER`];
	
 	`$INSTANCE_NAME`_CLI_COMMAND *CommandTable;
	int idx;
	char *argv[25];
	int argc;
	
	
	/*
	 * Grab the adadress of the command table from the OS parameters
	 * passed to the task, and assign them to the local data used in the CLI
	 */
	CommandTable = (`$INSTANCE_NAME`_CLI_COMMAND*) pvParameters;

	/*
	 * User CLI initialization code for Performing any operations to setup
	 * special hardware or other item prior to the connection validation.
	 */
	/* `#START USER_CLI_INITIALIZATION_BEFORE_CONNECT` */
	
	/* `#END` */

	/*
	 * CLI Initialization:
	 * Wait for user input to confirm that the CLI has connected with a
	 * terminal.. essentially, since the USB port is open and connected
	 * the second it attaches, we must wait for user input to validate the
	 * connection, and make sure that the prompts are visible.
	 */
	`$COM_INSTANCE`_GetChar();
	lineBuffer[0] = 0;
	/*
	 * The connection has been validated, this merge region allows the
	 * definition of functions that are performed once the connection
	 * is connected and valid. For Example, this might be a great place to
	 * add a welcome string, logon, or some other thing such as this.
	 */
	/* `#START USER_CLI_INITIALIZATION_AFTER_CONNECT` */
	
	/* `#END` */
	
	for(;;) {
		/*
		 * Wait for user input.
		 */
		`$INSTANCE_NAME`_CliShowPrompt(lineBuffer);
		
		/* Read the input line from the user with blocking functions */
		`$COM_INSTANCE`_GetString( lineBuffer );
		
		/* Set the color to neutral to avoid screen junk when executing commands */
		`$COM_INSTANCE`_SetColor(7,0);
		
		/*
		 * Strip arguments from the line buffer, and handle compound
		 * statements by looping through the input buffer until the NULL
		 * is encountered.
		 */
		idx = 0;
		while ( lineBuffer[ idx ] != 0) {
			idx += `$INSTANCE_NAME`_CliGetArguments(&lineBuffer[idx],&argc,argv);
			if (argc > 0) {
				`$INSTANCE_NAME`_CliProcessCommand(CommandTable,argc,argv);
			}
		}
		
		/* Erase the contents of the line buffer at the end of the command */
		memset((void*)&lineBuffer[0],0,idx);
	}
}
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* [] END OF FILE */
