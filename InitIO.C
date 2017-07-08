

#include <p18cxxx.h>		// Chip-specific definitions for PIC family

#include "util.h"

//----------------------------------------------------------------------------
//  void InitIO()
//
//  Description:
//      Function to initialize the I/O Ports
//----------------------------------------------------------------------------
void InitIO()
{

// MAPPING BUSES TO RELEVANT PINS
	
	RPINR8_9 = (RPINR8_9 & 0x0F) | 0x40; // MAP SDI TO RP16
	RPOR16_17 = (RPOR16_17 & 0x0F) | 0x40; // MAP SDO TO RP17
	RPOR14_15 = (RPOR14_15 & 0x0F) | 0x30; // MAP SCK TO RP15
	RPINR0_1 = (RPINR0_1 & 0xF0) | 0x04; // MAP PC UART RX TO RP19
	RPOR18_19 = (RPOR18_19 & 0xF0) | 0x01;  // MAP PC UART TX TO RP18
	RPINR2_3 = (RPINR2_3 & 0xF0) | 0x05; // MAP SPARE UART RX TO RP22
	RPOR22_23 = (RPOR22_23 & 0x0F) | 0x20;  // MAP SPARE UART TX TO RP23
	RPINR26_27 = (RPINR26_27 & 0xF0) | 0x07; // MAP INT1 TO RP29
	RPINR38_39 = (RPINR38_39 & 0x0F) | 0x70; // MAP TOCKI TO RP28
	RPINR40_41 = (RPINR40_41 & 0x0F) | 0x50; // MAP T1CKI TO RP21
	Registers_Map_Lock = 1; // LOCK THE MAPPING REGISTERS CONFIGURATION TO AVOID ERASE 

// ANALOG INPUTS

	 A2D_REFFERENCE_VOLTAGE = 0;  
	 tris_A2D_REFFERENCE_VOLTAGE = 1; 
	 AMP_RVRS_PWR_SNS_AN2 = 0; 
	 tris_AMP_RVRS_PWR_SNS_AN2 = 1;  
	 AMP_FRWD_PWR_SNS_AN2 = 0;  
	 tris_AMP_FRWD_PWR_SNS_AN2 = 1; 
	 AMP_FRWD_PWR_SNS_AN1 = 0;  
	 tris_AMP_FRWD_PWR_SNS_AN1 = 1;  
	 HP_TEC_Power_AN = 0;   
	 tris_HP_TEC_Power_AN = 1;   
	 PHN_ADC_SPR_AN = 0;  
	 tris_PHN_ADC_SPR_AN = 1;  
	 AMP_TEMP_SNS_AN = 0;  
	 tris_AMP_TEMP_SNS_AN = 1; 
	 DDS1_BEFORE_SW_MSR = 0;  
	 tris_DDS1_BEFORE_SW_MSR = 1;  
	 SMA1_OUT_MSR = 0;  
	 tris_SMA1_OUT_MSR = 1; 
	 DDS2_BEFORE_SW_MSR = 0;  
	 tris_DDS2_BEFORE_SW_MSR = 1;  
	 SMA2_OUT_MSR = 0;  
	 tris_SMA2_OUT_MSR = 1;
	 VAC_PRESR_SENS_AN = 0;  
	 tris_VAC_PRESR_SENS_AN = 1;  
	 N5V_SENS = 0;  
	 tris_N5V_SENS = 1;  
	 GEN_ADC1_AN = 0;  
	 tris_GEN_ADC1_AN = 1;  
	 P12V_SENS = 0;  
	 tris_P12V_SENS = 1;  
	 P5V_SENS = 0; 
	 tris_P5V_SENS = 1;  
	 P3_3V_SENS = 0;  
	 tris_P3_3V_SENS = 1;  
	 P48V_SENS = 0;  
	 tris_P48V_SENS = 1;  
	 P150V_SENS = 0; 
	 tris_P150V_SENS = 1; 
	 P24V_SENS = 0;  
	 tris_P24V_SENS = 1;  
	 N12V_SENS = 0;  
	 tris_N12V_SENS = 1;  
	 AMP_RVRS_PWR_SNS_AN1 = 0;  
	 tris_AMP_RVRS_PWR_SNS_AN1 = 1;  
	 AMP_CUR_SNS_AN2 = 0;  
	 tris_AMP_CUR_SNS_AN2 = 1; 
	 AMP_CUR_SNS_AN1 = 0;  
	 tris_AMP_CUR_SNS_AN1 = 1;  
	
// DDS1 CONTROL

	 DDS1_SYN_RESET = 1; 
	 tris_DDS1_SYN_RESET = 0; 
	 DDS1_POT_SDI = 0; 
	 tris_DDS1_POT_SDI = 0; 
	 DDS1_POT_SDO = 0; 
	 tris_DDS1_POT_SDO = 1; 
	 DDS1_SYN_FQ_UD = 0; 
	 tris_DDS1_SYN_FQ_UD = 0; 
	 DDS1_SQ_OUT = 0; 
	 tris_DDS1_SQ_OUT = 1; 
	 DDS1_OUT_ENABLE = 1; 
	 tris_DDS1_OUT_ENABLE = 0; 
	 DDS1_GEN_FAIL_INT = 0; 
	 tris_DDS1_GEN_FAIL_INT = 0; 
	 DDS1_SYN_D7 = 0; 
	 tris_DDS1_SYN_D7 = 0; 
	 DDS1_POT_SCK = 0; 
	 tris_DDS1_POT_SCK = 0; 
	 DDS1_SYN_W_CLK = 0; 
	 tris_DDS1_SYN_W_CLK = 0; 
	 DDS1_POT_RESETn = 0; 
	 tris_DDS1_POT_RESETn = 0; 
	/* DDS1_OUT_B_ENABLE = 0; 
	 tris_DDS1_OUT_B_ENABLE = 0; */
	 DDS1_POT_SSn = 1; 
	 tris_DDS1_POT_SSn = 0; 
	
// DDS2 CONTROL

	 DDS2_SYN_RESET = 1; 
	 tris_DDS2_SYN_RESET = 0; 
	 DDS2_POT_SDI = 0; 
	 tris_DDS2_POT_SDI = 0; 
	 DDS2_POT_SDO = 0; 
	 tris_DDS2_POT_SDO = 1; 
	 DDS2_SYN_FQ_UD = 0; 
	 tris_DDS2_SYN_FQ_UD = 0; 
	 DDS2_SQ_OUT = 0; 
	 tris_DDS2_SQ_OUT = 1; 
	 DDS2_OUT_ENABLE = 1; 
	 tris_DDS2_OUT_ENABLE = 0; 
	 DDS2_GEN_FAIL_INT = 0; 
	 tris_DDS2_GEN_FAIL_INT = 0; 
	 DDS2_SYN_D7 = 0; 
	 tris_DDS2_SYN_D7 = 0; 
	 DDS2_POT_SCK = 0; 
	 tris_DDS2_POT_SCK = 0; 
	 DDS2_SYN_W_CLK = 0; 
	 tris_DDS2_SYN_W_CLK = 0; 
	 DDS2_POT_RESETn = 0; 
	 tris_DDS2_POT_RESETn = 0; 
	/* DDS2_OUT_B_ENABLE = 0; 
	 tris_DDS2_OUT_B_ENABLE = 0; */
	 DDS2_POT_SSn = 1; 
	 tris_DDS2_POT_SSn = 0; 
	
// AMPLIFIER CONTROL

	 AMP_GPIO_OUT1 = 0; 
	 tris_AMP_GPIO_OUT1 = 0; 
	 AMP_GPIO_OUT2 = 0; 
	 tris_AMP_GPIO_OUT2 = 0; 
/*	 AMP_GPIO_OUT3 = 0; 
	 tris_AMP_GPIO_OUT3 = 0; 
	 AMP_GPIO_OUT4 = 0; 
	 tris_AMP_GPIO_OUT4 = 0; */
	 AMP_GPIO_OUT5 = 0; 
	 tris_AMP_GPIO_OUT5 = 0; 
	 AMP_GPIO_OUT6 = 0; 
	 tris_AMP_GPIO_OUT6 = 0;
	 AMP_GPIO_OUT7 = 0; 
	 tris_AMP_GPIO_OUT7 = 0; 
	 AMP_GPIO_OUT8 = 0; 
	 tris_AMP_GPIO_OUT8 = 0; 
	 AMP_GPIO_OUT9 = 0; 
	 tris_AMP_GPIO_OUT9 = 0; 
	 AMP_ENABLE = 0; 
	 tris_AMP_ENABLE = 0; 
	 AMP_ENABLE2 = 0; 
	 tris_AMP_ENABLE2 = 0; 

// FPGA PIC CONTROL

	FPGA_DSP_GPIO9 = 0;
	tris_FPGA_DSP_GPIO9 = 1;
	FPGA_DSP_GPIO10 = 0;
	tris_FPGA_DSP_GPIO10 = 1; 
	
// SPI INTERFACE CONTROL

	 DSP_SPIB_CLK = 0; 
	 tris_DSP_SPIB_CLK = 0; 
	 DSP_SPIB_MOSI = 0; 
	 tris_DSP_SPIB_MOSI = 0; 
	 DSP_SPIB_MISO = 0; 
	 tris_DSP_SPIB_MISO = 1; 
	 DSP_SPIB_CS = 1; 
	 tris_DSP_SPIB_CS = 0; 
	 DSP_SPIB_SEL0 = 0; 
	 tris_DSP_SPIB_SEL0 = 0; 
	 DSP_SPIB_SEL1 = 0; 
	 tris_DSP_SPIB_SEL1 = 0; 
	 DSP_SPIB_SEL2 = 0; 
	 tris_DSP_SPIB_SEL2 = 0; 
	 DSP_SPIB_SEL3 = 0; 
	 tris_DSP_SPIB_SEL3 = 0; 
	
// RS-232 INTERFACE CONTROL

	 DSP_SERB_TX = 0; 
	 tris_DSP_SERB_TX = 0; 
	 DSP_SERB_RX = 0; 
	 tris_DSP_SERB_RX = 1; 
	
	 SERB_GEN_TX = 0; 
	 tris_SERB_GEN_TX = 0; 
	 SERB_GEN_RX = 0; 
	 tris_SERB_GEN_RX = 1; 
	
// POWER DOMAINS CONTROL

	 POWER_ON = 0; 
	 tris_POWER_ON = 1; 
	 PS_24V_VAC_EN = 0; 
	 tris_PS_24V_VAC_EN = 0; 
	 PS_48V_EN = 0; 
	 tris_PS_48V_EN = 0; 
	
// MISCALLENIOUS DEVICES CONTROL

	 FPGA_PIC_INT = 0; 
	 tris_FPGA_PIC_INT = 1; 
	 PIC_PC_ACK = 0; 
	 tris_PIC_PC_ACK = 0; 
	 HP_DISP_1 = 0; 
	 tris_HP_DISP_1 = 1; 
	 HP_DISP_2 = 0; 
	 tris_HP_DISP_2 = 1; 
	 HP_SPI_MUX = 0; 
	 tris_HP_SPI_MUX = 0; 
	 
}	
