 /*
  *
  *
  */
  
#include "spi_core.h"

/*
#define SD_CARD BIT8
#define SD_SELECTION_DIR_SET (LPC_GPIO->DIR[0] |= (1<<2))
#define SD_SELECTION_SET (LPC_GPIO->PIN[0] |= (1<<2))
#define SD_SELECTION_CLR (LPC_GPIO->PIN[0] &= ~(1<<2))
*/

SPIDef SPI0Def = { LPC_SSP0, PIO0_2 }, SPI1Def = { LPC_SSP1, PIO1_23 };

uint32_t SPI_transfer(SPIDef * port, uint32_t data) {
  port->SSPx->DR = data;
  /* Wait until the Busy bit is cleared */
  while ( (port->SSPx->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
  data = port->SSPx->DR;
  return data;
}


void SPI_init(SPIDef * port, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin ncs) {  
  SPI_reset(port);
  SPI_config(port, sck, miso, mosi, ncs);
  SPI_start(port);
}


void SPI_config(SPIDef * port, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin ncs) {
  port->pin_SSPCS = ncs;

  if ( port->SSPx == LPC_SSP0 ) {
    LPC_SYSCON->PRESETCTRL |= (0x1<<0);  //SSP0 reset off
    LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<11); // power on
    LPC_SYSCON->SSP0CLKDIV = 0x2;			/* Divided by 2 */

    LPC_IOCON->PIO0_8 &= ~0x07;		/*  SSP I/O config */
    LPC_IOCON->PIO0_8 |= 0x01;		/* SSP MISO */
    LPC_IOCON->PIO0_9 &= ~0x07;	
    LPC_IOCON->PIO0_9 |= 0x01;		/* SSP MOSI */
    
    if ( sck == PIO0_10 ) {
      LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
      LPC_IOCON->SWCLK_PIO0_10 |= 0x02;		/* SSP CLK */
    } else if ( sck == PIO1_29 ) {
    /* On C1U, SSP CLK can be routed to different pins. */
      LPC_IOCON->PIO1_29 &= ~0x07;	
      LPC_IOCON->PIO1_29 = 0x01;
    } else {
      LPC_IOCON->PIO0_6 &= ~0x07;	
      LPC_IOCON->PIO0_6 = 0x02;
    }
    
    if ( ncs == NOT_A_PIN || ncs == PIN_NOT_DEFINED ) {
      LPC_IOCON->PIO0_2 &= ~0x07;	
      LPC_IOCON->PIO0_2 |= 0x01;		/* System SSP SSEL */
    } else {
      // as user controllable GPIO pin
      /* Enable AHB clock to the GPIO domain. */
      LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
      pinFuncClear(ncs);		/* SSP SSEL is a GPIO pin */
      pinMode(ncs, OUTPUT);
      digitalWrite(ncs, HIGH );
    }

  }
  else if (port->SSPx == LPC_SSP1) 		/* port number 1 */
  {
    LPC_SYSCON->PRESETCTRL |= (0x1<<2);  // SSP1 reset off
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18);  // power on
    LPC_SYSCON->SSP1CLKDIV = 0x02;			/* Divided by 2 */
    if ( sck == PIO1_20 ) {
      pinFuncClear(PIO1_20); // &= ~0x07;    /*  SSP I/O config */
      pinFuncSet(PIO1_20, 0x02); //0x02;		/* SSP CLK */
    } else {
      LPC_IOCON->PIO1_15 &= ~0x07;    /*  SSP I/O config */
      LPC_IOCON->PIO1_15 |= 0x03;		/* SSP CLK */
    }
    if ( miso == PIO1_21 ) {
      //LPC_IOCON->PIO1_21 &= ~0x07;	
      //LPC_IOCON->PIO1_21 |= 0x02;		/* SSP MISO */
      pinFuncClear(PIO1_21);
      pinFuncSet(PIO1_21, 0x02);
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


void SPI_start(SPIDef * port) {
  uint8_t i, Dummy=Dummy;

  /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
  port->SSPx->CR0 = 0x0707;
  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
  port->SSPx->CPSR = 0x2;

  for ( i = 0; i < FIFOSIZE; i++ ) {
    Dummy = port->SSPx->DR;		/* clear the RxFIFO */
  }

  /* Enable the SSP Interrupt */
  if ( port->SSPx == LPC_SSP0 ) {
    NVIC_EnableIRQ(SSP0_IRQn);
  } else {
    NVIC_EnableIRQ(SSP1_IRQn);
  }
  /* Device select as master, SSP Enabled */
  /* Master mode */
  port->SSPx->CR1 = SSPCR1_SSE;
  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  port->SSPx->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;

  return;
}


void SPI_reset(SPIDef * port) {
 if ( port->SSPx == LPC_SSP0 ) {
    LPC_SYSCON->PRESETCTRL &= ~SSP0_RST_N;
   __nop();
    LPC_SYSCON->PRESETCTRL |= SSP0_RST_N;
 } else if ( port->SSPx == LPC_SSP1 ) {
    LPC_SYSCON->PRESETCTRL &= ~SSP1_RST_N;
   __nop();
    LPC_SYSCON->PRESETCTRL |= SSP1_RST_N;
 }
}

void SPI_disable(SPIDef * port) {
}

void SPI_ClockDivier(SPIDef * port, uint32_t div) {
  // assumes SSP_PCLK = 24 MHz
  uint32 scrval, cpsval;
  switch(div) {
    case SPI_CLOCK_DIV4:
      cpsval = 1<<1; // 2
      scrval = 1; // 1+1
      break;
    case SPI_CLOCK_DIV8:
      cpsval = 1<<1; // 2
      scrval = 3;
      break;
    case SPI_CLOCK_DIV32:
      cpsval = 1<<1; // 2
      scrval = 15;
      break;
    case SPI_CLOCK_DIV64:
      cpsval = 2<<1; // 4
      scrval = 15;
      break;
    case SPI_CLOCK_DIV16:
    default:
      cpsval = 1<<1; // 2
      scrval = 7;
      break;
  }
  port->SSPx->CPSR = cpsval & 0xfe;
  port->SSPx->CR0 &= ~(0xffUL<<8);
  port->SSPx->CR0 |= (scrval & 0xff)<<8;
}


/*Set up output clocks per bit for SSP bus
void IP_SSP_Set_ClockRate(IP_SSP_001_T *pSSP, uint32_t clk_rate, uint32_t prescale)
{
	uint32_t temp;
	temp = pSSP->CR0 & (~(SSP_CR0_SCR(0xFF)));
	pSSP->CR0 = temp | (SSP_CR0_SCR(clk_rate));
	pSSP->CPSR = prescale;
}
*/

void SPI_DataSize(SPIDef * port, uint32_t dss) {
    port->SSPx->CR0 &= ~((uint32_t)0x0f);
    port->SSPx->CR0 |= (0x0F & dss);				/* Set dss value for select data size */
}

void SPI_DataMode(SPIDef * port, uint32_t mode) {
  port->SSPx->CR0 &= ~(SSPCR0_CPOL | SSPCR0_CPHA);
  if ( mode & 1 )
    port->SSPx->CR0 |= SSPCR0_CPOL;
  if ( mode & 2 )
    port->SSPx->CR0 |= SSPCR0_CPHA;
}


void SPI_select(void) {
  // cs high
  // set mode
}

