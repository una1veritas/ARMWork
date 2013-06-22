/*
 * USARTSerial.cpp
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

//#include <math.h>

#include "USARTSerial.h"

void USARTSerial::begin(const uint32_t baud) {
	USART_init(port, pinrx, pintx);
  USART_begin(port, baud);
}

size_t USARTSerial::write(const uint8_t w) {
	return USART_write(port, w);
}
