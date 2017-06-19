#include <p18cxxx.h>
// Microchip library for delays routines
#include <delays.h> 
/// Chip-specific and general utility macros.
#include "util.h"

/********************************************************************
*    Function Name:  Delay                                    *
                      *
*    Parameters:     Delay in m Seconds                                           *
                                        *
********************************************************************/
/*void Delay_100nSEC(int Msec)
 {
	while(Msec>0)
  		Msec--;
}*/

void Delay_0_625US(int Msec)
 {
	while(Msec>0)
		{
  			WAIT_0_625US(1);  
  			Msec--;
  		}
          
}

void Delay_6_25US(int Msec)
 {
	while(Msec>0)
		{
  			WAIT_6_25US(1);  
  			Msec--;
  		}
          
}

void Delay_62_5US(int Msec)
 {
	while(Msec>0)
		{
  			WAIT_62_5US(1);  
  			Msec--;
  		}
          
}

void Delay_625US(int Msec)
 {
	while(Msec>0)
		{
  			WAIT_625US(1);  
  			Msec--;
  		}
          
}