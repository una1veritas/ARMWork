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
<<<<<<< HEAD
	SPI_InitTypeDef SPI_InitStruct;
	uint8 txrxbuf[256];
=======
	SPI_InitTypeDef InitStruct;
	uint8 xbuf[256];
>>>>>>> origin/master
	uint16 xindex;
} SPIBuffer;

void spi_begin(SPI_TypeDef * SPIx, SPIBuffer * spi, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
void spi_transfer(SPIBuffer * spi, uint8_t * data, uint16_t nbytes);
<<<<<<< HEAD
=======
void spi_setModes(SPIBuffer * spi, uint16 clkdiv, uint16 cpol, uint16 cpha, uint16 msbfirst);
>>>>>>> origin/master

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
