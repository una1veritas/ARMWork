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

#include "Print.h"

class USARTSerial : public Print {
	USART port;

public:
	void begin(USART_TypeDef * usart_id, GPIOPin rx, GPIOPin tx, const uint32_t baud);

	virtual size_t write(const uint8_t w);
	using Print::write;

	uint16_t read() { return usart_read(&port); }
	uint16_t available() { return usart_available(&port); }
	uint16_t peek() { return usart_peek(&port); }
	void flush() { usart_flush(&port); }

};


#endif /* USARTSERIAL_H_ */
