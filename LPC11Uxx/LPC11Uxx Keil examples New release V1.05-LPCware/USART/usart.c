/****************************************************************************
 *   $Id:: usart.c 4001 2010-07-28 20:41:55Z usb00423                       $
 *   Project: NXP LPC11Uxx USART example
 *
 *   Description:
 *     This file contains USART code example which include USART 
 *     initialization, USART interrupt handler, and related APIs for 
 *     UART access.
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

volatile uint32_t UARTStatus[BUFSIZE];
volatile uint32_t UARTStatusCount = 0;
volatile uint32_t UARTInterruptCount = 0;
volatile uint32_t UARTErrorBrkInterruptCount = 0;
volatile uint8_t  UARTTxEmpty = 1;
volatile uint8_t  UARTTxBuffer[BUFSIZE];
volatile uint8_t  UARTRxBuffer[BUFSIZE];
volatile uint32_t UARTTxCount = 0;
volatile uint32_t UARTRxCount = 0;
volatile uint32_t UARTBlockReceived = 0;
volatile uint32_t UARTBlockTransmitted = 0;

/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART_IRQHandler(void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  UARTInterruptCount++;
  IIRValue = LPC_USART->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if (IIRValue == IIR_RLS)		/* Receive Line Status */
  {
    LSRValue = LPC_USART->LSR;
    /* Receive Line Status */
    if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXErr | LSR_BI | LSR_TXErr))
    {
      /* There are errors or break interrupt */
      /* Read LSR will clear the interrupt */
      UARTStatus[UARTStatusCount++] = LSRValue;
      if (UARTStatusCount == BUFSIZE)
      {
        UARTStatusCount = 0;		/* buffer overflow */
      }	
	  UARTErrorBrkInterruptCount++;
      Dummy = LPC_USART->RBR;	/* Dummy read on RX to clear 
								interrupt, then bail out */
      return;
    }
    if (LSRValue & LSR_RDR)	/* Receive Data Ready */			
    {
      /* If no error on RLS, normal ready, save into the data buffer. */
      /* Note: read RBR will clear the interrupt */
      UARTRxBuffer[UARTRxCount++] = LPC_USART->RBR;
      if (UARTRxCount == BUFSIZE)
      {
        UARTRxCount = 0;		/* buffer overflow */
		UARTBlockReceived = 1;
      }	
    }
  }
  else if (IIRValue == IIR_RDA)	/* Receive Data Available */
  {
    /* Receive Data Available */
    UARTRxBuffer[UARTRxCount++] = LPC_USART->RBR;
    if (UARTRxCount == BUFSIZE)
    {
      UARTRxCount = 0;		/* buffer overflow */
	  UARTBlockReceived = 1;
    }
  }
  else if (IIRValue == IIR_CTI)	/* Character timeout indicator */
  {
    /* Character Time-out indicator */
    UARTStatus[UARTStatusCount++] |= 0x100;		/* Bit 9 as the CTI error */
    if (UARTStatusCount == BUFSIZE)
    {
      UARTStatusCount = 0;		/* buffer overflow */
    }	
  }
  else if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    LSRValue = LPC_USART->LSR;		/* Check status in the LSR to see if
								valid data in U0THR or not */
    if (LSRValue & LSR_THRE)
    {
      UARTTxEmpty = 1;
    }
    else
    {
      UARTTxEmpty = 0;
    }
  }
  return;
}

