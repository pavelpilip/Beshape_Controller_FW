#ifndef SPI_H
#define SPI_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

//char MSB_To_LSB(char value);

//void Send_Status_To_The_Master(char Word);

//long Write_SPI_Word(char Word);

char Read_From_SPI();

char Write_To_SPI(char Device, char Data);

/*         */