#include <LPC11Uxx.h>            	/* LPC11xx definitions */
#include "sd.h"
#include "type.h"
#include "ssp.h"
#include "utils.h"

#define SD_CARD BIT8
#define SD_SELECTION_DIR_SET (LPC_GPIO->DIR[0] |= (1<<2))
#define SD_SELECTION_SET (LPC_GPIO->PIN[0] |= (1<<2))
#define SD_SELECTION_CLR (LPC_GPIO->PIN[0] &= ~(1<<2))


esint8 if_initInterface(eint8* opts)
{
  if_spiInit();
  if(sd_Init()<0)
  {
    //		DBG((TXT("Card failed to init, breaking up...\n")));
    return(-1);
  }
  if(sd_State()<0)
  {
    //		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
    return(-2);
  }
//  file->sectorCount=4; /* FIXME ASAP!! */
  //DBG((TXT("Init done...\n")));
  return(0);
}

euint8 if_spiSend(euint8 outgoing)
{
  euint8 incoming=0;

  while (!(LPC_SSP0->SR & SSPSR_TNF));
  LPC_SSP0->DR = outgoing;
  while (!(LPC_SSP0->SR & SSPSR_RNE));
  incoming = LPC_SSP0->DR;
  return(incoming);
}

void if_spiInit()
{
  spi1_init();
  SD_SELECTION_DIR_SET;
  SD_SELECTION_SET;
  SD_SELECTION_CLR;
}



