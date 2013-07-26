/*****************************************************************************
 *   uarttest.c:  main C entry file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "type.h"
#include "uart.h"

extern volatile uint32_t UART3Count;
extern volatile uint8_t UART3Buffer[BUFSIZE];
extern volatile uint32_t UART1Count;
extern volatile uint8_t UART1Buffer[BUFSIZE];

/*****************************************************************************
**   Main Function  main()
This program has been test on Keil LPC1700 board.
*****************************************************************************/
int main (void)
{
  SystemInit();

  UARTInit(3, 38400);	/* baud rate setting */
  UARTInit(1, 38400);	/* baud rate setting */

  while (1) 
  {				/* Loop forever */
	if ( UART3Count != 0 )
	{
	  LPC_UART3->IER = IER_THRE | IER_RLS;			/* Disable RBR */
	  UARTSend( 3, (uint8_t *)UART3Buffer, UART3Count );
	  UART3Count = 0;
	  LPC_UART3->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
	}
	if ( UART1Count != 0 )
	{
	  LPC_UART1->IER = IER_THRE | IER_RLS;			/* Disable RBR */
	  UARTSend( 1, (uint8_t *)UART1Buffer, UART1Count );
	  UART1Count = 0;
	  LPC_UART1->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
	}
  }
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/
