/*
 * USARTSerial.cpp
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

#include <math.h>

#include "USARTSerial.h"

void USARTSerial::begin(const GPIOPin rx, const GPIOPin tx, const uint32_t baud) {
	usart_begin(&port, rx, tx, baud);
}

size_t USARTSerial::write(const uint8_t w) {
	return usart_write(&port, w);
}
