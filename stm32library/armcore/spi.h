/*
 * spi.h
 *
 *  Created on: 2012/11/18
 *      Author: sin
 */

#ifndef SPI_H_
#define SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "armcore.h"
#include "gpio.h"

typedef struct _SPIBuffer {
	SPI_TypeDef * SPIx;
	SPI_InitTypeDef SPI_InitStruct;
	uint8 txrxbuf[256];
	uint16 xindex;
} SPIBuffer;

void spi_begin(SPI_TypeDef * SPIx, SPIBuffer * spi, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
void spi_transfer(SPIBuffer * spi, uint8_t * data, uint16_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
