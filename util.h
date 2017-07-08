/*****************************************************************************
 *				    			                                             *
 * Generic Utility Include file for Microchip PIC 1866J15 					 *
 *				    			                                             *
 * Author:		Shimon BD							.	                     *
 * Targets:		Microchip 18F66J15									         *
 * 			
 * Created:		4/12/08					                                 *
 *				    			                                             *
 ****************************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

// For compatibility with Microchip firmware definitions in system\typedefs.h

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;

//MAIN DEFINITIONS

	#define MCU_Version_Number 0x1
	#define Vol_Mon_Max_Samples_Number 10
	#define Maximum_Allowable_Transactions 3
	#define Vref 3
	#define System_Registers_Number 45

/*************************************AMPLITUDE MEASUREMENT PARAMETERS********************/
	#define Divider 3.1276595
	// #define Total_Signal_Gain 4.65
/*****************************************************************************************/

/*************************************SYSTEM VOLTAGE DOMAINS MEASUREMENT PARAMETERS********************/
	#define P3V3_Resistor_Divider 0.68
	#define P5V_Resistor_Divider 0.5
	#define N5V_Resistor_Divider 0.5
	#define P12V_Resistor_Divider 0.204
	#define N12V_Resistor_Divider 0.21
	#define P24V_Resistor_Divider 0.11
	#define P150V_Resistor_Divider 0.01666
	#define P48V_Resistor_Divider 0.05
	#define HPTEC_Att_Coefficient 0.35
	
/*****************************************************************************************/

/*************************************TEC MODULE VOLTAGE SET PARAMETERS********************/
	#define TEC_Voltage_Low_Limit_Value 250 // 2.5V
	#define TEC_Voltage_High_Limit_Value 850 // 8.5V
