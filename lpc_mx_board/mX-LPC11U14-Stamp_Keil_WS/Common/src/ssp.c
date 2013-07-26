#include <LPC11Uxx.H>  			/* LPC17xx definitions */

#include "type.h"
#include "ssp.h"
#include "utils.h"

#define PIO1_29_IOCON_ADDRESS (0x400040D4UL)
/*****************************************************************************
** Function name:		spi1_init
**
** Descriptions:		SPI1(SSP) port initialization routine
**
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**				can't be installed correctly, return false.
**
*****************************************************************************/
U32 spi1_init(void)
{
	volatile U8 i, Dummy;
//	int *pio1_29_ptr = PIO1_29_IOCON_ADDRESS;

	  LPC_SYSCON->PRESETCTRL |= (0x1<<0);
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11);
	  LPC_SYSCON->SSP0CLKDIV = 0x02;			/* Divided by 2 */
	  LPC_IOCON->PIO0_8 &= ~0x07;	/*  SSP I/O config */
	  LPC_IOCON->PIO0_8 |= 0x01;		/* SSP MISO */
	  LPC_IOCON->PIO0_9 &= ~0x07;
	  LPC_IOCON->PIO0_9 |= 0x01;		/* SSP MOSI */
	 // GPIOSetDir( PORT0, 8, 1 );
//	  GPIOSetValue( PORT0, 8, 1 );
	//  GPIOSetDir( PORT0, 9, 1 );
	//  GPIOSetValue( PORT0, 9, 1 );
	  //LPC_IOCON->SCKLOC = 0x01;
//	  *pio1_29_ptr &= ~0x07;
//	  *pio1_29_ptr |= 0x01;

	  LPC_IOCON->PIO1_29 = 0x01;/* P2.11 function 1 is SSP clock, need to combined
	  		  					//	with IOCONSCKLOC register setting */
//	  GPIOSetDir( PORT0, 2, 1 );
	//  GPIOSetValue( PORT0, 2, 0 );


//	  LPC_IOCON->PIO0_2 &= ~0x07;		/* SSP SSEL is a GPIO pin */
	    /* port0, bit 2 is set to GPIO output and high */


	    //LPC_SSP->CR0 = 0x0707;
	    LPC_SSP0->CR0 = SSP_CONFIGURATION0;
	    LPC_SSP0->CR1 = SSPCR1_SSE;
	      /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
	      LPC_SSP0->CPSR = 0x02;

	      for ( i = 0; i < FIFOSIZE; i++ )
	      {
	    	Dummy = LPC_SSP0->DR;		/* clear the RxFIFO */
	      }

	      //LPC_SSP->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;

  return( TRUE );
}

/*****************************************************************************
** Function name:		spi1_send
**
** Descriptions:		Send a block of data to the SPI1(SSP) port, the
**				first parameter is the buffer pointer, the 2nd
**				parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
**
*****************************************************************************/
#if 0
void spi1_send(U8 *buf, U32 Length)
{
  U32 i;
  volatile U32 Dummy;

  for ( i = 0; i < Length; i++ )
  {
    /* as long as TNF bit is set (TxFIFO is not full), I can always transmit */
    while ( !(LPC_SSP1->SR & SSPSR_TNF) );
    LPC_SSP1->DR = *buf;
    buf++;
    while (!(LPC_SSP1->SR & SSPSR_RNE));
    Dummy = LPC_SSP1->DR;
  }
  return;
}

/*****************************************************************************
** Function name:		spi1_receive
** Descriptions:		the module will receive a block of data from
**				the SPI1(SSP), the 2nd parameter is the block
**				length.
** parameters:			buffer pointer, and block length
** Returned value:		None
**
*****************************************************************************/

void spi1_receive(U8 *buf, U32 Length)
{
  U32 i;

  for ( i = 0; i < Length; i++ )
  {
    /* As long as Receive FIFO is not empty, I can always receive. */
    /* since it's a loopback test, clock is shared for both TX and RX,
    no need to write dummy U8 to get clock to get the data */
    /* if it's a peer-to-peer communication, SSPDR needs to be written
    before a read can take place. */
    while (!(LPC_SSP1->SR & SSPSR_TNF));
    LPC_SSP1->DR = 0xAA;
    while (!(LPC_SSP1->SR & SSPSR_RNE));
    *buf = LPC_SSP1->DR;
    buf++;
  }

  return;
}

#endif

/******************************************************************************
**                            End Of File
******************************************************************************/



