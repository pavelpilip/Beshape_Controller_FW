

#include <p18cxxx.h>		// Chip-specific definitions for PIC family

#include "util.h"

#include "UART.h"

//extern PC_Word_Detected, Timeout_Counter;
BOOL Data_Receive_Error = FALSE;//, UART_Communication_Flag = FALSE, UART_Timeout = FALSE;
//char MSB_Byte = 0, LSB_Byte = 0;

void Init_UART()
{
	Transmit_Control_Register = 0x06; // BRGH = "1" 
	Receive_Control_Register = 0x00;
	Baud_Rate_Control = 0x40; // BRG16 = "0"
	Baud_Rate_Generator = 0x22; // BAUD RATE CONFIGURED TO 115200bps
	Baud_Rate_Generator_High = 0x00; 
	UART_Data_Transmit = 0x00;	
	UART_Data_Receive = 0x00;
	UART_Transmit_Data_Idle_High;
	UART_Receive_Data_Polarity_Not_Inverted;
	UART_Enable;
	Delay_6_25US(100);
	UART_Transmit_Enable;
	Delay_6_25US(100);
	UART_Receive_Enable;
	Delay_6_25US(100);
	EnableIntUART;
/*	Delay_0_625US(10);
	UART_Data_Transmit = 0x41;	*/
}	

BOOL Write_To_PC(char Data)
{
	if (!UART_Transmit_Ready) return (FALSE);
	else
	{
		UART_Data_Transmit = Data;
		while (!UART_Transmit_Ready);
		return (TRUE);
	}	
}

char Read_From_PC()
{
	char Tmp = 0;
	if ((Framing_Error) || (Overrun_Error))
		{
			Tmp = UART_Data_Receive;
			UART_Receive_Disable;
			Delay_6_25US(10); // delay 60 usec
			UART_Receive_Enable; // CLEAR ALL ERRORS
			Data_Receive_Error = TRUE;
		}
		else
		{
			Data_Receive_Error = FALSE;
			return (UART_Data_Receive);
		}
}		
