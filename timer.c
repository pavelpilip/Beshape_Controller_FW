

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
  DDS1_PULSE_COUNTER_DISABLE;
  DDS1_RESET_PULSE_COUNTER;
 }

void InitTimer1INTMode(void)
 {
  DDS2_PULSE_COUNTER_DISABLE;
  DDS2_RESET_PULSE_COUNTER;
 }

void InitTimer3INTMode(void)
 {
  RESET_TIMER3;
  EnableIntTMR3; // Enable timer3 interrupts
  Timer3Disable; 
 }
 
void InitTimer5INTMode(void)
 {
  RESET_TIMER5;
  EnableIntTMR5; // Enable timer5 interrupts
  Timer5Disable; 
 }