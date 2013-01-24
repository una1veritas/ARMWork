/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPIBUS_H_
#define _SPIBUS_H_

#include <stdio.h>
#include <stdint.h>
//#include <Arduino.h>
//#include <avr/pgmspace.h>
#include "armcore.h"
#include "gpio.h"
#include "spi.h"


class SPIBus {
	spi *spiport;
	GPIOPin sck, miso, mosi, nss;
	
public:
	
	SPIBus(SPI_TypeDef * SPIx, 
		GPIOPin sckpin, GPIOPin misopin, GPIOPin mosipin, GPIOPin nsspin) : 
		sck(sckpin), miso(misopin), mosi(mosipin), nss(nsspin)  {
			spiport->SPIx = SPIx;
	}

  // SPI Configuration methods

//  inline static void attachInterrupt();
//  inline static void detachInterrupt(); // Default

  void begin(); // Default
  void end();

	inline uint16 transfer(uint16 _data) {
		return spi_transfer(spiport, _data);
	}

  void setBitOrder(uint16_t);
  void setDataMode(uint16_t);
  void setClockDivider(uint16_t);
};

//sextern SPI SPIBus1();

/*
void SPIClass::attachInterrupt() {
  SPCR |= _BV(SPIE);
}

void SPIClass::detachInterrupt() {
  SPCR &= ~_BV(SPIE);
}
*/

#endif
