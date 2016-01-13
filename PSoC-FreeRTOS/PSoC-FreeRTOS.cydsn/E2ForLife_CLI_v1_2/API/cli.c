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
	
/* ------------------------------------------------------------------------ */
	
`$INSTANCE_NAME`_CLI_COMMAND `$INSTANCE_NAME`_CommandTable[ `$CLI_COMMANDS`+1 ];

/* ------------------------------------------------------------------------ */

uint8 `$INSTANCE_NAME`_initVar;
xTaskHandle `$INSTANCE_NAME`_Task;

/* ======================================================================== */
extern uint8 `$INSTANCE_NAME`_initVar;
/* ======================================================================== */

/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_Start( void )
{
	`$INSTANCE_NAME`_initVar = 1;
	
	/*
	 * Register commands for help and clear
	 */
	#if (`$EnableHelpCommand` == 1)
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_CliHelp,"help","Display command table descriptions");
	#endif
	
	#if (`$EnableClearCommand` == 1)
		#if (`$EnableEscapeSequence` == 1)
	`$INSTANCE_NAME`_RegisterCommand(`$INSTANCE_NAME`_CliClearScreen,"cls","Clear the screen");
		#endif
	#endif
	
	
	xTaskCreate( `$INSTANCE_NAME`_vCliTask, "Cli Task", `$StackSize`, (void*)&`$INSTANCE_NAME`_CommandTable[0], `$CLI_PRIORITY`, &`$INSTANCE_NAME`_Task);
}
/* ------------------------------------------------------------------------ */
//void `$INSTANCE_NAME`_SystemMsg(const char *str, uint8 level)
void `$INSTANCE_NAME`_Message(const char *str, uint8 level)
#if (`$EnableEscapeSequence` == 1)
{
	switch(level) {
		case `$INSTANCE_NAME`_NOTE:
			`$INSTANCE_NAME`_GenericPrintString("\r\n{c4}[{c10}NOTE{c4}]{c7}:", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_WARN:
			`$INSTANCE_NAME`_GenericPrintString("\r\n{c4}[{c11}WARNING{c4}]{c7}: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_ERROR:
			`$INSTANCE_NAME`_GenericPrintString("\r\n{c4}[{c9}ERROR{c4}]{c7}: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_FATAL:
			`$INSTANCE_NAME`_GenericPrintString("\r\n{c4}[{c9}FATAL{c4}]{c7}: {c9}", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		default:
			`$INSTANCE_NAME`_GenericPrintString("\r\n{c4}[{c15}????{c4}]{c7}: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
	}
}
#else
{
	switch(level) {
		case `$INSTANCE_NAME`_NOTE:
			`$INSTANCE_NAME`_GenericPrintString("\r\n[NOTE]:", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_WARN:
			`$INSTANCE_NAME`_GenericPrintString("\r\n[WARNING]: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_ERROR:
			`$INSTANCE_NAME`_GenericPrintString("\r\n[ERROR]: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		case `$INSTANCE_NAME`_FATAL:
			`$INSTANCE_NAME`_GenericPrintString("\r\n[FATAL]: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
		default:
			`$INSTANCE_NAME`_GenericPrintString("\r\n[????]: ", `$ErrorQueue`);
			`$INSTANCE_NAME`_GenericPrintString(str, `$ErrorQueue`);
			break;
	}
}
#endif
/* ------------------------------------------------------------------------ */
uint8 `$INSTANCE_NAME`_AreYouSure( char* msg, uint8 defVal )
{
	const char *values[] = {"No", "Yes"};
	char out[31];
	uint8 result;
	
	result = 255;
	
	`$INSTANCE_NAME`_PrintString(msg);
	if (defVal != 0) {
		#if (`$EnableEscapeSequence` == 1)
		sprintf(out," {c6}%s{c4}/{c14}%s{c7} : ", values[0], values[1] );
		#else
		sprintf(out," %s/%s : ", values[0], values[1] );
		#endif	
	}
	else {
		#if (`$EnableEscapeSequence` == 1)
		sprintf(out," {c14}%s{c4}/{c6}%s{c7} : ", values[0], values[1] );
		#else
		sprintf(out," %s/%s : ", values[0], values[1] );
		#endif
	}	
	`$INSTANCE_NAME`_PrintString(out);
	do {
		out[0] = `$INSTANCE_NAME`_GetChar();
	
		if (toupper((int)out[0]) == 'Y') {
			result = 1;
		}
		else if (toupper((int)out[0]) == 'N') {
			result = 0;
		}
		else if ((out[0] == '\r') || (out[0] == '\n')) {
			result = defVal;
		}
	} while (result > 1);
	
	return result;
}
/* ------------------------------------------------------------------------ */
uint32 `$INSTANCE_NAME`_GetValueParam(char *arg)
{
	uint32 value;
	
	value = 0;
	
	if (strncmp(arg,"0x",2) == 0) {
		value = `$INSTANCE_NAME`_convHex(&arg[2]);
	}
	else if (isdigit((int)arg[0])) {
		sscanf(arg,"%lu",&value);
	}
	
	return value;
}
/* ------------------------------------------------------------------------ */
uint32 `$INSTANCE_NAME`_convHex(char *hex)
{
	uint32 value;
	int idx;
	int digit;
	
	value = 0;
	for(idx=0;(hex[idx]!=0)&&(idx<8)&&(isxdigit((int)(hex[idx])));++idx) {
		value <<= 4;
		digit = toupper((int)hex[idx]);
		if (digit >= 'A') {
			value += 10 + (digit - 'A');
		}
		else {
			value += (digit - '0');
		}
	}
	
	return value;
}
/* ------------------------------------------------------------------------ */
cystatus `$INSTANCE_NAME`_CliHelp( int argc, char **argv )
{
	int idx;
	char bfr[51];
	
	#if (`$EnableEscapeSequence` == 1)
	`$INSTANCE_NAME`_PrintString("{row1;col1;mv;cls}");
	#else
	`$INSTANCE_NAME`_PrintString("\r\n\n");
	#endif
	
	idx = 0;
	while ( strlen(`$INSTANCE_NAME`_CommandTable[idx].name) != 0) {
		if ( strlen(`$INSTANCE_NAME`_CommandTable[idx].desc) > 0 ) {
			#if (`$EnableEscapeSequence` == 1)
			sprintf(bfr,"\r\n{c4}[{c15}%10s{c4}]{c7} : ",`$INSTANCE_NAME`_CommandTable[idx].name);
			`$INSTANCE_NAME`_PrintString(bfr);
			sprintf(bfr,"{c%d}",((idx&0x01)?10:2));
			`$INSTANCE_NAME`_PrintString(bfr);
			`$INSTANCE_NAME`_PrintString(`$INSTANCE_NAME`_CommandTable[idx].desc);
			#else
			sprintf(bfr,"\r\n[%10s] : ",`$INSTANCE_NAME`_CommandTable[idx].name);
			`$INSTANCE_NAME`_PrintString(bfr);
			`$INSTANCE_NAME`_PrintString(`$INSTANCE_NAME`_CommandTable[idx].desc);
			#endif
		}
		++idx;
	}
	`$INSTANCE_NAME`_PrintString("\r\n\n");
	return CYRET_SUCCESS;
}
/* ------------------------------------------------------------------------ */
#if (`$EnableEscapeSequence` == 1)
	#if (`$EnableClearCommand` == 1)
cystatus `$INSTANCE_NAME`_CliClearScreen( int argc, char **argv )
{
	argc = argc;
	argv = argv;
	
	`$INSTANCE_NAME`_PrintString("{row;col;mv;cls}");
	return CYRET_SUCCESS;
}
#endif
#endif
/* ------------------------------------------------------------------------ */
void `$INSTANCE_NAME`_CliShowPrompt( char *lineBuffer )
{

	`$INSTANCE_NAME`_PrintString("\r\n");
	`$INSTANCE_NAME`_PrintString("`$UserMessageString`");
	`$INSTANCE_NAME`_PrintString("`$UserPromptString`");
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
	while ( (buffer[idx] != 0) && (result == CYRET_STARTED) && (*argc < 25) ) {
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
	
	//static char outBuffer[`$MAX_CLI_OUTPUT_BUFFER`];
	
	cystatus result;
	
	if (tbl == NULL) {
		#if (`$EnableEscapeSequence`==1)
		`$INSTANCE_NAME`_Message("{c9}Invalid command table{c7}",`$INSTANCE_NAME`_FATAL);
		#else
		`$INSTANCE_NAME`_Message("Invalid command table",`$INSTANCE_NAME`_FATAL);
		#endif
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
					#if (`$EnableEscapeSequence`==1)
					`$INSTANCE_NAME`_Message("{c3}The Command {c11}",`$INSTANCE_NAME`_WARN);
					`$INSTANCE_NAME`_ErrorString(argv[0])					
					`$INSTANCE_NAME`_ErrorString("{c3} has not yet been implemented{c7}.");
					#else
					`$INSTANCE_NAME`_Message("The Command ",`$INSTANCE_NAME`_WARN);
					`$INSTANCE_NAME`_ErrorString(argv[0])					
					`$INSTANCE_NAME`_ErrorString(" has not yet been implemented.");
					#endif
				}
			}
			++idx;
		}
		
		if (result == CYRET_UNKNOWN) {
			#if (`$EnableEscapeSequence` == 1)
			`$INSTANCE_NAME`_Message("{c1}Unknown Command \"{c9}", `$INSTANCE_NAME`_ERROR);
			`$INSTANCE_NAME`_ErrorString(argv[0]);
			`$INSTANCE_NAME`_ErrorString("{c1}\"{c7}");
			#else
			`$INSTANCE_NAME`_Message("Unknown Command \"", `$INSTANCE_NAME`_ERROR);
			`$INSTANCE_NAME`_ErrorString(argv[0]);
			`$INSTANCE_NAME`_ErrorString("\"");
			#endif
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
	cystatus result;
	
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

	#if (`$WaitForInput` == 1)
	/*
	 * CLI Initialization:
	 * Wait for user input to confirm that the CLI has connected with a
	 * terminal.. essentially, since the USB port is open and connected
	 * the second it attaches, we must wait for user input to validate the
	 * connection, and make sure that the prompts are visible.
	 */
	`$INSTANCE_NAME`_GetChar();
	lineBuffer[0] = 0;
	#endif
	
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
		#if (`$EnablePrompt` == 1)
		`$INSTANCE_NAME`_CliShowPrompt(lineBuffer);
		#endif
		/* Read the input line from the user with blocking functions */
		`$INSTANCE_NAME`_GetString( lineBuffer );
		
		#if (`$EnableEscapeSequence` == 1)
		/* Set the color to neutral to avoid screen junk when executing commands */
		`$INSTANCE_NAME`_PrintString("{c7;b0}");
		#endif
		
		/*
		 * Strip arguments from the line buffer, and handle compound
		 * statements by looping through the input buffer until the NULL
		 * is encountered.
		 */
		idx = 0;
		while ( lineBuffer[ idx ] != 0) {
			idx += `$INSTANCE_NAME`_CliGetArguments(&lineBuffer[idx],&argc,argv);
			if (argc > 0) {
				result = `$INSTANCE_NAME`_CliProcessCommand(CommandTable,argc,argv);
				if (result != CYRET_SUCCESS) {
					`$INSTANCE_NAME`_PrintString("\r\n");
					#if (`$EnableEscapeSequence`==1)
					`$INSTANCE_NAME`_Message("{c1}Command Error{c7}: ",`$INSTANCE_NAME`_ERROR);
					sprintf(lineBuffer,"{c14}0x%08lX",result);
					#else
					`$INSTANCE_NAME`_Message("Command Error: ",`$INSTANCE_NAME`_ERROR);
					sprintf(lineBuffer,"0x%08lX",result);
					#endif
					`$INSTANCE_NAME`_ErrorString(lineBuffer);
					lineBuffer[idx] = 0;
				}
				#if (`$EnableEscapeSequence` == 1)
				`$INSTANCE_NAME`_PrintString("{c7;b0}\r\n\n");
				#else
				`$INSTANCE_NAME`_PrintString("\r\n\n");
				#endif
			}
		}
		
		/* Erase the contents of the line buffer at the end of the command */
		memset((void*)&lineBuffer[0],0,idx);
	}
}
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* [] END OF FILE */
