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
	usart * port;
	GPIOPin pinrx, pintx, pincts, pindtr;
  USART_TypeDef * USARTx;

public:
	USARTSerial(usart * usartx, USART_TypeDef * x, GPIOPin rx, GPIOPin tx) {
		port = usartx;
    USARTx = x;
		pinrx = rx;
		pintx = tx;
	}
	
  void pinconfig(const GPIOPin rx, GPIOPin tx) { pinrx = rx; pintx = tx; }
  
	void begin(const uint32_t baud);
	void begin(const uint32_t baud, GPIOPin rx, GPIOPin tx) { pinconfig(rx, tx); begin(baud); }

	virtual size_t write(const uint8_t w);
	using Stream::write;

	virtual int read() { return usart_read(port); }
	virtual int available() { return usart_available(port); }
	virtual int peek() { return usart_peek(port); }
	virtual void flush() { usart_flush(port); }

};


//extern USARTSerial Serial;

#endif /* USARTSERIAL_H_ */
