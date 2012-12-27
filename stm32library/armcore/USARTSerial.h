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

#include "Stream.h"

class USARTSerial : public Stream {
	USART port;

public:
	USARTSerial(USART_TypeDef * usartx) {
		port.USARTx = usartx;
	}
	
	void begin(const GPIOPin rx, const GPIOPin tx, const uint32_t baud);

	virtual size_t write(const uint8_t w);
	using Print::write;

	virtual int16_t read() { return usart_read(&port); }
	virtual size_t available() { return usart_available(&port); }
	virtual int16_t peek() { return usart_peek(&port); }
	virtual void flush() { usart_flush(&port); }

};


#endif /* USARTSERIAL_H_ */
