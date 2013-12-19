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
} USARTRing;

typedef struct {
	USART_TypeDef * USARTx;
	IRQn_Type irqn;
	USARTRing rxring, txring;
} usart;

extern usart stdserial;

#if defined (STDSERIAL) && defined(STM32F10X_MD)
#if (STDSERIAL == 2)
#define STDUSART 	USART2
#define STDRX 		PA2
#define STDTX 		PA1
//#warning "using STDSERIAL USART3 for STM32F3 Discovery"
#elif (STDSERIAL == 1)
#define STDUSART 	USART1
#define STDRX 		PA10
#define STDTX 		PA9
//#warning "using STDSERIAL USART1"
#endif
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */
