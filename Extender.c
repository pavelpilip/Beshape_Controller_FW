#include <p18cxxx.h>

#include <delays.h>

#include "util.h"

#include "SPI.h"

#include "Extender.h"

void Write_To_Extender(char Extender_Number, char Address, char Data) // EXTENDER NUMBER, REGISTER ADDRESS, REGISTER DATA
{
	switch (Extender_Number) // OPEN THE RELEVANT ADC INPUT
		{
			case Expander_A_SPI: 
				Write_To_SPI(Extender_Number, 0x40); // WRITE OPCODE TO THE BUS
				break;
			case Expander_B_SPI: 
				Write_To_SPI(Extender_Number, 0x42); // WRITE OPCODE TO THE BUS
				break;
			case Expander_C_SPI: 
				Write_To_SPI(Extender_Number, 0x44); // WRITE OPCODE TO THE BUS
				break;
			case Expander_D_SPI: 
				Write_To_SPI(Extender_Number, 0x46); // WRITE OPCODE TO THE BUS
				break;
			default:
				Write_To_SPI(Extender_Number, 0x40); // WRITE OPCODE TO THE BUS
				break;
		}
	Write_To_SPI(Extender_Number, Address); // WRITE ADDRESS TO THE BUS
	Write_To_SPI(Extender_Number, Data); // WRITE DATA TO THE BUS
	DSP_SPIB_CS = 1; // DISABLE SPI BUS
}

char Read_From_Extender(char Extender_Number, char Address) // EXTENDER NUMBER, REGISTER ADDRESS
{
	char Extender_Read_Data = 0;
	switch (Extender_Number) // OPEN THE RELEVANT ADC INPUT
		{
			case Expander_A_SPI: 
				Write_To_SPI(Extender_Number, 0x41); // WRITE OPCODE TO THE BUS
				break;
			case Expander_B_SPI: 
				Write_To_SPI(Extender_Number, 0x43); // WRITE OPCODE TO THE BUS
				break;
			case Expander_C_SPI: 
				Write_To_SPI(Extender_Number, 0x45); // WRITE OPCODE TO THE BUS
				break;
			case Expander_D_SPI: 
				Write_To_SPI(Extender_Number, 0x47); // WRITE OPCODE TO THE BUS
				break;
			default:
				Write_To_SPI(Extender_Number, 0x41); // WRITE OPCODE TO THE BUS
				break;
		}
	Write_To_SPI(Extender_Number, Address); // WRITE ADDRESS TO THE BUS
	Extender_Read_Data = Write_To_SPI(Extender_Number, 0x00); // READ DATA FROM THE BUS
	DSP_SPIB_CS = 1; // DISABLE SPI BUS
	return (Extender_Read_Data);
}

void Extender_Interrupt_Clear(char Extender_Number) // EXTENDER NUMBER
{
	Read_From_Extender(Extender_Number, 0x10); // CLEAR PORTA INTERRUPTS
	Read_From_Extender(Extender_Number, 0x11); // CLEAR PORTB INTERRUPTS
}

void Clear_All_Extender_Interrupts_Signals()
{
	Extender_Interrupt_Clear(Expander_A_SPI);
	Extender_Interrupt_Clear(Expander_B_SPI);
	Extender_Interrupt_Clear(Expander_C_SPI);
	Extender_Interrupt_Clear(Expander_D_SPI);
}

char Interrupt_Triggered_Device(char Extender_Number, char Extender_Port_Number) // CHECKS, WHAT DEVICE TRIGGERED INTERRUPT
{
	char Extender_Read_Data = 0;
	if (Extender_Port_Number == 0) Extender_Read_Data = Read_From_Extender(Extender_Number, 0x0E); // READ FROM PORT A
	else Extender_Read_Data = Read_From_Extender(Extender_Number, 0x0F);// READ FROM PORT B
	switch (Extender_Number) 
		{
			case Expander_A_SPI: 
				
				break;
			case Expander_B_SPI: 
				
				break;
			default:
				
				break;
		}
}

