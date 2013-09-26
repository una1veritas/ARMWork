/****************************************************************************
 *   $Id:: uart.h 9155 2012-02-14 23:23:34Z nxp41306                        $
 *   Project: NXP LPC11Uxx software example
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
#ifndef __UART_H 
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "gpio.h"

/* Synchronous mode control register definition. */
#define SYNC_ON             (0x1<<0)
#define SYNC_OFF            (0x0<<0)

#define SYNC_MASTER         (0x1<<1)
#define SYNC_SLAVE          (0x0<<1)

#define SYNC_RE             (0x0<<2)
#define SYNC_FE             (0x1<<2)

#define SYNC_CONT_CLK_EN    (0x1<<4)
#define SYNC_CONT_CLK_DIS   (0x0<<4)

#define SYNC_STARTSTOPOFF   (0x1<<5)
#define SYNC_STARTSTOPON    (0x0<<5)

#define SYNC_CON_CLK_CLR    (0x1<<6)

/* ------------------------------------------ */

#define AUTOBAUD_ENABLE 0
#define FDR_CALIBRATION 0
#define RS485_ENABLED   0
#define TX_INTERRUPT    0		/* 0 if TX uses polling, 1 interrupt driven. */
#define MODEM_TEST      0

#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)
#define IER_ABEO        (0x01<<8)
#define IER_ABTO        (0x01<<9)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01
#define IIR_ABEO        (0x01<<8)
#define IIR_ABTO        (0x01<<9)

#define LSR_RDR         (0x01<<0)
#define LSR_OE          (0x01<<1)
#define LSR_PE          (0x01<<2)
#define LSR_FE          (0x01<<3)
#define LSR_BI          (0x01<<4)
#define LSR_THRE        (0x01<<5)
#define LSR_TEMT        (0x01<<6)
#define LSR_RXFE        (0x01<<7)

#define USART_BUFSIZE         0x40

/* RS485 mode definition. */
#define RS485_NMMEN		(0x1<<0)
#define RS485_RXDIS		(0x1<<1)
#define RS485_AADEN		(0x1<<2)
#define RS485_SEL     (0x1<<3)
#define RS485_DCTRL		(0x1<<4)
#define RS485_OINV		(0x1<<5)

typedef struct {
  volatile uint32_t Status;
  volatile uint8_t  TxEmpty; // = 1;
  volatile uint8_t  Buffer[USART_BUFSIZE];
  volatile uint16_t Count; // = 0;
  volatile uint16_t Tail; //

  #if AUTOBAUD_ENABLE
  volatile uint32_t AutoBaud = 0, 
  volatile uint32_t AutoBaudTimeout = 0;
  #endif
} USARTDef;

// Global, unique for M0
extern USARTDef usart;

// original irq handler
void UART_IRQHandler(void);

//void ModemInit( void );
void USART_init(USARTDef *, const GPIOPin rx, const GPIOPin tx);
void USART_begin(USARTDef *, const uint32_t baudrate);
void USART_close(USARTDef *);
size_t USART_write(USARTDef * usart, uint8_t c);
size_t USART_puts(USARTDef *, const char *);
size_t USART_available(USARTDef * usart);
int16_t USART_read(USARTDef *);
size_t USART_gets(USARTDef * uart, char * buf);
size_t USART_polling_write(USARTDef * usart, uint8_t c );
int16_t USART_polling_read(USARTDef *);
uint16_t USART_linestate(USARTDef *);


void USART_flush(USARTDef *);
int16_t USART_peek(USARTDef *);

/* stm32 versioin
void usart_init(usart * usx, USART_TypeDef * usartx, const GPIOPin rx, const GPIOPin tx);
void usart_begin(usart * usx, const uint32_t baud);
size_t usart_polling_write(usart * usx, const uint16_t w);
size_t usart_write(usart * usx, const uint16_t w);
size_t usart_polling_print(usart * usx, const char * s);
size_t usart_print(usart * usx, const char * s);
uint16_t usart_polling_read(usart * usx);
uint16_t usart_read(usart * usx);
uint16_t usart_available(usart * usx);
void usart_flush(usart * usx);
uint16_t usart_peek(usart * usx);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void UART4_IRQHandler(void);
void UART5_IRQHandler(void);
void USART6_IRQHandler(void);
*/

#ifdef __cplusplus
}
#endif

#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
