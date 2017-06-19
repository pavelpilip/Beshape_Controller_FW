#ifndef TECH_DIGITAL_POTENTIOMETER_H
#define TECH_DIGITAL_POTENTIOMETER_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

char TEC_Value_MSB_To_LSB(char value);

int Send_TEC_Transaction(char Word);  

BOOL TEC_Power_Set(int Power);

/*         */