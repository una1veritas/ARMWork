/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : lpc2000_spi.c                                                     *
* Description : This  contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an LPC2xxx.        *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2005 Martin Thomas    *
*                                                                             *
\*****************************************************************************/

/*
	2006, Bertrik Sikken, modified for LPCUSB
*/


/*****************************************************************************/

// *******************************
// CodeRed - port to RDB1768 board
// *******************************

// CodeRed - point to LPCUSB version of type.h
//#include "lpcusb_type.h"
//#include "usbdebug.h"

#include "spi.h"
#include "type.h"
#include "LPC17xx.h"

/*****************************************************************************/

// SP0SPCR  Bit-Definitions
#define CPHA    3
#define CPOL    4
#define MSTR    5
// SP0SPSR  Bit-Definitions
#define SPIF	7

// Code Red - update defines for RDB1768
#define SPI_IODIR			LPC_GPIO0->FIODIR
#define SPI_SCK_PIN			15		/* Clock		P0.15 out		(PINSEL0) */
#define SPI_SSEL_PIN		16		/* Card-Select	P0.16 GPIO out	(PINSEL1) */
#define SPI_MISO_PIN		17		/* from Card	P0.17 in		(PINSEL1) */
#define SPI_MOSI_PIN		18		/* to Card		P0.18 out		(PINSEL1) */

#define SPI_PINSEL			LPC_PINCON->PINSEL1
#define SPI_SCK_FUNCMASK	(3U << 30) // P0.15 - PINSEL0 [31:30]
#define SPI_SSEL_FUNCMASK	(3 << 0)  // P0.16 - PINSEL1 [1:0]
#define SPI_MISO_FUNCMASK	(3 << 2)  // P0.17 - PINSEL1 [3:2]
#define SPI_MOSI_FUNCMASK	(3 << 4)  // P0.18 - PINSEL1 [5:4]

#define SPI_SCK_FUNCBIT		(3U << 30)
#define SPI_SSEL_FUNCBIT	(0 << 0) // Note - SSEL needed as GPIO
#define SPI_MISO_FUNCBIT	(3 << 2)
#define SPI_MOSI_FUNCBIT	(3 << 4)

#define SPI_PRESCALE_REG	LPC_SPI->SPCCR

#define UNSELECT_CARD()   LPC_GPIO0->FIOCLR = (1 << SPI_SSEL_PIN)
#define SELECT_CARD() LPC_GPIO0->FIOSET = (1 << SPI_SSEL_PIN)

/*****************************************************************************/

/*****************************************************************************/

// Utility-functions which does not toogle CS.
// Only needed during card-init. During init
// the automatic chip-select is disabled for SSP

uint8_t if_spiSendN(uint8_t outgoing)
{
	LPC_SPI->SPDR = outgoing;
	while (!(LPC_SPI->SPSR & (1 << SPIF)));
	return LPC_SPI->SPDR;
}

/*****************************************************************************/

void SPISetSpeed(uint8_t speed)
{
	speed &= 0xFE;
	if (speed < SPI_PRESCALE_MIN) {
		speed = SPI_PRESCALE_MIN;
	}
	SPI_PRESCALE_REG = speed;
}


void ms_delay(int n)
{
   volatile int d;
   for (d=0; d<n*3000; d++){}
}

void SPIInit(void)
{
	uint8_t i;

//	DBG("spiInit for SPI(0)\n");

/* Code Red - comment out original code
	// setup GPIO
	SPI_IODIR |= (1 << SPI_SCK_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_SS_PIN);
	SPI_IODIR &= ~(1 << SPI_MISO_PIN);

	// reset Pin-Functions  
	SPI_PINSEL &= ~((3 << SPI_SCK_FUNCBIT) | (3 << SPI_MISO_FUNCBIT) | (3 << SPI_MOSI_FUNCBIT));
	SPI_PINSEL |= ((1 << SPI_SCK_FUNCBIT) | (1 << SPI_MISO_FUNCBIT) | (1 << SPI_MOSI_FUNCBIT));

	PINSEL1 &= ~(3 << (SPI_SS_FUNCBIT - 32));
	PINSEL1 |= (0 << (SPI_SS_FUNCBIT - 32));
*/
	
// ** CodeRed - new SPI setup code	
	
	// Turn on the power
	LPC_SC->PCONP |= (1<<8); // PCSPI

	// CodeRed - clock
	LPC_SC->PCLKSEL0 &= ~(3<<16);  // PCLK_SPI
	LPC_SC->PCLKSEL0 |=  (1<<16);  // PCLK_periph = CCLK

	// setup GPIO
	SPI_IODIR |= (1 << SPI_SCK_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_SSEL_PIN);
	SPI_IODIR &= ~(1 << SPI_MISO_PIN);

	//// reset Pin-Functions
	// P0.15 set to SCK
	LPC_PINCON->PINSEL0 &= ~SPI_SCK_FUNCMASK;
	LPC_PINCON->PINSEL0 |=  SPI_SCK_FUNCBIT;	
	// P0.16, P0.17, P0.18 set to SSEL,MISI, MOSI
	SPI_PINSEL &= ~(SPI_MOSI_FUNCMASK | SPI_MISO_FUNCMASK | SPI_SSEL_FUNCMASK);
	SPI_PINSEL |=  (SPI_MOSI_FUNCBIT | SPI_MISO_FUNCBIT | SPI_SSEL_FUNCBIT);


	
	// enable SPI-Master	
	LPC_SPI->SPCR = (1 << MSTR) | (0 << CPOL);	// TODO: check CPOL
	
	UNSELECT_CARD();
	
	
	// CodeRed - add in code for GPIO setup
    /* Switch the SSI TX line to a GPIO and drive it high too. */
    //CRT - P0.18 back to GPIO
	LPC_PINCON->PINSEL1 &= ~(SPI_MOSI_FUNCMASK);
	LPC_GPIO0->FIODIR |= (1<<SPI_MOSI_PIN);
	LPC_GPIO0->FIOSET = (1<<SPI_MOSI_PIN);
    
	// low speed during init
	SPISetSpeed(254);

	/* Send 20 spi commands with card not selected */
	for (i = 0; i < 21; i++) {
		if_spiSendN(0xff);
	}

    //CodeRed - P0.18 back to SPI
	LPC_PINCON->PINSEL1 |= (SPI_MOSI_FUNCBIT);
 
}

/*****************************************************************************/

/*****************************************************************************/

uint8_t SPISend(uint8_t outgoing)
{
	uint8_t incoming;

	SELECT_CARD();
	LPC_SPI->SPDR = outgoing;
	while (!(LPC_SPI->SPSR & (1 << SPIF)));
	incoming = LPC_SPI->SPDR;
	UNSELECT_CARD();

	return incoming;
}


void SPISendN(uint8_t * pbBuf, int iLen)
//void spi1_send(uint8_t * pbBuf, int iLen)
{
	int i;

	SELECT_CARD();
	for (i = 0; i < iLen; i++) {
		LPC_SPI->SPDR = pbBuf[i];
		while (!(LPC_SPI->SPSR & (1 << SPIF)));
	}
	UNSELECT_CARD();
}


void SPIRecvN(uint8_t * pbBuf, int iLen)
//void spi1_receive(uint8_t * pbBuf, int iLen)
{
	int i;

	SELECT_CARD();
	for (i = 0; i < iLen; i++) {
		LPC_SPI->SPDR = 0xFF;
		while (!(LPC_SPI->SPSR & (1 << SPIF)));
		pbBuf[i] = LPC_SPI->SPDR;
	}
	UNSELECT_CARD();
}

/*****************************************************************************/
