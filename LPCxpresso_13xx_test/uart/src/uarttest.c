/*****************************************************************************
 *   uarttest.c:  UART test C file for NXP LPC13xx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.20  ver 1.00    Preliminary version, first Release
 *
 ******************************************************************************/
#include "LPC13xx.h"
#include "uart.h"

#include "gpio.h"
#include "config.h"
//#include "timer32.h"

#include <ctype.h>

extern volatile uint32_t UARTCount;
extern volatile uint8_t UARTBuffer[BUFSIZE];

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

int main(void) {
	/* Basic chip initialization is taken care of in SystemInit() called
	 * from the startup code. SystemInit() and chip settings are defined
	 * in the CMSIS system_<part family>.c file.
	 */

//	init_timer32(0, TIME_INTERVAL);
//	enable_timer32(0);

	GPIOInit();
	/* Set LED port pin to output */
	GPIOSetDir(LED_PORT, LED_BIT, 1);

	  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
	    while (1);                                  /* Capture error */
	  }

	  if ( !(SysTick->CTRL & (1<<SysTick_CTRL_CLKSOURCE_Msk)) )
	  {
		/* When external reference clock is used(CLKSOURCE in
		Systick Control and register bit 2 is set to 0), the
		SYSTICKCLKDIV must be a non-zero value and 2.5 times
		faster than the reference clock.
		When core clock, or system AHB clock, is used(CLKSOURCE
		in Systick Control and register bit 2 is set to 1), the
		SYSTICKCLKDIV has no effect to the SYSTICK frequency. See
		more on Systick clock and status register in Cortex-M3
		technical Reference Manual. */
		LPC_SYSCON->SYSTICKCLKDIV = 0x08;
	  }

	/* NVIC is installed inside UARTInit file. */
	UARTInit(115200);

	while (1) { /* Loop forever */
		if (UARTCount != 0) {
			GPIOSetValue(LED_PORT, LED_BIT, LED_ON);
			//
			LPC_UART->IER = IER_THRE | IER_RLS; /* Disable RBR */
			uint16_t i;
			for (i = 0; i < UARTCount; i++) {
				if (isalnum(UARTBuffer[i]))
					UARTBuffer[i] = (UARTBuffer[i] + 1) % 0x100;
			}
			UARTSend((uint8_t *) UARTBuffer, UARTCount);
			UARTCount = 0;
			LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR; /* Re-enable RBR */

			Delay(100);
			GPIOSetValue(LED_PORT, LED_BIT, LED_OFF);
		}

	}
}