/*****************************************************************************************/	
	
	
	
	//A2D DEFINITIONS
	
	#define AMP_FRWD_PWR_SNS_LOW_THRESHOLD2 0
	#define AMP_FRWD_PWR_SNS_LOW_THRESHOLD1 0
	#define AMP_RVRS_PWR_SNS_LOW_THRESHOLD2 0
	#define VREF_LOW_THRESHOLD 0
	#define PHN_ADC_SPR_LOW_THRESHOLD 0
	#define VAC_PRESR_SENS_LOW_THRESHOLD 0
	#define HP_TEC_Power_AN_LOW_THRESHOLD 1135
	#define AMP_TEMP_SNS_LOW_THRESHOLD 0
	#define N5V_LOW_THRESHOLD 3071 // -4.5V
	#define DDS1_BEFORE_SW_MSR_LOW_THRESHOLD 0
	#define DDS2_BEFORE_SW_MSR_LOW_THRESHOLD 0
	#define SMA1_OUT_MSR_LOW_THRESHOLD 0
	#define SMA2_OUT_MSR_LOW_THRESHOLD 0
	#define AMP_RVRS_PWR_SNS_LOW_THRESHOLD1 0
	#define AMP_CUR_SNS_LOW_THRESHOLD2 0
	#define AMP_CUR_SNS_LOW_THRESHOLD1 0
	#define P3_3V_LOW_THRESHOLD 2785 // 3V
	#define P5V_LOW_THRESHOLD 3071 // 4.5V
	#define P12V_LOW_THRESHOLD 3052 // 10.8V
	#define GEN_ADC1_LOW_THRESHOLD 0
	#define N12V_LOW_THRESHOLD 3136 // -10.8V
	#define P24V_LOW_THRESHOLD 3052 // 21.6V
	#define P150V_LOW_THRESHOLD 3071 // 135V
	#define P48V_LOW_THRESHOLD 3071 // 43.2V

	#define AMP_FRWD_PWR_SNS_HIGH_THRESHOLD2 0xFFE
	#define AMP_FRWD_PWR_SNS_HIGH_THRESHOLD1 0xFFE
	#define AMP_RVRS_PWR_SNS_HIGH_THRESHOLD2 0xFFE
	#define VREF_HIGH_THRESHOLD 0
	#define PHN_ADC_SPR_HIGH_THRESHOLD 0xFFF
	#define VAC_PRESR_SENS_HIGH_THRESHOLD 0xFFE
	#define HP_TEC_Power_AN_HIGH_THRESHOLD 0xFFE
	#define AMP_TEMP_SNS_HIGH_THRESHOLD 0xFFE
	#define N5V_HIGH_THRESHOLD 3755 // -5.5V
	#define DDS1_BEFORE_SW_MSR_HIGH_THRESHOLD 0xFFF
	#define DDS2_BEFORE_SW_MSR_HIGH_THRESHOLD 0xFFF
	#define SMA1_OUT_MSR_HIGH_THRESHOLD 0xFFF
	#define SMA2_OUT_MSR_HIGH_THRESHOLD 0xFFF
	#define AMP_RVRS_PWR_SNS_HIGH_THRESHOLD1 0xFFE
	#define AMP_CUR_SNS_HIGH_THRESHOLD2 0xFFE
	#define AMP_CUR_SNS_HIGH_THRESHOLD1 0xFFE
	#define P3_3V_HIGH_THRESHOLD 3342 // 3.6V
	#define P5V_HIGH_THRESHOLD 3755 // 5.5V
	#define P12V_HIGH_THRESHOLD 3730 // 13.2V
	#define GEN_ADC1_HIGH_THRESHOLD 0xFFF
	#define N12V_HIGH_THRESHOLD 3833 // -13.2V
	#define P24V_HIGH_THRESHOLD 3730 // 26.4V
	#define P150V_HIGH_THRESHOLD 3755 // 165V
	#define P48V_HIGH_THRESHOLD 3755 // 52.8V
	
	#define AMP_FRWD_PWR_SNS2 0
	#define AMP_FRWD_PWR_SNS1 1
	#define AMP_RVRS_PWR_SNS2 2
	#define PHN_ADC_SPR 3
	#define VAC_PRESR_SENS 4
	#define HP_TEC_Power_SNS 5
	#define AMP_TEMP_SNS 6
	#define N5V 7
	#define DDS1_BEFORE_SW_SNS 8
	#define DDS2_BEFORE_SW_SNS 9
	#define SMA1_OUT_SNS 10
	#define SMA2_OUT_SNS 11
	#define AMP_RVRS_PWR_SNS1 12
	#define AMP_CUR_SNS2 13
	#define AMP_CUR_SNS1 14
	#define P3_3V 15
	#define P5V 16
	#define P12V 17
	#define GEN_ADC1 18
	#define N12V 19
	#define P24V 20
	#define P150V 21
	#define P48V 22
	
//POWER DEFINITIONS

	#define PS_24V_VAC 0
	#define PS_48V 1
	#define PHN_12V 2
	#define PHN_5V 3
	#define VAC_VALVE 4
	#define HP_POWER 5
	#define HP_TEC 6
	#define DDS_5V 7
	#define DDS_12V 8
	#define DDS_N12V 9
	#define PC_POWER_12V 10
	#define PULSER_150V 11
	#define PULSER_5V 12
	#define PULSER_N5V 13
	#define FPGA2V5 14
	#define PC_MODULE_POWER 15
	
// SPI BUS DEVICES DEFINITION
	
	#define General_Isolated_SPI 0x0
	#define Hand_Piece_SPI 0x1
	#define Pulser_SPI 0x2
	#define Phantom_SPI 0x3
	#define General_SPI 0x4
	#define FPGA_SPI 0x5
	#define EEPROM_SPI 0x6
	#define Tech_Power_Digital_Potentiometer_SPI 0x7
	#define Expander_A_SPI 0x8
	#define Expander_B_SPI 0x9
	#define Expander_C_SPI 0xA
	#define Expander_D_SPI 0xB

