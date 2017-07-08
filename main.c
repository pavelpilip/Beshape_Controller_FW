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

#include "FPGA.h"

#include "Pseudo_SPI.h"

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
BOOL Controller_Handpiece_Serial_Read = FALSE, Errors2_Register_Read = FALSE, Amplifier_CW_Mode_Switches_Set = FALSE, Amplifier_Pulse_Mode_Switches_Set = FALSE;
BOOL CW_Mode_Switches_Register_Read = FALSE, Pulse_Mode_Switches_Register_Read = FALSE, FPGA_Interrupt_Flag = FALSE, Reset_All_Errors = FALSE;
BOOL Main_Control_Register_Read = FALSE, Tissue_Temperature_Measurement_Value_Read = FALSE, FPGA_Control_Register = FALSE, FPGA_Control_Register_Read = FALSE;

BOOL UART_Communication_Flag = FALSE, UART_Timeout = FALSE;
char MSB_Byte = 0, LSB_Byte = 0, FPGA_Version_Number = 0;

/////////////////////REGISTERS BITS FLAGS MODULE END///////////////////////////////

extern BOOL Data_Receive_Error;//, UART_Communication_Flag, UART_Timeout;
extern Frequency_Measurement_Completed;

BOOL PC_Word_Detected = FALSE, Read_Write_Transaction = FALSE;
BOOL Timeout = FALSE, System_Domains_Measure_Enable_Flag = FALSE, A2D_Timeout = FALSE;
char A2D_Less_Greater_Switch_Counter = 0, Received_Transaction_Address = 0, PC_Transaction_Counter = 1, PC_Transaction_Completed = FALSE;
char System_Work_Mode = System_Idle_Mode;
int Error1_Register = 0, Error2_Register = 0, Received_PC_Data = 0, System_Registers_Array[System_Registers_Number] = {0};
int DDS1_Frequency_Value = 0, DDS2_Frequency_Value = 0, DDS1_Amplitude_Value = 0, DDS2_Amplitude_Value = 0, Timeout_Counter = 0, A2D_Sine_Amplitude_Timeout_Counter = 0;
int Amplifier_CW_Mode_Switches_Value = 0, Amplifier_Pulse_Mode_Switches_Value = 0;
//float DDS1_Amplitude_Value = 0;

