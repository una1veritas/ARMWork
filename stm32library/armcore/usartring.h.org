/*
 * usart.h
 *
 *  Created on: 2012/10/24
 *      Author: sin
 */

#ifndef USARTRING_H_
#define USARTRING_H_


#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USART_BUFFER_SIZE 128
typedef struct _USARTRing {
	uint16_t buf[USART_BUFFER_SIZE];
	int16_t head, tail;
	uint16_t count;
} USARTRing;

extern USARTRing rxring[6], txring[6];

void buffer_clear(USARTRing * r);
uint16_t buffer_count(USARTRing * r);
uint8_t buffer_is_full(USARTRing * r);
uint16_t buffer_enque(USARTRing * r, uint16_t w);
uint16_t buffer_deque(USARTRing * r);
uint16_t buffer_peek(USARTRing * r);

#ifdef __cplusplus
}
#endif

#endif /* USARTRING_H_ */