// SYSTEM WORKING MODES DEFINITION
	
	#define System_Idle_Mode 0x0
	#define System_Configuration_Mode 0x1
	#define System_Operation_Mode 0x2		

	
// ERROR BITS DEFINITION
	
	#define PC_Communication_Fault 0x0
	#define Emergency_Switch_State 0x1
	#define UART_Receiver_Error 0x2
	#define Word_Format_Error 0x3
	#define UART_Transaction_Error 0x4
	#define TEC_Voltage_Error 0x5
	#define UART_Transmitter_Error 0x6
	#define DDS1_Frequency_Error 0x7
	#define DDS1_Amplitude_Error 0x8
	#define DDS2_Frequency_Error 0x9
	#define DDS2_Amplitude_Error 0xA
	#define DDS1_Amplitude_Measure_Error 0xB
	#define DDS2_Amplitude_Measure_Error 0xC
	#define Amplifier_Pulse_Mode_Word_Error 0xD
	#define P3V3_Domain_Error 0xE
	#define P5V_Domain_Error 0xF
	#define N5V_Domain_Error 0x10
	#define P12V_Domain_Error 0x11
	#define N12V_Domain_Error 0x12
	#define P24V_Domain_Error 0x13
	#define P150V_Domain_Error 0x14
	#define P48V_Domain_Error 0x15
	#define Parser_Working_Error 0x16
	#define Pulser_IC_Error 0x17
	#define Tissue_temperature_measure_Error 0x18
	
// ANALOG INPUTS

	#define A2D_REFFERENCE_VOLTAGE PORTAbits.RA3
	#define tris_A2D_REFFERENCE_VOLTAGE TRISAbits.TRISA3
	#define AMP_RVRS_PWR_SNS_AN2 PORTAbits.RA2
	#define tris_AMP_RVRS_PWR_SNS_AN2 TRISAbits.TRISA2
	#define AMP_FRWD_PWR_SNS_AN2 PORTAbits.RA0
	#define tris_AMP_FRWD_PWR_SNS_AN2 TRISAbits.TRISA0
	#define AMP_FRWD_PWR_SNS_AN1 PORTAbits.RA1
	#define tris_AMP_FRWD_PWR_SNS_AN1 TRISAbits.TRISA1
	#define HP_TEC_Power_AN PORTAbits.RA4
	#define tris_HP_TEC_Power_AN TRISAbits.TRISA4
	#define PHN_ADC_SPR_AN PORTAbits.RA5
	#define tris_PHN_ADC_SPR_AN TRISAbits.TRISA5
	#define AMP_TEMP_SNS_AN PORTFbits.RF2
	#define tris_AMP_TEMP_SNS_AN TRISFbits.TRISF2
	#define DDS1_BEFORE_SW_MSR PORTCbits.RC1
	#define tris_DDS1_BEFORE_SW_MSR TRISCbits.TRISC1
	#define SMA1_OUT_MSR PORTFbits.RF6
	#define tris_SMA1_OUT_MSR TRISFbits.TRISF6
	#define DDS2_BEFORE_SW_MSR PORTFbits.RF5
	#define tris_DDS2_BEFORE_SW_MSR TRISFbits.TRISF5
	#define SMA2_OUT_MSR PORTHbits.RH4
	#define tris_SMA2_OUT_MSR TRISHbits.TRISH4
	#define VAC_PRESR_SENS_AN PORTFbits.RF7
	#define tris_VAC_PRESR_SENS_AN TRISFbits.TRISF7
	#define N5V_SENS PORTGbits.RG0
	#define tris_N5V_SENS TRISGbits.TRISG0
	#define GEN_ADC1_AN PORTGbits.RG1
	#define tris_GEN_ADC1_AN TRISGbits.TRISG1
	#define P12V_SENS PORTGbits.RG2
	#define tris_P12V_SENS TRISGbits.TRISG2
	#define P5V_SENS PORTGbits.RG3
	#define tris_P5V_SENS TRISGbits.TRISG3
	#define P3_3V_SENS PORTGbits.RG4
	#define tris_P3_3V_SENS TRISGbits.TRISG4
	#define P48V_SENS PORTHbits.RH0
	#define tris_P48V_SENS TRISHbits.TRISH0
	#define P150V_SENS PORTHbits.RH1
	#define tris_P150V_SENS TRISHbits.TRISH1
	#define P24V_SENS PORTHbits.RH2
	#define tris_P24V_SENS TRISHbits.TRISH2
	#define N12V_SENS PORTHbits.RH3
	#define tris_N12V_SENS TRISHbits.TRISH3
	#define AMP_RVRS_PWR_SNS_AN1 PORTHbits.RH5
	#define tris_AMP_RVRS_PWR_SNS_AN1 TRISHbits.TRISH5
	#define AMP_CUR_SNS_AN2 PORTHbits.RH6
	#define tris_AMP_CUR_SNS_AN2 TRISHbits.TRISH6
	#define AMP_CUR_SNS_AN1 PORTHbits.RH7
	#define tris_AMP_CUR_SNS_AN1 TRISHbits.TRISH0

