/****************************************************************************
 *   $Id:: usart_s.c 4001 2010-07-28 20:41:55Z usb00423                     $
 *   Project: NXP LPC1Uxx USART example
 *
 *   Description:
 *     This file contains USART Slave test modules, main entry, to test 
 *     USART APIs.
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

int main (void) 
{
  uint32_t i;

SystemCoreClockUpdate();

  for ( i = 0; i < BUFSIZE; i++ )
  {
	UARTTxBuffer[i] = UARTRxBuffer[i] = 0x00;
  }

  /* To run USART test, reset at the slave side first, master will
  send a block of data first, if it's not in the continuous mode,
  master send a block of dummy data to clock in the data from the slave,
  on the slave side, it's ready to receive, once it's received a block
  of data, send the same data back to the master. Once the master receives
  the block of data from the slave. It verifies the result. */

  /* NVIC is installed inside USARTInit file. */
  USARTInit(9600, SLAVE_MODE);

  while ( !UARTBlockReceived );

  for ( i = 0; i < BUFSIZE; i++ )
  {
	if ( (UARTRxBuffer[i] != 0xAA) && ((i&0x1)==0x0) )
	{
	  while ( 1 );
	}
	if ( (UARTRxBuffer[i] != 0x55) && ((i&0x1)==0x1) )
	{
	  while ( 1 );
	}
	UARTTxBuffer[i] = UARTRxBuffer[i];			/* Copy to the transmit buffer. */
	UARTRxBuffer[i] = 0;	/* Clear the Rx buffer for next block if necessary. */
  }
  USARTSend( (uint8_t *)UARTTxBuffer, BUFSIZE );
  UARTBlockReceived = 0;
  
  while ( 1 );	/* Never exit from main for debugging purpose. */

}
