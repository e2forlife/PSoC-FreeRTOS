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
#if !defined (`$INSTANCE_NAME`_H)
	#define `$INSTANCE_NAME`_H

#include <cytypes.h>

#define `$INSTANCE_NAME`_YES            ( 1 )
#define `$INSTANCE_NAME`_NO             ( 0 )


/*
 * read function/write function pointer types
 * These types are used to fill in the symbol table, which ultimately defines
 * the symbol table commands for execution.  defining a funtion as NULL will
 * disallow the mode of operation, writes will flag an error, but reads are
 * quiet.  The value is passed as a token type for reads (not an actual value)
 * so that the writer can handle the parsing of the value string for typing.
 * i.e. POWER wants on or off.
 */
typedef cystatus (*`$INSTANCE_NAME`_CLIfunc)( int, char** );

typedef struct {
	char name[15];   /* Command name */
	`$INSTANCE_NAME`_CLIfunc fn;     /* Parser callback to be executed */
	char desc[71];   /* ASCII description of function (for helper) */
} `$INSTANCE_NAME`_CLI_COMMAND;
	
#define `$INSTANCE_NAME`_NOTE       ( 0 )
#define `$INSTANCE_NAME`_WARN       ( 1 )
#define `$INSTANCE_NAME`_ERROR      ( 2 )
#define `$INSTANCE_NAME`_FATAL      ( 0xFF )

void `$INSTANCE_NAME`_Start( void );

cystatus `$INSTANCE_NAME`_RegisterCommand( `$INSTANCE_NAME`_CLIfunc fn, char *cmd, char *description);
void `$INSTANCE_NAME`_SystemMsg(const char *str, uint8 level);
int `$INSTANCE_NAME`_CliGetArguments( char *buffer, int *argc, char **argv );

cystatus `$INSTANCE_NAME`_CliHelp( int argc, char **argv );
cystatus `$INSTANCE_NAME`_CliClearScreen( int argc, char **argv );

/**
 * When the trace facilities are being used in FreeRTOS, enable the task
 * monitoring, removall, and priority switching.
 */
#if (configUSE_TRACE_FACILITY != 0)
cystatus `$INSTANCE_NAME`_TaskList(int argc, char **argv );
cystatus `$ISTANCE_NAME`_KillTask(int argc, char **argv )
#endif
/*
 * When the CLI TASK option is selected, add in the prototype for the
 * CLI task.
 */
void `$INSTANCE_NAME`_vCliTask( void *pvParameters );



#endif
/* [] END OF FILE */