// DDS1 CONTROL

	#define DDS1_SYN_RESET PORTBbits.RB1
	#define tris_DDS1_SYN_RESET TRISBbits.TRISB1
	#define DDS1_POT_SDI PORTBbits.RB2
	#define tris_DDS1_POT_SDI TRISBbits.TRISB2
	#define DDS1_POT_SDO PORTBbits.RB3
	#define tris_DDS1_POT_SDO TRISBbits.TRISB3
	#define DDS1_SYN_FQ_UD PORTBbits.RB4
	#define tris_DDS1_SYN_FQ_UD TRISBbits.TRISB4
	#define DDS1_SQ_OUT PORTDbits.RD1
	#define tris_DDS1_SQ_OUT TRISDbits.TRISD1
	/*#define DDS1_OUT_ENABLE PORTFbits.RF3
	#define tris_DDS1_OUT_ENABLE TRISFbits.TRISF3*/
	#define DDS1_GEN_FAIL_INT PORTJbits.RJ2
	#define tris_DDS1_GEN_FAIL_INT TRISJbits.TRISJ2
	#define DDS1_SYN_D7 PORTJbits.RJ3
	#define tris_DDS1_SYN_D7 TRISJbits.TRISJ3
	#define DDS1_POT_SCK PORTKbits.RK3
	#define tris_DDS1_POT_SCK TRISKbits.TRISK3
	#define DDS1_SYN_W_CLK PORTKbits.RK4
	#define tris_DDS1_SYN_W_CLK TRISKbits.TRISK4
	#define DDS1_POT_RESETn PORTKbits.RK5
	#define tris_DDS1_POT_RESETn TRISKbits.TRISK5
	#define DDS1_OUT_ENABLE PORTLbits.RL4
	#define tris_DDS1_OUT_ENABLE TRISLbits.TRISL4
	#define DDS1_POT_SSn PORTLbits.RL5
	#define tris_DDS1_POT_SSn TRISLbits.TRISL5
	
