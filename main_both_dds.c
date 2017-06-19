/********************************************************************************/
/** @file 
 * 						 Firmware Source Code				 				 	  *
 *  										                  					  *
 * Author:		Pilip Pavel 								     					  *
 * Target:		Microchip 18F4520 									              *
 *				Clock speed: 40 MHz (HS-PLL configuration)	       				  *
 * Tools:		MPLAB v8.01								                	 	  *
 *		                                    		                              *
 ********************************************************************************
 */

#include <p18cxxx.h>

#include <delays.h>
 
#include "util.h"

#include "SPI.h"

#include "a2d.h"

#include "UART.h"

#include "Power.h"

#include "Extender.h"

#include "Synthesizer.h"

#include "Potentiometer.h"

#include "DDS.h"

#include "Tech_Digital_Potentiometer.h"

/********   L O C A L   M A C R O S  ***********************************************************/
// #pragma idata gprtx // Look in the .lkr file for section "gprtx" definition look the buffer size in specific location in the RAM memory

/////////////////////REGISTERS BITS FLAGS MODULE BEGIN///////////////////////////////
									
BOOL POWER_24V_VAC_EN = FALSE, POWER_48V_EN = FALSE, DDS1_2_POWER_ON_OFF = FALSE, DDS1_OUTPUT_ENABLE_DISABLE = FALSE, DDS1_HW_RESET = FALSE;
BOOL DDS1_SW_RESET = FALSE, DDS2_OUTPUT_ENABLE_DISABLE = FALSE, DDS2_HW_RESET = FALSE, DDS2_SW_RESET = FALSE;
BOOL AMP1_ENABLE = FALSE, AMP2_ENABLE = FALSE, VAC_VALVE2_ON_OFF = FALSE, HP_TEC_ON_OFF = FALSE;
BOOL HP_LED_1 = FALSE, HP_LED_2 = FALSE, HP_LED_3 = FALSE, TEC_Voltage_Set = FALSE, DDS1_Frequency_Set = FALSE; 
BOOL DDS1_Amplitude_Set = FALSE, DDS2_Frequency_Set = FALSE, DDS2_Amplitude_Set = FALSE;
BOOL Power_Control_Register_Read = FALSE, P3_3V_Voltage_Read = FALSE, P5V_Voltage_Read = FALSE, P12V_Voltage_Read = FALSE;
BOOL P24V_Voltage_Read = FALSE, P48V_Voltage_Read = FALSE, DDS1_Control_Register_Read = FALSE, DDS1_Frequency_Read = FALSE;
BOOL DDS1_Frequency_Measure = FALSE, DDS1_Amplitude_Read = FALSE, DDS1_Amplitude_Measure = FALSE, DDS2_Control_Register_Read = FALSE;
BOOL DDS2_Frequency_Read = FALSE, DDS2_Frequency_Measure = FALSE, DDS2_Amplitude_Read = FALSE, DDS2_Amplitude_Measure = FALSE; 						
BOOL Amplifier1_Control_Register_Read = FALSE, Amplifier1_Temperature_Read = FALSE, Amplifier1_Current_Read = FALSE, Amplifier1_Reverse_Power_Read = FALSE;	
BOOL Amplifier1_Forward_Power_Read = FALSE, Amplifier2_Control_Register_Read = FALSE, Amplifier2_Temperature_Read = FALSE, Amplifier2_Current_Read = FALSE;
BOOL Amplifier2_Reverse_Power_Read = FALSE,	Amplifier2_Forward_Power_Read = FALSE, Handpiece_Control_Register_Read = FALSE,	Handpiece_Temperature1_Read = FALSE;				 
BOOL Handpiece_Temperature2_Read = FALSE, Handpiece_Temperature3_Read = FALSE, Handpiece_Temperature4_Read = FALSE,	Handpiece_5V_3_3V_Read = FALSE;				
BOOL Vacuum_Pressure_Read = FALSE, TEC_Voltage_Read = FALSE, TEC_Voltage_Measure = FALSE, Errors1_Register_Read = FALSE, MCU_FPGA_Version_Read = FALSE;												   
BOOL Controller_Handpiece_Serial_Read = FALSE, Errors2_Register_Read = FALSE;

/////////////////////REGISTERS BITS FLAGS MODULE END///////////////////////////////

extern BOOL Data_Receive_Error;

BOOL A2D_Manual_Mode = FALSE, A2D_Less_Greater_Switch_Flag = FALSE, A2D_Less_Greater_Switch = FALSE, A2D_Voltage_Domains_Measurement_Error = FALSE;
BOOL A2D_Idle_Mode = FALSE, A2D_Operational_Mode = FALSE, PC_Word_Detected = FALSE, Read_Write_Transaction = FALSE, Frequency_Measurement_Completed = FALSE;
BOOL Timeout = FALSE;
char A2D_Less_Greater_Switch_Counter = 0, Received_Transaction_Address = 0, PC_Transaction_Counter = 1, PC_Transaction_Completed = FALSE;
char MSB_Byte = 0, LSB_Byte = 0;
int Error1_Register = 0, Error2_Register = 0, Received_PC_Data = 0, System_Registers_Array[System_Registers_Number] = {0};
int DDS1_Frequency_Value = 0, DDS2_Frequency_Value = 0, DDS1_Amplitude_Value = 0, DDS2_Amplitude_Value = 0, Timeout_Counter = 0;

#pragma interrupt high_priority_interrupt
void high_priority_interrupt (void)
{

	/*if (SPI_INTFLAG)
	{
		DisableIntSPI;
		return;	
	}*/
	
	if (UART_INTFLAG)
	{
		UART_INTFLAG = 0;
		DisableIntUART;
		PIC_PC_ACK = 0; // CONTROLLER IS BUSY
		PC_Word_Detected = TRUE;
		return;
	}
	
	if (TMR2_INTFLAG)
	{
		Timer2Disable;
		TMR2_INTFLAG = 0;
		RESET_TIMER2;
		if (A2D_Idle_Mode)
		{
			if (A2D_Less_Greater_Switch_Counter > 2) // SWITCH EVERY 16.5MS
				{
					A2D_Less_Greater_Switch_Counter = 0;
					A2D_Less_Greater_Switch_Flag = TRUE;
				}
			else 
				{
					A2D_Less_Greater_Switch_Counter++;
					Timer2Enable;
				}
		}
		else if (A2D_Manual_Mode)
			{
				Timeout_Counter ++;
				if (Timeout_Counter > 30) // 195 MILI SECONDS PASSED
					{
						Timeout_Counter = 0;
						Timeout = TRUE;
					}
				else Timer2Enable;
			}
		return;
	}
	
	if (TMR3_INTFLAG)
	{
		DDS1_PULSE_COUNTER_DISABLE; // DISABLE DDS1 PULSE COUNTER
		DDS2_PULSE_COUNTER_DISABLE; // DISABLE DDS2 PULSE COUNTER
		Timer3Disable;
		TMR3_INTFLAG = 0;
		RESET_TIMER3;
		Frequency_Measurement_Completed = TRUE;
		return;
	}
	
	if (A2D_INTFLAG)
	{
		A2D_INTFLAG = 0;
		if ((A2D_Idle_Mode) || (A2D_Operational_Mode)) // A2D AUTOMATIC MODE MEASUREMENT ERROR INTERRUPT
		{
			if (A2D_Idle_Mode) A2D_Automatic_Convert(IDLE, A2D_Less_Greater_Switch, FALSE); // DISABLE A2D MODULE
			else A2D_Automatic_Convert(Operational_Working_Mode, A2D_Less_Greater_Switch, FALSE);
			A2D_Voltage_Domains_Measurement_Error = TRUE;
		}
		return;
	}

}

 // Peripheral interrupt Subroutine 

