#ifndef UART_H
#define UART_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

BOOL Write_To_PC(char Data);

char Read_From_PC();
