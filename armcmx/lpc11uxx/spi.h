/****************************************************************************
 *   $Id:: ssp.h 4102 2010-08-02 20:53:05Z usb00423                         $
 *   Project: NXP LPC11xx SSP example
 *
 *   Description:
 *     This file contains SSP code header definition.
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
****************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

//#include <stdint.h>
#include "LPC11Uxx.h"
#include "armcmx.h"

#ifdef __cplusplus
extern "C" {
#endif
  
typedef struct {
//  uint8_t Num;
  LPC_SSPx_Type * SSPx;
  GPIOPin pin_SSPCS;
  //
} SPIDef;

#define SSP0_RST_N (1<<0)
#define SSP1_RST_N (1<<2)

/* SPI read and write buffer size */
#define SSP_BUFSIZE     16
#define FIFOSIZE        8

#define SFLASH_INDEX	4
#define SPI_MAX_TIMEOUT     0xFF
	
/* SSP Status register */
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1) 
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3) 
#define SSPSR_BSY       (0x1<<4)

/* SSP CR0 register */
#define SSPCR0_DSS      (0x1<<0)
#define SSPCR0_FRF      (0x1<<4)
#define SSPCR0_SPO      (0x1<<6)
#define SSPCR0_SPH      (0x1<<7)
#define SSPCR0_SCR      (0x1<<8)

/* SSP CR1 register */
#define SSPCR1_LBM      (0x1<<0)
#define SSPCR1_SSE      (0x1<<1)
#define SSPCR1_MS       (0x1<<2)
#define SSPCR1_SOD      (0x1<<3)

/* SSP Interrupt Mask Set/Clear register */
#define SSPIMSC_RORIM   (0x1<<0)
#define SSPIMSC_RTIM    (0x1<<1)
#define SSPIMSC_RXIM    (0x1<<2)
#define SSPIMSC_TXIM    (0x1<<3)

/* SSP0 Interrupt Status register */
#define SSPRIS_RORRIS   (0x1<<0)
#define SSPRIS_RTRIS    (0x1<<1)
#define SSPRIS_RXRIS    (0x1<<2)
#define SSPRIS_TXRIS    (0x1<<3)

/* SSP0 Masked Interrupt register */
#define SSPMIS_RORMIS   (0x1<<0)
#define SSPMIS_RTMIS    (0x1<<1)
#define SSPMIS_RXMIS    (0x1<<2)
#define SSPMIS_TXMIS    (0x1<<3)

/* SSP0 Interrupt clear register */
#define SSPICR_RORIC    (0x1<<0)
#define SSPICR_RTIC     (0x1<<1)


void SPI_reset(SPIDef * port);

void SPI_init(SPIDef * port, GPIOPin clk, GPIOPin miso, GPIOPin mosi, GPIOPin ssel);
void SPI_config(SPIDef * port, GPIOPin sck, GPIOPin miso, GPIOPin mosi, GPIOPin ncs);
void SPI_start(SPIDef * port);

void SPI_disable(SPIDef * port);


#define SPIMODE_CPOL    (1<<6)
#define SPIMODE_CPHA    (1<<7)
#define SPIMODE_DIVSET  (1<<8)


#define SPI_DSS_8BIT 0x07
#define SPI_DSS_16BIT 0x0f

void SPI_DataMode(SPIDef * port, uint32_t mode);
void SPI_ClockDivier(SPIDef *, uint32_t);
void SPI_DataSize(SPIDef * port, uint32_t dss);

uint32_t SPI_transfer(SPIDef * port, uint32_t data); // uint16 may be enough

#define SPI_RNE(SSPx)  (SSPx->SR & SSPSR_RNE)


extern SPIDef SPI0Def, SPI1Def;

#define ETHER_CS  PIO0_16
#define ETHER_SPI SPI1Def

#ifdef __cplusplus
}
#endif

#endif  /* __SPI_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