#pragma interruptlow low_priority_interrupt
void low_priority_interrupt (void)
{ 


	
}

//=======================================================================================================
// LOW PRIORITY INTERRUPT HANDLER

#pragma code low_priority_int = 0x18
void low_vector (void)
{
  _asm goto low_priority_interrupt _endasm
}
#pragma code

#pragma code high_priority_int = 0x8
void high_vector (void)
{
 _asm goto high_priority_interrupt _endasm
}
#pragma code

//----------------------------------------------------------------------------
//  void Init(void)
//
//  Description:
//      Function to initialize the IOs, timers and UART 
//
//  Arguments:
//      None 
//  Return value:
//      None
//----------------------------------------------------------------------------

void Init(void) // INITIALIZATION OF SYSTEM 
{

	OSCCON = 0x30;
	OSCCON4 = 0x20;
	OSCTUNE = 0x00; 	

	InitSPI1Master();
	InitTimer0INTMode();
	InitTimer1INTMode();
	InitTimer2INTMode();
	InitTimer3INTMode();
	Init_UART();
	Init_ADC(IDLE, FALSE);
	
	PIC_SPI_Enable; // ENABLE SPI BUS
	
	Extender_Init(Expander_A_SPI);
	Extender_Init(Expander_B_SPI);
	Extender_Init(Expander_C_SPI);
	Extender_Init(Expander_D_SPI); 
	
	Init_Interrupts();
	System_Power_Init();
	Synthesizer_Device_Init(FALSE);
	Synthesizer_Device_Init(TRUE);
	Digital_Potentiometer_Init(FALSE);
	Digital_Potentiometer_Init(TRUE);
	TEC_Power_Init();
	
	Delay_6_25US(9); // delay 55 usec
	
}

void Interrupt_Signal_Strobe() // STROBE INTERRUPT SIGNAL
{
	char Tmp = 0;
	Tmp = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
	Write_To_Extender(Expander_C_SPI, 0x13, Tmp | 0x08); // SET INTERRUPT TRIGGER
	Write_To_Extender(Expander_C_SPI, 0x13, Tmp & 0xF7); // CLEAR INTERRUPT TRIGGER
}

void Send_Error_Word(int Error)
{
	switch (Error) 
		{
			case PC_Communication_Fault: 
				Error1_Register = Error1_Register | 0x01; // SET PC COMMUNICATION ERROR BIT
			break;
			case Emergency_Switch_State: 
				Error1_Register = Error1_Register | 0x02; // SET EMERGENCY SWITCH ERROR BIT
			break;
			case UART_Receiver_Error: 
				Error1_Register = Error1_Register | 0x04; // SET UART RECEIVER ERROR BIT
			break;
			case Word_Format_Error: 
				Error1_Register = Error1_Register | 0x08; // SET WORD FORMAT ERROR BIT
			break;
			case UART_Transaction_Error: 
				Error1_Register = Error1_Register | 0x10; // SET WORD FORMAT ERROR BIT
			break;
			case TEC_Voltage_Error: 
				Error1_Register = Error1_Register | 0x20; // SET TEC VOLTAGE ERROR BIT
			break;
			case UART_Transmitter_Error: 
				Error1_Register = Error1_Register | 0x100; // SET UART TRANSMITTER ERROR BIT
			break;
			case DDS1_Frequency_Error: 
				Error1_Register = Error1_Register | 0x200; // SET DDS1 FREQUENCY ERROR BIT
			break;
			case DDS1_Amplitude_Error: 
				Error1_Register = Error1_Register | 0x400; // SET DDS1 AMPLITUDE ERROR BIT
			break;
			case DDS2_Frequency_Error: 
				Error1_Register = Error1_Register | 0x800; // SET DDS2 FREQUENCY ERROR BIT
			break;
			case DDS2_Amplitude_Error: 
				Error1_Register = Error1_Register | 0x1000; // SET DDS2 AMPLITUDE ERROR BIT
			break;
			case DDS1_Amplitude_Measure_Error: 
				Error2_Register = Error2_Register | 0x01; // SET DDS1 AMPLITUDE MEASURE ERROR BIT
			break;
			case DDS2_Amplitude_Measure_Error: 
				Error2_Register = Error2_Register | 0x02; // SET DDS2 AMPLITUDE MEASURE ERROR BIT
			break;
			default:
													
			break;
		}
	Interrupt_Signal_Strobe();
}

