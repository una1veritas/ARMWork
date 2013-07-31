/*
 * delay.c
 *
 *  Created on: 2012/11/15
 *      Author: sin
 */

/* Includes ------------------------------------------------------------------*/
#include "LPC11Uxx.h"
#include "timer16.h"
#include "nmi.h"

#include "armcmx.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile uint32_t __counter_micros;
volatile uint32_t __counter_millis;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		None
** 
******************************************************************************/
void init_timer16_1(void) 
{
  ///* For 16-bit timer, make sure that TIME_INTERVAL should be no
//greater than 0xFFFF. */
#ifndef TIME_INTERVAL
#define TIME_INTERVAL	(1000 - 1)
#define TIME_PRESCALER (SystemCoreClock/1000000 - 1)
#endif

  uint32_t TimerInterval = TIME_INTERVAL;
  
    /* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);
    LPC_CT16B1->MR0 = TimerInterval;
    LPC_CT16B1->MR1 = TimerInterval;
#if TIMER_MATCH
    __counter_micros = 0;
    __counter_millis = 0;
	  set_timer16_match(1, 0x07, 0);
    LPC_CT16B1->EMR &= ~(0xFF<<4);
    LPC_CT16B1->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8));
		LPC_CT16B1->MCR = (0x3<<0)|(0x3<<3);  /* Interrupt and Reset on MR0 and MR1 */
		#endif
		
		#if CAPTURE
    timer16_1_capture[0] = 0;
    timer16_1_capture[1] = 0;
    set_timer16_capture(timer_num, 0);
    /* Capture 0 and 1 on rising edge, interrupt enable. */
    LPC_CT16B1->CCR = (0x5<<0)|(0x5<<3);
		#endif
    
    /* Enable the TIMER1 Interrupt */
#if NMI_ENABLED
    NVIC_DisableIRQ(TIMER_16_1_IRQn);
    NMI_Init( TIMER_16_1_IRQn );
#else
    NVIC_EnableIRQ(TIMER_16_1_IRQn);
#endif
return;
}


void start_delay(void) {
  init_timer16_1();
  enable_timer16_1();
}

void TIMER16_1_IRQHandler(void)
{
  if ( LPC_CT16B1->IR & (0x1<<0) )
  {  
	LPC_CT16B1->IR = 0x1<<0;			/* clear interrupt flag */
	__counter_micros += 1000;
    __counter_millis++;
  }
  if ( LPC_CT16B1->IR & (0x1<<1) )
  {  
	LPC_CT16B1->IR = 0x1<<1;			/* clear interrupt flag */
//	timer16_1_counter[1]++;
  }
  if ( LPC_CT16B1->IR & (0x1<<4) )
  {
	LPC_CT16B1->IR = 0x1<<4;		/* clear interrupt flag */
//	timer16_1_capture[0]++;
  }
  if ( LPC_CT16B1->IR & (0x1<<5) )
  {
	LPC_CT16B1->IR = 0x1<<5;		/* clear interrupt flag */
//	timer16_1_capture[1]++;
  }
  return;
}


uint32_t micros(void) {
	return __counter_micros + LPC_CT16B1->TC;
}

uint32_t millis(void) {
	return __counter_millis;
}

void enable_timer16_1(void) {
  LPC_CT16B1->TCR = 0x02;	//	 reset timer 
  LPC_CT16B1->PR = (SystemCoreClock/1000000 - 1);//		 set prescaler  
    LPC_CT16B1->MR0 =  (1000 - 1);
//    LPC_CT16B1->IR  = 0xff;	//	 reset all interrrupts 
//    LPC_CT16B1->MCR = 0x04;	//	 stop timer on match 
    LPC_CT16B1->TCR = 0x01;	//	 start timer 
}

void reset_timer16_1(void) {
	__counter_millis = 0;
	__counter_micros = 0;
  LPC_CT16B1->TCR = 0x02; // reset timer 
    LPC_CT16B1->TCR = 0x01;	//	 start timer 
}


void delay_ms(uint32_t w) {
	uint32_t wtill = millis() + w;
	if ( wtill < millis() ) {
		while ( millis() >= wtill );
	}
	while (millis() < wtill) ;
}

void delay_us(uint32_t w) {
	uint32 tim2 = LPC_CT16B1->TC;
	for ( ; w > 0 ; ) {
		if (tim2 == LPC_CT16B1->TC) 
			continue;
		tim2 = LPC_CT16B1->TC;
		w--;
	}
}