// DDS2 CONTROL

	#define DDS2_SYN_RESET PORTEbits.RE6
	#define tris_DDS2_SYN_RESET TRISEbits.TRISE6
	#define DDS2_POT_SDI PORTKbits.RK7
	#define tris_DDS2_POT_SDI TRISKbits.TRISK7
	#define DDS2_POT_SDO PORTKbits.RK6
	#define tris_DDS2_POT_SDO TRISKbits.TRISK6
	#define DDS2_SYN_FQ_UD PORTDbits.RD6
	#define tris_DDS2_SYN_FQ_UD TRISDbits.TRISD6
	#define DDS2_SQ_OUT PORTEbits.RE0
	#define tris_DDS2_SQ_OUT TRISEbits.TRISE0
	#define DDS2_OUT_ENABLE PORTKbits.RK2
	#define tris_DDS2_OUT_ENABLE TRISKbits.TRISK2
	#define DDS2_GEN_FAIL_INT PORTJbits.RJ2
	#define tris_DDS2_GEN_FAIL_INT TRISJbits.TRISJ2
	#define DDS2_SYN_D7 PORTLbits.RL0
	#define tris_DDS2_SYN_D7 TRISLbits.TRISL0
	#define DDS2_POT_SCK PORTGbits.RG6
	#define tris_DDS2_POT_SCK TRISGbits.TRISG6
	#define DDS2_SYN_W_CLK PORTDbits.RD7
	#define tris_DDS2_SYN_W_CLK TRISDbits.TRISD7
	#define DDS2_POT_RESETn PORTEbits.RE7
	#define tris_DDS2_POT_RESETn TRISEbits.TRISE7
	/*#define DDS2_OUT_B_ENABLE PORTKbits.RK1
	#define tris_DDS2_OUT_B_ENABLE TRISKbits.TRISK1*/
	#define DDS2_POT_SSn PORTJbits.RJ0
	#define tris_DDS2_POT_SSn TRISJbits.TRISJ0
	
	#define DDS1_OUT_ON DDS1_OUT_ENABLE = 0; 
	#define DDS1_OUT_OFF DDS1_OUT_ENABLE = 1; 
	#define DDS2_OUT_ON DDS2_OUT_ENABLE = 0; 
	#define DDS2_OUT_OFF DDS2_OUT_ENABLE = 1; 
	
// AMPLIFIER CONTROL

	#define AMP_GPIO_OUT1 PORTLbits.RL6
	#define tris_AMP_GPIO_OUT1 TRISLbits.TRISL6
	#define AMP_GPIO_OUT2 PORTLbits.RL7
	#define tris_AMP_GPIO_OUT2 TRISLbits.TRISL7
/*	#define AMP_GPIO_OUT3 PORTCbits.RC1 // TRANSFFERED TO EXPANDER
	#define tris_AMP_GPIO_OUT3 TRISCbits.TRISC1
	#define AMP_GPIO_OUT4 PORTCbits.RC0
	#define tris_AMP_GPIO_OUT4 TRISCbits.TRISC0*/
	#define AMP_GPIO_OUT5 PORTKbits.RK0
	#define tris_AMP_GPIO_OUT5 TRISKbits.TRISK0
	#define AMP_GPIO_OUT6 PORTJbits.RJ4
	#define tris_AMP_GPIO_OUT6 TRISJbits.TRISJ4
	#define AMP_GPIO_OUT7 PORTJbits.RJ5
	#define tris_AMP_GPIO_OUT7 TRISJbits.TRISJ5
	#define AMP_GPIO_OUT8 PORTJbits.RJ6
	#define tris_AMP_GPIO_OUT8 TRISJbits.TRISJ6
	#define AMP_GPIO_OUT9 PORTJbits.RJ7
	#define tris_AMP_GPIO_OUT9 TRISJbits.TRISJ7
	#define AMP_ENABLE PORTEbits.RE3
	#define tris_AMP_ENABLE TRISEbits.TRISE3
	#define AMP_ENABLE2 PORTEbits.RE2
	#define tris_AMP_ENABLE2 TRISEbits.TRISE2

// FPGA PIC CONTROL

	#define FPGA_DSP_GPIO9 PORTCbits.RC1 
	#define tris_FPGA_DSP_GPIO9 TRISCbits.TRISC1
	#define FPGA_DSP_GPIO10 PORTCbits.RC0
	#define tris_FPGA_DSP_GPIO10 TRISCbits.TRISC0
	
