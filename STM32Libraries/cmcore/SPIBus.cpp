/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

//#include "pins_arduino.h"
#include "SPIBus.h"

void SPIBus::begin() {
  spi_init(&spiport, spiport.SPIx, sck, miso, mosi, nss); 
}

void SPIBus::end() {
	spi_disable(&spiport);
}

