#include <p18cxxx.h>

/// Chip-specific and general utility macros.
#include "util.h"

#include <delays.h> 

#include "Synthesizer.h"

void Synthesizer_Bit_Strobe(BOOL DDS_Number) // INSERT WORD/BIT TO REGISTER/S
 {
	if (!DDS_Number)
		{
			DDS1_SYN_W_CLK = 0;
			DDS1_SYN_W_CLK = 1;
			DDS1_SYN_W_CLK = 0;
		}
	else
		{
			DDS2_SYN_W_CLK = 0;
			DDS2_SYN_W_CLK = 1;
			DDS2_SYN_W_CLK = 0;
		}
 }
 
/* void Synthesizer_Bit_Strobe(BOOL DDS_Number) // INSERT WORD/BIT TO REGISTER/S
 {
			DDS1_SYN_W_CLK = 0;
			DDS1_SYN_W_CLK = 1;
			DDS1_SYN_W_CLK = 0;
		
			DDS2_SYN_W_CLK = 0;
			DDS2_SYN_W_CLK = 1;
			DDS2_SYN_W_CLK = 0;
 }*/
 
 void Synthesizer_Apply_Changes(BOOL DDS_Number) // APPLY CHANGES OF REGISTERS TO SYNTHESIZER DEVICE
 {
	if (!DDS_Number)
		{
			DDS1_SYN_FQ_UD = 0;
			DDS1_SYN_FQ_UD = 1;
			DDS1_SYN_FQ_UD = 0;
		}
	else
		{
			DDS2_SYN_FQ_UD = 0;
			DDS2_SYN_FQ_UD = 1;
			DDS2_SYN_FQ_UD = 0;
		}
 }
 
 /*void Synthesizer_Apply_Changes(BOOL DDS_Number) // APPLY CHANGES OF REGISTERS TO SYNTHESIZER DEVICE
 {
	
			DDS1_SYN_FQ_UD = 0;
			DDS1_SYN_FQ_UD = 1;
			DDS1_SYN_FQ_UD = 0;
		
			DDS2_SYN_FQ_UD = 0;
			DDS2_SYN_FQ_UD = 1;
			DDS2_SYN_FQ_UD = 0;
 }*/
	
void Synthesizer_Control_Write(long frequency, char phase, char Power_Down, BOOL DDS_Number)
 {
	
	int i = 0;
	
	for (i = 0; i < 32; i++) // INSERT FREQUENCY WORD
		{
			if (!DDS_Number) DDS1_SYN_D7 = frequency & 0x01;
			else DDS2_SYN_D7 = frequency & 0x01;
			frequency = frequency >> 1;
			Synthesizer_Bit_Strobe(DDS_Number);
		}
		
		if (!DDS_Number) DDS1_SYN_D7 = 0;
		else DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe(DDS_Number);
		if (!DDS_Number) DDS1_SYN_D7 = 0;
		else DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe(DDS_Number);
		
		if (!DDS_Number) DDS1_SYN_D7 = Power_Down; // BIT 34 IS POWER DOWN, IF "1" SO DEVICE POWER DOWN
		else DDS2_SYN_D7 = Power_Down; 
		Synthesizer_Bit_Strobe(DDS_Number);
		
		for (i = 0; i < 5; i++) // INSERT PHASE WORD
		{
			if (!DDS_Number) DDS1_SYN_D7 = phase & 0x01; 
			else DDS2_SYN_D7 = phase & 0x01; 
			phase = phase >> 1;
			Synthesizer_Bit_Strobe(DDS_Number);
		}
		
	Synthesizer_Apply_Changes(DDS_Number);
		
 }
 
/* void Synthesizer_Control_Write(long frequency, char phase, char Power_Down, BOOL DDS_Number)
 {
	
	int i = 0;
	
	for (i = 0; i < 32; i++) // INSERT FREQUENCY WORD
		{
			DDS1_SYN_D7 = frequency & 0x01;
			DDS2_SYN_D7 = frequency & 0x01;
			frequency = frequency >> 1;
			Synthesizer_Bit_Strobe(DDS_Number);
		}
		
		DDS1_SYN_D7 = 0;
		DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe(DDS_Number);
		DDS1_SYN_D7 = 0;
		DDS2_SYN_D7 = 0;
		Synthesizer_Bit_Strobe(DDS_Number);
		
		DDS1_SYN_D7 = Power_Down; // BIT 34 IS POWER DOWN, IF "1" SO DEVICE POWER DOWN
		DDS2_SYN_D7 = Power_Down; 
		Synthesizer_Bit_Strobe(DDS_Number);
		
		for (i = 0; i < 5; i++) // INSERT PHASE WORD
		{
			DDS1_SYN_D7 = phase & 0x01; 
			DDS2_SYN_D7 = phase & 0x01; 
			phase = phase >> 1;
			Synthesizer_Bit_Strobe(DDS_Number);
		}
		
	Synthesizer_Apply_Changes(DDS_Number);
}*/
	
void Synthesizer_Device_Init(BOOL DDS_Number) // SYNTHESIZER INITIALIZATION SEQUENCE
 {

	int i = 0;

	if (!DDS_Number) DDS1_SYN_RESET = 1; // RESET SYNTHESIZER DEVICE
	else DDS2_SYN_RESET = 1; // RESET SYNTHESIZER DEVICE
	Delay_0_625US(3); // delay 2 usec
	if (!DDS_Number) DDS1_SYN_RESET = 0; 
	else DDS2_SYN_RESET = 0; 
	Delay_0_625US(3);

	Synthesizer_Bit_Strobe(DDS_Number); // ENTER TO SERIAL CONTROL MODE
	Synthesizer_Apply_Changes(DDS_Number);

	Delay_0_625US(3);

	for (i = 0; i < 40; i++) // INSERT FREQUENCY WORD
		{
			if (!DDS_Number) DDS1_SYN_D7 = 0;
			else DDS2_SYN_D7 = 0;
			Synthesizer_Bit_Strobe(DDS_Number);
		}
	Synthesizer_Apply_Changes(DDS_Number);

 }