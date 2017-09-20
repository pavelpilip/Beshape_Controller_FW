#ifndef UART_H
#define UART_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

#define Debug_Messages_Enable

#ifdef Debug_Messages_Enable
	#define DEBUG_PRINT_STRING(x)  			PrintConstString(x)
	#define DEBUG_PRINT_CHAR(x)  			Write_Debug_Char(x)
	#define DEBUG_PRINT_VARIABLE(x)  		PrintVariable(x)
#else 	
	#define DEBUG_PRINT_STRING(x)
	#define DEBUG_PRINT_CHAR(x)
	#define DEBUG_PRINT_VARIABLE(x)
#endif

BOOL Write_To_PC(char Data);

char Read_From_PC();

BOOL Write_Debug_Char(char Data);

void PrintConstString(const rom char* s);

void PrintVariable(int value);