#pragma interrupt high_priority_interrupt
void high_priority_interrupt (void)
{

	if (UART_INTFLAG)
	{
		UART_INTFLAG = 0;
		DisableIntUART;
		PIC_PC_ACK = 0; // CONTROLLER IS BUSY
		PC_Word_Detected = TRUE;
		return;
	}
	
	if (TMR3_INTFLAG)
	{
		Timer3Disable;
		TMR3_INTFLAG = 0;
		RESET_TIMER3;
		if ((DDS1_Frequency_Measure) || (DDS2_Frequency_Measure))
			{
				DDS1_PULSE_COUNTER_DISABLE; // DISABLE DDS1 PULSE COUNTER
				DDS2_PULSE_COUNTER_DISABLE; // DISABLE DDS2 PULSE COUNTER
				Frequency_Measurement_Completed = TRUE;
			}
		else if ((DDS1_Amplitude_Measure) || (DDS2_Amplitude_Measure))
			{
				A2D_Sine_Amplitude_Timeout_Counter ++;
				if (System_Work_Mode == System_Configuration_Mode) 
					{
						if (A2D_Sine_Amplitude_Timeout_Counter > 39) // 195 MILI SECONDS PASSED
							{
								A2D_Sine_Amplitude_Timeout_Counter = 0;
								Timeout = TRUE;	
							}
						else Timer3Enable;
					}
			}
		return;
	}
	
	if (TMR5_INTFLAG)
	{
		Timer5Disable;
		TMR5_INTFLAG = 0;
		RESET_TIMER5;
		Timeout_Counter ++;
				
		if (System_Work_Mode == System_Idle_Mode) 
			{
				if (Timeout_Counter > 3200) // 16 SECONDS PASSED
					{
						Timeout_Counter = 0;
						System_Domains_Measure_Enable_Flag = TRUE;
					}
				/*else if ((Timeout_Counter > 77) && (UART_Communication_Flag)) // 500 MILI SECONDS PASSED AND UART COMMUNICATION ENABLED
					{
						Timeout_Counter = 0;	
						UART_Timeout = TRUE; 
					}*/
				else Timer5Enable;
			}
		return;
	}

	if (Main_Interrupt_Detected)
	{
		Main_Interrupt_Detection_Disable;
		Main_Interrupt_Detected = 0;
		PIC_PC_ACK = 0; // CONTROLLER IS BUSY
		FPGA_Interrupt_Flag = TRUE;
		return;	
	}

	if (Power_On_Detected)
	{
		Power_On_Detection_Disable;
		Power_On_Detected = 0;
		Power_On_Detection_Enable;
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

	OSCCON = 0x33;
	OSCCON2 = 0xC4;
	OSCCON3 = 0x00;
	OSCCON4 = 0x20;
	OSCTUNE = 0x00; 	

	Init_Interrupts();
	
	InitSPI1Master();
	InitTimer0INTMode();
	InitTimer1INTMode();
	InitTimer3INTMode();
	InitTimer5INTMode();
	Init_UART();
	Init_ADC();
	
	PIC_SPI_Enable; // ENABLE SPI BUS
	
	Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
	Write_To_Extender(Expander_B_SPI, 0x05, 0x00); // DISABLE ALL INTERRUPTS
	
	Extender_Init(Expander_C_SPI);
	Extender_Init(Expander_D_SPI);
	Delay_625US(100); // delay 62.5 msec, let to the all domains time to disconnect 
	Extender_Init(Expander_A_SPI);
	Extender_Init(Expander_B_SPI);
	
	System_Power_Init();
	TEC_Power_Init();
	
	FPGA_Version_Number = Read_From_FPGA(0x9);
	
	Delay_6_25US(9); // delay 55 usec
}

void Interrupt_Signal_Strobe() // STROBE INTERRUPT SIGNAL
{
	char Tmp = 0;
	Tmp = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
	Write_To_Extender(Expander_C_SPI, 0x13, Tmp | 0x08); // SET INTERRUPT TRIGGER
	Write_To_Extender(Expander_C_SPI, 0x13, Tmp & 0xF7); // CLEAR INTERRUPT TRIGGER
}

void Test_LED_Operation(char LED_Number, BOOL On_Off)
{
	char Tmp = 0;
	if (On_Off)
		{
			switch (LED_Number) 
				{
					case 1: 
						Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
						Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0xFD); // ON TEST LED
					break;
					case 2: 
						Tmp = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tmp & 0x7F); // ON TEST LED
					break;
					case 3: 
						Tmp = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x13, Tmp & 0xFE); // ON TEST LED
					break;
					default:
															
					break;
				}
		}
	else
		{
			switch (LED_Number) 
				{
					case 1: 
						Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
						Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x02); // OFF TEST LED
					break;
					case 2: 
						Tmp = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x12, Tmp & 0x80); // OFF TEST LED
					break;
					case 3: 
						Tmp = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
						Write_To_Extender(Expander_C_SPI, 0x13, Tmp & 0x01); // OFF TEST LED
					break;
					default:
															
					break;
				}
		}
}

void Send_Error_Word(int Error)
{
	switch (Error) 
		{
			case PC_Communication_Fault: 
				Error1_Register = Error1_Register | 0x01; // SET PC COMMUNICATION ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Emergency_Switch_State: 
				Error1_Register = Error1_Register | 0x02; // SET EMERGENCY SWITCH ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case UART_Receiver_Error: 
				Error1_Register = Error1_Register | 0x04; // SET UART RECEIVER ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Word_Format_Error: 
				Error1_Register = Error1_Register | 0x08; // SET WORD FORMAT ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case UART_Transaction_Error: 
				Error1_Register = Error1_Register | 0x10; // SET WORD FORMAT ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case TEC_Voltage_Error: 
				Error1_Register = Error1_Register | 0x20; // SET TEC VOLTAGE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Parser_Working_Error: 
				Error1_Register = Error1_Register | 0x40; // PARSER ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case UART_Transmitter_Error: 
				Error1_Register = Error1_Register | 0x100; // SET UART TRANSMITTER ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS1_Frequency_Error: 
				Error1_Register = Error1_Register | 0x200; // SET DDS1 FREQUENCY ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS1_Amplitude_Error: 
				Error1_Register = Error1_Register | 0x400; // SET DDS1 AMPLITUDE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS2_Frequency_Error: 
				Error1_Register = Error1_Register | 0x800; // SET DDS2 FREQUENCY ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS2_Amplitude_Error: 
				Error1_Register = Error1_Register | 0x1000; // SET DDS2 AMPLITUDE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS1_Amplitude_Measure_Error: 
				Error2_Register = Error2_Register | 0x01; // SET DDS1 AMPLITUDE MEASURE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case DDS2_Amplitude_Measure_Error: 
				Error2_Register = Error2_Register | 0x02; // SET DDS2 AMPLITUDE MEASURE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Amplifier_Pulse_Mode_Word_Error: 
				Error2_Register = Error2_Register | 0x04; // SET A2D CONVERSION ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case P3V3_Domain_Error: 
				Error2_Register = Error2_Register | 0x08; // SET P3V3 ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case P5V_Domain_Error: 
				Error2_Register = Error2_Register | 0x10; // SET P5V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case N5V_Domain_Error: 
				Error2_Register = Error2_Register | 0x20; // SET N5V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Tissue_temperature_measure_Error: 
				Error2_Register = Error2_Register | 0x40; // SET TISSUE TEMPERATURE MEASURE ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;			
			case P12V_Domain_Error: 
				Error2_Register = Error2_Register | 0x100; // SET P12V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case N12V_Domain_Error: 
				Error2_Register = Error2_Register | 0x200; // SET N12V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case P24V_Domain_Error: 
				Error2_Register = Error2_Register | 0x400; // SET P24V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case P150V_Domain_Error: 
				Error2_Register = Error2_Register | 0x800; // SET P150V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case P48V_Domain_Error: 
				Error2_Register = Error2_Register | 0x1000; // SET P48V ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			case Pulser_IC_Error: 
				Error2_Register = Error2_Register | 0x2000; // SET PULSER IC ERROR BIT
				Test_LED_Operation(2, TRUE);
			break;
			default:
													
			break;
		}
	Interrupt_Signal_Strobe();
}
      			
