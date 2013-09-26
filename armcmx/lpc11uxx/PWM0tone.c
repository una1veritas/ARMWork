/*
 */
 
#include "LPC11Uxx.h"                        /* LPC11xx definitions */
#include "armcmx.h"
#include "gpio.h"
#include "timer16.h"
#include "delay.h"

#include "PWM0tone.h"

void TIMER16_0_IRQHandler(void)
{
  if ( LPC_CT16B0->IR & (0x1<<0) )
  {
	LPC_CT16B0->IR = 0x1<<0;			/* clear interrupt flag */
//	timer16_0_counter[0]++;
  }
  if ( LPC_CT16B0->IR & (0x1<<1) )
  {
	LPC_CT16B0->IR = 0x1<<1;			/* clear interrupt flag */
//	timer16_0_counter[1]++;
  }
  if ( LPC_CT16B0->IR & (0x1<<4) ) /* Bit4 CR0INT */
  {
	LPC_CT16B0->IR = 0x1<<4;		/* clear interrupt flag */
//	timer16_0_capture[0]++;
  }
  if ( LPC_CT16B0->IR & (0x1<<6) ) /* Bit6 CR1INT */
  {
	LPC_CT16B0->IR = 0x1<<6;		/* clear interrupt flag */
//	timer16_0_capture[1]++;
  }
  return;
}

void PWM0_toneOff(void) {
  LPC_CT16B0->TCR = 0;
  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1UL<<7);
}

void PWM0_tone(GPIOPin outpin, uint32_t freq, uint32_t duration) {
  uint32_t period = SystemCoreClock/freq;
  uint8_t match_enable = MATCH0;

  LPC_CT16B0->TCR = 0;

  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
  /* Setup the outputs */
  
  if ( outpin == PIO0_8 ) {
    match_enable = MATCH0;
    LPC_IOCON->PIO0_8           &= ~0x07;	
    LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 */
  } else if ( outpin == PIO1_13 ) {
    match_enable = MATCH0;
    LPC_IOCON->PIO1_13           &= ~0x07;
    LPC_IOCON->PIO1_13           |= 0x02;		/* Timer0_16 MAT0 */
  } else if ( outpin == PIO0_9 ) {
    match_enable = MATCH1;
    LPC_IOCON->PIO0_9           &= ~0x07;
    LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 */
  } else if ( outpin == PIO1_14 ) {
    match_enable = MATCH1;
    LPC_IOCON->PIO1_14           &= ~0x07;
    LPC_IOCON->PIO1_14           |= 0x02;		/* Timer0_16 MAT1 */
  } else if ( outpin == PIO0_10 ) {
    match_enable = MATCH2;
    LPC_IOCON->SWCLK_PIO0_10    &= ~0x07;
    LPC_IOCON->SWCLK_PIO0_10    |= 0x03;		/* Timer0_16 MAT2 */
  } else if ( outpin == PIO1_15 ) {
    match_enable = MATCH2;
    LPC_IOCON->PIO1_15           &= ~0x07;
    LPC_IOCON->PIO1_15           |= 0x02;		/* Timer0_16 MAT0 */
  } 
  // timer16_1 
	else if ( outpin == PIO0_21 ) {
    match_enable = MATCH0; // 0x01
		LPC_IOCON->PIO0_21           &= ~0x07;
		LPC_IOCON->PIO0_21           |= 0x01;		/* Timer1_16 MAT0 */
	} else if ( outpin == PIO0_22 ) {
	  /*  Timer1_16 I/O config */
    match_enable = MATCH1;
		LPC_IOCON->PIO0_22           &= ~0x07;
		LPC_IOCON->PIO0_22           |= 0x02;		/* Timer1_16 MAT1 */
	} else if ( outpin == PIO1_23 )	{
    match_enable = MATCH1;
	  LPC_IOCON->PIO1_23           &= ~0x07;
		LPC_IOCON->PIO1_23           |= 0x01;		/* Timer1_16 MAT1 */
	}

  /* Setup the external match register */
  LPC_CT16B0->EMR = (1<<EMC3)|(1<<EMC2)|(1<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);
  /* Enable the selected PWMs and enable Match3 */
  LPC_CT16B0->PWMC = (1<<3)|(match_enable);
  /* Setup the match registers */
  /* set the period value to a global variable */
  LPC_CT16B0->MR3 = period;
  LPC_CT16B0->MR0 = period/2;
  LPC_CT16B0->MR1 = period/2;
  LPC_CT16B0->MR2 = period/2;
  /* Set the match control register */
  LPC_CT16B0->MCR = 1<<10;				/* Reset on MR3 */
  /* Enable the TIMER1 Interrupt */
  NVIC_EnableIRQ(TIMER_16_0_IRQn);
  
  LPC_CT16B0->TCR = 1; //enable_timer16_0();
  if ( duration ) {
    delay(duration);
    PWM0_toneOff(); // LPC_CT16B0->TCR = 0; //disable_timer16_0();
  }
}

