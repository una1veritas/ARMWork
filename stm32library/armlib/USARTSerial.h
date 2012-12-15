/*
 * USARTSerial.h
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

#ifndef USARTSERIAL_H_
#define USARTSERIAL_H_

#include <stdio.h>
#include <stdint.h>

#include <stm32f4xx_usart.h>

#include "armcore.h"
#include "usart.h"

class USARTSerial {
	USART port;

public:
	void begin(USART_TypeDef * usart_id, GPIOPin rx, GPIOPin tx, const uint32_t baud);

	size_t write(const uint8_t w);
	inline size_t print(const char c) {
		return write(c);
	}
	size_t print(const char * str) {
		size_t n = 0;
		while (str[n])
			write(str[n++]);
		return n;
	}
	size_t print(const uint32_t val, uint8_t base = DEC) {
		char tmp[12];
		switch (base) {
		case DEC:
		default:
		sprintf(tmp, "%ld", val);
		}
		return print(tmp);
	}
	size_t print(const int32_t val) {
		size_t s = 0;
		if (val < 0)
			s = print('-');
		return print((uint32_t)abs(val)) + s;
	}

	void flush() {
		usart_flush(&port);
	}

};


#endif /* USARTSERIAL_H_ */
