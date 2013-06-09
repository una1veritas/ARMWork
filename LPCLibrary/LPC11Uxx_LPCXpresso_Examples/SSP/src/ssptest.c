/****************************************************************************
 *   $Id:: ssptest.c 4103 2010-08-02 20:56:20Z usb00423                     $
 *   Project: NXP LPC11Uxx SSP example
 *
 *   Description:
 *     This file contains SSP test modules, main entry, to test SSP APIs.
*
****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include "LPC11Uxx.h"                        /* LPC11xx definitions */
#include "gpio.h"
#include "ssp.h"
#if SSP_DEBUG
#include "uart.h"
#endif

#define SSP_NUM			0

uint8_t src_addr[SSP_BUFSIZE]; 
uint8_t dest_addr[SSP_BUFSIZE];

/*****************************************************************************
** Function name:		LoopbackTest
**
** Descriptions:		Loopback test
**				
** parameters:			port #
** Returned value:		None
** 
*****************************************************************************/
void SSP_LoopbackTest( uint8_t portNum )
{
  uint32_t i;

  if ( portNum == 0 )
  {
#if !USE_CS
	/* Set SSEL pin to output low. */
	GPIOSetBitValue( PORT0, 2, 0 );
#endif
	i = 0;
	while ( i <= SSP_BUFSIZE )
	{
	  /* to check the RXIM and TXIM interrupt, I send a block data at one time 
	  based on the FIFOSIZE(8). */
	  SSP_Send( portNum, (uint8_t *)&src_addr[i], FIFOSIZE );
	  /* If RX interrupt is enabled, below receive routine can be
	  also handled inside the ISR. */
	  SSP_Receive( portNum, (uint8_t *)&dest_addr[i], FIFOSIZE );
	  i += FIFOSIZE;
	}
#if !USE_CS
	/* Set SSEL pin to output high. */
	GPIOSetBitValue( PORT0, 2, 1 );
#endif
  }
  else
  {
#if !USE_CS
	/* Set SSEL pin to output low. */
	GPIOSetBitValue( PORT1, 23, 0 );
#endif
	i = 0;
	while ( i <= SSP_BUFSIZE )
	{
	  /* to check the RXIM and TXIM interrupt, I send a block data at one time 
	  based on the FIFOSIZE(8). */
	  SSP_Send( portNum, (uint8_t *)&src_addr[i], FIFOSIZE );
	  /* If RX interrupt is enabled, below receive routine can be
	  also handled inside the ISR. */
	  SSP_Receive( portNum, (uint8_t *)&dest_addr[i], FIFOSIZE );
	  i += FIFOSIZE;
	}
#if !USE_CS
	/* Set SSEL pin to output high. */
	GPIOSetBitValue( PORT1, 23, 1 );
#endif
  }
  return;
}