/*****************************************************************************
** Function name:		USARTInit
**
** Descriptions:		Initialize UART0 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate, sync mode(1 is master, 0 is slave.)
** Returned value:		None
** 
*****************************************************************************/
void USARTInit(uint32_t baudrate, uint32_t sync_mode)
{
  uint32_t Fdiv;
  uint32_t regVal;

  UARTTxEmpty = 1;
  NVIC_DisableIRQ(UART_IRQn);

#if 1
  LPC_IOCON->PIO0_18 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO0_18 |= 0x01;     /* UART RXD */
  LPC_IOCON->PIO0_19 &= ~0x07;	
  LPC_IOCON->PIO0_19 |= 0x01;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_14 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_14 |= 0x03;     /* UART RXD */
  LPC_IOCON->PIO1_13 &= ~0x07;	
  LPC_IOCON->PIO1_13 |= 0x03;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_17 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_17 |= 0x02;     /* UART RXD */
  LPC_IOCON->PIO1_18 &= ~0x07;	
  LPC_IOCON->PIO1_18 |= 0x02;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_26 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_26 |= 0x02;     /* UART RXD */
  LPC_IOCON->PIO1_27 &= ~0x07;	
  LPC_IOCON->PIO1_27 |= 0x02;     /* UART TXD */
#endif

#if 1
  LPC_IOCON->PIO0_17 &= ~0x07;	
  LPC_IOCON->PIO0_17 |= 0x03;    /* UART SCLK */
#endif
#if 0
  LPC_IOCON->PIO1_28 &= ~0x07;	
  LPC_IOCON->PIO1_28 |= 0x02;    /* UART SCLK */
#endif

  /* Enable UART clock */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
  LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

  LPC_USART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
  regVal = LPC_SYSCON->UARTCLKDIV;
  Fdiv = ((SystemCoreClock/regVal)/2)/baudrate ;	/* baud rate */

  LPC_USART->DLM = Fdiv / 256;							
  LPC_USART->DLL = Fdiv % 256;
  LPC_USART->LCR = 0x03;		/* DLAB = 0 */
  LPC_USART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

  /* Read to clear the line status. */
  regVal = LPC_USART->LSR;

  /* Ensure a clean start, no data in either TX or RX FIFO. */
  while (( LPC_USART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
  while ( LPC_USART->LSR & LSR_RDR )
  {

	regVal = LPC_USART->RBR;	/* Dump data from RX FIFO */
  }

  /* Sync Mode Control configuration */
  if ( sync_mode )
  {
	/* Set to master mode. */
	LPC_USART->SYNCCTRL |= SYNCCTRL_CLKSEL;
  }
  else
  {
	/* Set to slave mode. */
	LPC_USART->SYNCCTRL &= ~SYNCCTRL_CLKSEL;
  }

#if 0
  /* Be very careful that, when NOSS is set(disabled), the rx data is always 
  sampling at the falling edge of the clock. */
  LPC_USART->SYNCCTRL |= SYNCCTRL_NOSS;
  if ( LPC_USART->SYNCCTRL & SYNCCTRL_NOSS )
  {
	LPC_USART->SYNCCTRL |= SYNCCTRL_FES;
  }  
#endif

#if 1
  /* Be very careful that, when NOSS is set(disabled), the rx data is always 
  sampling at the falling edge of the clock. */
  LPC_USART->SYNCCTRL |= SYNCCTRL_FES;
#endif

#if 0
  LPC_USART->SYNCCTRL |= SYNCCTRL_CSCEN;
#endif

#if 0
  LPC_USART->SYNCCTRL |= SYNCCTRL_CCCLR;
#endif

#if 0
  LPC_USART->SYNCCTRL |= SYNCCTRL_TSBPASS;
#endif

  /* Enable SYNC mode, a falling edge will be generated here on SYSC clock. */
  LPC_USART->SYNCCTRL |= SYNCCTRL_ENA;

  /* Enable the UART Interrupt */
  NVIC_EnableIRQ(UART_IRQn);

#if TX_INTERRUPT
  LPC_USART->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART interrupt */
#else
  LPC_USART->IER = IER_RBR | IER_RLS;	/* Enable UART interrupt */
#endif
  return;
}

/*****************************************************************************
** Function name:		USARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**				on the data length
**
** parameters:		buffer pointer, and data length
** Returned value:	None
** 
*****************************************************************************/
void USARTSend(uint8_t *BufferPtr, uint32_t Length)
{
  
  while ( Length != 0 )
  {
	/* THRE status, contain valid data */
#if !TX_INTERRUPT
	while ( !(LPC_USART->LSR & LSR_THRE) );
	LPC_USART->THR = *BufferPtr;
#else
	/* Below flag is set inside the interrupt handler when THRE occurs. */
    while ( !(UARTTxEmpty & 0x01) );
	LPC_USART->THR = *BufferPtr;
    UARTTxEmpty = 0;	/* not empty in the THR until it shifts out */
#endif
    BufferPtr++;
    Length--;
  }
  UARTTxEmpty = 1;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
