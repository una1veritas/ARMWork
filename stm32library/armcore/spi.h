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

#include "gpio.h"

/*
typedef enum _SPIBus {
	SPI1Bus = 0,
	SPI2Bus,
	SPI3Bus
} SPIBus;
*/

void spi_begin(SPI_TypeDef * SPIx /*SPIBus spibus*/, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin nss);
void spi_transfer(SPI_TypeDef * SPIx /*SPIBus spibus*/, uint8_t * data, uint16_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */
