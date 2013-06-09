/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB HID Demo
 *      Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "lpc11Uxx.h"                        /* LPC11xx definitions */
#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "demo.h"
#include "io_event.h"
#if POWERDOWN_MODE_USB_WAKEUP
#include "timer16.h"
extern volatile uint32_t SuspendFlag;
extern volatile uint32_t timer16_0_counter;
extern volatile uint32_t USBWakeupInterruptFlag;
volatile uint32_t WakeupFlag = 0;
#endif

#if USB_WORKAROUND
extern volatile uint32_t VBusLow;
#endif

uint8_t InReport;                           /* HID Input Report    */
                                            /*   Bit0   : Buttons  */
                                            /*   Bit1..7: Reserved */
uint8_t OutReport;                          /* HID Out Report      */
                                            /*   Bit0..7: LEDs     */

/*
 *  Get HID Input Report -> InReport
 */

void GetInReport (void) {
  if ( LPC_GPIO->PIN[0] & (1<<1) ) {		/* Check if PBINT is pressed */
    InReport = 0x01;
  } else {
    InReport = 0x00;
  }
  return;
}


/*
 *  Set HID Output Report <- OutReport
 */

void SetOutReport (void) {
  /* port1, bit 14~17 are used as LED indication from
  HID utility. */
  LPC_GPIO->PIN[1] &= ~(0x0F<<14);
  LPC_GPIO->PIN[1] |= (OutReport<<14);
  return; 
}


/* Main Program */

int main (void) 
{
#if POWERDOWN_MODE_USB_WAKEUP
  uint32_t i, j;
#endif

  /* It's very important that POWERDOWN_MODE_USB_WAKEUP and REMOTE_WAKEUP_ENABLE
  flags should not be turned on at the same time!!!!!! */
    
  SystemCoreClockUpdate();

  /* P0.1 is push-button input, P1.14~17 are LED output. */
  LPC_GPIO->DIR[1] |= (0x1<<14)|(0x1<<15)|(0x1<<16)|(0x1<<17);
  LPC_GPIO->DIR[0] &= ~(0x1<<1);

#if POWERDOWN_MODE_USB_WAKEUP
  init_timer16(0, TIME_INTERVAL);		/* the timer is for USB suspend and resume */
#endif

#if REMOTE_WAKEUP_ENABLE
  SetRemoteWakeupSource();
#endif

#if USB_WORKAROUND
  SetVBUSInterrupt();
#endif
  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1)                                /* Loop forever */
  {
#if USB_WORKAROUND
	if ( VBusLow )
	{
	  if ( LPC_USB->DEVCMDSTAT & USB_VBUS_DBOUNCE )
	  {
		/* Force a disconnect on the pull-up on D+. */
		LPC_IOCON->PIO0_6 &= ~0x07;
		GPIOSetBitValue( PORT0, 6, 1 );
		GPIOSetDir( PORT0, 6, 1 );
		while ( LPC_USB->DEVCMDSTAT & USB_VBUS_DBOUNCE );
		LPC_IOCON->PIO0_6 &= ~0x07;
		LPC_IOCON->PIO0_6 |= (0x01<<0);	/* Secondary function SoftConn */
	  }
	  VBusLow = 0;
	}
#endif

#if POWERDOWN_MODE_USB_WAKEUP
	if ( USBWakeupInterruptFlag )
	{
	  /* If the MCU is woken up from power down mode, the PLL needs to
	  be reconfigured, and USB block needs to be reset and reconnected. */
	  USBWakeupInterruptFlag = 0;
//	  SystemInit();
	  USB_Init();                           /* USB Initialization */
	  USB_Connect(TRUE);                    /* USB Connect */
	  LPC_GPIO->DIR[1] = 0x1<<5;		/* P1.5 defined as Outputs */
	  LPC_GPIO->CLR[1] = 0x1<<5;		/* turn off the LEDs */
      /* Wake up, blink 20 times, and back to normal operation. 
	  If suspend and resume again, the same sequence will apply. */
	  for ( j = 0; j < 10; j++ )
	  {
		for ( i = 0; i < 0x200000; i++ );
		LPC_GPIO->SET[1] = 0x1<<5;
		for ( i = 0; i < 0x200000; i++ );
		LPC_GPIO->CLR[1] = 0x1<<5;
	  }
	}
	/* bit 8 on USN_INT_STAT is USB NeedClk bit. */
	if ( SuspendFlag == 1 )
	{
	  if ( timer16_0_counter > 600 )
//	  if ( timer16_0_counter > 200 )	/* Need longer delay for SUSP to settle on C1US??? */
	  {
		while ( (LPC_SYSCON->USBCLKST & 0x01) );
		/* Enable USB activity as wakeup source. */
		/* Regular USB interrupt is disabled to test USBWakeup interrupt. 
		It will be enabled once it's waken up. */
		/* USB_Activity interrupt seems to be pennding here already. */
		NVIC_DisableIRQ(USB_IRQn);
		NVIC_ClearPendingIRQ(USBWakeup_IRQn);
		NVIC_EnableIRQ(USBWakeup_IRQn);   /* enable USB activity interrupt */
		LPC_SYSCON->STARTERP1 = 0x1<<19;  /* USB_Needclk in STARTERP logic domain. */
				 
		WakeupFlag = 1;
		LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
		/* USB Activity interrupt won't occur until Deepsleep bit is set in SCR.
		UM needs to be updated regarding this. */
		SCB->SCR |= 0x04;	/* Set SLEEPDEEP bit in SCR in Cortex M0 core. */	
		LPC_PMU->PCON = 0x1;
		__WFI();
	  }
	}
#endif
  }
}
