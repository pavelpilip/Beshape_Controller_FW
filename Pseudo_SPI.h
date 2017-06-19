#ifndef Pseudo_SPI_H
#define Pseudo_SPI_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

void Pseudo_SCK_Strobe(BOOL Potentiometer_Number);

int Send_SPI_Transaction(int Word, BOOL Potentiometer_Number);

int MSB_To_LSB(int value);
