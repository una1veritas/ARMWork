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

typedef struct __SPIPort {
	SPI_TypeDef * SPIx;
	GPIOPin pin_sck, pin_mosi, pin_miso, pin_ncs;
	SPI_InitTypeDef modeStruct;
//	uint8 xbuf[256];
//	uint16 xindex;
} SPIPort;

void spi_init(SPIPort * spi, SPI_TypeDef * SPIx, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
void spi_disable(SPIPort *);
uint16 spi_transfer(SPIPort * spi, uint16 data);
void spi_setDataMode(SPIPort * spi, uint16 modeid);
void spi_setMode(SPIPort * spi, uint16 clkdiv, uint16 cpol, uint16 cpha, uint16 msbfirst);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */
