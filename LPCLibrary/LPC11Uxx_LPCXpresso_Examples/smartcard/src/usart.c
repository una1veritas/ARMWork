/****************************************************************************
 *   $Id:: usart.c 5143 2010-10-07 23:06:18Z usb00423                       $
 *   Project: NXP LPC11Uxx Blinky example
 *
 *   Description:
 *     This file contains USART code example for smart card which include 
 *     USART initialization, USART interrupt handler, and related APIs for 
 *     smart card access.
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

volatile uint32_t USARTStatus;
volatile uint32_t USARTDisregard = 0;
volatile uint8_t  USARTTxEmpty = 1;
volatile uint8_t  USARTBuffer[USART_BUFSIZE];
volatile uint32_t USARTCount = 0;
volatile uint32_t USARTErrorCount = 0;
volatile uint32_t USARTOverflow = 0;
volatile uint32_t USARTRetryError = 0;

volatile uint32_t USARTIsrCount = 0;
volatile uint32_t USARTIsrRlsCount = 0;

/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART_IRQHandler (void) 
{
  uint32_t IIRValue, LSRValue;
  uint32_t IIR;
  volatile uint8_t Dummy;
	
  IIR = LPC_USART->IIR;
  IIRValue = (IIR >> 1) & 0x7; /* check bit 1~3, interrupt identification */
  LSRValue = LPC_USART->LSR;

  USARTIsrCount++;

  if (IIR & 1)
    Dummy = LPC_USART->IIR;

  switch (IIRValue)
  {
    case IIR_RLS:
      USARTIsrRlsCount++;
      /* Look for TX retry errors */
      if (LSRValue & LSR_TXE)
      {
        LPC_USART->FCR |= (1UL << 2); /* Reset TX FIFO. */
        USARTRetryError = 1;
        USARTStatus |= LSRValue;
      }
      /* Look for overrun, parity, framing, and break interrupt errors */
      if (LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI))
      {
        /* There are errors or break interrupt */
        /* Read LSR will clear the interrupt */
        if (!USARTDisregard)
        {
          USARTStatus |= LSRValue;
          USARTErrorCount++;
        }
        Dummy = LPC_USART->RBR;	/* Dummy read on RX to clear error */
        LSRValue = LPC_USART->LSR;
      }
      break;

    case IIR_RDA:
      /* fall through */

    case IIR_CTI:
      while (LSRValue & LSR_RDR)
      {
        if (USARTDisregard)
        {
          Dummy = LPC_USART->RBR;
          LSRValue = LPC_USART->LSR;
          continue;
        }
    
        if (USARTCount < USART_BUFSIZE)
          USARTBuffer[USARTCount++] = LPC_USART->RBR;
        else
          Dummy = LPC_USART->RBR;	/* Dummy read on RX to clear */
        LSRValue = LPC_USART->LSR;
        if (USARTCount >= USART_BUFSIZE)
        {
          USARTOverflow++;
          USARTCount = USART_BUFSIZE - 1; /* buffer overflow */
        }
      }
      break;

    case IIR_THRE:
      /* THRE interrupt */
      if (LSRValue & LSR_THRE)
        USARTTxEmpty = 1;
      else
        USARTTxEmpty = 0;

      break;

    default:
      break;

  }
      
  return;
}

/*****************************************************************************
** Function name:		delay
**
** Descriptions:		SHort delay
**
** parameters:			delay length
** Returned value:		None
** 
*****************************************************************************/
void delay(uint32_t count)
{
  while(count != 0) count--;
  return;
}

/*****************************************************************************
** Function name:		USARTConfig
**
** Descriptions:		Configures Smart Card USART without full initialization
**
** parameters:			baudrate - bit rate for receive and transmit
**                  databits - number of data bits to receive and transmit
**                  parity - none, even, odd, force 0, and force 1 enumerations
**                  guardbits - number of additional idle bits transmitted
**                  retries - number of times transmitter retries when receiving NACK
** Returned value:  Always 0
** 
*****************************************************************************/
uint32_t USARTConfig(uint32_t baudrate, int databits, int parity, int guardbits, int retries)
{
  uint32_t temp = 0;

  NVIC_DisableIRQ(UART_IRQn);
  
  switch (parity)
  {
    case PARITY_ODD:      temp = _BV(3) | (0x0UL << 4); break;
    case PARITY_EVEN:     temp = _BV(3) | (0x1UL << 4); break;
    case PARITY_FORCE_1:  temp = _BV(3) | (0x2UL << 4); break;
    case PARITY_FORCE_0:  temp = _BV(3) | (0x3UL << 4); break;
  }

  temp |= (databits - 1) & 0x3; /* databits */

  LPC_USART->LCR = temp | _BV(7);

  /* Calculate clock divider for given baudrate.  Round the number by (temp * 2 + 1) / 2 */
  temp = ( SystemCoreClock / (SC_OVERSAMPLE / 2) ) / baudrate ;	/* baud rate */
  temp = (temp + 1) >> 1;
  LPC_USART->DLM = (temp >> 8) & 0xff;
  LPC_USART->DLL = temp & 0xff;
  LPC_USART->OSR = (SC_OVERSAMPLE - 1) << 4;
  LPC_USART->LCR &= ~(1 << 7);		/* DLAB = 0 */
  LPC_USART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */
  LPC_USART->SCICTRL &= ~((0xffUL << 8) | (0x3UL << 5)); /* clear guard bits and retries */
  LPC_USART->SCICTRL |=
    ((guardbits & 0xff) << 8) | /* guard bits */
    ((retries & 0x7) << 5);       /* retries */

  NVIC_EnableIRQ(UART_IRQn);
  return 0;
}

