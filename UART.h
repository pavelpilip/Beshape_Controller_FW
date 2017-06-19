#ifndef UART_H
#define UART_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

BOOL Write_To_PC(char Data);

char Read_From_PC();

//void Word_To_Byte(int Parameter);

//BOOL Transmitt_To_PC(int Word);
