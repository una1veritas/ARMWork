/****************************************************************************
 *   $Id:: usart_m.c 4001 2010-07-28 20:41:55Z usb00423                     $
 *   Project: NXP LPC11Uxx USART example
 *
 *   Description:
 *     This file contains USART master test modules, main entry, to test 
 *     USART APIs.
 *
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

#include "LPC11Uxx.h"
#include "usart.h"

extern volatile uint8_t UARTTxBuffer[BUFSIZE];
extern volatile uint8_t UARTRxBuffer[BUFSIZE];
extern volatile uint32_t UARTBlockReceived;

volatile uint8_t UARTTxDummyBuffer[BUFSIZE];

int main (void) 
{
  volatile uint32_t i;

  SystemCoreClockUpdate();

  for ( i = 0; i < BUFSIZE; i++ )
  {
	if ( i & 0x01 )
	  UARTTxBuffer[i] = 0x55;
	else
	  UARTTxBuffer[i] = 0xAA;
	UARTTxDummyBuffer[i] = UARTRxBuffer[i] = 0x00;	
  }

  /* NVIC is installed inside UARTInit file. */
  USARTInit(9600, MASTER_MODE);

  /* To run USART test, reset at the slave side first, master will
  send a block of data first, if it's not in the continuous mode,
  master send a block of dummy data to clock in the data from the slave,
  on the slave side, it's ready to receive, once it's received a block
  of data, send the same data back to the master. Once the master receives
  the block of data from the slave. It verifies the result. */

  /* For Sync comminication with STARTSTOP disabled, the sync. is very important
  or received data will be one bit off. If SYNCCTRL_NOSS bit is set, the program
  should have a break point to stop below, then, reset the slave side. Without
  doing so, this program will fail. If SYNCCTRL_NOSS is not set, STARTSTOP bit
  is enabled, then break point is not necessary. */
  USARTSend( (uint8_t *)UARTTxBuffer, BUFSIZE );

  for ( i = 0; i < 0x4000; i++ );		/* Short delay and wait for slave to be ready. */
  UARTBlockReceived = 0;

  /* Send dummy block for read, one more than needed. It's not necessary 
  if continuous clock is turned on while STARTSTOP is enabled. */
  if ( LPC_USART->SYNCCTRL & SYNCCTRL_NOSS )
  {
	/* If StartStop is disabled, no continuous clock should be turned on. */
	USARTSend( (uint8_t *)UARTTxDummyBuffer, BUFSIZE );
  }
  else if ( !(LPC_USART->SYNCCTRL & SYNCCTRL_CSCEN) )
  {
	USARTSend( (uint8_t *)UARTTxDummyBuffer, BUFSIZE );
  }
	
  while ( !UARTBlockReceived );		/* Whole data block received from the slave. */

  /* Verifying buffer. */
  for ( i = 0; i < BUFSIZE; i++ )
  {
	if ( UARTTxBuffer[i] != UARTRxBuffer[i] )
	{
	  while ( 1 );
	}
  }
  UARTBlockReceived = 0;
  while ( 1 );	/* never exit from main for debugging. */
}
