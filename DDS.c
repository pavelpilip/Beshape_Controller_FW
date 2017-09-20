#include <p18cxxx.h>

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

#include "Synthesizer.h"

#include "DDS.h"

#include "a2d.h"

BOOL Frequency_Measurement_Completed = FALSE;
extern System_Domains_Measure_Enable_Flag, System_Work_Mode, Timeout_Counter, A2D_Sine_Amplitude_Timeout_Counter, Timeout, DDS1_Frequency_Value, DDS2_Frequency_Value;
extern System_Registers_Array[System_Registers_Number];

void Frequency_Value_Set(long Value, BOOL DDS_Number) // SET DDS GENERATOR
{
	Value = Value / 0.02328; // NUMBER TO WORD CONVERTION
	Synthesizer_Control_Write(Value, 0x0, 0x0, DDS_Number); // CONFIGURE SYNTHESIZER WITH NEW VALUES
	
}

int Swing_Compensation_Table(int DDS_Frequency_Value, int DDS_Amplitude_Value)
{
	int Amplitude_Swing_Value = 0;
	if ((DDS_Frequency_Value >= 2000) && (DDS_Frequency_Value <= 2250) && (DDS_Amplitude_Value > 100)) Amplitude_Swing_Value = DDS_Amplitude_Value * 1.1;
	else if ((DDS_Frequency_Value > 2250) && (DDS_Frequency_Value <= 2500) && (DDS_Amplitude_Value > 100)) Amplitude_Swing_Value = DDS_Amplitude_Value * 1.11;
	else Amplitude_Swing_Value = DDS_Amplitude_Value;
	return(Amplitude_Swing_Value);	
}

