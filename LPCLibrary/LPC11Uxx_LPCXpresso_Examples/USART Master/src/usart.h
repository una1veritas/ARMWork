/****************************************************************************
 *   $Id:: usart.h 4001 2010-07-28 20:41:55Z usb00423                       $
 *   Project: NXP LPC11Uxx software example
 *
 *   Description:
 *     This file contains definition and prototype for USART configuration.
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

#define RS485_ENABLED   0
#define TX_INTERRUPT    0		/* 0 if TX uses polling, 1 interrupt driven. */
#define MODEM_TEST      0

#define SLAVE_MODE		0
#define MASTER_MODE		1

#define BUFSIZE         0x40

#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)
#define IER_MS          (0x01<<3)
#define IER_CTS         (0x01<<7)
#define IER_ABEO        (0x01<<8)
#define IER_ABTO        (0x01<<9)
#define IER_WAKEUP      (0x01<<10)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01
#define IIR_MODEM       0x00	/* Low */

#define LSR_RDR         (0x01<<0)
#define LSR_OE          (0x01<<1)
#define LSR_PE          (0x01<<2)
#define LSR_FE          (0x01<<3)
#define LSR_BI          (0x01<<4)
#define LSR_THRE        (0x01<<5)
#define LSR_TEMT        (0x01<<6)
#define LSR_RXFE        (0x01<<7)	/* bit definition on LPC11xx, UART is used. */
#define LSR_RXErr       (0x01<<7)   /* bit definition on LPC11Uxx, USART is used. */
#define LSR_TXErr       (0x01<<8)

/* Mode Control definition. */
#define MCR_DTR         (0x01<<0)
#define MCR_RTS         (0x01<<1)
#define MCR_OUT1        (0x01<<2)
#define MCR_OUT2        (0x01<<3)
#define MCR_LPBK        (0x01<<4)
#define MCR_RTSE        (0x01<<6)
#define MCR_CTSE        (0x01<<7)

/* Mode status definition. */
#define MSR_DTCS        (0x01<<0)
#define MSR_DDSR        (0x01<<1)
#define MSR_TERI        (0x01<<2)
#define MSR_DDCD        (0x01<<3)
#define MSR_CTS         (0x01<<4)
#define MSR_DSR         (0x01<<5)
#define MSR_RI          (0x01<<6)
#define MSR_DCD         (0x01<<7)

/* Auto=baud Control(ACR) definition. */
#define ACR_START       (0x01<<0)
#define ACR_MODE        (0x01<<1)
#define ACR_ASTART      (0x01<<2)
#define ACR_ABEO        (0x01<<8)
#define ACR_ABTO        (0x01<<9)

/* IrDA Control(ICR) definition. */
#define ICR_ENA         (0x01<<0)
#define ICR_INV         (0x01<<1)
#define ICR_FIXPUL      (0x01<<2)
#define ICR_PDIV_MASK   (0x07<<3)

/* Smart Card Interface Control(SCICTRL) definition. */
#define SCICTRL_ENA         (0x01<<0)
#define SCICTRL_NACK        (0x01<<1)
#define SCICTRL_PROTOCOL    (0x01<<2)
#define SCICTRL_RETRY_MASK  (0x07<<5)
#define SCICTRL_GTIME_MASK  (0xFF<<8)

/* RS485 mode definition. */
#define RS485_NMMEN		(0x01<<0)
#define RS485_RXDIS		(0x01<<1)
#define RS485_AADEN		(0x01<<2)
#define RS485_SEL       (0x01<<3)
#define RS485_DCTRL		(0x01<<4)
#define RS485_OINV		(0x01<<5)

/* Syncronous Mode Control(SYNCCTRL) definition. */
#define SYNCCTRL_ENA         (0x01<<0)
#define SYNCCTRL_CLKSEL      (0x01<<1)
#define SYNCCTRL_FES         (0x01<<2)
#define SYNCCTRL_TSBPASS     (0x01<<3)
#define SYNCCTRL_CSCEN       (0x01<<4)
#define SYNCCTRL_NOSS        (0x01<<5)
#define SYNCCTRL_CCCLR       (0x01<<6)

/* Interrupt enable(Set/Clear), status share the definition below. */
#define INTS_DCTS            (0x01<<0)
#define INTS_DDSR            (0x01<<1)
#define INTS_TER             (0x01<<2)
#define INTS_DDCD            (0x01<<3)
#define INTS_THRE            (0x01<<4)
#define INTS_RXTO            (0x01<<5)
#define INTS_RXDA            (0x01<<6)
#define INTS_WAKEUP          (0x01<<7)

#define INTS_ABEO            (0x01<<8)
#define INTS_ABTO            (0x01<<10)
#define INTS_TXErr           (0x01<<11)
#define INTS_BI              (0x01<<12)
#define INTS_FE              (0x01<<13)
#define INTS_PE              (0x01<<14)
#define INTS_OE              (0x01<<15)


void ModemInit( void );
void USARTInit(uint32_t Baudrate, uint32_t sync_mode);
void UART_IRQHandler(void);
void USARTSend(uint8_t *BufferPtr, uint32_t Length);

#endif /* end __USART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
