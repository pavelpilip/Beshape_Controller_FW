
#include <p18cxxx.h>
// Microchip library for delays routines

#include "util.h"

#include "Tech_Digital_Potentiometer.h"

char TEC_Value_MSB_To_LSB(char value)
{

	int acc1 = 0x0, acc2 = 0x0;
	int counter = 0, sub1 = 7;
	for (counter = 0; counter < 8; counter ++)
	{
		acc1 = value & 0x01;
		acc1 = acc1 << sub1;
		value = value >> 1;
		acc2 = acc2 | acc1;
		sub1 --;
	}

return(acc2);
}

void TEC_SCK_Strobe()
{
	Delay_0_625US(2); // delay 1.5 usec
	DSP_SPIB_CLK = 1;
	Delay_0_625US(2); // delay 1.5 usec
	DSP_SPIB_CLK = 0;
}

int Send_TEC_Transaction(char Word) 
{
	int i = 0, SPI_Word_Backup = 0;

	Word = TEC_Value_MSB_To_LSB(Word);
	
	for (i = 0; i < 9; i++) 
		{
			if (i == 0) DSP_SPIB_MOSI = 1;
			else
				{
					DSP_SPIB_MOSI = Word & 0x1;
					Word = Word >> 1;
				}
			TEC_SCK_Strobe();
			SPI_Word_Backup = SPI_Word_Backup | DSP_SPIB_MISO;
			SPI_Word_Backup = SPI_Word_Backup << 1;
		}
	
return (SPI_Word_Backup);
}

BOOL TEC_Power_Set(int Power) // TO CALIBRATE TO 8.19V, YOU SHOULD TYPE 819 
{
	unsigned char Digital_Potentiometer_Word = 0;
	
	if ((Power < TEC_Voltage_Low_Limit_Value) || (Power > TEC_Voltage_High_Limit_Value)) return (FALSE);
	
	PIC_SPI_Disable; // DISABLE SPI MOULE
	Delay_0_625US(2); // delay 1.5 usec
	tris_DSP_SPIB_MOSI = 0; 
	tris_DSP_SPIB_MISO = 1;
	tris_DSP_SPIB_CLK = 0;
	tris_DSP_SPIB_CS = 0;
	DSP_SPIB_CLK = 0;
	DSP_SPIB_CS = 1;
	
	DSP_SPIB_SEL0 = 1; // ROUTE TO POTENTIOMETER 
	DSP_SPIB_SEL1 = 1; 
	DSP_SPIB_SEL2 = 1; 
	DSP_SPIB_SEL3 = 0;
	
	//Digital_Potentiometer_Word = (char)((float)21.333333333 * ((float)Power / (float)100) - (float)12.8768); // VOLTAGE TO WORD CONVERSION
	Digital_Potentiometer_Word = (char)(((float)Power / (float)100 - (float)0.73) / (float)0.0522) + 1; // VOLTAGE TO WORD CONVERSION
	
	if ((Digital_Potentiometer_Word >= 0) && (Digital_Potentiometer_Word <= 255))
		{
			DSP_SPIB_CS = 0;
			Send_TEC_Transaction(Digital_Potentiometer_Word);
			DSP_SPIB_CS = 1;
			Delay_0_625US(2); // delay 1.5 usec
			PIC_SPI_Enable; // ENABLE SPI MOULE
			return (TRUE);
		}
	else 
		{
			PIC_SPI_Enable; // ENABLE SPI MOULE
			return (FALSE);
		}
}

void TEC_Power_Init()
{
	TEC_Power_Set(250); // SET THE TEC POWER TO 2.5V 
}
