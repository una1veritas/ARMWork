#ifndef _SDFATFS_H_
#define _SDFATFS_H_

 /*
  * Wrapper for FatFs by ChaN
  */

#include "armcmx.h"
#include "Print.h"
#include "Stream.h"

#include "SPIBus.h"

#include "ff.h"

class SDFatFs : public Print {
  SPIBus & spibus;
  
  public:
    SDFatFs(SPIBus & bus) : spibus(bus) {}
};

#endif
