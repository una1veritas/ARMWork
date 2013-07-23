//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2010 Code Red Technologies Ltd.
//
// timer.c provides example contains setup and configuration functions for the
// 4 timers to be found in the LPC1768 MCU fitted to the RDB1768 development
// board.
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.

#include "LPC17xx.h"
#include "timer.h"

volatile uint32_t timer0_counter = 0;
volatile uint32_t timer1_counter = 0;
volatile uint32_t timer2_counter = 0;
volatile uint32_t timer3_counter = 0;

/*************************************************
 * enable_timer()
 *
 * Enables appropriate timer, based on 'timer_num'
 *************************************************/
void enable_timer( uint8_t timer_num )
{
	if ( timer_num == 0 )
	{
		LPC_TIM0->TCR = 1;
	}
	else if ( timer_num == 1 )
	{
		LPC_TIM1->TCR = 1;
	}
	else if ( timer_num == 2 )
	{
		LPC_TIM2->TCR = 1;
	}
	else if ( timer_num == 3 )
	{
		LPC_TIM3->TCR = 1;
	}
}

/**************************************************
 * disable_timer()
 *
 * Disables appropriate timer, based on 'timer_num'
 **************************************************/
void disable_timer( uint8_t timer_num )
{
	if ( timer_num == 0 )
	{
		LPC_TIM0->TCR = 0;
	}
	else if ( timer_num == 1 )
	{
		LPC_TIM1->TCR = 0;
	}
	else if ( timer_num == 2 )
	{
		LPC_TIM2->TCR = 0;
	}
	else if ( timer_num == 3 )
	{
		LPC_TIM3->TCR = 0;
	}
}

/**************************************************
 * reset_timer()
 *
 * Resets appropriate timer, based on 'timer_num'
 **************************************************/
void reset_timer( uint8_t timer_num )
{
	uint32_t regVal;

	if ( timer_num == 0 )
	{
		regVal = LPC_TIM0->TCR;
		regVal |= 0x02;
		LPC_TIM0->TCR = regVal;
	}
	else if ( timer_num == 1 )
	{
		regVal = LPC_TIM1->TCR;
		regVal |= 0x02;
		LPC_TIM1->TCR = regVal;
	}
	else if ( timer_num == 2 )
	{
		regVal = LPC_TIM2->TCR;
		regVal |= 0x02;
		LPC_TIM2->TCR = regVal;
	}
	else if ( timer_num == 3 )
	{
		regVal = LPC_TIM3->TCR;
		regVal |= 0x02;
		LPC_TIM3->TCR = regVal;
	}
}

/******************************************************
 * init_timer()
 *
 * Initialises appropriate timer, based on 'timer_num',
 * setting timer interval, resetting timer and
 * enabling timer interrupt.
 ******************************************************/
void init_timer ( uint8_t timer_num, uint32_t TimerInterval )
{
	if ( timer_num == 0 )
	{
		timer0_counter = 0;
		LPC_TIM0->MR0 = TimerInterval;
		LPC_TIM0->MCR = 3;				/* Interrupt and Reset on MR0 */
		NVIC_EnableIRQ(TIMER0_IRQn);
	}
	else if ( timer_num == 1 )
	{
		timer1_counter = 0;
		LPC_TIM1->MR0 = TimerInterval;
		LPC_TIM1->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER1_IRQn);
	}
	else if ( timer_num == 2 )
	{
		timer2_counter = 0;
		LPC_TIM2->MR0 = TimerInterval;
		LPC_TIM2->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER2_IRQn);
	}
	else if ( timer_num == 3 )
	{
		timer3_counter = 0;
		LPC_TIM3->MR0 = TimerInterval;
		LPC_TIM3->MCR = 3;				/* Interrupt and Reset on MR1 */
		NVIC_EnableIRQ(TIMER3_IRQn);
	}
}

