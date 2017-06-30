#ifndef SPI_H
#define SPI_H                   // Only include this header file once

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

char Read_From_SPI(void);

char Write_To_SPI(char Device, char Data);

/*         */