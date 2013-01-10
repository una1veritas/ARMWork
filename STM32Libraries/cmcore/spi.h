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

#define SPI_CLOCK_DIV2 SPI_BaudRatePrescaler_2
#define SPI_CLOCK_DIV4 	SPI_BaudRatePrescaler_4
#define SPI_CLOCK_DIV8 SPI_BaudRatePrescaler_8
#define SPI_CLOCK_DIV16 SPI_BaudRatePrescaler_16
#define SPI_CLOCK_DIV32 SPI_BaudRatePrescaler_32
#define SPI_CLOCK_DIV64 SPI_BaudRatePrescaler_64
#define SPI_CLOCK_DIV128 SPI_BaudRatePrescaler_128
#define SPI_CLOCK_DIV256 SPI_BaudRatePrescaler_256

// CPOL:CPHA
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define SPI_MSBFIRST 0
#define SPI_LSBFIRST 1

typedef struct _SPIBuffer {
	SPI_TypeDef * SPIx;
	SPI_InitTypeDef initStruct;
	uint8 xbuf[256];
	uint16 xindex;
} SPIBuffer;

void spi_begin(SPIBuffer * spi, SPI_TypeDef * SPIx, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
uint16 spi_transfer(SPIBuffer * spi, uint16 data);
void spi_setDataMode(SPIBuffer * spi, uint16 modeid);
void spi_setMode(SPIBuffer * spi, uint16 clkdiv, uint16 cpol, uint16 cpha, uint16 msbfirst);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
