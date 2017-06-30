

#include <p18cxxx.h>		// Chip-specific definitions for PIC family

#include "util.h"

void Init_Interrupts()
{
	
Main_Interrupt_Detected = 0;
Main_Interrupt_Detection_Rizedge;
//Main_Interrupt_Detection_Enable;
Main_Interrupt_Detection_Disable;

Power_On_Detected = 0;
Power_On_Detection_High_Priority;
Power_On_Detection_Rizedge;
//Power_On_Detection_Enable;
Power_On_Detection_Disable;
	 
GlobalINT = 1; // enable all interrupts
Peripheral_Int_En = 1; // enable usart interrupts
ENABLE_INT_PRIORITY	= 1; // enable high priority interrupts

}	