/*****************************************************************************
** Function name:		SEEPROMTest
**
** Descriptions:		Serial EEPROM(Atmel 25xxx) test
**				
** parameters:			port #
** Returned value:		None
** 
*****************************************************************************/
void SSP_SEEPROMTest( uint8_t portNum )
{
  uint32_t i, timeout;

  if ( portNum == 0 )
  {
	LPC_IOCON->PIO0_2 &= ~0x07;	/* SSP SSEL is a GPIO pin */
	GPIOSetBitValue( PORT0, 2, 1 );
	/* port0, bit 2 is set to GPIO output and high */
	GPIOSetDir( PORT0, 2, 1 );
  
	GPIOSetBitValue( PORT0, 2, 0 );
	/* Test Atmel AT25DF041 Serial flash. */
	src_addr[0] = WREN;			/* set write enable latch */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );

	for ( i = 0; i < 0x80; i++ );	/* delay minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = RDSR;	/* check status to see if write enabled is latched */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	SSP_Receive( portNum, (uint8_t *)dest_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );
	if ( (dest_addr[0] & (RDSR_WEN|RDSR_RDY)) != RDSR_WEN )
	/* bit 0 to 0 is ready, bit 1 to 1 is write enable */
	{
	  while ( 1 );
	}

	for ( i = 0; i < 0x80; i++ );	/* delay minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = WRSR;
	src_addr[1] = 0x00;				/* Make the whole device unprotected. */
	SSP_Send( portNum, (uint8_t *)src_addr, 2 );
	GPIOSetBitValue( PORT0, 2, 1 );

	for ( i = 0; i < 0x80; i++ );	/* delay minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = RDSR;				/* check status to see if write enabled is latched */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	SSP_Receive( portNum, (uint8_t *)dest_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );
	if ( (dest_addr[0] & (RDSR_WEN|RDSR_RDY)) != RDSR_WEN )
	/* bit 0 to 0 is ready, bit 1 to 1 is write enable */
	{
	  while ( 1 );
	}

	for ( i = 0; i < 0x80; i++ );	/* delay minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = CHIP_ERASE;	/* Write command is 0x02, low 256 bytes only */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );

	for ( i = 0; i < 0x1400000; i++ );	/* Be careful with the dumb delay, it
										may vary depending on the system clock.  */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = RDSR;	/* check status to see if write enabled is latched */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	SSP_Receive( portNum, (uint8_t *)dest_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );
	if ( (dest_addr[0] & (RDSR_EPE|RDSR_RDY)) != 0x0 )
	/* bit 0 to 0 is ready, bit 1 to 1 is write enable */
	{
	  while ( 1 );
	}

	GPIOSetBitValue( PORT0, 2, 0 );
	/* Test Atmel AT25DF041 Serial flash. */
	src_addr[0] = WREN;			/* set write enable latch */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );

	for ( i = 0; i < 0x80; i++ );	/* delay minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = RDSR;	/* check status to see if write enabled is latched */
	SSP_Send( portNum, (uint8_t *)src_addr, 1 );
	SSP_Receive( portNum, (uint8_t *)dest_addr, 1 );
	GPIOSetBitValue( PORT0, 2, 1 );
	if ( (dest_addr[0] & (RDSR_WEN|RDSR_RDY)) != RDSR_WEN )
	/* bit 0 to 0 is ready, bit 1 to 1 is write enable */
	{
	  while ( 1 );
	}

	for ( i = 0; i < SSP_BUFSIZE; i++ )	/* Init RD and WR buffer */    
	{
	  src_addr[i+SFLASH_INDEX] = i;	/* leave four bytes for cmd and offset(16 bits) */
	  dest_addr[i] = 0;
	}

	/* please note the first four bytes of WR and RD buffer is used for
	commands and offset, so only 4 through SSP_BUFSIZE is used for data read,
	write, and comparison. */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = WRITE;	/* Write command is 0x02, low 256 bytes only */
	src_addr[1] = 0x00;	/* write address offset MSB is 0x00 */
	src_addr[2] = 0x00;	/* write address offset LSB is 0x00 */
	src_addr[3] = 0x00;	/* write address offset LSB is 0x00 */
	SSP_Send( portNum, (uint8_t *)src_addr, SSP_BUFSIZE );
	GPIOSetBitValue( PORT0, 2, 1 );

	for ( i = 0; i < 0x400000; i++ );	/* Be careful with the dumb delay, it
										may vary depending on the system clock.  */
	timeout = 0;
	while ( timeout < MAX_TIMEOUT )
	{
	  GPIOSetBitValue( PORT0, 2, 0 );
	  src_addr[0] = RDSR;	/* check status to see if write cycle is done or not */
	  SSP_Send( portNum, (uint8_t *)src_addr, 1);
	  SSP_Receive( portNum, (uint8_t *)dest_addr, 1 );
	  GPIOSetBitValue( PORT0, 2, 1 );
	  if ( (dest_addr[0] & RDSR_RDY) == 0x00 )	/* bit 0 to 0 is ready */
	  {
		break;
	  }
	  timeout++;
	}
	if ( timeout == MAX_TIMEOUT )
	{
	  while ( 1 );
	}

	for ( i = 0; i < 0x80; i++ );	/* delay, minimum 250ns */
	GPIOSetBitValue( PORT0, 2, 0 );
	src_addr[0] = READ;		/* Read command is 0x03, low 256 bytes only */
	src_addr[1] = 0x00;		/* Read address offset MSB is 0x00 */
	src_addr[2] = 0x00;		/* Read address offset LSB is 0x00 */
	src_addr[3] = 0x00;		/* Read address offset LSB is 0x00 */
	SSP_Send( portNum, (uint8_t *)src_addr, SFLASH_INDEX ); 
	SSP_Receive( portNum, (uint8_t *)&dest_addr[SFLASH_INDEX], SSP_BUFSIZE-SFLASH_INDEX );
	GPIOSetBitValue( PORT0, 2, 1 );
  }
  else			/* Port 1 */
  {
	/* Test on Port 0 only. */
	while ( 1 );
  }
  return;
}

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  uint32_t i;
#if SSP_DEBUG
  int8_t temp[2];
