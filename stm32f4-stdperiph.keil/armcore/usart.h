/*
 * usart.h
 *
 *  Created on: 2012/10/24
 *      Author: sin
 */

#ifndef USART_H_
#define USART_H_

#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx_usart.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"

#define USART_BUFFER_SIZE 128
typedef struct {
	uint16_t buf[USART_BUFFER_SIZE];
	int16_t head, tail;
	uint16_t count;
} USARTRing;

typedef struct _USART {
	uint8_t usid;
	USART_TypeDef * USARTx;
	USARTRing * rxring, * txring;
} USART;


/*
PB6 			USART1_TX
PB7			USART1_RX
PA9 			USART1_TX
PA10 		USART1_RX
PD5 			USART2_TX
PD6 			USART2_RX
PA2			USART2_TX
PA3			USART2_RX
PB10 		USART3_TX
PB11		USART3_RX
PD8 			USART3_TX
PD9 			USART3_RX
PC10 		USART3_TX
PC11		USART3_RX
PA0-WKUP	USART4_TX
PA1			USART4_RX
PC12		UART5_TX
PD2 			UART5_RX
PG9			USART6_RX
PG14 		USART6_TX
PC6 			USART6_TX
PC7 			USART6_RX
*/

void usart_begin(USART_TypeDef * usartx, USART * usx, GPIOPin rx, GPIOPin tx, const uint32_t baud);
size_t usart_write(USART * usx, const uint16_t w);
//size_t usart_print(USART * usx, const char * s);
uint16_t usart_read(USART * usx);
uint16_t usart_available(USART * usx);
void usart_flush(USART * usx);
uint16_t usart_peek(USART * usx);

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
