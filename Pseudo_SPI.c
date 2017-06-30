
#include <p18cxxx.h>
// Microchip library for delays routines

#include "util.h"

#include "Pseudo_SPI.h"

void Pseudo_SCK_Strobe(BOOL Potentiometer_Number)
{
	if (!Potentiometer_Number) DDS1_POT_SCK = 1;
	else DDS2_POT_SCK = 1;
	Delay_0_625US(2); // delay 1.5 usec
	if (!Potentiometer_Number) DDS1_POT_SCK = 0;
	else DDS2_POT_SCK = 0;
}

int MSB_To_LSB(int value)
{

	int acc1 = 0x0, acc2 = 0x0;
	int counter = 0, sub1 = 15;
	for (counter = 0; counter < 16; counter ++)
	{
		acc1 = value & 0x01;
		acc1 = acc1 << sub1;
		value = value >> 1;
		acc2 = acc2 | acc1;
		sub1 --;
	}

return(acc2);
}

int Send_SPI_Transaction(int Word, BOOL Potentiometer_Number) 
{
	int i = 0, SPI_Word_Backup = 0;
 
	Word = MSB_To_LSB(Word);

	for (i = 0; i < 16; i++) 
		{
			if (!Potentiometer_Number) DDS1_POT_SDI = Word & 0x1;
			else DDS2_POT_SDI = Word & 0x1;
			Word = Word >> 1;
			Pseudo_SCK_Strobe(Potentiometer_Number);
			SPI_Word_Backup = SPI_Word_Backup << 1;
			if (!Potentiometer_Number) SPI_Word_Backup = SPI_Word_Backup | DDS1_POT_SDO;
			else SPI_Word_Backup = SPI_Word_Backup | DDS2_POT_SDO;
		}
	
return (SPI_Word_Backup);
}