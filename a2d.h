#ifndef A2D_H
#define A2D_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

void Init_ADC(char ADC_Working_Mode, BOOL Less_Greater_Mode); 

void A2D_Manual_Convert(char A2D_Channel_Number);

int A2D_Value_Read(char A2D_Channel_Number);

void A2D_Automatic_Convert(char ADC_Working_Mode, BOOL Less_Greater_Mode, BOOL Off_On);

BOOL Wait_For_Completion(void);

void Set_A2D_Mode(char Mode);

/*         */ 