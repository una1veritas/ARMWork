  /*
  *
  *
  */
  
#include "ssp.h"
#include "spi.h"

#include "usart.h"

#define SD_CARD BIT8
#define SD_SELECTION_DIR_SET (LPC_GPIO->DIR[0] |= (1<<2))
#define SD_SELECTION_SET (LPC_GPIO->PIN[0] |= (1<<2))
#define SD_SELECTION_CLR (LPC_GPIO->PIN[0] &= ~(1<<2))

SPIDef SPI0Def = { 0, PIO1_29, PIO0_8, PIO0_9, PIO0_2 }, SPI1Def = { 1, PIO1_20, PIO1_21, PIO1_22, PIO1_23 };

uint8_t SPI_transfer(SPIDef * port, uint8_t data) {
  volatile uint8_t rcv;
	if ( port->Num == 0 ) {
	  LPC_SSP0->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  rcv = LPC_SSP0->DR;
	} else {
	  LPC_SSP1->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  rcv = LPC_SSP1->DR;
	}

  return rcv;
}


uint8_t SPI_send( SPIDef * port, uint8_t data ) {
  uint8_t Dummy;
    
	if ( port->Num == 0 ) {
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP0->DR = data;
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  Dummy = LPC_SSP0->DR;
	}
	else
	{
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = data;
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  Dummy = LPC_SSP1->DR;
  }
  return Dummy;
}


uint8_t SPI_receive(SPIDef * port, uint8_t data ) {
  uint8_t Dummy;
  
  char tmp[64];
  sprintf(tmp, "%02x/", data);
  USART_puts(&usart, tmp);

	if ( port->Num == 0 )
	{
	  LPC_SSP0->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  Dummy = LPC_SSP0->DR;
	}
	else
	{
	  LPC_SSP1->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  Dummy = LPC_SSP1->DR;
  }
    sprintf(tmp, "%02x ", Dummy);
  USART_puts(&usart, tmp);
  return Dummy; 
}


void SPI_init(SPIDef * port, GPIOPin clk, GPIOPin miso, GPIOPin mosi, GPIOPin ssel) {
  SSP_IOConfig( port );
  SSP_Init( port );			
}

void SPI_disable(SPIDef * port) {
}


