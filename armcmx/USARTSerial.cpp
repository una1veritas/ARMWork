/*
 * USARTSerial.cpp
 *  for LPC11Uxx
 *
 *  Created on: 2012/12/14
 *      Author: Sin
 */

//#include <math.h>

#include "USARTSerial.h"

//USARTSerial Serial = USARTSerial(&usart, PIO1_26, PIO1_27);

void USARTSerial::begin(const uint32_t baud) {
	usart_init(port, USARTx, pinrx, pintx);
  usart_begin(port, baud);
}

size_t USARTSerial::write(const uint8_t w) {
	return usart_write(port, w);
}
