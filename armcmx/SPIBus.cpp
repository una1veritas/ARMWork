/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPIBus.h"

SPIBus SPI0(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, PIO0_2); // sck, miso, mosi, cs
SPIBus SPI1(&SPI1Def, PIO1_20, PIO1_21, PIO1_22, PIO1_23); // sck, miso, mosi, cs
#define SPI SPI1

void SPIBus::begin() {
  SPI_init(SPIPort, sck, miso, mosi, nss); 
}

void SPIBus::end() {
	SPI_disable(SPIPort);
}

