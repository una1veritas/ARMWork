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

  spi_begin(SPIx, spibuffer, pin_sck, pin_miso, pin_mosi, pin_nss); 

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
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_FirstBit = ( bitOrder == SPI_MSBFIRST ? SPI_FirstBit_MSB : SPI_FirstBit_LSB);
	SPI_InitStruct.SPI_CRCPolynomial = SPI_CRC_Rx;

	SPI_Init(SPIx, &SPI_InitStruct);

}

void SPIBus::setDataMode(uint8_t mode)
{
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = SPI_CRC_Rx;

	SPI_Init(SPIx, &SPI_InitStruct);
}

void SPIBus::setClockDivider(uint8_t rate)
{
//  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
//  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

