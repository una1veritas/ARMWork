/*
 * USARTSerial.cpp
 *  for LPC11Uxx
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

//#include <math.h>

#include "USARTSerial.h"

#ifdef STDSERIAL
USARTSerial Serial = USARTSerial(&stdserial);
#endif

void USARTSerial::begin(const uint32_t baud) {
	usart_init(port);
	port->baud = baud;
  usart_begin(port, port->baud);
}

size_t USARTSerial::write(const uint8_t w) {
	return usart_write(port, w);
}