void Extender_Init(char Extender_Number) 
{

	char Tmp = 0;

	Write_To_Extender(Extender_Number, 0x0A, 0x2A); // SEQUENTAL PORT NUMBERING; INTERRUPTS ARE SEPARATED, NOT OPEN DRAIN AND ACTIVE HIGH; ADDRESS PINS ENABLED
	Write_To_Extender(Extender_Number, 0x0B, 0x2A); // SEQUENTAL PORT NUMBERING; INTERRUPTS ARE SEPARATED, NOT OPEN DRAIN AND ACTIVE HIGH; ADDRESS PINS ENABLED	
	
	switch (Extender_Number) // OPEN THE RELEVANT ADC INPUT
		{
			case Expander_A_SPI: 
				Write_To_Extender(Extender_Number, 0x00, 0xCF); // ALL PORT ARE INPUTS, OUTSIDE PORT GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x01, 0xF9); // ALL PORT ARE INPUTS, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x02, 0x00); // DON'T INVERT INPUTS
				Write_To_Extender(Extender_Number, 0x03, 0x00); // DON'T INVERT INPUTS
				Write_To_Extender(Extender_Number, 0x04, 0x0F); // ENABLE FOR INTERRUPT AT CHANGE, OUTSIDE PORT GPA7, GPA6, GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x05, 0xF9); // ENABLE FOR INTERRUPT AT CHANGE, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x08, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x09, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x0C, 0xCF); // PULL UP, ARE ENABLED FOR ALL PORTS, OUTSIDE PORT GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x0D, 0xF9); // PULL UP, ARE ENABLED FOR ALL PORTS, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x12, 0xCF); // CONFIGURE FPGA RESET AND PULSER CONTROL SIGNAL TO "0"
				Write_To_Extender(Extender_Number, 0x13, 0xF9); // CONFIGURE AMPLIFIER SWITCHES CONTROL SIGNALS TO "0"
				Write_To_Extender(Extender_Number, 0x14, 0xCF); // CONFIGURE FPGA RESET AND PULSER CONTROL SIGNAL TO "0"
				Write_To_Extender(Extender_Number, 0x15, 0xF9); // CONFIGURE AMPLIFIER SWITCHES CONTROL SIGNALS TO "0"
				Extender_Interrupt_Clear(Extender_Number);
				break;
			case Expander_B_SPI: 
				Write_To_Extender(Extender_Number, 0x00, 0xFF); // ALL PORT ARE INPUTS 
				Write_To_Extender(Extender_Number, 0x01, 0xFF); // ALL PORT ARE INPUTS
				Write_To_Extender(Extender_Number, 0x02,0xFF); //  INVERT INPUTS
				Write_To_Extender(Extender_Number, 0x03, 0x02); // DON'T INVERT INPUTS, OUTSIDE GPB1
				Extender_Interrupt_Clear(Extender_Number);
				Write_To_Extender(Extender_Number, 0x04, 0xFF); // ENABLE FOR INTERRUPT AT CHANGE 
				Write_To_Extender(Extender_Number, 0x05, 0x00); // ENABLE FOR INTERRUPT AT CHANGE, OUTSIDE PORT GPB7, GPB6, GPB5, GPB4
				Write_To_Extender(Extender_Number, 0x08, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x09, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x0C, 0xFF); // PULL UP, ARE ENABLED FOR ALL PORTS
				Write_To_Extender(Extender_Number, 0x0D, 0xFF); // PULL UP, ARE ENABLED FOR ALL PORTS
				//Extender_Interrupt_Clear(Extender_Number);
				break;
			case Expander_C_SPI: 
				Write_To_Extender(Extender_Number, 0x00, 0x00); // ALL PORT ARE OUTPUTS
				Write_To_Extender(Extender_Number, 0x01, 0x00); // ALL PORT ARE OUTPUTS
				Write_To_Extender(Extender_Number, 0x04, 0x00); // DISABLE ALL INTERRUPTS 
				Write_To_Extender(Extender_Number, 0x05, 0x00); // DISABLE ALL INTERRUPTS 
				Write_To_Extender(Extender_Number, 0x12, 0xBC); // CONFIGURE ALL LEDS TO POWER OFF
				Write_To_Extender(Extender_Number, 0x13, 0x25); // CONFIGURE ALL OUTPUT PORTS FOR POWER DOMAINS OFF 
				Write_To_Extender(Extender_Number, 0x14, 0xBC); // CONFIGURE ALL LEDS TO POWER OFF
				Write_To_Extender(Extender_Number, 0x15, 0x25); // CONFIGURE ALL OUTPUT LATCHES FOR FOR POWER DOMAINS OFF
				Extender_Interrupt_Clear(Extender_Number);
				break;
			case Expander_D_SPI: 
				Write_To_Extender(Extender_Number, 0x00, 0x00); // ALL PORT ARE OUTPUTS 
				Write_To_Extender(Extender_Number, 0x01, 0x00); // ALL PORT ARE OUTPUTS
				Write_To_Extender(Extender_Number, 0x04, 0x00); // DISABLE ALL INTERRUPTS 
				Write_To_Extender(Extender_Number, 0x05, 0x00); // DISABLE ALL INTERRUPTS 
				Write_To_Extender(Extender_Number, 0x12, 0xFE); // CONFIGURE ALL OUTPUT PORTS FOR POWER DOMAINS OFF 
				Write_To_Extender(Extender_Number, 0x13, 0xAB); // CONFIGURE ALL OUTPUT PORTS FOR POWER DOMAINS OFF 
				Write_To_Extender(Extender_Number, 0x14, 0xFE); // CONFIGURE ALL OUTPUT LATCHES FOR FOR POWER DOMAINS OFF 
				Write_To_Extender(Extender_Number, 0x15, 0xAB); // CONFIGURE ALL OUTPUT LATCHES FOR FOR POWER DOMAINS OFF
				Extender_Interrupt_Clear(Extender_Number);
				break;
			default:
				Write_To_Extender(Extender_Number, 0x00, 0xCF); // ALL PORT ARE INPUTS, OUTSIDE PORT GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x01, 0xF9); // ALL PORT ARE INPUTS, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x02, 0x00); // DON'T INVERT INPUTS
				Write_To_Extender(Extender_Number, 0x03, 0x00); // DON'T INVERT INPUTS
				Write_To_Extender(Extender_Number, 0x04, 0x0F); // ENABLE FOR INTERRUPT AT CHANGE, OUTSIDE PORT GPA7, GPA6, GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x05, 0xF9); // ENABLE FOR INTERRUPT AT CHANGE, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x08, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x09, 0x00); // INTERRUPT OCCURS, WHEN CURRENT VALUE IS DIFFERENT FROM THE PREVIOUS VALUE
				Write_To_Extender(Extender_Number, 0x0C, 0xCF); // PULL UP, ARE ENABLED FOR ALL PORTS, OUTSIDE PORT GPA5, GPA4
				Write_To_Extender(Extender_Number, 0x0D, 0xF9); // PULL UP, ARE ENABLED FOR ALL PORTS, OUTSIDE PORT GPB1, GPB2
				Write_To_Extender(Extender_Number, 0x12, 0xCF); // CONFIGURE FPGA RESET AND PULSER CONTROL SIGNAL TO "0"
				Write_To_Extender(Extender_Number, 0x13, 0xF9); // CONFIGURE AMPLIFIER SWITCHES CONTROL SIGNALS TO "0"
				Write_To_Extender(Extender_Number, 0x14, 0xCF); // CONFIGURE FPGA RESET AND PULSER CONTROL SIGNAL TO "0"
				Write_To_Extender(Extender_Number, 0x15, 0xF9); // CONFIGURE AMPLIFIER SWITCHES CONTROL SIGNALS TO "0"
				Extender_Interrupt_Clear(Extender_Number);
				break;
		}
}