BOOL PC_Word_Parser(char Word) // RETURN "0", ERROR IN PARSING
{
	char Tmp = 0, Register_BIT_Counter = 0;
	int Received_Data_Backup = 0;
	
/////////////////DETECT THE RECEIVED WORD IS TYPE MODULE BEGIN/////////////////////////	

	Tmp = Word >> 6;
	Word = Word & 0x3F; // CUT HEADER BITS
	if (Tmp == 0x1) // RECEIVED WORD IS DATA
		{
			if ((PC_Transaction_Counter > 1) && (!Read_Write_Transaction)) // RECEIVED WORD IS DATA AND WRITE TRANSACTION
				{
					if (PC_Transaction_Counter == 2) 
						{
							Received_PC_Data = Word;
							Received_PC_Data = Received_PC_Data << 8;
						}
					if (PC_Transaction_Counter == 3) 
						{
							Received_PC_Data = Received_PC_Data | Word; 
							PC_Transaction_Counter = 1;
							PC_Transaction_Completed = TRUE;
						}
				}
			else 
				{
					Send_Error_Word(Word_Format_Error);
					PIC_PC_ACK = 1;
					return (FALSE);
				}
		}
	else if (Tmp == 0x2) // RECEIVED WORD IS ADDRESS WRITE
		{
			if (PC_Transaction_Counter == 1) 
				{
					Received_Transaction_Address = Word;
					Read_Write_Transaction = FALSE;
					PC_Transaction_Counter ++;
					PIC_PC_ACK = 1;
					return (TRUE);
				}
			else 
				{
					Send_Error_Word(Word_Format_Error);
					PIC_PC_ACK = 1;
					return (FALSE);
				}
		}
	else if (Tmp == 0x3) // RECEIVED WORD IS ADDRESS READ
		{
			if (PC_Transaction_Counter == 1) 
				{
					Received_Transaction_Address = Word;
					Read_Write_Transaction = TRUE;
				}
			else 
				{
					Send_Error_Word(Word_Format_Error);
					PIC_PC_ACK = 1;
					return (FALSE);
				}
		}
	else 
		{
			Send_Error_Word(Word_Format_Error);
			PIC_PC_ACK = 1;
			return (FALSE);
		}
		
/////////////////DETECT THE RECEIVED WORD IS TYPE MODULE END/////////////////////////	

	if ((PC_Transaction_Completed) && (!Read_Write_Transaction)) // WRITE TRANSACTION PARSING
		{
			switch (Received_Transaction_Address) 
				{
					case 0x1: // Power control register 
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xFFE3; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														POWER_24V_VAC_EN = TRUE;
													break;
												case 0x1: 
														POWER_48V_EN = TRUE;
													break;
												default:
													
													break;
											}
									}
								else 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														POWER_24V_VAC_EN = FALSE;
													break;
												case 0x1: 
														POWER_48V_EN = FALSE;
													break;
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x7: // DDS1 control register 
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xFFC3; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														DDS1_2_POWER_ON_OFF = TRUE;
													break;
												case 0x1: 
														DDS1_OUTPUT_ENABLE_DISABLE = TRUE;
													break;
												case 0x8: 
														DDS1_HW_RESET = TRUE;
													break;
												case 0x9: 
														DDS1_SW_RESET = TRUE;
													break;	
												default:
													
													break;
											}
									}
								else
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														DDS1_2_POWER_ON_OFF = FALSE;
													break;
												case 0x1: 
														DDS1_OUTPUT_ENABLE_DISABLE = FALSE;
													break;
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x8: // DDS1 FREQUENCY SET REGISTER 
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						DDS1_Frequency_Set = TRUE;
						break;
					case 0xA: // DDS1 AMPLITUDE SET REGISTER
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						DDS1_Amplitude_Set = TRUE;
						break;
					case 0xC: // DDS2 control register 
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xFFC3; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x1: 
														DDS2_OUTPUT_ENABLE_DISABLE = TRUE;
													break;
												case 0x8: 
														DDS2_HW_RESET = TRUE;
													break;
												case 0x9: 
														DDS2_SW_RESET = TRUE;
													break;	
												default:
													
													break;
											}
									}
								else
									{
										switch (Register_BIT_Counter) 
											{
												case 0x1: 
														DDS2_OUTPUT_ENABLE_DISABLE = FALSE;
													break;
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0xD: // DDS2 FREQUENCY SET REGISTER 
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						DDS2_Frequency_Set = TRUE;
						break;
					case 0xF: // DDS2 AMPLITUDE SET REGISTER
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						DDS2_Amplitude_Set = TRUE;
						break;
					case 0x11: // Amplifier1 control register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xFFF9; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														AMP1_ENABLE = TRUE;
													break;	
												default:
													
													break;
											}
									}
								else
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														AMP1_ENABLE = FALSE;
													break;	
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x16: // Amplifier2 control register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xFFF9; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														AMP2_ENABLE = TRUE;
													break;	
												default:
													
													break;
											}
									}
								else
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														AMP2_ENABLE = FALSE;
													break;	
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x1B: // Handpiece control register   
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						System_Registers_Array[Received_Transaction_Address] = System_Registers_Array[Received_Transaction_Address] & 0xC0FF; // ZEROIZE ALL READ ONLY BITS
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														VAC_VALVE2_ON_OFF = TRUE;
													break;
												case 0x1: 
														HP_TEC_ON_OFF = TRUE;
													break;
												case 0x2: 
														HP_LED_1 = TRUE;
													break;
												case 0x3: 
														HP_LED_2 = TRUE;
													break;
												case 0x4: 
														HP_LED_3 = TRUE;
													break;
												default:
													
													break;
											}
									}
								else
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														VAC_VALVE2_ON_OFF = FALSE;
													break;
												case 0x1: 
														HP_TEC_ON_OFF = FALSE;
													break;
												case 0x2: 
														HP_LED_1 = FALSE;
													break;
												case 0x3: 
														HP_LED_2 = FALSE;
													break;
												case 0x4: 
														HP_LED_3 = FALSE;
													break;
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x22: // TEC voltage set register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						TEC_Voltage_Set = TRUE;
						break;
					default:
						Send_Error_Word(UART_Transaction_Error);
						PIC_PC_ACK = 1;
						return (FALSE);
						break;
				}
			return (TRUE);
		}
	else if ((!PC_Transaction_Completed) && (Read_Write_Transaction)) // READ TRANSACTION PARSING
		{
			switch (Received_Transaction_Address) 
				{
					case 0x1: // Power control register 
						Power_Control_Register_Read = TRUE;	
						break;
					case 0x2: // 3.3V voltage domain register 
						P3_3V_Voltage_Read = TRUE;
						break;
					case 0x3: // 5V voltage domain register  
						P5V_Voltage_Read = TRUE;
						break;
					case 0x4: // 12V voltage domain register  
						P12V_Voltage_Read = TRUE;
						break;
					case 0x5: // 24V voltage domain register   
						P24V_Voltage_Read = TRUE;
						break;
					case 0x6: // 48V voltage domain register 
						P48V_Voltage_Read = TRUE;
						break;
					case 0x7: // DDS1 control register  
						DDS1_Control_Register_Read = TRUE;
						break;
					case 0x8: // DDS1 frequency set register   
						DDS1_Frequency_Read = TRUE;
						break;
					case 0x9: // DDS1 frequency measurement register  
						DDS1_Frequency_Measure = TRUE;
						break;
					case 0xA: // DDS1 amplitude set register  
						DDS1_Amplitude_Read = TRUE;
						break;
					case 0xB: // DDS1 amplitude measurement register   
						DDS1_Amplitude_Measure = TRUE;
						break;
					case 0xC: // DDS2 control register  
						DDS2_Control_Register_Read = TRUE;
						break;
					case 0xD: // DDS2 frequency set register   
						DDS2_Frequency_Read = TRUE;
						break;
					case 0xE: // DDS2 frequency measurement register  
						DDS2_Frequency_Measure = TRUE;
						break;
					case 0xF: // DDS2 amplitude set register  
						DDS2_Amplitude_Read = TRUE;
						break;
					case 0x10: // DDS2 amplitude measurement register   
						DDS2_Amplitude_Measure = TRUE;
						break;
					case 0x11: // Amplifier1 control register    
						Amplifier1_Control_Register_Read = TRUE;
						break;
					case 0x12: // Amplifier1 temperature value register     
						Amplifier1_Temperature_Read = TRUE;
						break;
					case 0x13: // Amplifier1 current value register  
						Amplifier1_Current_Read = TRUE;
						break;
					case 0x14: // Amplifier1 reverse power value register      
						Amplifier1_Reverse_Power_Read = TRUE;
						break;
					case 0x15: // Amplifier1 forward power value register     
						Amplifier1_Forward_Power_Read = TRUE;
						break;
					case 0x16: // Amplifier2 control register    
						Amplifier2_Control_Register_Read = TRUE;
						break;
					case 0x17: // Amplifier2 temperature value register     
						Amplifier2_Temperature_Read = TRUE;
						break;
					case 0x18: // Amplifier2 current value register  
						Amplifier2_Current_Read = TRUE;
						break;
					case 0x19: // Amplifier2 reverse power value register      
						Amplifier2_Reverse_Power_Read = TRUE;
						break;
					case 0x1A: // Amplifier2 forward power value register     
						Amplifier2_Forward_Power_Read = TRUE;
						break;
					case 0x1B: // Handpiece control register     
						Handpiece_Control_Register_Read = TRUE;
						break;
					case 0x1C: // Handpiece temperature value1 register     
						Handpiece_Temperature1_Read = TRUE;
						break;
					case 0x1D: // Handpiece temperature value2 register      
						Handpiece_Temperature2_Read = TRUE;
						break;
					case 0x1E: // Handpiece temperature value3 register     
						Handpiece_Temperature3_Read = TRUE;
						break;
					case 0x1F: // Handpiece temperature value4 register     
						Handpiece_Temperature4_Read = TRUE;
						break;
					case 0x20: // Handpiece 5v/3.3v value register      
						Handpiece_5V_3_3V_Read = TRUE;
						break;
					case 0x21: // Vacuum pressure value register       
						Vacuum_Pressure_Read = TRUE;
						break;
					case 0x22: // TEC voltage set register        
						TEC_Voltage_Read = TRUE;
						break;
					case 0x23: // TEC voltage measurement register       
						TEC_Voltage_Measure = TRUE;
						break;
					case 0x24: // Errors1 register       
						Errors1_Register_Read = TRUE;
						break;
					case 0x25: // MCU / FPGA version register      
						MCU_FPGA_Version_Read = TRUE;
						break;
					case 0x26: // Controller / Handpiece serial register      
						Controller_Handpiece_Serial_Read = TRUE;
						break;
					case 0x27: // Errors2 register      
						Errors2_Register_Read = TRUE;
						break;
					default:
						Send_Error_Word(UART_Transaction_Error);
						PIC_PC_ACK = 1;
						return (FALSE);
						break;
				}
			PC_Transaction_Counter = 1;
			return (TRUE);
		}
	else
		{
			Send_Error_Word(Word_Format_Error);
			PIC_PC_ACK = 1;
			return (FALSE);
		}
}

