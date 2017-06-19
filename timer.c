

#include <p18cxxx.h>
/// Chip-specific and general utility macros.
#include "util.h"

/********************************************************************
*    Function Name:  timers                                     *
*    Return Value:   char: Timer0 8-bit value                       *
*    Parameters:     void                                           *
                                        *
********************************************************************/
 
void InitTimer0INTMode(void)
 {
  DDS1_RESET_PULSE_COUNTER;
 }

void InitTimer1INTMode(void)
 {
  DDS2_RESET_PULSE_COUNTER;
 }

void InitTimer2INTMode(void)
 {
  RESET_TIMER2;
  EnableIntTMR2; // Enable timer2 interrupts
  Timer2Disable; 
 }

void InitTimer3INTMode(void)
 {
  RESET_TIMER3;
  EnableIntTMR3; // Enable timer3 interrupts
  Timer3Disable; 
 }
 
 void InitTimer4INTMode(void)
 {
  RESET_TIMER4;
  EnableIntTMR4; // Enable timer2 interrupts
  Timer4Disable; 
 }