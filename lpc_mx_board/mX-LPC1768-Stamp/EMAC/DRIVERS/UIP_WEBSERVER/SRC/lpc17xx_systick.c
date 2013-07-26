
#include "lpc17xx_systick.h"
#include "lpc_types.h"
#include "lpc17xx.h"

volatile UNS_32 systick_counter = 0;
volatile UNS_32 led_tick_counter = 0;								//LED Blink Counter

void SysTick_Handler (void) 
{
  ++systick_counter;

  if(led_tick_counter++>10)	   										//100ms(10x10mS) LED[0] Blink
  {    
//    GPIO2->FIOPIN ^= 1 << 0;										//Toggle LED[0]
	led_tick_counter=0;
  }
}

void init_timer ( UNS_32 TimerInterval ) 
{
	systick_counter = 0;
				   
	SysTick_Config(SystemFrequency/TimerInterval);               /* Generate interrupt every 10 ms */	
}

/******************************************************************************
**                            End Of File
******************************************************************************/