void Set_A2D_Mode(char Mode) // SETTING A2D MODE 
{
	switch (Mode) 
		{
			case IDLE: 
				A2D_Idle_Mode = TRUE;
				A2D_Manual_Mode = FALSE;
				A2D_Operational_Mode = FALSE;
				break;
			case Manual_Working_Mode: 
				A2D_Idle_Mode = FALSE;
				A2D_Manual_Mode = TRUE;
				A2D_Operational_Mode = FALSE;
				break;
			case Operational_Working_Mode: 
				A2D_Idle_Mode = FALSE;
				A2D_Manual_Mode = FALSE;
				A2D_Operational_Mode = TRUE;
				break;
			default:
				A2D_Idle_Mode = TRUE;
				A2D_Manual_Mode = FALSE;
				A2D_Operational_Mode = FALSE;
				break;
		}
}

int Clear_Headers_From_The_Word(int Value)
{
	int Cleared_Header_Word = 0;
	Cleared_Header_Word = Value & 0x003F; 
	Value = (Value >> 2) & 0x0FC0;
	Cleared_Header_Word = Cleared_Header_Word | Value;
	return (Cleared_Header_Word);
}

void Word_To_Byte(int Parameter) // DIVIDE ONE WORD TO TWO BYTES
{
	LSB_Byte = Parameter & 0x00FF;
	MSB_Byte = (Parameter & 0xFF00) >> 8;
}

BOOL Transmitt_To_PC(int Word) 
	{
		Word_To_Byte(Word);
		DisableIntUART;
		if (!Write_To_PC(MSB_Byte)) return (FALSE);
		PIC_PC_ACK = 1;
		Delay_0_625US(9); // delay 5.5 usec
		PIC_PC_ACK = 0;
		if (!Write_To_PC(LSB_Byte)) return (FALSE);
		PIC_PC_ACK = 1;
		EnableIntUART;
		return (TRUE);
	}
	
long Output_Frequency_Measure(BOOL DDS_Number) // MEASURE OUTPUT SINUS FREQUENCY
{	
	char Tmp1 = 0, Tmp2 = 0;
	int Pulse_Counter = 0, Frequency_Sampling_Counter = 0;
	long Frequency_Measured_Value = 0;

	for (Frequency_Sampling_Counter = 0; Frequency_Sampling_Counter < 10; Frequency_Sampling_Counter ++)
		{
			RESET_TIMER3;
			if (!DDS_Number) { DDS1_RESET_PULSE_COUNTER; }
			else { DDS2_RESET_PULSE_COUNTER; }
	
			Timer3Enable;
			if (!DDS_Number) { DDS1_PULSE_COUNTER_ENABLE; }
			else { DDS2_PULSE_COUNTER_ENABLE; }
			
			while(!Frequency_Measurement_Completed); // WAIT 5 mSEC
			
			Frequency_Measurement_Completed = FALSE;
			
			if (!DDS_Number)
				{
					Tmp1 = TMR0L;
					Tmp2 = TMR0H;
				}
			else
				{
					Tmp1 = TMR1L;
					Tmp2 = TMR1H;
				}
			Pulse_Counter = Tmp2 * 0x100 + Tmp1;
			Frequency_Measured_Value = Frequency_Measured_Value + Pulse_Counter; // ACCUMULATE DDS1 COUNTED PULSES
			Pulse_Counter = 0;
		}	
	RESET_TIMER3;
	if (!DDS_Number) { DDS1_RESET_PULSE_COUNTER; }
	else { DDS2_RESET_PULSE_COUNTER; }
	
	Frequency_Measured_Value = Frequency_Measured_Value / 10; // CALCULATE COUNTED PULSES AVERAGE
	Frequency_Measured_Value = Frequency_Measured_Value * 200; // CALCULATE MEASURED FREQUENCY (NUMBER OF PULSES AT 1 SEC)
					
	return (Frequency_Measured_Value);
}

BOOL Validate_Frequency_Setting(long Value, BOOL DDS_Number) // FREQUENCY VALIDATION TABLE
{
	float Backup = 0, Backup1 = 0;
	
	if (DDS_Number)
		{
			Backup = (float)((float)Value / (float)DDS2_Frequency_Value);
			Backup1 = (float)((float)DDS2_Frequency_Value / (float)Value);
		}
	else
		{
			Backup = (float)((float)Value / (float)DDS1_Frequency_Value);
			Backup1 = (float)((float)DDS1_Frequency_Value / (float)Value);
		}
	
	if ((Backup < 0.9) || (Backup1 < 0.9)) return (FALSE);
	else return (TRUE);
}

