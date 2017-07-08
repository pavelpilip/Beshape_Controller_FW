#ifndef DDS_H
#define DDS_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

void Frequency_Value_Set(long Value, BOOL DDS_Number);

int Swing_Compensation_Table(int DDS_Frequency_Value, int DDS_Amplitude_Value);

//float Swing_Compensation_Table(int DDS_Frequency_Value, int DDS_Amplitude_Value);

long Output_Frequency_Measure(BOOL DDS_Number);

BOOL Validate_Frequency_Setting(long Value, BOOL DDS_Number);

int Get_Amplitude_Value(BOOL DDS_Number, BOOL Before_After_Switch);

int Output_Amplitude_Measure(BOOL DDS_Number, BOOL Before_After_Switch);

BOOL Validate_Amplitude_Setting(int Value, BOOL DDS_Number);
	