// SPI INTERFACE CONTROL

	#define DSP_SPIB_CLK PORTCbits.RC3
	#define tris_DSP_SPIB_CLK TRISCbits.TRISC3
	#define DSP_SPIB_MOSI PORTCbits.RC4
	#define tris_DSP_SPIB_MOSI TRISCbits.TRISC4
	#define DSP_SPIB_MISO PORTCbits.RC5
	#define tris_DSP_SPIB_MISO TRISCbits.TRISC5
	#define DSP_SPIB_CS PORTEbits.RE5
	#define tris_DSP_SPIB_CS TRISEbits.TRISE5
	#define DSP_SPIB_SEL0 PORTEbits.RE4
	#define tris_DSP_SPIB_SEL0 TRISEbits.TRISE4
	#define DSP_SPIB_SEL1 PORTLbits.RL2
	#define tris_DSP_SPIB_SEL1 TRISLbits.TRISL2
	#define DSP_SPIB_SEL2 PORTLbits.RL1
	#define tris_DSP_SPIB_SEL2 TRISLbits.TRISL1
	#define DSP_SPIB_SEL3 PORTGbits.RG7
	#define tris_DSP_SPIB_SEL3 TRISGbits.TRISG7
	
// RS-232 INTERFACE CONTROL

	#define DSP_SERB_TX PORTCbits.RC6
	#define tris_DSP_SERB_TX TRISCbits.TRISC6
	#define DSP_SERB_RX PORTCbits.RC7
	#define tris_DSP_SERB_RX TRISCbits.TRISC7
	
	#define SERB_GEN_TX PORTDbits.RD3
	#define tris_SERB_GEN_TX TRISDbits.TRISD3
	#define SERB_GEN_RX PORTDbits.RD2
	#define tris_SERB_GEN_RX TRISDbits.TRISD2
	
// POWER DOMAINS CONTROL

	#define POWER_ON PORTEbits.RE1
	#define tris_POWER_ON TRISEbits.TRISE1
	#define PS_24V_VAC_EN PORTFbits.RF4
	#define tris_PS_24V_VAC_EN TRISFbits.TRISF4
	#define PS_48V_EN PORTLbits.RL3
	#define tris_PS_48V_EN TRISLbits.TRISL3
	
// MISCALLENIOUS DEVICES CONTROL

	#define FPGA_PIC_INT PORTBbits.RB0
	#define tris_FPGA_PIC_INT TRISBbits.TRISB0
	#define PIC_PC_ACK PORTBbits.RB5
	#define tris_PIC_PC_ACK TRISBbits.TRISB5
	#define HP_DISP_1 PORTDbits.RD4
	#define tris_HP_DISP_1 TRISDbits.TRISD4
	#define HP_DISP_2 PORTDbits.RD5
	#define tris_HP_DISP_2 TRISDbits.TRISD5
	#define HP_SPI_MUX PORTJbits.RJ1
	#define tris_HP_SPI_MUX TRISJbits.TRISJ1
	
// MAIN SYSTEM CONFIGURATIONS	
	
	#define Registers_Map_Lock OSCCON2bits.IOLOCK 

// INTERRUPT CONFIGURATIONS
	
	// Peripheral interrupt enable
	#define Peripheral_Int_En INTCONbits.GIEL 	

// Global flag for enabling interrupts priority 
	#define ENABLE_INT_PRIORITY RCONbits.IPEN 

// Global flag for enabling all high priority interrupts
	#define GlobalINT INTCONbits.GIEH 		// Global interrupt
	
	#define Main_Interrupt_Detected INTCONbits.INT0IF

	#define Main_Interrupt_Detection_Enable INTCONbits.INT0IE =  1;
	#define Main_Interrupt_Detection_Disable INTCONbits.INT0IE = 0;

	#define Main_Interrupt_Detection_Rizedge INTCON2bits.INTEDG0 = 1;
	#define Main_Interrupt_Detection_Falledge INTCON2bits.INTEDG0 = 0;
	
	
	
	#define Power_On_Detected INTCON3bits.INT1IF

	#define Power_On_Detection_Enable INTCON3bits.INT1IE =  1;
	#define Power_On_Detection_Disable INTCON3bits.INT1IE = 0;

	#define Power_On_Detection_Rizedge INTCON2bits.INTEDG1 = 1;
	#define Power_On_Detection_Falledge INTCON2bits.INTEDG1 = 0;
	
	#define Power_On_Detection_High_Priority INTCON3bits.INT1IP = 1;
	#define Power_On_Detection_Low_Priority INTCON3bits.INT1IP = 0;