BOOL PC_Word_Parser(unsigned char Word) // RETURN "0", ERROR IN PARSING
{
	unsigned char Tmp = 0, Register_BIT_Counter = 0;
	int Received_Data_Backup = 0;
	
/////////////////DETECT THE RECEIVED WORD IS TYPE MODULE BEGIN/////////////////////////	
	
	Test_LED_Operation(1, TRUE); // POWER ON COMMUNICATION OPERATION LED
	
	Tmp = Word >> 6;
	Word = Word & 0x3F; // CUT HEADER BITS
	if (Tmp == 0x1) // RECEIVED WORD IS DATA
		{
			if ((PC_Transaction_Counter > 1) && (!Read_Write_Transaction)) // RECEIVED WORD IS DATA AND WRITE TRANSACTION
				{
					if (PC_Transaction_Counter == 2) 
						{
							Received_PC_Data = Word;
							Received_PC_Data = Received_PC_Data << 6;
							PC_Transaction_Counter ++;
							PIC_PC_ACK = 1;
							return (TRUE);
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
					Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
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
					Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
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
					Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
					return (FALSE);
				}
		}
	else 
		{
			Send_Error_Word(Word_Format_Error);
			PIC_PC_ACK = 1;
			Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
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
					case 0x28: // Amplifier CW mode switches register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						Amplifier_CW_Mode_Switches_Set = TRUE;
						break;
					case 0x29: // Amplifier pulse mode switches register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						Amplifier_Pulse_Mode_Switches_Set = TRUE;
						break;
					case 0x2A: // FPGA control register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														FPGA_Control_Register = TRUE;
													break;
												default:
													
													break;
											}
									}
								else 
									{
										switch (Register_BIT_Counter) 
											{
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					case 0x2B: // Main control register  
						System_Registers_Array[Received_Transaction_Address] = Received_PC_Data; // SAVE THE REGISTER VALUE IN THE ARRAY
						for (Register_BIT_Counter = 0; Register_BIT_Counter < 16; Register_BIT_Counter++) // SHIFT AND CHECK EACH BIT, IGNORE READ ONLY BITS 
							{
								Received_Data_Backup = Received_PC_Data & 0x01;
								if (Received_Data_Backup == 1) 
									{
										switch (Register_BIT_Counter) 
											{
												case 0x0: 
														Reset_All_Errors = TRUE;
													break;
												default:
													
													break;
											}
									}
								else 
									{
										switch (Register_BIT_Counter) 
											{
												default:
													
													break;
											}
									}
								Received_PC_Data = Received_PC_Data >> 1;
							}
						break;
					default:
						Send_Error_Word(UART_Transaction_Error);
						PIC_PC_ACK = 1;
						return (FALSE);
						break;
				}
			Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
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
					case 0x28: // Amplifier CW mode switches register     
						CW_Mode_Switches_Register_Read = TRUE;
						break;
					case 0x29: // Amplifier pulse mode switches register 
						Pulse_Mode_Switches_Register_Read = TRUE;
						break;
					case 0x2A: // FPGA control register
						FPGA_Control_Register_Read = TRUE;
						break;
					case 0x2B: // Main control register
						Main_Control_Register_Read = TRUE;
						break;
					case 0x2C: // Tissue temperature measurement value register
						Tissue_Temperature_Measurement_Value_Read = TRUE;
						break;
					default:
						Send_Error_Word(UART_Transaction_Error);
						PIC_PC_ACK = 1;
						return (FALSE);
						break;
				}
			Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
			PC_Transaction_Counter = 1;
			return (TRUE);
		}
	else
		{
			Send_Error_Word(Parser_Working_Error);
			Test_LED_Operation(1, FALSE); // POWER OFF COMMUNICATION OPERATION LED
			PIC_PC_ACK = 1;
			return (FALSE);
		}
}

