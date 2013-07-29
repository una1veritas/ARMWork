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
//  volatile uint8_t rcv;
//  char tmp[64];
//  sprintf(tmp, "%02x/", data);
//  USART_puts(&usart, tmp);

	if ( port->Num == 0 ) {
	  LPC_SSP0->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  data = LPC_SSP0->DR;
	} else {
	  LPC_SSP1->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (LPC_SSP1->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  data = LPC_SSP1->DR;
	}

//  sprintf(tmp, "%02x ", data);
//  USART_puts(&usart, tmp);
  return data;
}



void SPI_init(SPIDef * port, GPIOPin clk, GPIOPin miso, GPIOPin mosi, GPIOPin ssel) {
  SSP_IOConfig( port );
  SSP_Init( port );			
}

void SPI_disable(SPIDef * port) {
}


