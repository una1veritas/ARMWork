/*
 * USARTSerial.h
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

#ifndef USARTSERIAL_H_
#define USARTSERIAL_H_

//#include <stdio.h>
//#include <stdint.h>

//#include <stm32f4xx_usart.h>

#include "armcmx.h"
#include "usart.h"
#include "Stream.h"

class USARTSerial : public Stream {
	USARTDef * port;
	GPIOPin pinrx, pintx, pincts, pindtr;

public:
	USARTSerial(USARTDef * usartx, GPIOPin rx, GPIOPin tx) {
		port = usartx;
		pinrx = rx;
		pintx = tx;
	}
	
	void begin(const uint32_t baud);

	virtual size_t write(const uint8_t w);
	using Print::write;

	virtual int16_t read() { return USART_read(port); }
	virtual size_t available() { return USART_available(port); }
	virtual int16_t peek() { return USART_peek(port); }
	virtual void flush() { USART_flush(port); }

};


extern USARTSerial Serial;

#endif /* USARTSERIAL_H_ */
