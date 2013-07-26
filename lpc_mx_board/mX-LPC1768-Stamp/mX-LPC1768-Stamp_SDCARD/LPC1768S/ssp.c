#include <LPC17xx.H>  			/* LPC17xx definitions */

#include "type.h"
#include "ssp.h"
#include "utils.h"

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
  U8 i;
  volatile U8 Dummy;
  /* Configure PIN connect block */
  /* bit 32, 54, 76 are 0x10, bit 98 are 0x00 */
  /* port 0 bits 17, 18, 19, 20 are SSP port SCK1, MISO1, MOSI1, and SSEL1 */
  /* set SSEL to GPIO pin that you will have the totoal freedom to set/reset 
  the SPI chip-select pin */
  LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(3 << 18)) | (1 << 19);
  LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(3 << 16)) | (1 << 17);
  LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(3 << 14)) | (1 << 15);
  
  LPC_SSP1->CR0 = SSP_CONFIGURATION0;
  LPC_SSP1->CR1 |= SSPCR1_SSE;
  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  LPC_SSP1->CPSR = 0x2;
  
  for ( i = 0; i < FIFOSIZE; i++ )
  {
    Dummy = LPC_SSP1->DR;		/* clear the RxFIFO */
  }
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



/******************************************************************************
**                            End Of File
******************************************************************************/



