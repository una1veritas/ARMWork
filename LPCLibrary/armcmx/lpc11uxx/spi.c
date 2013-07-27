  /*
  *
  *
  */
  
#include "ssp.h"

#include "spi.h"

#define SD_CARD BIT8
#define SD_SELECTION_DIR_SET (LPC_GPIO->DIR[0] |= (1<<2))
#define SD_SELECTION_SET (LPC_GPIO->PIN[0] |= (1<<2))
#define SD_SELECTION_CLR (LPC_GPIO->PIN[0] &= ~(1<<2))

SPIDef SPI0Def = { 0, LPC_SSP0, PIO0_2 }, SPI1Def = { 1, LPC_SSP1, PIO1_23 };

uint8_t SPI_transfer(SPIDef * port, uint8_t data) {
  
	if ( port->Num == 0 ) {
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP0->DR = data;
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  data = LPC_SSP0->DR;
	} else {
	  /* Move on only if NOT busy and TX FIFO not full. */
	  while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	  LPC_SSP1->DR = data;
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	  is left in the FIFO. */
	  data = LPC_SSP1->DR;
	}

  return data;
}

void SPI_init(SPIDef * port, GPIOPin clk, GPIOPin miso, GPIOPin mosi, GPIOPin ssel) {
  SSP_IOConfig( port );
  SSP_Init( port );			
}

void SPI_disable(SPIDef * port) {
}


