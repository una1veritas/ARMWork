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
  spi_init(SPIxBuf, SPIx, pin_sck, pin_miso, pin_mosi, pin_nss); 
}

void SPIBus::end() {
	SPI_Cmd(SPIxBuf->SPIx, DISABLE);
}

void SPIBus::setBitOrder(uint16_t bitOrder)
{
	SPIxBuf->initStruct.SPI_FirstBit = ( bitOrder == SPI_MSBFIRST ? SPI_FirstBit_MSB : SPI_FirstBit_LSB);

	SPI_Init(SPIx, &SPIxBuf->initStruct);

}

void SPIBus::setDataMode(uint16_t mode)
{
	spi_setDataMode(SPIxBuf, mode);
}

void SPIBus::setClockDivider(uint16_t rate)
{	
	SPIxBuf->initStruct.SPI_BaudRatePrescaler = rate;
	
	SPI_Init(SPIx, &SPIxBuf->initStruct);
}

