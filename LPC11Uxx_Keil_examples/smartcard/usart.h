/****************************************************************************
 *   $Id:: usart.h 5143 2010-10-07 23:06:18Z usb00423                       $
 *   Project: NXP LPC11xx software example
 *
 *   Description:
 *     This file contains definition and prototype for UART configuration.
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
****************************************************************************/
#ifndef __USART_H 
#define __USART_H

#include "config.h"

#ifndef _BV
#define _BV(_x_) (1UL << (_x_))
#endif

/* default is master mode, to test slave mode, turn off below flag */
#define MASTER_MODE                         (1)

/* TX is interrupt driven or polling. */
#ifndef TX_INTERRUPT
#define TX_INTERRUPT                        (0)
#endif

#define USART_DEBUG                         (0)

#define USART_BUFSIZE                       (64)  /* Size in bytes for RX and TX buffers */

#ifndef IER_RBR
#define IER_RBR                             _BV(0)
#endif
#ifndef IER_THRE
#define IER_THRE                            _BV(1)
#endif
#ifndef IER_RLS
#define IER_RLS                             _BV(2)
#endif

#ifndef IIR_PEND
#define IIR_PEND                            (0x01)
#endif
#ifndef IIR_RLS
#define IIR_RLS                             (0x03)
#endif
#ifndef IIR_RDA
#define IIR_RDA                             (0x02)
#endif
#ifndef IIR_CTI
#define IIR_CTI                             (0x06)
#endif
#ifndef IIR_THRE
#define IIR_THRE                            (0x01)
#endif

#ifndef LSR_RDR
#define LSR_RDR                             _BV(0)
#endif
#ifndef LSR_OE
#define LSR_OE                              _BV(1)
#endif
#ifndef LSR_PE
#define LSR_PE                              _BV(2)
#endif
#ifndef LSR_FE
#define LSR_FE                              _BV(3)
#endif
#ifndef LSR_BI
#define LSR_BI                              _BV(4)
#endif
#ifndef LSR_THRE
#define LSR_THRE                            _BV(5)
#endif
#ifndef LSR_TEMT
#define LSR_TEMT                            _BV(6)
#endif
#ifndef LSR_RXFE
#define LSR_RXFE                            _BV(7)
#endif
#ifndef LSR_TXE
#define LSR_TXE                             _BV(8)
#endif

/* Synchronous mode control register definition. */
#ifndef SYNC_ON
#define SYNC_ON                             _BV(0)
#endif
#ifndef SYNC_OFF
#define SYNC_OFF                            (0)
#endif

#ifndef SYNC_MASTER
#define SYNC_MASTER                         _BV(1)
#endif
#ifndef SYNC_SLAVE
#define SYNC_SLAVE                          (0)
#endif

#ifndef SYNC_RE
#define SYNC_RE                             (0)
#endif
#ifndef SYNC_FE
#define SYNC_FE                             _BV(2)
#endif

#define SYNC_CONT_CLK_EN                    _BV(4)
#define SYNC_CONT_CLK_DIS                   (0)

#define SYNC_STARTSTOPOFF                   _BV(5)
#define SYNC_STARTSTOPON                    (0)

#define SYNC_CON_CLK_CLR                    _BV(6)

enum
{
  PARITY_NONE = 0,
  PARITY_ODD,
  PARITY_EVEN,
  PARITY_FORCE_1,
  PARITY_FORCE_0,
};

void delay(uint32_t count);
uint32_t USARTConfig(uint32_t baudrate, int databits, int parity, int guardbits, int retries);
void USARTInit( uint32_t baudrate );
void USARTStart( uint8_t SScfg, uint8_t contclk, uint8_t edge, uint8_t mode );
void USARTStop( void );
void USARTSend( uint8_t *BufferPtr, uint32_t Length );

extern volatile uint32_t USARTStatus;
extern volatile uint32_t USARTDisregard;
extern volatile uint8_t  USARTTxEmpty;
extern volatile uint8_t  USARTBuffer[USART_BUFSIZE];
extern volatile uint32_t USARTCount;
extern volatile uint32_t USARTErrorCount;
extern volatile uint32_t USARTOverflow;
extern volatile uint32_t USARTRetryError;

static __INLINE USARTClear(void)
{
  USARTStatus = 0;
  USARTCount = 0;
  USARTErrorCount = 0;
  USARTOverflow = 0;
  USARTRetryError = 0;
}

static __INLINE USARTIgnore(void) { USARTDisregard = 1; }
static __INLINE USARTAccept(void) { USARTDisregard = 0; }

static __INLINE USARTNackEnable(void)
{ 
  LPC_USART->SCICTRL &= ~_BV(1);
}

static __INLINE USARTNackDisable(void)
{ 
  LPC_USART->SCICTRL |= _BV(1);
}

static __INLINE USARTProtocolT0(void)
{ 
  LPC_USART->SCICTRL &= ~_BV(2);
}

static __INLINE USARTProtocolT1(void)
{ 
  LPC_USART->SCICTRL |= _BV(2);
}


#endif /* end __USART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