// TIMERS CONFIGURATIONS


// Timer0 Init
//--------------

#define DDS2_PULSE_COUNTER_ENABLE T0CON = 0xA8;

#define DDS2_PULSE_COUNTER_DISABLE T0CON = 0x28; // CONFIGURE TIMER0 TO COUNTER WITHOUT PRESCALER, PRESCALER RATIO 1:2
		
#define DDS2_RESET_PULSE_COUNTER T0CON = 0x28; \
		TMR0H = 0x00; \
		TMR0L = 0x00;
		
// Timer1 Init
//--------------

#define DDS1_PULSE_COUNTER_ENABLE T1CON = 0x87;

#define DDS1_PULSE_COUNTER_DISABLE T1CON = 0x86; // CONFIGURE TIMER1 TO COUNTER WITHOUT PRESCALER, PRESCALER RATIO 1:1, EXTERNAL CLOCK INPUT NOT SYNCHRONIZED
		
#define DDS1_RESET_PULSE_COUNTER T1CON = 0x86; \
		TMR1H = 0x00; \
		TMR1L = 0x00;

// Timer3 Init
//--------------

#define TMR3_INTFLAG PIR2bits.TMR3IF

#define Timer3Enable T3CONbits.TMR3ON =  1;	// enable, 8bits timer
#define Timer3Disable T3CONbits.TMR3ON =  0; // disable

//prescaler = (64MHz / 4 / 8). Each tick = 0.5 us
#define RESET_TIMER3 T3CON = 0x32; \
		TMR3H = 0xD8; \
		TMR3L = 0xEF; //  timer programmed to interrupt after 5mS
					 
// Macro to setup timer interrupt
#define EnableIntTMR3 TMR3_INTFLAG = 0; \
					  IPR2bits.TMR3IP = 1; \
					  PIE2bits.TMR3IE = 1;

// Macro to disable timer interrupt
#define DisableIntTMR3 TMR3_INTFLAG = 0; \
					   IPR2bits.TMR3IP = 1; \
					   PIE2bits.TMR3IE = 0;
					   
// Timer5 Init
//--------------

#define TMR5_INTFLAG PIR5bits.TMR5IF

#define Timer5Enable T5CONbits.TMR5ON =  1;	// enable, 8bits timer
#define Timer5Disable T5CONbits.TMR5ON =  0; // disable

//prescaler = (64MHz / 4 / 8). Each tick = 0.5 us
#define RESET_TIMER5 T5CON = 0x32; \
		TMR5H = 0xD8; \
		TMR5L = 0xEF; //  timer programmed to interrupt after 5mS
					 
// Macro to setup timer interrupt
#define EnableIntTMR5 TMR5_INTFLAG = 0; \
					  IPR5bits.TMR5IP = 1; \
					  PIE5bits.TMR5IE = 1;

// Macro to disable timer interrupt
#define DisableIntTMR5 TMR5_INTFLAG = 0; \
					   IPR5bits.TMR5IP = 1; \
					   PIE5bits.TMR5IE = 0;
					   

// DELAY IDENTIFIER VARIABLE

#define WAIT_0_625US(_a_) Delay10TCYx(_a_);
#define WAIT_6_25US(_a_) Delay100TCYx(_a_);
#define WAIT_62_5US(_a_) Delay1KTCYx(_a_);
#define WAIT_625US(_a_) Delay10KTCYx(_a_);

