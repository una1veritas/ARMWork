/****************************************************************************
 *   $Id:: pmu.c 4087 2010-07-31 00:16:13Z usb01267                         $
 *   Project: NXP LPC11Uxx PMU example
 *
 *   Description:
 *     This file contains PMU code example which include PMU 
 *     initialization, PMU interrupt handler, and APIs for PMU
 *     reading.
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
//#include "type.h"
#include <stdint.h>
#include "gpio.h"
#include "pmu.h"

/*****************************************************************************
** Function name:		PMU_Init
**
** Descriptions:		Initialize PMU and setup wakeup source.
**						For Sleep and deepsleep, any of the I/O pins can be 
**						used as the wakeup source.
**						For Deep Powerdown, only pin P1.4 can be used as 
**						wakeup source from deep powerdown. 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PMU_Init( void )
{
  #if Gint_Enable
	uint32_t bitPattern[2];
  uint32_t eventPattern[2];
	#endif
	
  /* Enable all clocks, even those turned off at power up. */
  LPC_SYSCON->PDRUNCFG &= ~(WDT_OSC_PD | ADC_PD);

#if 0
  /* FLEX_INT0 source */
  GPIOSetFlexInterrupt( CHANNEL0, PORT1, 1, 0, 0 );
  /* FLEX_INT0 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<0;
#endif

#if 0
  /* FLEX_INT1 source */
  GPIOSetFlexInterrupt( CHANNEL1, PORT1, 2, 0, 0 );
  /* FLEX_INT1 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<1;
#endif

#if 0
  /* FLEX_INT2 source */
  GPIOSetFlexInterrupt( CHANNEL2, PORT1, 3, 0, 0 );
  /* FLEX_INT2 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<2;
#endif

#if 0
  /* FLEX_INT3 source */
  GPIOSetFlexInterrupt( CHANNEL3, PORT1, 4, 0, 0 );
  /* FLEX_INT3 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<3;
#endif

#if 0
  /* FLEX_INT4 source */
  GPIOSetFlexInterrupt( CHANNEL4, PORT0, 2, 0, 0 );
  /* FLEX_INT4 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<4;
#endif

#if 0
  /* FLEX_INT5 source */
  GPIOSetFlexInterrupt( CHANNEL5, PORT0, 3, 0, 0 );
  /* FLEX_INT5 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<5;
#endif

#if 0
  /* FLEX_INT6 source */
  GPIOSetFlexInterrupt( CHANNEL6, PORT0, 4, 0, 0 );
  /* FLEX_INT6 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<6;
#endif

#if 1
  /* FLEX_INT7 source */  
	/* PIO 0_17 set as wake-up pin */ 
  GPIOSetFlexInterrupt( CHANNEL7, PORT0, 17, 0, 0 );
  /* FLEX_INT7 as wakeup source */
  LPC_SYSCON->STARTERP0 = 0x1<<7;
#endif

#if Gint_Enable
#if 0
  bitPattern[0] = 0x00000020;		/* PIO0_5 enabled */
  bitPattern[1] = 0x00000004;		/* PIO1_2 enabled */
  eventPattern[0] = 0x00000020;		/* PIO0_5 rising edge */
  eventPattern[1] = 0x00000000;		/* PIO1_2 falling edge */
  /* GINT0_INT sources OR together */
  GPIOSetGroupedInterrupt( GROUP0, &bitPattern[0], 0, 0, &eventPattern[0] );
  /* GINT0_INT as wakeup source */
  LPC_SYSCON->STARTERP1 = 0x1<<20;
#endif

#if 0
  bitPattern[0] = 0x00000020;		/* PIO0_5 enabled */
  bitPattern[1] = 0x00000004;		/* PIO1_2 enabled */
  eventPattern[0] = 0x00000020;		/* PIO0_5 rising edge */
  eventPattern[1] = 0x00000000;		/* PIO1_2 falling edge */
  /* GINT0_INT sources AND together */
  GPIOSetGroupedInterrupt( GROUP0, &bitPattern[0], 1, 0, &eventPattern[0] );
  /* GINT0_INT as wakeup source */
  LPC_SYSCON->STARTERP1 = 0x1<<20;
