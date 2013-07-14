#include <LPC17xx.H>            	/* LPC17xx definitions */
#include "sd.h"
#include "type.h"
#include "ssp.h"
#include "utils.h"

#define SD_CARD BIT6
#define SD_SELECTION_DIR_SET (LPC_GPIO0->FIODIR |= SD_CARD)
#define SD_SELECTION_SET (LPC_GPIO0->FIOSET = SD_CARD)
#define SD_SELECTION_CLR (LPC_GPIO0->FIOCLR = SD_CARD)


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
  
  while ( !(LPC_SSP1->SR & SSPSR_TNF) );
  LPC_SSP1->DR = outgoing;
  while (!(LPC_SSP1->SR & SSPSR_RNE));
  incoming = LPC_SSP1->DR;
  
  return(incoming);
}

void if_spiInit()
{
  spi1_init();
  SD_SELECTION_DIR_SET;
  SD_SELECTION_CLR;	
}
#if 0
esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
  return(sd_readSector(file,address,buf,512));
}

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
  return(sd_writeSector(file,address, buf));
}
#endif


