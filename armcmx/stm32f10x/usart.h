/*
 * usartport.h
 *
 *  Created on: 2012/10/24
 *      Author: sin
 */

#ifndef _USARTPORT_H_
#define _USARTPORT_H_

#include <stdio.h>
#include <stdlib.h>

#include <stm32f10x_usart.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "armcmx.h"

#define USART_BUFFER_SIZE 128
typedef struct {
	uint16_t buf[USART_BUFFER_SIZE];
	__IO int16_t head, tail;
	__IO uint16_t count;
} USARTRingBuffer;

/*
  USART_InitStructure.USART_BaudRate = 230400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
*/

#define WORDLENGTH_8BIT (0x00<<7)
#define WORDLENGTH_9BIT (0x01<<7)
#define STOPBITS_1  0x00
#define STOPBITS_05 0x01
#define STOPBITS_2  0x02
#define STOPBITS_15 0x03
#define PARITY_NONE (0x00<<2)
#define PARITY_EVEN (0x01<<2)
#define PARITY_ODD  (0x02<<2)
#define FLOW_NONE (0x00<<4)
#define FLOW_RTS (0x01<<4)
#define FLOW_CTS (0x02<<4)

typedef struct {
	USART_TypeDef * USARTx;
	GPIOPin rx, tx;
	uint32_t baud;
	uint8_t mode;
	//
	IRQn_Type irqn;
	USARTRingBuffer rxring, txring;
} usart;


void usart_init(usart * usx);
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

#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */
