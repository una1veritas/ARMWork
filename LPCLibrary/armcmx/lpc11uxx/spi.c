  /*
  *
  *
  */
  
#include "ssp.h"
#include "spi.h"

#include "gpio.h"
//#include "usart.h"

/*
#define SD_CARD BIT8
#define SD_SELECTION_DIR_SET (LPC_GPIO->DIR[0] |= (1<<2))
#define SD_SELECTION_SET (LPC_GPIO->PIN[0] |= (1<<2))
#define SD_SELECTION_CLR (LPC_GPIO->PIN[0] &= ~(1<<2))
*/

//SPIDef SPI0Def = { 0, PIO1_29, PIO0_8, PIO0_9, PIO0_2, LPC_SSP0 }, SPI1Def = { 1, PIO1_20, PIO1_21, PIO1_22, PIO1_23, LPC_SSP1 };
SPIDef SPI0Def = { 0, LPC_SSP0 }, SPI1Def = { 1, LPC_SSP1 };

uint8_t SPI_transfer(SPIDef * port, uint8_t data) {
//  volatile uint8_t rcv;
//  char tmp[64];
//  sprintf(tmp, "%02x/", data);
//  USART_puts(&usart, tmp);

/*
	if ( port->Num == 0 ) {
	  LPC_SSP0->DR = data;
	  // Wait until the Busy bit is cleared 
	  while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  data = LPC_SSP0->DR;
	} else {
*/
//	  LPC_SSP1->DR = data;
	  port->SSPx->DR = data;
	  /* Wait until the Busy bit is cleared */
	  while ( (port->SSPx->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	  data = port->SSPx->DR;
//	}

//  sprintf(tmp, "%02x ", data);
//  USART_puts(&usart, tmp);
  return data;
}



void SPI_init(SPIDef * port, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin ssel) {
  SPI_config( port, sck, miso, mosi, ssel );
  SSP_Init( port );			
}

void SPI_config(SPIDef * port, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin ncs) {

  if ( port->SSPx == LPC_SSP0 ) {
    LPC_SYSCON->PRESETCTRL |= (0x1<<0);
    LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<11);
    LPC_SYSCON->SSP0CLKDIV = 0x2;			/* Divided by 2 */

    LPC_IOCON->PIO0_8 &= ~0x07;		/*  SSP I/O config */
    LPC_IOCON->PIO0_8 |= 0x01;		/* SSP MISO */
    LPC_IOCON->PIO0_9 &= ~0x07;	
    LPC_IOCON->PIO0_9 |= 0x01;		/* SSP MOSI */
#ifdef __SWD_DISABLED
    LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
    LPC_IOCON->SWCLK_PIO0_10 |= 0x02;		/* SSP CLK */
#else
    if ( sck == PIO1_29 ) {
    /* On C1U, SSP CLK can be routed to different pins. */
      LPC_IOCON->PIO1_29 &= ~0x07;	
      LPC_IOCON->PIO1_29 = 0x01;
    } else {
      LPC_IOCON->PIO0_6 &= ~0x07;	
      LPC_IOCON->PIO0_6 = 0x02;
    }
#endif	/* endif __SWD_DISABLED */  

    if ( ncs == PIO0_2 ) {
      // as user controllable GPIO pin
      /* Enable AHB clock to the GPIO domain. */
      LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
      PIO_ClearFunc(PIO0_2);		/* SSP SSEL is a GPIO pin */
      /* port0, bit 2 is set to GPIO output and high */
      pinMode(PIO0_2, OUTPUT);
      digitalWrite( PIO0_2, HIGH );
    } else {
      LPC_IOCON->PIO0_2 &= ~0x07;	
      LPC_IOCON->PIO0_2 |= 0x01;		/* System SSP SSEL */
    }
  }
  else if (port->SSPx == LPC_SSP1) 		/* port number 1 */
  {
    LPC_SYSCON->PRESETCTRL |= (0x1<<2);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18);
    LPC_SYSCON->SSP1CLKDIV = 0x02;			/* Divided by 2 */
    if ( sck == PIO1_20 ) {
      PIO_ClearFunc(PIO1_20); // &= ~0x07;    /*  SSP I/O config */
      PIO_SetFunc(PIO1_20, 0x02); //0x02;		/* SSP CLK */
    } else {
      LPC_IOCON->PIO1_15 &= ~0x07;    /*  SSP I/O config */
      LPC_IOCON->PIO1_15 |= 0x03;		/* SSP CLK */
    }
    if ( miso == PIO1_21 ) {
      //LPC_IOCON->PIO1_21 &= ~0x07;	
      //LPC_IOCON->PIO1_21 |= 0x02;		/* SSP MISO */
      PIO_ClearFunc(PIO1_21);
      PIO_SetFunc(PIO1_21, 0x02);
    } else {
      LPC_IOCON->PIO0_22 &= ~0x07;	
      LPC_IOCON->PIO0_22 |= 0x03;		/* SSP MISO */
    }
    if ( mosi == PIO1_22 ) {
      LPC_IOCON->PIO1_22 &= ~0x07;	
      LPC_IOCON->PIO1_22 |= 0x02;		/* SSP MOSI */
    } else {
      LPC_IOCON->PIO0_21 &= ~0x07;	
      LPC_IOCON->PIO0_21 |= 0x02;		/* SSP MOSI */
    }
    
    if ( ncs == PIN_NOT_DEFINED ) {
      LPC_IOCON->PIO1_23 &= ~0x07;	
      LPC_IOCON->PIO1_23 |= 0x02;		/* SSP SSEL */
      // alternative:
      //LPC_IOCON->PIO1_19 &= ~0x07;	
      //LPC_IOCON->PIO1_19 |= 0x02;		/* SSP SSEL */
    } else {
      /* Enable AHB clock to the GPIO domain. */
      LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
      if ( ncs == PIO1_23 ) {
        LPC_IOCON->PIO1_23 &= ~0x07;		/* SSP SSEL is a GPIO pin */
        /* port2, bit 0 is set to GPIO output and high */
        pinMode( PIO1_23, OUTPUT );
        digitalWrite( PIO1_23, HIGH );
      } else {
        LPC_IOCON->PIO1_19 &= ~0x07;		/* SSP SSEL is a GPIO pin */
        /* port2, bit 0 is set to GPIO output and high */
        pinMode( PIO1_19, OUTPUT );
        digitalWrite( PIO1_19, HIGH );
      }
    }
  }
  return;		
}


void SPI_disable(SPIDef * port) {
}