/*float Swing_Compensation_Table(int DDS_Frequency_Value, int DDS_Amplitude_Value)
{
	float Amplitude_Swing_Value = 0;
	if ((DDS_Frequency_Value >= 2000) && (DDS_Frequency_Value <= 2250) && (DDS_Amplitude_Value > 1000)) Amplitude_Swing_Value = DDS_Amplitude_Value * 1.1;
	else if ((DDS_Frequency_Value > 2250) && (DDS_Frequency_Value <= 2500) && (DDS_Amplitude_Value > 1000)) Amplitude_Swing_Value = DDS_Amplitude_Value * 1.11;
	else Amplitude_Swing_Value = DDS_Amplitude_Value;
	return(Amplitude_Swing_Value);	
}*/

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
					Tmp1 = TMR1L;
					Tmp2 = TMR1H;
				}
			else
				{
					Tmp1 = TMR0L;
					Tmp2 = TMR0H;
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
	
	Wait_For_Completion();
	
	if (!Before_After_Switch)
		{
			if (!DDS_Number) Value = A2D_Value_Read(DDS1_BEFORE_SW_SNS);
			else Value = A2D_Value_Read(DDS2_BEFORE_SW_SNS);
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
	BOOL DDS1_Out_Flag = FALSE, DDS2_Out_Flag = FALSE;
	
	#ifdef Voltage_Domain_Validation_Enable_Flag 
		Timer5Disable;
	#endif
	Timer3Disable;
	TMR3_INTFLAG = 0; // DISABLE TIMEOUT TIMER
	RESET_TIMER3;
	System_Work_Mode = System_Configuration_Mode;
	A2D_Sine_Amplitude_Timeout_Counter = 0;
	
	if (!DDS_Number) // SOURCE THAT NOT MEASURED SHOULD BE DC
		{
			Frequency_Value_Set(100, FALSE); // SET FREQUENCY TO 100HZ
			Frequency_Value_Set(0, TRUE); // SET FREQUENCY TO DC
			if (DDS1_OUT_ENABLE) // IF DDS1 DISABLED 
				{
					DDS1_OUT_ON;
					DDS1_Out_Flag = TRUE;
				}
			if (!DDS2_OUT_ENABLE) // IF DDS2 ENABLED
				{
					DDS2_OUT_OFF;
					DDS2_Out_Flag = TRUE;
				}
		}
	else
		{
			Frequency_Value_Set(0, FALSE); // SET FREQUENCY TO DC
			Frequency_Value_Set(100, TRUE); // SET FREQUENCY TO 100HZ
			if (!DDS1_OUT_ENABLE) // IF DDS1 ENABLED 
				{
					DDS1_OUT_OFF;
					DDS1_Out_Flag = TRUE;
				}
			if (DDS2_OUT_ENABLE) // IF DDS2 DISABLED
				{
					DDS2_OUT_ON;
					DDS2_Out_Flag = TRUE;
				}
		}
	

	Amplitude_Tmp_Value = Get_Amplitude_Value(DDS_Number, Before_After_Switch); // TAKE FIRST SAMPLE

	Timer3Enable; // SWITCH ON TIMEOUT TIMER
	
	while (Amplitude_Tmp_Value != 0) // WAIT UNTIL SINUS WILL RIZE
		{
			Amplitude_Tmp_Value = Get_Amplitude_Value(DDS_Number, Before_After_Switch);
			if (Timeout) 
				{
					Timeout = FALSE;
					if (!DDS_Number) Send_Error_Word(DDS1_Amplitude_Measure_Error);
					else Send_Error_Word(DDS2_Amplitude_Measure_Error);
					//break;
					return(0);
				}
		}
	
	TMR3_INTFLAG = 0; // DISABLE TIMEOUT TIMER
	Timer3Disable;
	RESET_TIMER3;
	A2D_Sine_Amplitude_Timeout_Counter = 0;
	Timer3Enable; // SWITCH ON TIMEOUT TIMER

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
					return(0);
				}
		}
		
	TMR3_INTFLAG = 0; // DISABLE TIMEOUT TIMER
	Timer3Disable;
	RESET_TIMER3;
	A2D_Sine_Amplitude_Timeout_Counter = 0;
	
	Frequency_Value_Set((long)((long)DDS1_Frequency_Value * (long)1000), FALSE); // SET ORIGINAL FREQUENCY
	Frequency_Value_Set((long)((long)DDS2_Frequency_Value * (long)1000), TRUE);
	
	if (!DDS_Number)
		{
			if (DDS1_Out_Flag) DDS1_OUT_OFF;
			if (DDS2_Out_Flag) DDS2_OUT_ON;
		}
	else
		{
			if (DDS1_Out_Flag) DDS1_OUT_ON;
			if (DDS2_Out_Flag) DDS2_OUT_OFF;
		}
	
	Amplitude_Real_Value = Amplitude_Value; // CONVERT TO FLOAT 
	Amplitude_Real_Value  = (Amplitude_Real_Value * Vref) / 4096; // CALCULATE REAL SINE (STEP * DIGITAL NUMBER)
	Amplitude_Real_Value = (Amplitude_Real_Value * Divider) / 1.41421; //	CALCULATE RMS SINE

	if (!DDS_Number)
		{
			if ((DDS1_Frequency_Value >= 1000) && (DDS1_Frequency_Value < 2000) && (System_Registers_Array[10] >= 100)) Amplitude_Real_Value = Amplitude_Real_Value / 1.1;
			if ((DDS1_Frequency_Value >= 2000) && (DDS1_Frequency_Value <= 2500) && (System_Registers_Array[10] >= 100)) Amplitude_Real_Value = Amplitude_Real_Value / 1.21;
		}
	else
		{
			if ((DDS2_Frequency_Value >= 1000) && (DDS2_Frequency_Value < 2000) && (System_Registers_Array[15] >= 100)) Amplitude_Real_Value = Amplitude_Real_Value / 1.1;
			if ((DDS2_Frequency_Value >= 2000) && (DDS2_Frequency_Value <= 2500) && (System_Registers_Array[15] >= 100)) Amplitude_Real_Value = Amplitude_Real_Value / 1.21;
		}
		
	Amplitude_Value = Amplitude_Real_Value * 1000;
	
	System_Work_Mode = System_Idle_Mode;
	
	#ifdef Voltage_Domain_Validation_Enable_Flag 
		TMR5_INTFLAG = 0; // DISABLE TIMEOUT TIMER
		RESET_TIMER5;
		Timeout_Counter = 0;
		Timer5Enable;
	#endif
	
	return(Amplitude_Value);
}

BOOL Validate_Amplitude_Setting(float Value, BOOL DDS_Number) // AMPLITUDE VALIDATION TABLE
{
	float Backup = 0, Backup1 = 0;

	Value = Value / 10;
	
	if (!DDS_Number)
		{
			Backup = (float)(Value / (float)System_Registers_Array[10]);
			Backup1 = (float)((float)System_Registers_Array[10] / Value);
		}
	else 
		{
			Backup = (float)(Value / (float)System_Registers_Array[15]);
			Backup1 = (float)((float)System_Registers_Array[15] / Value);
		}

	if (!DDS_Number)
		{
			if (System_Registers_Array[10] > 20) 
				if ((Backup < 0.75) || (Backup1 < 0.75)) 
					return (FALSE);
				else 
					return (TRUE);
			else
				if ((Backup < 0.5) || (Backup1 < 0.5)) 
					return (FALSE);
				else 
					return (TRUE);
		}
	else
		{
			if (System_Registers_Array[15] > 20) 
				if ((Backup < 0.75) || (Backup1 < 0.75)) 
					return (FALSE);
				else 
					return (TRUE);
			else
				if ((Backup < 0.5) || (Backup1 < 0.5)) 
					return (FALSE);
				else 
					return (TRUE);
		}
} 