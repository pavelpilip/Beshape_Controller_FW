#ifndef Potentiometer_H
#define Potentiometer_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

void Write_To_RDAC(char Address, char Word, BOOL Potentiometer_Number);

void Write_To_Input_Register(char Address, char Word, BOOL Potentiometer_Number);

int Read_From_RDAC(char Address, BOOL Potentiometer_Number);

int Read_From_Input_Register(char Address, BOOL Potentiometer_Number);

void Wiper_Step_Up(BOOL Potentiometer_Number);

void Wiper_Step_Down(BOOL Potentiometer_Number);

void Load_Input_Register_To_RDAC(BOOL Potentiometer_Number);

void Update_Wiper_Position(char Word, BOOL Potentiometer_Number);

void Software_Reset_Run(BOOL Potentiometer_Number);

void Digital_Potentiometer_Init(BOOL Potentiometer_Number);	

/*         */ 