void Word_To_Byte(int Parameter) // DIVIDE ONE WORD TO TWO BYTES
{
	LSB_Byte = Parameter & 0x00FF;
	MSB_Byte = (Parameter & 0xFF00) >> 8;
}

BOOL Transmitt_To_PC(int Word) 
	{
		char PC_Received_Word = 0;

		//UART_Communication_Flag = TRUE; // FOR DEBUG ONLY
		Word_To_Byte(Word);
		DisableIntUART;
		if (!Write_To_PC(MSB_Byte)) 
			{
				UART_Communication_Flag = FALSE; // FOR DEBUG ONLY
			//	Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
				return (FALSE);
			}

/**************************************** FOR DEBUG ONLY ***********************************************/
		
		UART_INTFLAG = 0;
		EnableIntUART;
		
		/*Timeout_Counter = 0;
		Timer2Enable;*/
		while (!PC_Word_Detected);
		/*	{
				if (UART_Timeout) 
					{
						UART_Timeout = FALSE;
						Timeout_Counter = 0;
						UART_Communication_Flag = FALSE;
						Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
						return (FALSE);
					}
			}
		Timer2Disable;
		TMR2_INTFLAG = 0; // DISABLE TIMEOUT TIMER
		RESET_TIMER2;
		Timeout_Counter = 0;*/
			
		PC_Word_Detected = FALSE;
		PC_Received_Word = Read_From_PC();
		if (!Data_Receive_Error) 
			{
				if (PC_Received_Word != 0x55) 
					{
						UART_Communication_Flag = FALSE;
					//	Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
						return (FALSE);
					}
			}
		else 
			{
				Send_Error_Word(UART_Receiver_Error);
				UART_Communication_Flag = FALSE;
			//	Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
				return (FALSE);
			}

/*******************************************************************************************************/
		
		PIC_PC_ACK = 1;
		Delay_0_625US(9); // delay 5.5 usec
		PIC_PC_ACK = 0;
		
		DisableIntUART;
		
		if (!Write_To_PC(LSB_Byte)) 
			{
				UART_Communication_Flag = FALSE; // FOR DEBUG ONLY
			//	Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
				return (FALSE);
			}
		PIC_PC_ACK = 1;
		
/**************************************** FOR DEBUG ONLY ***********************************************/

		UART_INTFLAG = 0;
		EnableIntUART;
		
		/*Timeout_Counter = 0;
		Timer2Enable;*/
		while (!PC_Word_Detected);
		/*	{
				if (UART_Timeout) 
					{
						UART_Timeout = FALSE;
						Timeout_Counter = 0;
						UART_Communication_Flag = FALSE;
						Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
						return (FALSE);
					}
			}
		Timer2Disable;
		TMR2_INTFLAG = 0; // DISABLE TIMEOUT TIMER
		RESET_TIMER2;
		Timeout_Counter = 0;*/
		
		PC_Word_Detected = FALSE;
		PC_Received_Word = Read_From_PC();
		if (!Data_Receive_Error) 
			{
				if (PC_Received_Word != 0x55) 
					{
						UART_Communication_Flag = FALSE;
						//Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
						return (FALSE);
					}
			}
		else 
			{
				Send_Error_Word(UART_Receiver_Error);
				UART_Communication_Flag = FALSE;
				//Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
				return (FALSE);
			}


/*******************************************************************************************************/		
		
		EnableIntUART;
		UART_Communication_Flag = FALSE;
		//Timer2Enable; // ENABLE 16 SECONDS TIMER FOR VOLTAGE DOMAINS MEASUREMENT
		return (TRUE);
	}

/*int Clear_Headers_From_The_Word(int Value)
{
	int Cleared_Header_Word = 0;
	Cleared_Header_Word = Value & 0x003F; 
	Value = (Value >> 2) & 0x0FC0;
	Cleared_Header_Word = Cleared_Header_Word | Value;
	return (Cleared_Header_Word);
}*/

