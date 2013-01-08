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

SPIBus SPIBus1();

void SPIBus::begin() {
  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).

  spi_begin(spibuffer, SPIx, pin_sck, pin_miso, pin_mosi, pin_nss); 

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI 
  // automatically switches to Slave, so the data direction of 
  // the SS pin MUST be kept as OUTPUT.
//  SPCR |= _BV(MSTR);
//  SPCR |= _BV(SPE);
}

void SPIBus::end() {
//  SPCR &= ~_BV(SPE);
}

void SPIBus::setBitOrder(uint8_t bitOrder)
{
	spibuffer->initStruct.SPI_FirstBit = ( bitOrder == SPI_MSBFIRST ? SPI_FirstBit_MSB : SPI_FirstBit_LSB);

	SPI_Init(SPIx, &spibuffer->initStruct);

}

void SPIBus::setDataMode(uint8_t mode)
{
	spibuffer->initStruct.SPI_CPOL = ((mode & 2 == 0) ? SPI_CPOL_Low : SPI_CPOL_High );
	spibuffer->initStruct.SPI_CPHA = ((mode & 1 == 0) ? SPI_CPHA_1Edge : SPI_CPHA_2Edge);
	
	SPI_Init(SPIx, &spibuffer->initStruct);
}

void SPIBus::setClockDivider(uint16_t rate)
{	
	spibuffer->initStruct.SPI_BaudRatePrescaler = rate;
	
	SPI_Init(SPIx, &spibuffer->initStruct);
}

