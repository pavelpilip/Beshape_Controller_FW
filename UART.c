

#include <p18cxxx.h>		// Chip-specific definitions for PIC family

#include "util.h"

#include "UART.h"

//extern PC_Word_Detected, Timeout_Counter; 
extern PC_Transaction_Counter;
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

void UART_Communication_Reload()
{
	UART_Receive_Disable; // CLEAR ALL ERRORS
	UART_Transmit_Disable;
	UART_Disable;
	DisableIntUART;
	UART_INTFLAG = 0;
			
	Delay_62_5US(20); // delay 1 msec
			
	Init_UART(); // RELOAD UART MODULE
	UART_INTFLAG = 0;
	PC_Transaction_Counter = 1; // RESET PARSER MODULE
}

char Read_From_PC()
{
	char Tmp = 0;
	if ((Framing_Error) || (Overrun_Error))
		{
			Tmp = UART_Data_Receive;
			UART_Communication_Reload();
			Data_Receive_Error = TRUE;
		}
		else
		{
			Data_Receive_Error = FALSE;
			return (UART_Data_Receive);
		}
}	

/////////////////////*******************************************************DEBUG UART FUNCTIONS********************************////////////////////////////

void Init_Debug_UART()
{
	#ifdef Debug_Messages_Enable 
		Transmit1_Control_Register = 0x06; // BRGH = "1" 
		Baud_Rate_Control1 = 0x40; // BRG16 = "0"
		Baud_Rate_Generator1 = 0x22; // BAUD RATE CONFIGURED TO 115200bps
		Baud_Rate_Generator_High1 = 0x00; 
		UART1_Data_Transmit = 0x00;	
		UART1_Transmit_Data_Idle_High;
		UART1_Enable;
		Delay_6_25US(100);
		UART1_Transmit_Enable;
	#endif
}	

BOOL Write_Debug_Char(char Data)
{
	if (!UART1_Transmit_Ready) return (FALSE);
	else
	{
		UART1_Data_Transmit = Data;
		while (!UART1_Transmit_Ready);
		return (TRUE);
	}
}

void PrintConstString(const rom char* s)
{
	char c, i=0;
	c = s[i];
	while (c) 
		{
			while (!UART1_Transmit_Ready); 
			Write_Debug_Char(c);
			c = s[++i];
		}
}

void PrintVariable(int value)
{
	int ten_thousand_digit = 0, thousand_digit = 0, hundred_digit = 0, decimal_digit = 0, once_digit = 0;
	/*char hexadecimalNumber[5] = {0}, temp = 0, i = 0;
	
		while(value!=0)
			{
				temp = value % 16;

			  //To convert integer into character
			  if( temp < 10)
				   temp =temp + 48;
			  else
				 temp = temp + 55;

			  hexadecimalNumber[i++] = temp;
			  value = value / 16;
			}*/

		ten_thousand_digit = (value % 100000) / 10000 + 48;
		thousand_digit = (value % 10000) / 1000 + 48;
		hundred_digit = (value % 1000) / 100 + 48;
		decimal_digit = (value % 100) / 10 + 48;
		once_digit = (value % 100) % 10 + 48;
		/*for (i = 5; i > 0; i--)
			{
				while (!UART1_Transmit_Ready); 
				Write_Debug_Char(hexadecimalNumber[i]);
			}*/
		while (!UART1_Transmit_Ready); 
		Write_Debug_Char(ten_thousand_digit);
		while (!UART1_Transmit_Ready); 
		Write_Debug_Char(thousand_digit);
		while (!UART1_Transmit_Ready); 
		Write_Debug_Char(hundred_digit);
		while (!UART1_Transmit_Ready); 
		Write_Debug_Char(decimal_digit);
		while (!UART1_Transmit_Ready); 
		Write_Debug_Char(once_digit);
}
	
