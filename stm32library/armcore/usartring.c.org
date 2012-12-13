/*
 * usart.c
 *
 *  Created on: 2012/10/24
 *      Author: sin
 */

#include "usartring.h"

USARTRing rxring[6], txring[6];

void buffer_clear(USARTRing * r) {
	r->head = 0;
	r->tail = 0;
	r->count = 0;
}

uint16_t buffer_count(USARTRing * r) {
	return r->count;
}

uint8_t buffer_is_full(USARTRing * r) {
	if ((r->head == r->tail) && (r->count > 0)) {
		return 1;
	}
	return 0;
}

uint16_t buffer_enque(USARTRing * r, uint16_t w) {
	if (buffer_is_full(r))
		return 0xffff;
	r->buf[r->head++] = w;
	r->count++;
	r->head %= USART_BUFFER_SIZE;
	return w;
}

uint16_t buffer_deque(USARTRing * r) {
	uint16_t w;
	if (buffer_count(r) == 0)
		return 0xffff;
	w = r->buf[r->tail++];
	r->count--;
	r->tail %= USART_BUFFER_SIZE;
	return w;
}

uint16_t buffer_peek(USARTRing * r) {
	if (buffer_count(r) == 0)
		return 0xffff;
	return r->buf[r->tail];
}