int Get_Amplitude_Value(BOOL DDS_Number, BOOL Before_After_Switch) // SAMPLE AMPLITUDE VALUE
{
	int Value = 0;
	if (!Before_After_Switch)
		{
			if (!DDS_Number) A2D_Manual_Convert(DDS1_BEFORE_SW_SNS);
			else A2D_Manual_Convert(DDS2_BEFORE_SW_SNS);
		}
	else 
		{
			if (!DDS_Number) A2D_Manual_Convert(SMA1_OUT_SNS);
			else A2D_Manual_Convert(SMA2_OUT_SNS);
		}
	
	while (!A2D_Conversion_Completed);
	
	if (!Before_After_Switch)
		{
			if (!DDS_Number) Value = A2D_Value_Read(DDS1_BEFORE_SW_SNS);
			else Value = A2D_Value_Read(DDS1_BEFORE_SW_SNS);
		}
	else 
		{
			if (!DDS_Number) Value = A2D_Value_Read(SMA1_OUT_SNS);
			else Value = A2D_Value_Read(SMA2_OUT_SNS);
		}
	
	return(Value);
}

int Output_Amplitude_Measure(BOOL DDS_Number, BOOL Before_After_Switch) // MEASURE AMPLITUDE VALUE
{
	int Amplitude_Tmp_Value = 0, Amplitude_Value = 0, counter = 0;
	float Amplitude_Real_Value = 0;
	
	Frequency_Value_Set(100, DDS_Number); // SET FREQUENCY TO 100HZ
	
	//Delay_625US(45); // delay 30 msec
	
	Init_ADC(Manual_Working_Mode, FALSE);
	Set_A2D_Mode(Manual_Working_Mode);
	A2D_Module_Enable;

	Amplitude_Tmp_Value = Get_Amplitude_Value(DDS_Number, Before_After_Switch); // TAKE FIRST SAMPLE

	Timer2Enable; // SWITCH ON TIMEOUT TIMER
	
	while (Amplitude_Tmp_Value != 0) // WAIT UNTIL SINUS WILL RIZE
		{
			Amplitude_Tmp_Value = Get_Amplitude_Value(DDS_Number, Before_After_Switch);
			if (Timeout) 
				{
					if (!DDS_Number) Send_Error_Word(DDS1_Amplitude_Measure_Error);
					else Send_Error_Word(DDS2_Amplitude_Measure_Error);
					Interrupt_Signal_Strobe();
					//break;
					return(0);
				}
		}
	
	TMR2_INTFLAG = 0; // DISABLE TIMEOUT TIMER
	Timer2Disable;
	RESET_TIMER2;
	Timeout_Counter = 0;

	if (!Timeout) Timer2Enable; // SWITCH ON TIMEOUT TIMER

	while (Amplitude_Tmp_Value >= Amplitude_Value) // WAIT UNTIL SINUS WILL FALL
		{
			Amplitude_Value = Amplitude_Tmp_Value;
			Amplitude_Tmp_Value = Get_Amplitude_Value(DDS_Number, Before_After_Switch);
			if (Timeout) 
				{
					Amplitude_Value = 0;
					Timeout = FALSE;
					if (!DDS_Number) Send_Error_Word(DDS1_Amplitude_Measure_Error);
					else Send_Error_Word(DDS2_Amplitude_Measure_Error);
					Interrupt_Signal_Strobe();
					//break;
					return(0);
				}
		}
		
	TMR2_INTFLAG = 0; // DISABLE TIMEOUT TIMER
	Timer2Disable;
	RESET_TIMER2;
	Timeout_Counter = 0;
		
	Set_A2D_Mode(IDLE);
	A2D_Automatic_Convert(IDLE, FALSE, TRUE);
	Timer2Enable;

	if (!DDS_Number) Frequency_Value_Set(DDS1_Frequency_Value * 1000, DDS_Number); // SET ORIGINAL FREQUENCY
	else Frequency_Value_Set(DDS2_Frequency_Value * 1000, DDS_Number);

	Amplitude_Real_Value = Amplitude_Value; // CONVERT TO FLOAT 
	Amplitude_Real_Value  = (Amplitude_Real_Value * Vref) / 4096; // CALCULATE REAL SINE (STEP * DIGITAL NUMBER)
	Amplitude_Real_Value = (Amplitude_Real_Value * Divider) / 1.41421; //	CALCULATE RMS SINE

	if (!DDS_Number)
		{
			if ((DDS1_Frequency_Value >= 1000) && (DDS1_Frequency_Value < 2000) && (System_Registers_Array[10] > 1000)) Amplitude_Real_Value = Amplitude_Real_Value / 1.1;
			if ((DDS1_Frequency_Value >= 2000) && (DDS1_Frequency_Value <= 2500) && (System_Registers_Array[10] > 1000)) Amplitude_Real_Value = Amplitude_Real_Value / 1.21;
		}
	else
		{
			if ((DDS2_Frequency_Value >= 1000) && (DDS2_Frequency_Value < 2000) && (System_Registers_Array[15] > 1000)) Amplitude_Real_Value = Amplitude_Real_Value / 1.1;
			if ((DDS2_Frequency_Value >= 2000) && (DDS2_Frequency_Value <= 2500) && (System_Registers_Array[15] > 1000)) Amplitude_Real_Value = Amplitude_Real_Value / 1.21;
		}
		
	Amplitude_Value = Amplitude_Real_Value * Total_Signal_Gain * 1000;
	
	return(Amplitude_Value);
}

BOOL Validate_Amplitude_Setting(int Value, BOOL DDS_Number) // AMPLITUDE VALIDATION TABLE
{
	float Backup = 0, Backup1 = 0;
	
	if (!DDS_Number)
		{
			Backup = (float)((float)Value / (float)System_Registers_Array[10]);
			Backup1 = (float)((float)System_Registers_Array[10] / (float)Value);
		}
	else 
		{
			Backup = (float)((float)Value / (float)System_Registers_Array[15]);
			Backup1 = (float)((float)System_Registers_Array[15] / (float)Value);
		}

	if (!DDS_Number)
		{
			if (System_Registers_Array[10] > 200) 
				if ((Backup < 0.75) || (Backup1 < 0.75)) return (FALSE);
				else return (TRUE);
			else
				if ((Backup < 0.5) || (Backup1 < 0.5)) return (FALSE);
				else return (TRUE);
		}
	else
		{
			if (System_Registers_Array[15] > 200) 
				if ((Backup < 0.75) || (Backup1 < 0.75)) return (FALSE);
				else return (TRUE);
			else
				if ((Backup < 0.5) || (Backup1 < 0.5)) return (FALSE);
				else return (TRUE);
		}
} 

void Synthesizer_Bit_Strobe1() // INSERT WORD/BIT TO REGISTER/S
 {
			DDS1_SYN_W_CLK = 0;
			DDS1_SYN_W_CLK = 1;
			DDS1_SYN_W_CLK = 0;
		
			DDS2_SYN_W_CLK = 0;
			DDS2_SYN_W_CLK = 1;
			DDS2_SYN_W_CLK = 0;
 }
 
 void Synthesizer_Apply_Changes1() // APPLY CHANGES OF REGISTERS TO SYNTHESIZER DEVICE
 {
	
			DDS1_SYN_FQ_UD = 0;
			DDS2_SYN_FQ_UD = 0;
			DDS1_SYN_FQ_UD = 1;
			DDS2_SYN_FQ_UD = 1;
			DDS1_SYN_FQ_UD = 0;
			DDS2_SYN_FQ_UD = 0;
 }

