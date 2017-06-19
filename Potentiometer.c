#include <p18cxxx.h>

/// Chip-specific and general utility macros.
#include "util.h"

#include "Pseudo_SPI.h"

#include <delays.h> 

#include "Potentiometer.h" 

void Write_To_RDAC(char Address, char Word, BOOL Potentiometer_Number) // PROGRAM WIPER POSITION (Address = 0 -> RDAC1; Address = 1 -> RDAC2)
 {
	
	int Configuration_Word_Value = 0x1000, tmp = 0;
	
	tmp = Address;
	tmp = tmp << 8;
	Configuration_Word_Value = Configuration_Word_Value | tmp; // INSERT ADDRESS VALUE TO CONFIGURATION WORD
	Configuration_Word_Value = Configuration_Word_Value | Word; // INSERT DATA VALUE TO CONFIGURATION WORD
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(Configuration_Word_Value, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }
 
void Write_To_Input_Register(char Address, char Word, BOOL Potentiometer_Number) // PRE PROGRAM WIPER POSITION (Address = 0 -> REGISTER1; Address = 1 -> REGISTER2)
 {
	
	int Configuration_Word_Value = 0x2000, tmp = 0;
	
	tmp = Address;
	tmp = tmp << 8;
	Configuration_Word_Value = Configuration_Word_Value | tmp; // INSERT ADDRESS VALUE TO CONFIGURATION WORD
	tmp = Word;
	tmp = tmp & 0x00ff;
	Configuration_Word_Value = Configuration_Word_Value | tmp; // INSERT DATA VALUE TO CONFIGURATION WORD
	

	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(Configuration_Word_Value, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER INPUT REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }
	
int Read_From_RDAC(char Address, BOOL Potentiometer_Number) // READ WIPER POSITION (Address = 0 -> RDAC1; Address = 1 -> RDAC2)
 {
	
	int Configuration_Word_Value = 0x3003, tmp = 0;
	
	tmp = Address;
	tmp = tmp << 8;
	Configuration_Word_Value = Configuration_Word_Value | tmp; // INSERT ADDRESS VALUE TO CONFIGURATION WORD
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(Configuration_Word_Value, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	tmp = Send_SPI_Transaction(0x0, Potentiometer_Number); // GET THE CONFIGURATION WORD FROM THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;
	
	tmp = tmp & 0xff;
	
return(tmp);
	
 }	
 
int Read_From_Input_Register(char Address, BOOL Potentiometer_Number) // READ INPUT REGISTER (Address = 0 -> REGISTER1; Address = 1 -> REGISTER2)
 {
	
	int Configuration_Word_Value = 0x3000, tmp = 0;
	
	tmp = Address;
	tmp = tmp << 8;
	Configuration_Word_Value = Configuration_Word_Value | tmp; // INSERT ADDRESS VALUE TO CONFIGURATION WORD
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	tmp = Send_SPI_Transaction(Configuration_Word_Value, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	tmp = Send_SPI_Transaction(0x0, Potentiometer_Number); // GET THE CONFIGURATION WORD FROM THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;
	
	tmp = tmp & 0xff;
	
return(tmp);
	
 }	
 
void Wiper_Step_Up(BOOL Potentiometer_Number) // INCREMENT THE BOTH WIPER POSITIONS ONE STEP UP
 {
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(0x4801, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }
 
void Wiper_Step_Down(BOOL Potentiometer_Number) // DECREMENT THE BOTH WIPER POSITIONS ONE STEP DOWN
 {
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(0x4800, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }
 
void Load_Input_Register_To_RDAC(BOOL Potentiometer_Number) // LOAD BOTH INPUT REGISTERS CONTENT TO RDAC SYNCHRONOUSLY
 {
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(0x6800, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }
 
void Update_Wiper_Position(char Word, BOOL Potentiometer_Number) // UPDATE WIPER POSITION OF BOTH POTENTIOMETERS SYNCHRONOUSLY
 {
	
	Write_To_Input_Register(0x00, Word, Potentiometer_Number);
	Write_To_Input_Register(0x01, Word, Potentiometer_Number);
	Load_Input_Register_To_RDAC(Potentiometer_Number);

 }

 void Software_Reset_Run(BOOL Potentiometer_Number) // RUN SOFTWARE RESET
 {
	
	if (!Potentiometer_Number) DDS1_POT_SSn = 0;
	else DDS2_POT_SSn = 0;
	Send_SPI_Transaction(0xB000, Potentiometer_Number); // SEND THE CONFIGURATION WORD TO THE POTENTIOMETER RDAC REGISTER
	if (!Potentiometer_Number) DDS1_POT_SSn = 1;
	else DDS2_POT_SSn = 1;

 }

void Digital_Potentiometer_Init(BOOL Potentiometer_Number) // DAC INITIALIZATION SEQUENCE
 {
	
	if (!Potentiometer_Number) DDS1_POT_RESETn = 0; // DIGITAL POTENTIOMETER RESET
	else DDS2_POT_RESETn = 0; 
	Delay_6_25US(2); // WAIT FOR RESET DELAY TIME
	if (!Potentiometer_Number) DDS1_POT_RESETn = 1; // DIGITAL POTENTIOMETER RESET
	else DDS2_POT_RESETn = 1; 
	Delay_6_25US(20); // WAIT FOR POWER UP TIME
	
	Update_Wiper_Position(0x0, Potentiometer_Number);
	
 }
 
