#ifndef _SDFATFS_H_
#define _SDFATFS_H_

#include <stdint.h>
#include "ff.h"

#include "armcmx.h"
#include "SPI.h"


class SDFatFs {
  FATFS fatfs;		/* File system object */
//public:
//  FIL file;			/* File object */  

private:
  SPIBus & bus;
  GPIOPin chipselect;
  GPIOPin carddetect, busyled;

public:
  SDFatFs(SPIBus & spibus, GPIOPin cs, GPIOPin detect = PIN_NOT_DEFINED, GPIOPin busy = PIN_NOT_DEFINED) 
    : bus(spibus), chipselect(cs), carddetect(detect), busyled(busy) {}

  void mount(void) {
    f_mount(0, &fatfs);		/* Register volume work area (never fails) */

  }
  void unmount(void) {
    f_mount(0, NULL);
  }
};

class SDFile {
  FRESULT errcode;
  
public:
  FIL * file;

  SDFile(FIL * f) : file(f) {}
  
};

extern SDFatFs SD;

#endif