//----------------------------------------------------------------------------
//  int main(void)
//
//  Description:
//      Main function. Initializes everything then starts running.
//
//  Arguments:
//      None 
//  Return value:
//      None
//----------------------------------------------------------------------------

void main (void)
{
	char Tempr = 0;
	int TEC_Voltage_Value = 0, i = 0, j = 0;
	int Backup = 0, Backup1 = 0;
	long Temporary = 0, frequency1 = 0, frequency2 = 0;

	InitIO(); // IO INIT
	Init(); // MAIN SYSTEM INITIALIZATIONS
	
	//EnableIntSPI;
	
	Powerup_Domains_On();
	Set_A2D_Mode(IDLE);
	A2D_Automatic_Convert(IDLE, FALSE, TRUE);
//	Timer2Enable;
	
	DDS1_Frequency_Value = 1500; //1.5MHZ
//	DDS1_Frequency_Set = TRUE;
	DDS1_Amplitude_Value = 2500; // 2.5V
	DDS1_Amplitude_Set = TRUE;
	DDS2_Frequency_Value = 1500; //1.5MHZ
//	DDS2_Frequency_Set = TRUE;
	DDS2_Amplitude_Value = 2500; // 2.5V
	DDS2_Amplitude_Set = TRUE;
	PC_Transaction_Completed = TRUE;
	DDS1_2_POWER_ON_OFF = TRUE;
	DDS1_OUTPUT_ENABLE_DISABLE = TRUE;
	DDS2_OUTPUT_ENABLE_DISABLE = TRUE;
	/*DDS1_SYN_RESET = 1; 
	DDS2_SYN_RESET = 1; 
	DDS1_POT_RESETn = 0;
	DDS2_POT_RESETn = 0;
	Delay_6_25US(100);
	DDS1_SYN_RESET = 0; 
	DDS2_SYN_RESET = 0; 
	DDS1_POT_RESETn = 1;
	DDS2_POT_RESETn = 1;
	Delay_6_25US(7);*/
	DDS1_POT_RESETn = 1;
	DDS2_POT_RESETn = 1;
	
/*	Init_ADC(Manual_Working_Mode, FALSE);
	Set_A2D_Mode(Manual_Working_Mode);
	A2D_Module_Enable;
	A2D_Manual_Convert(P12V);
	while (!A2D_Conversion_Completed);
	Backup = A2D_Value_Read(P12V);*/
	
	while (1)
	{
		char PC_Received_Word = 0;
		
		frequency1 = DDS1_Frequency_Value / 0.02328;
		frequency2 = DDS2_Frequency_Value / 0.02328;

		Delay_625US(1000);
		
		for (j = 0; j < 32; j++) // INSERT FREQUENCY WORD
		{
			DDS1_SYN_D7 = frequency1 & 0x01;
			DDS2_SYN_D7 = frequency2 & 0x01;
			frequency1 = frequency1 >> 1;
			frequency2 = frequency2 >> 1;
			Synthesizer_Bit_Strobe1();
		}
		
		DDS1_SYN_D7 = 0;
		DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe1();
		DDS1_SYN_D7 = 0;
		DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe1();
		
		DDS1_SYN_D7 = 0; // BIT 34 IS POWER DOWN, IF "1" SO DEVICE POWER DOWN
		DDS2_SYN_D7 = 0; 
		Synthesizer_Bit_Strobe1();
		
		for (j = 0; j < 5; j++) // INSERT PHASE WORD
		{
			DDS1_SYN_D7 = 0; 
			DDS2_SYN_D7 = 0; 
			Synthesizer_Bit_Strobe1();
		}
		
	Synthesizer_Apply_Changes1();
		
		if (PC_Word_Detected)
			{
				PC_Word_Detected = FALSE;
				PC_Received_Word = Read_From_PC();

				if (!Data_Receive_Error) PC_Word_Parser(PC_Received_Word);
				else 
					{
						Send_Error_Word(UART_Receiver_Error);
						Interrupt_Signal_Strobe();
					}
				EnableIntUART;
			}
		
		if (A2D_Less_Greater_Switch_Flag) // SWITCH BETWEEN LESS AND GREATER THEN THRESHOLD VALUE MODULE, VALID ONLY AT IDLE MODE
			{
				A2D_Less_Greater_Switch_Flag = FALSE;
				A2D_Automatic_Convert(IDLE, A2D_Less_Greater_Switch, FALSE); // DISABLE A2D MODULE
				A2D_Less_Greater_Switch = !A2D_Less_Greater_Switch;
				A2D_Automatic_Convert(IDLE, A2D_Less_Greater_Switch, TRUE); // ENABLE A2D MODULE
				Timer2Enable;
			}
			
		if (A2D_Voltage_Domains_Measurement_Error) // A2D MEASUREMENT ERROR BEHAVIOUR MODULE
			{
				A2D_Voltage_Domains_Measurement_Error = FALSE;
			}
		
		if (PC_Transaction_Completed)
			{
				PC_Transaction_Completed = FALSE;
				if (POWER_24V_VAC_EN) 
					{
						Domains_Power_Control(PS_24V_VAC, TRUE);
					}
				else
					{
						Domains_Power_Control(PS_24V_VAC, FALSE);
					}
				
				if (POWER_48V_EN) 
					{
						Domains_Power_Control(PS_48V, TRUE);
					}
				else
					{
						Domains_Power_Control(PS_48V, FALSE);
					}
				
				if (DDS1_2_POWER_ON_OFF) 
					{
						Domains_Power_Control(DDS_12V, TRUE);
						Domains_Power_Control(DDS_N12V, TRUE);
						Domains_Power_Control(DDS_5V, TRUE);
					}
				else
					{
						Domains_Power_Control(DDS_5V, FALSE);
						Domains_Power_Control(DDS_12V, FALSE);
						Domains_Power_Control(DDS_N12V, FALSE);
					}
					
				if (DDS1_OUTPUT_ENABLE_DISABLE) 
					{
						DDS1_OUT_ON;
					}
				else
					{
						DDS1_OUT_OFF;
					}
					
				if (DDS1_HW_RESET) 
					{
						DDS1_HW_RESET = FALSE;
						DDS1_SYN_RESET = 1; 
						DDS1_POT_RESETn = 0;
						Delay_6_25US(3);
						DDS1_SYN_RESET = 0; 
						DDS1_POT_RESETn = 1;
						Delay_6_25US(7);
						System_Registers_Array[7] = System_Registers_Array[7] & 0xFEC3; // ZEROIZE DDS1 HARDWARE RESET BIT 
					}
					
				if (DDS1_SW_RESET) 
					{
						DDS1_SW_RESET = FALSE;
						DDS1_GEN_FAIL_INT = 0;
						System_Registers_Array[7] = System_Registers_Array[7] & 0xFDC3; // ZEROIZE DDS1 SOFTWARE RESET BIT 
					}
				
				if (DDS2_OUTPUT_ENABLE_DISABLE) 
					{
						DDS2_OUT_ON;
					}
				else
					{
						DDS2_OUT_OFF;
					}
					
				if (DDS2_HW_RESET) 
					{
						DDS2_HW_RESET = FALSE;
						DDS2_SYN_RESET = 1; 
						DDS2_POT_RESETn = 0;
						Delay_6_25US(3);
						DDS2_SYN_RESET = 0; 
						DDS2_POT_RESETn = 1;
						Delay_6_25US(7);
						System_Registers_Array[12] = System_Registers_Array[12] & 0xFEC3; // ZEROIZE DDS2 HARDWARE RESET BIT
					}
					
				if (DDS2_SW_RESET) 
					{
						DDS2_SW_RESET = FALSE;
						DDS2_GEN_FAIL_INT = 0;
						System_Registers_Array[12] = System_Registers_Array[12] & 0xFDC3; // ZEROIZE DDS2 SOFTWARE RESET BIT 
					}
					
				if (AMP1_ENABLE) 
					{
						AMP_ENABLE = 1;
					}
				else
					{
						AMP_ENABLE = 0;
					}
				
				if (AMP2_ENABLE) 
					{
						AMP_ENABLE2 = 1;
					}
				else
					{
						AMP_ENABLE2 = 0;
					}
					
				if (VAC_VALVE2_ON_OFF) 
					{
						Domains_Power_Control(VAC_VALVE, TRUE);
					}
				else
					{
						Domains_Power_Control(VAC_VALVE, FALSE);
					}
					
				if (HP_TEC_ON_OFF) 
					{
						Domains_Power_Control(HP_TEC, TRUE);
					}
				else
					{
						Domains_Power_Control(HP_TEC, FALSE);
					}
					
				if (HP_LED_1) 
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tempr | 0x10); // SET RELEVANT BIT
						
					}
				else
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tempr & 0xEF); // CLEAR RELEVANT BIT
					}
					
				if (HP_LED_2) 
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tempr | 0x20); // SET RELEVANT BIT
						
					}
				else
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tempr & 0xDF); // CLEAR RELEVANT BIT
					}
					
				if (HP_LED_3) 
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x13, Tempr | 0x20); // SET RELEVANT BIT
						
					}
				else
					{
						Tempr = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x13, Tempr & 0xDF); // CLEAR RELEVANT BIT
					}
					
				if (TEC_Voltage_Set) 
					{
						TEC_Voltage_Set = FALSE;
						
						Backup = System_Registers_Array[34];
						TEC_Voltage_Value = Clear_Headers_From_The_Word(System_Registers_Array[34]);
						System_Registers_Array[34] = Backup;
						if (!TEC_Power_Set(TEC_Voltage_Value))
							{
								Send_Error_Word(TEC_Voltage_Error);
							}
					}
										
				if (DDS1_Frequency_Set) 
					{
						DDS1_Frequency_Set = FALSE;
						Backup = System_Registers_Array[8];
				//		DDS1_Frequency_Value = Clear_Headers_From_The_Word(System_Registers_Array[8]);
						System_Registers_Array[8] = Backup;
						Frequency_Value_Set((long)((long)DDS1_Frequency_Value * (long)1000), FALSE); // CONVERT THE FREQUENCY TO KILO HERZ 
					}
					
				if (DDS1_Amplitude_Set) 
					{
						DDS1_Amplitude_Set = FALSE;
						Backup = System_Registers_Array[10];
				//		DDS1_Amplitude_Value = Clear_Headers_From_The_Word(System_Registers_Array[10]);
						System_Registers_Array[10] = Backup;
						DDS1_Amplitude_Value = Swing_Compensation_Table(DDS1_Frequency_Value, DDS1_Amplitude_Value);
						DDS1_Amplitude_Value = DDS1_Amplitude_Value / 30.373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
				
						Update_Wiper_Position(DDS1_Amplitude_Value, FALSE); // COMPENSATION OF THE QUANTIZATION MISTAKE
					}
					
				if (DDS2_Frequency_Set) 
					{
						DDS2_Frequency_Set = FALSE;
						Backup = System_Registers_Array[13];
					//	DDS2_Frequency_Value = Clear_Headers_From_The_Word(System_Registers_Array[13]);
						System_Registers_Array[13] = Backup;
						Frequency_Value_Set((long)((long)DDS2_Frequency_Value * (long)1000), TRUE); // CONVERT THE FREQUENCY TO KILO HERZ 
					}
					
				if (DDS2_Amplitude_Set) 
					{
						DDS2_Amplitude_Set = FALSE;
						Backup = System_Registers_Array[15];
				//		DDS2_Amplitude_Value = Clear_Headers_From_The_Word(System_Registers_Array[15]);
						System_Registers_Array[15] = Backup;
						DDS2_Amplitude_Value = Swing_Compensation_Table(DDS2_Frequency_Value, DDS2_Amplitude_Value);
						DDS2_Amplitude_Value = DDS2_Amplitude_Value / 30.373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
				
						Update_Wiper_Position(DDS2_Amplitude_Value, TRUE); // COMPENSATION OF THE QUANTIZATION MISTAKE
					}
				Interrupt_Signal_Strobe();
				PIC_PC_ACK = 1;
			}
			
		if (Power_Control_Register_Read) // ALL READ ONLY BITS SHOULD BE READ FROM FPGA, WHEN FPGA WILL BE BUILT, SHOULD BE COMPLETED
			{
				Power_Control_Register_Read = FALSE;
				
				if (!Transmitt_To_PC(System_Registers_Array[1])) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (P3_3V_Voltage_Read) 
			{
				P3_3V_Voltage_Read = FALSE;
				Backup = A2D_Value_Read(P3_3V);
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (P5V_Voltage_Read) 
			{
				P5V_Voltage_Read = FALSE;
				Backup = A2D_Value_Read(P5V);
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}

		if (P12V_Voltage_Read) 
			{
				P12V_Voltage_Read = FALSE;
				Backup = A2D_Value_Read(P12V);
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (P24V_Voltage_Read) 
			{
				P24V_Voltage_Read = FALSE;
				Backup = A2D_Value_Read(P24V);
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (P48V_Voltage_Read) 
			{
				P48V_Voltage_Read = FALSE;
				Backup = A2D_Value_Read(P48V);
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS1_Control_Register_Read) 
			{
				DDS1_Control_Register_Read = FALSE;
				Backup = System_Registers_Array[7];
				Tempr = Read_From_Extender(Expander_B_SPI, 0x12); // READ PORT STATUS FROM EXPANDER B
				Tempr = Tempr & 0x0C; // MASK ALL OUTSIDE DDS_12V AND DDS_5V BITS
				Backup = Backup | (Tempr & 0x08); // MASK ALL BITS OUTSIDE DDS_12V AND INSERT IT TO IT IS LOCATION TO THE REGISTER
				Tempr = (Tempr & 0x04) << 3; // MASK ALL BITS OUTSIDE DDS_5V 
				Backup = Backup | Tempr; // INSERT IT TO IT IS LOCATION TO THE REGISTER 
				Backup1 = A2D_Value_Read(N12V);
				if ((Backup1 > N12V_LOW_THRESHOLD) && (Backup1 < N12V_HIGH_THRESHOLD)) Backup = Backup & 0xFFEF; // IF -12V MEASUREMENT CORRECT CLEAR -12 ERROR BIT
				else Backup = Backup | 0x0010;
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS1_Frequency_Read) 
			{
				DDS1_Frequency_Read = FALSE;
				if (!Transmitt_To_PC(DDS1_Frequency_Value)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS1_Frequency_Measure) 
			{
				DDS1_Frequency_Measure = FALSE;
				Temporary = Output_Frequency_Measure(FALSE);
				if (Validate_Frequency_Setting(Temporary, FALSE))
					{
						Backup = (int)(Temporary / 1000); // CONVERT TO KILO HERZ
						if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
					}
				else Send_Error_Word(DDS1_Frequency_Error);
			}
			
		if (DDS1_Amplitude_Read) 
			{
				DDS1_Amplitude_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[10])) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS1_Amplitude_Measure) 
			{
				DDS1_Amplitude_Measure = FALSE;
				Tempr = Output_Amplitude_Measure(FALSE, FALSE);
				if (Validate_Amplitude_Setting(Tempr, FALSE))
					{
						if (!Transmitt_To_PC(Tempr)) Send_Error_Word(UART_Transmitter_Error);
					}
				else Send_Error_Word(DDS1_Amplitude_Error);
			}
			
		if (DDS2_Control_Register_Read) 
			{
				DDS2_Control_Register_Read = FALSE;
				Backup = System_Registers_Array[12];
				Tempr = Read_From_Extender(Expander_B_SPI, 0x12); // READ PORT STATUS FROM EXPANDER B
				Tempr = Tempr & 0x0C; // MASK ALL OUTSIDE DDS_12V AND DDS_5V BITS
				Backup = Backup | (Tempr & 0x08); // MASK ALL BITS OUTSIDE DDS_12V AND INSERT IT TO IT IS LOCATION TO THE REGISTER
				Tempr = (Tempr & 0x04) << 3; // MASK ALL BITS OUTSIDE DDS_5V 
				Backup = Backup | Tempr; // INSERT IT TO IT IS LOCATION TO THE REGISTER 
				Backup1 = A2D_Value_Read(N12V);
				if ((Backup1 > N12V_LOW_THRESHOLD) && (Backup1 < N12V_HIGH_THRESHOLD)) Backup = Backup & 0xFFEF; // IF -12V MEASUREMENT CORRECT CLEAR -12 ERROR BIT
				else Backup = Backup | 0x0010;
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
		
		if (DDS2_Frequency_Read) 
			{
				DDS2_Frequency_Read = FALSE;
				if (!Transmitt_To_PC(DDS2_Frequency_Value)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS2_Frequency_Measure) 
			{
				DDS2_Frequency_Measure = FALSE;
				Temporary = Output_Frequency_Measure(TRUE);
				if (Validate_Frequency_Setting(Temporary, TRUE))
					{
						Backup = (int)(Temporary / 1000); // CONVERT TO KILO HERZ
						if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
					}
				else Send_Error_Word(DDS2_Frequency_Error);
			}
			
		if (DDS2_Amplitude_Read) 
			{
				DDS2_Amplitude_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[15])) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (DDS2_Amplitude_Measure) 
			{
				DDS2_Amplitude_Measure = FALSE;
				Tempr = Output_Amplitude_Measure(TRUE, FALSE);
				if (Validate_Amplitude_Setting(Tempr, TRUE))
					{
						if (!Transmitt_To_PC(Tempr)) Send_Error_Word(UART_Transmitter_Error);
					}
				else Send_Error_Word(DDS2_Amplitude_Error);
			}
			
		if (Amplifier1_Control_Register_Read) 
			{
				Amplifier1_Control_Register_Read = FALSE;
			}
			
		if (Amplifier1_Temperature_Read) 
			{
				Amplifier1_Temperature_Read = FALSE;
			}
			
		if (Amplifier1_Current_Read) 
			{
				Amplifier1_Current_Read = FALSE;
			}
		
		if (Amplifier1_Reverse_Power_Read) 
			{
				Amplifier1_Reverse_Power_Read = FALSE;
			}
			
		if (Amplifier1_Forward_Power_Read) 
			{
				Amplifier1_Forward_Power_Read = FALSE;
			}
			
		if (Amplifier2_Control_Register_Read) 
			{
				Amplifier2_Control_Register_Read = FALSE;
			}
			
		if (Amplifier2_Temperature_Read) 
			{
				Amplifier2_Temperature_Read = FALSE;
			}
			
		if (Amplifier2_Current_Read) 
			{
				Amplifier2_Current_Read = FALSE;
			}
			
		if (Amplifier2_Reverse_Power_Read) 
			{
				Amplifier2_Reverse_Power_Read = FALSE;
			}
			
		if (Amplifier2_Forward_Power_Read) 
			{
				Amplifier2_Forward_Power_Read = FALSE;
			}
			
		if (Handpiece_Control_Register_Read) 
			{
				Handpiece_Control_Register_Read = FALSE;
			}
			
		if (Handpiece_Temperature1_Read) 
			{
				Handpiece_Temperature1_Read = FALSE;
			}
			
		if (Handpiece_Temperature2_Read) 
			{
				Handpiece_Temperature2_Read = FALSE;
			}
			
		if (Handpiece_Temperature3_Read) 
			{
				Handpiece_Temperature3_Read = FALSE;
			}
			
		if (Handpiece_Temperature4_Read) 
			{
				Handpiece_Temperature4_Read = FALSE;
			}
			
		if (Handpiece_5V_3_3V_Read) 
			{
				Handpiece_5V_3_3V_Read = FALSE;
			}

		if (Vacuum_Pressure_Read) 
			{
				Vacuum_Pressure_Read = FALSE;
			}

		if (TEC_Voltage_Read) 
			{
				TEC_Voltage_Read = FALSE;
			}
			
		if (TEC_Voltage_Measure) 
			{
				TEC_Voltage_Measure = FALSE;
			}
			
		if (Errors1_Register_Read) 
			{
				Errors1_Register_Read = FALSE;
			}
		
		if (Errors2_Register_Read) 
			{
				Errors2_Register_Read = FALSE;
			}
			
		if (MCU_FPGA_Version_Read) 
			{
				MCU_FPGA_Version_Read = FALSE;
			}
			
		if (Controller_Handpiece_Serial_Read) 
			{
				Controller_Handpiece_Serial_Read = FALSE;
			}

/*	while (1)
	{
	Delay_0_625US(10); // delay 55 usec
	DDS1_OUT_ON;
	Delay_0_625US(10); // delay 55 usec
	DDS1_OUT_OFF;
	}*/
	for (i = 0; i < 10; i++)
	Delay_625US(1000);
	DDS1_Frequency_Value = DDS1_Frequency_Value + 100;
	DDS2_Frequency_Value = DDS2_Frequency_Value + 100;
	
	}
	
} // main
