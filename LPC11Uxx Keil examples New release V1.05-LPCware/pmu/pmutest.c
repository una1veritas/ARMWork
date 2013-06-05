/****************************************************************************
 *   $Id:: pmutest.c 4086 2010-07-31 00:15:47Z usb01267                     $
 *   Project: NXP LPC11Uxx PMU (Power Management Unit) example
 *
 *   Description:
 *     This file contains PMU slave test modules, main entry, to test PMU 
 *     slave APIs.
*
****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include "LPC11Uxx.h"			/* LPC11xx Peripheral Registers */
#include "gpio.h"
#include "timer16.h"
#include "pmu.h"

extern volatile uint32_t timer16_0_counter;

/*****************************************************************************
** Function name:	GPIO_DriveLow
**
** Descriptions:	Configure pins as GPIO outputs, internal pull-ups disabled, 
**									and drive low
**
** parameters:		None
** 						
** Returned value:	None
*****************************************************************************/
void GPIO_DriveLow()
{
  LPC_IOCON->RESET_PIO0_0 = 0xC1;    /* Configure PIO0_0 as gpio */
  LPC_IOCON->PIO0_1 = 0xC0;           
  LPC_IOCON->PIO0_2 = 0xC0;              
  LPC_IOCON->PIO0_3 = 0xC0;                
  LPC_IOCON->PIO0_4 = 0xC0;                
  LPC_IOCON->PIO0_5 = 0xC0;      
  LPC_IOCON->PIO0_6 = 0xC0;           
  LPC_IOCON->PIO0_7 = 0xC0;           
  LPC_IOCON->PIO0_8 = 0xC0;                 
  LPC_IOCON->PIO0_9 = 0xC0;
           
  LPC_IOCON->SWCLK_PIO0_10 = 0xC1;                

  LPC_IOCON->TDI_PIO0_11 = 0xC1;            
  LPC_IOCON->TMS_PIO0_12 = 0xC1;            
  LPC_IOCON->TDO_PIO0_13 = 0xC1;              
  LPC_IOCON->TRST_PIO0_14 = 0xC1;                                    

  LPC_IOCON->SWDIO_PIO0_15 = 0xC1;        
  
  LPC_IOCON->PIO0_16 = 0xC0;                
  LPC_IOCON->PIO0_17 = 0xC0;         
  LPC_IOCON->PIO0_18 = 0xC0;    
  LPC_IOCON->PIO0_19 = 0xC0;   
  LPC_IOCON->PIO0_20 = 0xC0;               
  LPC_IOCON->PIO0_21 = 0xC0;     
  LPC_IOCON->PIO0_22 = 0xC0;      
  LPC_IOCON->PIO0_23 = 0xC0;
    
  LPC_IOCON->PIO1_0 = 0xC0;                 
  LPC_IOCON->PIO1_1 = 0xC0;         
  LPC_IOCON->PIO1_2 = 0xC0;       
  LPC_IOCON->PIO1_3 = 0xC0;      
  LPC_IOCON->PIO1_4 = 0xC0;                 
  LPC_IOCON->PIO1_5 = 0xC0;      
  LPC_IOCON->PIO1_6 = 0xC0;     
  LPC_IOCON->PIO1_7 = 0xC0;       
  LPC_IOCON->PIO1_8 = 0xC0;                 
  LPC_IOCON->PIO1_9 = 0xC0;        
  LPC_IOCON->PIO1_10 = 0xC0;        
  LPC_IOCON->PIO1_11 = 0xC0;       
  LPC_IOCON->PIO1_12 = 0xC0;                
  LPC_IOCON->PIO1_13 = 0xC0;       
  LPC_IOCON->PIO1_14 = 0xC0;       
  LPC_IOCON->PIO1_15 = 0xC0;       
  LPC_IOCON->PIO1_16 = 0xC0;               
  LPC_IOCON->PIO1_17 = 0xC0;
  LPC_IOCON->PIO1_18 = 0xC0;           
  LPC_IOCON->PIO1_19 = 0xC0;        
  LPC_IOCON->PIO1_20 = 0xC0;                
  LPC_IOCON->PIO1_21 = 0xC0;       
  LPC_IOCON->PIO1_22 = 0xC0;                
  LPC_IOCON->PIO1_23 = 0xC0;       
  LPC_IOCON->PIO1_24 = 0xC0;                            
  LPC_IOCON->PIO1_25 = 0xC0;
  LPC_IOCON->PIO1_26 = 0xC0;                
  LPC_IOCON->PIO1_27 = 0xC0;
  LPC_IOCON->PIO1_28 = 0xC0;                            
  LPC_IOCON->PIO1_29 = 0xC0;        
  LPC_IOCON->PIO1_30 = 0xC0;                            
  LPC_IOCON->PIO1_31 = 0xC0;                                        
  
//Configure direction of pins as outputs
  LPC_GPIO->DIR[0] = 0xFFFDFFFF; /* Configure PIO0_17 as input , for wakeup source */
  LPC_GPIO->DIR[1] = 0xFFFFFFFF;

 //Drive all ports to low
LPC_GPIO->CLR[0] =  0xFFFFFFFF;
LPC_GPIO->CLR[1] =  0xFFFFFFFF;
  return;
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
#if !TEST_DEEPPOWERDOWN
  uint32_t regVal;
#endif

  SystemCoreClockUpdate();
  GPIOInit();
	
#if !TEST_DEEPPOWERDOWN
  PMU_Init();
  //regVal = LPC_SYSCON->PDSLEEPCFG;
  //regVal &= ~(WDT_OSC_PD | BOD_PD);
	//turn off BOD and WDT Osc in deep sleep and power-down modes
	regVal = 0xFF;
  
	#if MCU_SLEEP
	GPIO_DriveLow();
        PMU_Sleep( MCU_SLEEP, regVal );
	#endif 
	
	#if MCU_DEEP_SLEEP
	GPIO_DriveLow();
        PMU_Sleep( MCU_DEEP_SLEEP, regVal );
	#endif 
	
	#if MCU_POWER_DOWN
	PMU_Sleep( MCU_POWER_DOWN, regVal );
	#endif

#else
  PMU_DeepPowerDown();
#endif

  /* Using 16-bit timer 0 for LED blinky rate. */
  init_timer16(0, TIME_INTERVAL);
  enable_timer16(0);
  
  /* Set port 1_20 to output as LED control */
  GPIOSetDir( 1, 20, 1 );

  while (1)                                /* Loop forever */
  {
	/* I/O configuration and LED setting pending. */
	if ( (timer16_0_counter > 0) && (timer16_0_counter <= 200) )
	{
	  GPIOSetBitValue( 1, 20, 0 );
	}
	if ( (timer16_0_counter > 200) && (timer16_0_counter <= 400) )
	{
	  GPIOSetBitValue( 1, 20, 1 );
	}
	else if ( timer16_0_counter > 400 )
	{
	  timer16_0_counter = 0;
	}
  }
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