#endif

#if 0
  bitPattern[0] = 0x00000020;		/* PIO0_5 enabled */
  bitPattern[1] = 0x00000004;		/* PIO1_2 enabled */
  eventPattern[0] = 0x00000000;		/* PIO0_5 falling edge */
  eventPattern[1] = 0x00000004;		/* PIO1_2 rising edge */
  /* GINT0_INT sources OR together */
  GPIOSetGroupedInterrupt( GROUP1, &bitPattern[0], 0, 0, &eventPattern[0] );
  /* GINT0_INT as wakeup source */
  LPC_SYSCON->STARTERP1 = 0x1<<21;
#endif

#if 0
  bitPattern[0] = 0x00000020;		/* PIO0_5 enabled */
  bitPattern[1] = 0x00000004;		/* PIO1_2 enabled */
  eventPattern[0] = 0x00000000;		/* PIO0_5 falling edge */
  eventPattern[1] = 0x00000004;		/* PIO1_2 rising edge */
  /* GINT0_INT sources OR together */
  GPIOSetGroupedInterrupt( GROUP1, &bitPattern[0], 1, 0, &eventPattern[0] );
  /* GINT0_INT as wakeup source */
  LPC_SYSCON->STARTERP1 = 0x1<<21;
#endif
#endif
  return;
}

/*****************************************************************************
** Function name:		PMU_Sleep
**
** Descriptions:		Put some of the peripheral in sleep mode.
**
** parameters:			SleepMode: 2 is power down, 1 is deep sleep, 0 is sleep, 
**						Sleep peripheral module(s)
** Returned value:		None
** 
*****************************************************************************/
void PMU_Sleep( uint32_t SleepMode, uint32_t SleepCtrl )
{
  LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
  LPC_SYSCON->PDSLEEPCFG = SleepCtrl;
  /* If normal sleep, not deep sleep, don't do anything to SCR reg. */
 
	if(SleepMode == MCU_POWER_DOWN)
	{
	SCB->SCR |= NVIC_LP_SLEEPDEEP;
	LPC_PMU->PCON = 0x2;
	}
	
	if(SleepMode == MCU_DEEP_SLEEP)
	{
	SCB->SCR |= NVIC_LP_SLEEPDEEP;
	LPC_PMU->PCON = 0x1;
	}
	
	if(SleepMode == MCU_SLEEP)
	{
	SCB->SCR = 0x00;
	LPC_PMU->PCON = 0x0;
	}
	
	__WFI();
  return;
}

/*****************************************************************************
** Function name:		PMU_DeepPowerDown
**
** Descriptions:		Some of the content should not be touched 
**						during the deep power down to wakeup process.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PMU_DeepPowerDown( void )
{
  uint32_t regVal;

  if ( (LPC_PMU->PCON & (0x1<<11)) != 0x0 )
  {
    /* Check deep power down bits. If deep power down mode is entered, 
    clear the PCON bits. */
    regVal = LPC_PMU->PCON;
    regVal |= (0x1<<11);
    LPC_PMU->PCON = regVal;

    if ( (LPC_PMU->GPREG0 != 0x12345678)||(LPC_PMU->GPREG1 != 0x87654321)
         ||(LPC_PMU->GPREG2 != 0x56781234)||(LPC_PMU->GPREG3 != 0x43218765) )
    {
      while (1);
    }
  }
  else
  {
	/* If in neither sleep nor deep power mode, enter deep power
	down mode now. */
    LPC_PMU->GPREG0 = 0x12345678;
    LPC_PMU->GPREG1 = 0x87654321;
    LPC_PMU->GPREG2 = 0x56781234;
    LPC_PMU->GPREG3 = 0x43218765;
	SCB->SCR |= NVIC_LP_SLEEPDEEP;
    LPC_PMU->PCON = 0x3;
    __WFI();
  }
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
