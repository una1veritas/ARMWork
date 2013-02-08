/*
 * spi.h
 *
 *  Created on: 2012/11/18
 *      Author: sin
 */

#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdio.h>
#include <stdint.h>
#include "cmcore.h"

#include <stm32f4xx_SPI.h>

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

#define SPI_MODE_Low1Edge 0
#define SPI_MODE_Low2Edge 1
#define SPI_MODE_High1Edge 2
#define SPI_MODE_High2Edge 3

#define SPI_MSBFIRST 0
#define SPI_LSBFIRST 1

typedef struct {
	SPI_TypeDef * SPIx;
	GPIOPin sck, mosi, miso, nss;
	SPI_InitTypeDef modedef;
//	uint8 xbuf[256];
//	uint16 xindex;
} spi;

void spi_init(spi * port, SPI_TypeDef * SPIx, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
void spi_begin(spi * port);
void spi_disable(spi *);
uint16 spi_transfer(spi * port, uint16 data);
void spi_setDataMode(spi * port, uint16 id);
void spi_setClockDivier(spi * port, uint16 id);
void spi_setBitOrder(spi * port, uint16 id);
void spi_setMode(spi * port, uint16 clkdiv, uint16 cpol, uint16 cpha, uint16 firstbit);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */
