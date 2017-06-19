#ifndef EXTENDER_H
#define EXTENDER_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

void Write_To_Extender(char Extender_Number, char Address, char Data);

char Read_From_Extender(char Extender_Number, char Address);

void Extender_Interrupt_Clear(char Extender_Number);

char Interrupt_Triggered_Device(char Extender_Number, char Extender_Port_Number);

void Extender_Init(char Extender_Number); 

/*         */ 