BOOL Validate_Voltage_Domains(int Domain, float Value) // VOLTAGE DOMAINS TABLE
{
	float Backup = 0;
	
	switch (Domain) // SELECT RELEVANT DOMAIN
		{
			case N5V: 
				Backup  = (Value * Vref) / (4096 * N5V_Resistor_Divider);
				if (((float)((float)Backup / (float)5) < 0.9) || ((float)((float)5) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P3_3V:
				Backup  = (Value * Vref) / (4096 * P3V3_Resistor_Divider);
				if (((float)((float)Backup / (float)3.3) < 0.9) || ((float)((float)3.3) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P5V: 
				Backup  = (Value * Vref) / (4096 * P5V_Resistor_Divider);
				if (((float)((float)Backup / (float)5) < 0.9) || ((float)((float)5) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P12V: 
				Backup  = (Value * Vref) / (4096 * P12V_Resistor_Divider);
				if (((float)((float)Backup / (float)12) < 0.9) || ((float)((float)12) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case N12V: 
				Backup  = (Value * Vref) / (4096 * N12V_Resistor_Divider);
				if (((float)((float)Backup / (float)12) < 0.9) || ((float)((float)12) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P24V: 
				Backup  = (Value * Vref) / (4096 * P24V_Resistor_Divider);
				if (((float)((float)Backup / (float)24) < 0.9) || ((float)((float)24) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P150V: 
				Backup  = (Value * Vref) / (4096 * P150V_Resistor_Divider);
				if (((float)((float)Backup / (float)150) < 0.9) || ((float)((float)150) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			case P48V: 
				Backup  = (Value * Vref) / (4096 * P48V_Resistor_Divider);
				if (((float)((float)Backup / (float)48) < 0.9) || ((float)((float)48) / (float)Backup < 0.9)) return (FALSE);
				else return (TRUE);
				break;
			default:
				return (FALSE);
				break;
		}
} 

BOOL Voltage_Domain_Validate(int Domain) // MONITOR SELECTED SYSTEM VOLTAGE DOMAIN AND VALIDATE IT
{
	A2D_Manual_Convert(Domain);
	Wait_For_Completion();
	if (!Validate_Voltage_Domains(Domain, A2D_Value_Read(Domain)))
		{
			return (FALSE);
		}
	return(TRUE);
}

void Voltage_Domain_Read(int Domain, float Resistor_Divider_Value)
{
	float A2D_Calculated_Value = 0;
	int Backup = 0;
	System_Domains_Measure_Enable_Flag = TRUE;
	A2D_Manual_Convert(Domain);
	Wait_For_Completion();
	Backup = A2D_Value_Read(Domain);
	A2D_Calculated_Value = (Backup * Vref) / (4096 * Resistor_Divider_Value);
	System_Domains_Measure_Enable_Flag = FALSE;
	if (!Transmitt_To_PC(A2D_Calculated_Value * 1000)) Send_Error_Word(UART_Transmitter_Error);
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
	int TEC_Voltage_Value = 0, i = 0;
	int Backup = 0, Backup1 = 0;
	long Temporary = 0;
	BOOL DDS_Power_Latch = FALSE;

	InitIO(); // IO INIT
	Init(); // MAIN SYSTEM INITIALIZATIONS

	//EnableIntSPI;
	Powerup_Domains_On();
	A2D_Module_Enable;
//	Timer5Enable;

/*	Tempr = Read_From_Extender(Expander_A_SPI, 0x12); // READ PORT STATUS FROM EXPANDER A
	Tempr = Read_From_FPGA(0x9);
	Tempr = Read_From_Extender(Expander_B_SPI, 0x12); // READ PORT STATUS FROM EXPANDER A
	Tempr = Read_From_FPGA(0x9);
	Tempr = Read_From_Extender(Expander_C_SPI, 0x12); // READ PORT STATUS FROM EXPANDER A
	Tempr = Read_From_FPGA(0x9);
	Tempr = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER A
	Tempr = Read_From_FPGA(0x9);
	Tempr = Read_From_FPGA(0x9);
	TEC_Power_Set(250);
	Tempr = Read_From_FPGA(0x9);
	Write_To_FPGA(0x2, 0x1);       
	
	DDS_Power_On(); */
	
	DDS_Power_On();
	Synthesizer_Device_Init(FALSE);
	Synthesizer_Device_Init(TRUE);
	Digital_Potentiometer_Init(FALSE);
	Digital_Potentiometer_Init(TRUE);
	
	DDS1_Frequency_Value = 2000;
	Backup1 = 560;
	
	DDS1_OUT_ON;
	
	while (DDS1_Frequency_Value <= 2500)
			{
				Frequency_Value_Set((long)((long)DDS1_Frequency_Value * (long)1000), FALSE); // CONVERT THE FREQUENCY TO KILO HERZ 
				Delay_625US(15); // delay 55 usec
				
				while (Backup1 <= 740)
					{
						DDS1_Amplitude_Value = Backup1 ;
						System_Registers_Array[10] = Backup1;
						DDS1_Amplitude_Value = Swing_Compensation_Table(DDS1_Frequency_Value, DDS1_Amplitude_Value);
						DDS1_Amplitude_Value = DDS1_Amplitude_Value / 3.0373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
						Update_Wiper_Position(DDS1_Amplitude_Value, FALSE); // COMPENSATION OF THE QUANTIZATION MISTAKE		
						
						DDS1_Amplitude_Measure = TRUE;
						Backup = Output_Amplitude_Measure(FALSE, FALSE);
						
						Backup1 = Backup1 + 40;
					}
				Backup1 = 100;
				DDS1_Frequency_Value = DDS1_Frequency_Value + 100;
			}
	
	while (1)
	{
		char PC_Received_Word = 0;
		if (PC_Word_Detected) // CHECK FOR UART MODULE ACTIVITY
			{
				PC_Word_Detected = FALSE;
				PC_Received_Word = Read_From_PC();

				if (!Data_Receive_Error) PC_Word_Parser(PC_Received_Word);
				else 
					{
						Send_Error_Word(UART_Receiver_Error);
					}
				EnableIntUART;
			}

		if (System_Domains_Measure_Enable_Flag) // SWITCH BETWEEN LESS AND GREATER THEN THRESHOLD VALUE MODULE, VALID ONLY AT IDLE MODE
			{
				if (!Voltage_Domain_Validate(P3_3V)) Send_Error_Word(P3V3_Domain_Error);
				if (!Voltage_Domain_Validate(P5V)) Send_Error_Word(P5V_Domain_Error);
				if (!Voltage_Domain_Validate(N5V)) Send_Error_Word(N5V_Domain_Error);
				if (!Voltage_Domain_Validate(P12V)) Send_Error_Word(P12V_Domain_Error);
				if (!Voltage_Domain_Validate(N12V)) Send_Error_Word(N12V_Domain_Error);
				if (!Voltage_Domain_Validate(P24V)) Send_Error_Word(P24V_Domain_Error);
				if (!Voltage_Domain_Validate(P150V)) Send_Error_Word(P150V_Domain_Error);
				if (!Voltage_Domain_Validate(P48V)) Send_Error_Word(P48V_Domain_Error);
				
				System_Domains_Measure_Enable_Flag = FALSE;
//				Timer5Enable;
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
						if (DDS_Power_Latch)
							{
								DDS_Power_Latch = FALSE;
								DDS_Power_On();
								Synthesizer_Device_Init(FALSE);
								Synthesizer_Device_Init(TRUE);
								Digital_Potentiometer_Init(FALSE);
								Digital_Potentiometer_Init(TRUE);
							}
					}
				else
					{
						DDS_Power_Latch = TRUE;
						DDS_Power_Off();
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
						
						//Backup = System_Registers_Array[34];
				//		TEC_Voltage_Value = Clear_Headers_From_The_Word(System_Registers_Array[34]);
						//System_Registers_Array[34] = Backup;
						TEC_Voltage_Value = System_Registers_Array[34];
						if (!TEC_Power_Set(TEC_Voltage_Value))
							{
								Send_Error_Word(TEC_Voltage_Error);
							}
					}
										
				if (DDS1_Frequency_Set) 
					{
						DDS1_Frequency_Set = FALSE;
					//	Backup = System_Registers_Array[8];
						DDS1_Frequency_Value = System_Registers_Array[8];
					//	System_Registers_Array[8] = Backup;
						Frequency_Value_Set((long)((long)System_Registers_Array[8] * (long)1000), FALSE); // CONVERT THE FREQUENCY TO KILO HERZ 
					}
					
				if (DDS1_Amplitude_Set) 
					{
						DDS1_Amplitude_Set = FALSE;
					//	Backup = System_Registers_Array[10];
					//	DDS1_Amplitude_Value = Clear_Headers_From_The_Word(System_Registers_Array[10]);
					//	System_Registers_Array[10] = Backup;
						DDS1_Amplitude_Value = Swing_Compensation_Table(DDS1_Frequency_Value, System_Registers_Array[10]);
						DDS1_Amplitude_Value = DDS1_Amplitude_Value / 3.0373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
				
						Update_Wiper_Position(DDS1_Amplitude_Value, FALSE); // COMPENSATION OF THE QUANTIZATION MISTAKE
					}
					
				/*if (DDS1_Amplitude_Set) 
					{
						DDS1_Amplitude_Set = FALSE;
						DDS1_Amplitude_Value = Swing_Compensation_Table(DDS1_Frequency_Value, System_Registers_Array[10]);
						DDS1_Amplitude_Value = DDS1_Amplitude_Value * 10;
						DDS1_Amplitude_Value = DDS1_Amplitude_Value / 30.373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
				
						Update_Wiper_Position((char)DDS1_Amplitude_Value, FALSE); // COMPENSATION OF THE QUANTIZATION MISTAKE
					}*/
					
				if (DDS2_Frequency_Set) 
					{
						DDS2_Frequency_Set = FALSE;
					//	Backup = System_Registers_Array[13];
						DDS2_Frequency_Value = System_Registers_Array[13];
					//	System_Registers_Array[13] = Backup;
						Frequency_Value_Set((long)((long)System_Registers_Array[13] * (long)1000), TRUE); // CONVERT THE FREQUENCY TO KILO HERZ 
					}
					
				if (DDS2_Amplitude_Set) 
					{
						DDS2_Amplitude_Set = FALSE;
					//	Backup = System_Registers_Array[15];
					//	DDS2_Amplitude_Value = Clear_Headers_From_The_Word(System_Registers_Array[15]);
					//	System_Registers_Array[15] = Backup;
						DDS2_Amplitude_Value = Swing_Compensation_Table(DDS2_Frequency_Value, System_Registers_Array[15]);
						DDS2_Amplitude_Value = DDS2_Amplitude_Value / 3.0373; // NUMBER TO WORD CONVERTION (CONVERT TO VOLTS AND DIVIDE TO THE STEP) VALUE / 1000 / 0.0209803 = WORD
				
						Update_Wiper_Position(DDS2_Amplitude_Value, TRUE); // COMPENSATION OF THE QUANTIZATION MISTAKE
					}
					
				if (Amplifier_CW_Mode_Switches_Set)
					{
						Amplifier_CW_Mode_Switches_Set = FALSE;
					//	Amplifier_CW_Mode_Switches_Value = Clear_Headers_From_The_Word(System_Registers_Array[40]);
						Amplifier_CW_Mode_Switches_Value = System_Registers_Array[40];
						Amplifier_CW_Mode_Convertion_Table(Amplifier_CW_Mode_Switches_Value);
					}
					
				if (Amplifier_Pulse_Mode_Switches_Set)
					{
						Amplifier_Pulse_Mode_Switches_Set = FALSE;
					//	Amplifier_Pulse_Mode_Switches_Value = Clear_Headers_From_The_Word(System_Registers_Array[41]);
						Amplifier_Pulse_Mode_Switches_Value = System_Registers_Array[41];
						if (!Amplifier_Pulse_Mode_Convertion_Table(Amplifier_Pulse_Mode_Switches_Value)) Send_Error_Word(Amplifier_Pulse_Mode_Word_Error);
					}
					
				if (FPGA_Control_Register)
					{
						FPGA_Control_Register = FALSE;
						if (!Tissue_Temperature_Measure()) Send_Error_Word(Tissue_temperature_measure_Error); // RUN TISSUE TEMPERATURE MEASUREMENT FUNCTION
						System_Registers_Array[44] = 0;
						System_Registers_Array[44] = (System_Registers_Array[44] | Read_From_FPGA(0x7)) << 8; // READ MSB VALUE
						System_Registers_Array[44] = (System_Registers_Array[44] | Read_From_FPGA(0x5)); // READ LSB VALUE
					}
				
				if (Reset_All_Errors)
					{
						Reset_All_Errors = FALSE;
						Error1_Register = 0; // ZEROIZE ERROR1 REGISTER
						Error2_Register = 0; // ZEROIZE ERROR2 REGISTER	
						System_Registers_Array[42] = 0xFFC1; // ZEROIZE RELEVANT BITS AT FPGA CONTROL REGISTER
						Write_To_FPGA(0x2, 0x1); // RESET FPGA ERRORS
						System_Registers_Array[43] = 0xFFFE; // CLEAR RESET ALL ERRORS BIT
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
				Voltage_Domain_Read(P3_3V, P3V3_Resistor_Divider);
			}
			
		if (P5V_Voltage_Read) 
			{
				P5V_Voltage_Read = FALSE;
				Voltage_Domain_Read(P5V, P5V_Resistor_Divider);
			}

		if (P12V_Voltage_Read) 
			{
				P12V_Voltage_Read = FALSE;
				Voltage_Domain_Read(P12V, P12V_Resistor_Divider);
			}
			
		if (P24V_Voltage_Read) 
			{
				P24V_Voltage_Read = FALSE;
				Voltage_Domain_Read(P24V, P24V_Resistor_Divider);
			}
			
		if (P48V_Voltage_Read) 
			{
				P48V_Voltage_Read = FALSE;
				Voltage_Domain_Read(P48V, P48V_Resistor_Divider);
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
				Temporary = Output_Frequency_Measure(FALSE);
				DDS1_Frequency_Measure = FALSE;
				if (Validate_Frequency_Setting(Temporary / 1000, FALSE))
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
				Backup = Output_Amplitude_Measure(FALSE, FALSE);
				DDS1_Amplitude_Measure = FALSE;
				if (Validate_Amplitude_Setting(Backup, FALSE))
					{
						if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
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
				Temporary = Output_Frequency_Measure(TRUE);
				DDS2_Frequency_Measure = FALSE;
				if (Validate_Frequency_Setting(Temporary / 1000, TRUE))
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
				Backup = Output_Amplitude_Measure(TRUE, FALSE);
				DDS2_Amplitude_Measure = FALSE;
				if (Validate_Amplitude_Setting(Backup, TRUE))
					{
						if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
					}
				else Send_Error_Word(DDS2_Amplitude_Error);
			}
			
		if (Amplifier1_Control_Register_Read) 
			{
				Amplifier1_Control_Register_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[17])) Send_Error_Word(UART_Transmitter_Error);
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
				if (!Transmitt_To_PC(System_Registers_Array[22])) Send_Error_Word(UART_Transmitter_Error);
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
				if (!Transmitt_To_PC(System_Registers_Array[27])) Send_Error_Word(UART_Transmitter_Error);
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
				if (!Transmitt_To_PC(System_Registers_Array[35])) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (TEC_Voltage_Measure) 
			{
				TEC_Voltage_Measure = FALSE;
				Voltage_Domain_Read(HP_TEC_Power_SNS, HPTEC_Att_Coefficient);
			}	
		
		if (Errors1_Register_Read) 
			{
				Errors1_Register_Read = FALSE;
				if (!Transmitt_To_PC(Error1_Register)) Send_Error_Word(UART_Transmitter_Error);
			}
		
		if (Errors2_Register_Read) 
			{
				Errors2_Register_Read = FALSE;
				if (!Transmitt_To_PC(Error2_Register)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (MCU_FPGA_Version_Read) 
			{
				MCU_FPGA_Version_Read = FALSE;
				Backup = FPGA_Version_Number;
				Backup = (Backup << 8) | MCU_Version_Number;
				if (!Transmitt_To_PC(Backup)) Send_Error_Word(UART_Transmitter_Error);
			}
			
		if (Controller_Handpiece_Serial_Read) 
			{
				Controller_Handpiece_Serial_Read = FALSE;
			}
			
		if (CW_Mode_Switches_Register_Read)
			{
				CW_Mode_Switches_Register_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[40])) Send_Error_Word(UART_Transmitter_Error);	
			}
			
		if (Pulse_Mode_Switches_Register_Read)
			{
				Pulse_Mode_Switches_Register_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[41])) Send_Error_Word(UART_Transmitter_Error);	
			}
			
		if (Main_Control_Register_Read) 
			{
				Main_Control_Register_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[43])) Send_Error_Word(UART_Transmitter_Error);
			}		
		
		if (FPGA_Control_Register_Read)
			{
				FPGA_Control_Register_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[42])) Send_Error_Word(UART_Transmitter_Error);	
			}
		if (FPGA_Interrupt_Flag)
			{
				FPGA_Interrupt_Flag = FALSE;
				FPGA_Interrupt_Parsing();
				Main_Interrupt_Detection_Enable;	
			}
			
		if (Tissue_Temperature_Measurement_Value_Read)
			{
				Tissue_Temperature_Measurement_Value_Read = FALSE;
				if (!Transmitt_To_PC(System_Registers_Array[44])) Send_Error_Word(UART_Transmitter_Error);	
			}
	}
	
} // main
