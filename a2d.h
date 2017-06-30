#ifndef A2D_H
#define A2D_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

void Init_ADC(); 

void A2D_Manual_Convert(char A2D_Channel_Number);

int A2D_Value_Read(char A2D_Channel_Number);

void Wait_For_Completion(void);

/*         */ 