#include <p18cxxx.h>

#include <delays.h>

#include "util.h"

#include "Power.h"

#include "SPI.h"

#include "Extender.h"

BOOL Domains_Power_Control(char Domain, BOOL Off_On) // POWER ON AND OFF MAIN DOMAINS. RETURN "0", WHEN WRONG DOMAIN
{
  char Tmp = 0;
  switch (Domain) // POWER DOMAIN CONTROL
		{
			case PS_24V_VAC: 
				if (Off_On) PS_24V_VAC_EN = 1;
				else PS_24V_VAC_EN = 0;
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case PS_48V: 
				if (Off_On) PS_48V_EN = 1;
				else PS_48V_EN = 0;
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case PHN_12V:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x80); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0x7F); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case PHN_5V:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x40); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xBF); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case VAC_VALVE:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x20); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xDF); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case HP_POWER:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x10); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xEF); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case HP_TEC:
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x08); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xF7); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case DDS_5V:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x04); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xFB); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case DDS_12V:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x02); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xFD); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case DDS_N12V:
				Tmp = Read_From_Extender(Expander_D_SPI, 0x12); // READ PORT STATUS FROM EXPANDER D
				if (Off_On) Write_To_Extender(Expander_D_SPI, 0x12, Tmp | 0x01); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x12, Tmp & 0xFE); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case PC_POWER_12V:
				Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x80); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0x7F); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case PULSER_150V:
				Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
				if (Off_On) Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x40); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0xBF); // CLEAR RELEVANT BIT
				Delay_625US(500); // delay 300 msec
				return(TRUE);
				break;
			case PULSER_5V:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x20); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0xDF); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case PULSER_N5V:
				Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
				if (Off_On) Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x10); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0xEF); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				return(TRUE);
				break;
			case FPGA2V5:
				Write_To_Extender(Expander_B_SPI, 0x04, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_D_SPI, 0x13); // READ PORT STATUS FROM EXPANDER D
				if (!Off_On) Write_To_Extender(Expander_D_SPI, 0x13, Tmp | 0x08); // SET RELEVANT BIT
				else Write_To_Extender(Expander_D_SPI, 0x13, Tmp & 0xF7); // CLEAR RELEVANT BIT
				
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x04, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			case PC_MODULE_POWER:
				Write_To_Extender(Expander_B_SPI, 0x05, 0x00); // DISABLE ALL INTERRUPTS
				Tmp = Read_From_Extender(Expander_C_SPI, 0x13); // READ PORT STATUS FROM EXPANDER C
				if (!Off_On) Write_To_Extender(Expander_C_SPI, 0x13, Tmp | 0x04); // SET RELEVANT BIT
				else Write_To_Extender(Expander_C_SPI, 0x13, Tmp & 0xFB); // CLEAR RELEVANT BIT
				Delay_62_5US(240); // delay 15 msec
				Extender_Interrupt_Clear(Expander_B_SPI);
				Write_To_Extender(Expander_B_SPI, 0x05, 0xFF); // ENABLE ALL INTERRUPTS
				return(TRUE);
				break;
			default:
				return(FALSE);
				break;
		} 
}

void Pulser_Power_On()
{
	Domains_Power_Control(PULSER_N5V, TRUE);
	Domains_Power_Control(PULSER_5V, TRUE);
}

void Pulser_Power_Off()
{
	Domains_Power_Control(PULSER_150V, FALSE);
	Domains_Power_Control(PULSER_5V, FALSE);
	Domains_Power_Control(PULSER_N5V, FALSE);
}

void DDS_Power_On()
{
	Domains_Power_Control(DDS_5V, TRUE);
	Domains_Power_Control(DDS_12V, TRUE);
	Domains_Power_Control(DDS_N12V, TRUE);
}

void DDS_Power_Off()
{
	Domains_Power_Control(DDS_12V, FALSE);
	Domains_Power_Control(DDS_N12V, FALSE);
	Domains_Power_Control(DDS_5V, FALSE);
}

void System_Power_Init()
{
	Domains_Power_Control(PS_24V_VAC, FALSE);
	Domains_Power_Control(PS_48V, FALSE);
	Domains_Power_Control(PHN_12V, FALSE);
	Domains_Power_Control(PHN_5V, FALSE);
	Domains_Power_Control(HP_POWER, FALSE);
	Domains_Power_Control(HP_TEC, FALSE);
	Domains_Power_Control(PC_POWER_12V, FALSE);
	Domains_Power_Control(PC_MODULE_POWER, FALSE);
	Pulser_Power_Off();
	DDS_Power_Off();
	Domains_Power_Control(FPGA2V5, FALSE);
}

void Powerup_Domains_On()
{
	Domains_Power_Control(FPGA2V5, TRUE);
	Domains_Power_Control(PC_MODULE_POWER, TRUE);	
	Domains_Power_Control(PC_POWER_12V, TRUE);
	Domains_Power_Control(HP_POWER, TRUE);
	Pulser_Power_On();
}