#endif
  
  SystemCoreClockUpdate();

#if SSP_DEBUG
  UARTInit(115200);
#endif

  SSP_IOConfig( SSP_NUM );
  SSP_Init( SSP_NUM );			
						
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	src_addr[i] = (uint8_t)i;
	dest_addr[i] = 0;
  }

#if TX_RX_ONLY
  /* For the inter-board communication, one board is set as
  master transmit, the other is set to slave receive. */
#if SSP_SLAVE
  /* Slave receive */
  SSP_Receive( SSP_NUM, (uint8_t *)dest_addr, SSP_BUFSIZE );
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	if ( src_addr[i] != dest_addr[i] )
	{
	  while ( 1 );				/* Verification failure, fatal error */
	} 
  }
#else
  /* Master transmit */
  SSP_Send( SSP_NUM, (uint8_t *)src_addr, SSP_BUFSIZE);
#endif
#else
  /* TX_RX_ONLY=0, it's either an internal loopback test
  within SSP peripheral or communicate with a serial EEPROM. */
#if LOOPBACK_MODE
  SSP_LoopbackTest( SSP_NUM );
#else
  SSP_SEEPROMTest( SSP_NUM );
  /* If SW CLK is used as SSP clock, change the setting before
  serial EEPROM test, restore after the test. */
#ifdef __SWD_DISABLED
  LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;	/* Restore SWD CLK */
#endif  
#endif			/* endif NOT LOOPBACK_MODE */
#if LOOPBACK_MODE
  for ( i = 0; i < SSP_BUFSIZE; i++ )
#else
  /* for EEPROM test, verifying, ignore the difference in the first 
  four bytes which are used as command and parameters. */
  for ( i = SFLASH_INDEX; i < SSP_BUFSIZE; i++ )
#endif
  {
	if ( src_addr[i] != dest_addr[i] )
	{
#if SSP_DEBUG
	  temp[0] = (dest_addr[i] & 0xF0) >> 4;
	  if ( (temp[0] >= 0) && (temp[0] <= 9) )
	  {
		temp[0] += 0x30;
	  }
	  else
	  {
		temp[0] -= 0x0A;
		temp[0] += 0x41;
	  }
	  temp[1] = dest_addr[i] & 0x0F;
	  if ( (temp[1] >= 0) && (temp[1] <= 9) )
	  {
		temp[1] += 0x30;
	  }
	  else
	  {
		temp[1] -= 0x0A;
		temp[1] += 0x41;
	  }
	  UARTSend((uint8_t *)&temp[0], 2);
	  UARTSend((uint8_t *)"\r\n", 2);
#endif
	  while( 1 );			/* Verification failed */
	}
  }
#if SSP_DEBUG
  UARTSend((uint8_t *)"PASS\r\n", 6);
#endif
#endif			/* endif NOT TX_RX_ONLY */
  while ( 1 );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

