#ifndef Synthesizer_H
#define Synthesizer_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

void Synthesizer_Bit_Strobe(BOOL DDS_Number);

void Synthesizer_Apply_Changes(BOOL DDS_Number);

void Synthesizer_Control_Write(long frequency, char phase, char Power_Down, BOOL DDS_Number);

void Synthesizer_Device_Init(BOOL DDS_Number);
	