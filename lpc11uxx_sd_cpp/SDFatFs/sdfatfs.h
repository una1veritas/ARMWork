#ifndef _SDFATFS_H_
#define _SDFATFS_H_

#include "armcmx.h"

typedef 
struct SDFatFs {
  GPIOPin cs;
  GPIOPin detect;
  GPIOPin led;
  
  FATFS fatfs;		/* File system object */
  FIL file;			/* File object */
  uint8_t buff[32];
  
  FRESULT errcode;
  uint32_t time, cal;
} 
SDFatFs;

static SDFatFs SD;

#endif