/*****************************************************************************
** Function name:		USARTInit
**
** Descriptions:		Initialize USART port, setup pin select,
**						clock, parity, stop bits, FIFO, etc.
**
** parameters:			USART baudrate
** Returned value:		None
** 
*****************************************************************************/
void USARTInit( uint32_t baudrate )
{
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
  LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */
#if 1
  LPC_IOCON->PIO0_19 &= ~0x07;	
  LPC_IOCON->PIO0_19 |= 0x01;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_13 &= ~0x07;	
  LPC_IOCON->PIO1_13 |= 0x03;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_18 &= ~0x07;	
  LPC_IOCON->PIO1_18 |= 0x02;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_27 &= ~0x07;	
  LPC_IOCON->PIO1_27 |= 0x02;     /* UART TXD */
#endif

#if 1
  LPC_IOCON->PIO0_17 &= ~0x07;	
//  LPC_IOCON->PIO0_17 |= 0x03;     /* UART SCLK */
#endif

  LPC_USART->SCICTRL = _BV(0); /* Enable Smart Card, default values for everything else */

  /* Default configuration is 8E1 with no retries, T=0, and NACL */
  USARTConfig(baudrate, 8, PARITY_EVEN, 1, 0);

#if TX_INTERRUPT
  LPC_USART->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART interrupt */
#else
  LPC_USART->IER = IER_RBR | IER_RLS;	/* Enable UART interrupt */
#endif
  return;
}

/*****************************************************************************
** Function name:		USARTStart
**
** Descriptions:		Configure USART then start.
**
** parameters:			SSdis:    1 is DONT Send Start and Stop bits, 0 is DO Send
**						Start and Stop Bits.
**						cscen:    1 is continuous clock enabled, 0 is disabled.
**                      fes:      edge sampling: 1 is raising, 0 is falling
**                      csrc:     clock source: 1 is master, 0 is slave
** Returned value:		None
** 
*****************************************************************************/
void USARTStart( uint8_t SSdis, uint8_t cscen, uint8_t fes, uint8_t csrc )
{
  LPC_USART->SYNCCTRL = 0;

  if ( SSdis )
  {
	/* Dont Send Start and Stops bits. */
    LPC_USART->SYNCCTRL |= SYNC_STARTSTOPOFF; 
  }
  else
  {
	/* Send Start and Stops bits at default. */
    LPC_USART->SYNCCTRL &= ~SYNC_STARTSTOPOFF;
  }

  if ( cscen )
  {
	/* Continuous serial clock is enabled. */
    LPC_USART->SYNCCTRL |= SYNC_CONT_CLK_EN;
  } 
  else
  {
	/* Continuous serial clock is disabled, at default. */
    LPC_USART->SYNCCTRL &= ~SYNC_CONT_CLK_EN;
  }

  if ( fes )
  {
	/* Falling edge */
    LPC_USART->SYNCCTRL |= SYNC_FE;
  } 
  else
  {
	if ( SSdis )
	{
	  /* Falling edge or H/W will overwrite internally when SSdis is set. */
	  LPC_USART->SYNCCTRL |= SYNC_FE;
	}
	else
	{
	  /* Raising edge */
	  LPC_USART->SYNCCTRL &= ~SYNC_FE;
	}
  }

  if ( csrc )
    LPC_USART->SYNCCTRL |= SYNC_MASTER; 
  else
    LPC_USART->SYNCCTRL &= ~SYNC_MASTER;

  LPC_USART->SYNCCTRL |= SYNC_ON; //turn the synchron mode on
  return;
}

/*****************************************************************************
** Function name:		USARTStop
**
** Descriptions:		Stop USART
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void USARTStop( void )
{
#if MASTER_MODE
#if USART_DEBUG
  LPC_GPIO0->SET = 0x1<<1;
  delay(20);
  LPC_GPIO0->CLR = 0x1<<1;
#endif
  while ( !(LPC_USART->LSR & (LSR_TEMT)) );
#if USART_DEBUG
  LPC_GPIO0->SET = 0x1<<2;
  delay(20);
  LPC_GPIO0->CLR = 0x1<<2;
#endif
#endif
  LPC_USART->SYNCCTRL = SYNC_OFF; //turn-off the synchro mode
}
  
/*****************************************************************************
** Function name:		USARTSend
**
** Descriptions:		Send a block of data to the UART port based
**						on the data length
**
** parameters:			buffer pointer, and data length
** Returned value:		None
** 
*****************************************************************************/
void USARTSend( uint8_t *BufferPtr, uint32_t Length )
{
//  USARTIgnore();
  while ( Length != 0 )
  {
#if TX_INTERRUPT
    while ( !(USARTTxEmpty & 0x01) )
    {
      /* There may be a retry failure if the transmitter receives NACK signals */
      if (USARTStatus & LSR_TXE)
        goto USARTSend_Exit;
    }	
  	LPC_USART->THR = *BufferPtr;
  	USARTTxEmpty = 0;	/* not empty in the THR until it shifts out */
#else
  	/* THRE status, contain valid data */
  	while ( !(LPC_USART->LSR & (LSR_THRE)) )
    {
      /* There may be a retry failure if the transmitter receives NACK signals */
      if (USARTRetryError)
//      if (USARTStatus & LSR_TXE)
        goto USARTSend_Exit;
    }
  	LPC_USART->THR = *BufferPtr;
#endif
  	BufferPtr++;
  	Length--;
  }

USARTSend_Exit:
//  USARTAccept();
  return;
}