// SPI INTERFACE SIGNALS

	#define PIC_SPI_Status SSP1STAT 
	#define PIC_SPI_Control SSP1CON1
	#define PIC_SPI_Buffer SSP1BUF
	#define PIC_SPI_Ready SSP1STATbits.BF
	#define PIC_SPI_Enable SSP1CON1bits.SSPEN = 1;
	#define PIC_SPI_Disable SSP1CON1bits.SSPEN = 0;
	
	#define SPI_INTFLAG PIR1bits.SSP1IF
	
	#define EnableIntSPI 	SPI_INTFLAG = 0; \
							IPR1bits.SSP1IP = 1; \
							PIE1bits.SSP1IE = 1;
					  
	#define DisableIntSPI 	SPI_INTFLAG = 0; \
							IPR1bits.SSP1IP = 1; \
							PIE1bits.SSP1IE = 0;
							
// UART INTERFACE SIGNALS

	#define Transmit_Control_Register TXSTA1
	#define Receive_Control_Register RCSTA1
	#define Baud_Rate_Control BAUDCON1
	#define Baud_Rate_Generator SPBRG1
	#define Baud_Rate_Generator_High SPBRGH1
	#define UART_Data_Transmit TXREG1
	#define UART_Data_Receive RCREG1
	
	#define UART_Enable RCSTA1bits.SPEN = 1;
	#define UART_Disable RCSTA1bits.SPEN = 0;
	
	#define UART_Transmit_Enable TXSTA1bits.TXEN = 1;
	#define UART_Transmit_Disable TXSTA1bits.TXEN = 0;
	
	#define UART_Transmit_Data_Idle_High BAUDCON1bits.TXCKP = 0;
	#define UART_Transmit_Data_Idle_Low BAUDCON1bits.TXCKP = 1;
	
	#define UART_Transmit_Ready TXSTA1bits.TRMT // "1" - TRANSMISSION SHIFT REGISTER IS EMPTY, READY FOR NEXT BYTE TRANSMISSION 
	
	#define UART_Receive_Enable RCSTA1bits.CREN = 1;
	#define UART_Receive_Disable RCSTA1bits.CREN = 0;
	
	#define UART_Receive_Data_Polarity_Inverted BAUDCON1bits.RXDTP = 1;
	#define UART_Receive_Data_Polarity_Not_Inverted BAUDCON1bits.RXDTP = 0;
	
	#define Receiver_Operation_Status BAUDCON1bits.RCIDL // "1" - RECEIVER OPERATION IS IDLE
	#define Framing_Error RCSTA1bits.FERR
	#define Overrun_Error RCSTA1bits.OERR
	
	#define UART_INTFLAG PIR1bits.RC1IF
	
	#define EnableIntUART 	UART_INTFLAG = 0; \
							IPR1bits.RC1IP = 1; \
							PIE1bits.RC1IE = 1;
					  
	#define DisableIntUART 	UART_INTFLAG = 0; \
							IPR1bits.RC1IP = 1; \
							PIE1bits.RC1IE = 0;
							
// A2D INTERFACE SIGNALS

	#define A2D_Module_Enable ADCON1Hbits.ADON = 1;
	#define A2D_Module_Disable ADCON1Hbits.ADON = 0;
	
	#define A2D_Sample_Start ADCON1Lbits.SAMP = 1;
	#define A2D_Convertion_Start ADCON1Lbits.SAMP = 0;
	
	#define A2D_Conversion_Completed ADCON1Lbits.DONE
	
	#define A2D_INTFLAG PIR1bits.ADIF
	
	#define EnableIntA2D 	A2D_INTFLAG = 0; \
							IPR1bits.ADIP = 1; \
							PIE1bits.ADIE = 1;
					  
	#define DisableIntA2D 	A2D_INTFLAG = 0; \
							IPR1bits.ADIP = 1; \
							PIE1bits.ADIE = 0;						

#endif // H
