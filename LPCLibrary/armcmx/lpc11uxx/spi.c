  /*
  *
  *
  */
  
#include "ssp.h"
#include "spi.h"

SPIDef SPI0 = { 0 }, SPI1 = { 1 };

uint8_t SPI_transfer(SPIDef * port, uint8_t data) {
  SSP_Send(port, &data, 1);
  SSP_Receive(port, &data, 1);
  return data;
}

void SPI_init(SPIDef * port, GPIOPin clk, GPIOPin miso, GPIOPin mosi, GPIOPin ssel) {
  SSP_IOConfig( port );
  SSP_Init( port );			
}

void SPI_disable(SPIDef * port) {
}
