
#include <p18cxxx.h>
// Microchip library for delays routines

#include "util.h"
#include "SPI.h"
#include <delays.h>

void InitSPI1Master()
{
	// Set SPI1 pins directions for Master mode

	tris_DSP_SPIB_MOSI = 0; // INITIALIZE SDO AS OUTPUT
	tris_DSP_SPIB_MISO = 1;
	tris_DSP_SPIB_CLK = 0;
	tris_DSP_SPIB_CS = 0;
	DSP_SPIB_CS = 1; // DISABLE SLAVE SELECT
	PIC_SPI_Status = 0x00; 			
	PIC_SPI_Control = 0x12; // SPI module configured as master with 1MHZ clock
//	PIC_SPI_Status = 0x40; 			
//	PIC_SPI_Control = 0x02; // SPI module configured as master with 1MHZ clock
	DisableIntSPI;
}

void InitSPI1Slave()
{
	// Set SPI1 pins directions for Master mode

	tris_DSP_SPIB_MOSI = 0; 
	tris_DSP_SPIB_MISO = 1;
	tris_DSP_SPIB_CLK = 1;
	tris_DSP_SPIB_CS = 1;
	DSP_SPIB_CS = 1; // DISABLE SLAVE SELECT
	PIC_SPI_Status = 0x00; 			
	PIC_SPI_Control = 0x15;
	DisableIntSPI;
	
}	

char Read_From_SPI(void) // IF RETURN 0, NO DATA READY IN THE BUFFER
{
	char SPI_Word_Backup = 0;
	
	if (PIC_SPI_Ready) 				// WAIT FOR READY, WORD IS RECEIVED
	{
		SPI_Word_Backup = PIC_SPI_Buffer; 	// Clear BF
		while (PIC_SPI_Ready);
		return (SPI_Word_Backup);
	}
	else return (0);		
}

char Write_To_SPI(char Device, char Data) // IF RETURN 0, TRANSACTION SEND ERROR
{
	char Received_SPI_Value = 0;
	if (!PIC_SPI_Ready) // WAIT FOR READY
	{	
		switch (Device) // OPEN THE RELEVANT ADC INPUT
		{
			case General_Isolated_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 0; 
				break;
			case Hand_Piece_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 0;
				break;
			case Pulser_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 0;
				break;
			case Phantom_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 0;
				break;
			case General_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 1; 
				DSP_SPIB_SEL3 = 0;
				break;
			case FPGA_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 1; 
				DSP_SPIB_SEL3 = 0;
				PIC_SPI_Disable;
				Delay_6_25US(1); // delay 7 usec
				PIC_SPI_Status = 0x40; 			
				PIC_SPI_Control = 0x02; // SPI module configured as master with 1MHZ clock
				PIC_SPI_Enable;
				Delay_6_25US(1); // delay 7 usec
				break;
			case EEPROM_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 1; 
				DSP_SPIB_SEL3 = 0;
				break;
			case Tech_Power_Digital_Potentiometer_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 1; 
				DSP_SPIB_SEL3 = 0;
				break;
			case Expander_A_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 1;
				break;
			case Expander_B_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 1;
				break;
			case Expander_C_SPI: 
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 1;
				break;
			case Expander_D_SPI: 
				DSP_SPIB_SEL0 = 1; 
				DSP_SPIB_SEL1 = 1; 
				DSP_SPIB_SEL2 = 0; 
				DSP_SPIB_SEL3 = 1;
				break;
			default:
				DSP_SPIB_SEL0 = 0; 
				DSP_SPIB_SEL1 = 0; 
				DSP_SPIB_SEL2 = 1; 
				DSP_SPIB_SEL3 = 1;
				break;
		}
		DSP_SPIB_CS = 0; // ENABLE SPI BUS
		PIC_SPI_Buffer = Data;
		while (!PIC_SPI_Ready);
		if ((Device != Expander_A_SPI) && (Device != Expander_B_SPI) && (Device != Expander_C_SPI) && (Device != Expander_D_SPI)) 
			DSP_SPIB_CS = 1; // DISABLE SPI BUS
		Received_SPI_Value = Read_From_SPI();
		
		if (Device == FPGA_SPI)
		{
			PIC_SPI_Disable;
			Delay_6_25US(1); // delay 7 usec
			PIC_SPI_Status = 0x00; 			
			PIC_SPI_Control = 0x12; // SPI module configured as master with 1MHZ clock
			PIC_SPI_Enable;
			Delay_6_25US(1); // delay 7 usec
		}
		return (Received_SPI_Value);
	}
	else return (0);
}