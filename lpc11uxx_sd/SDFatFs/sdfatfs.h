#ifndef _SDFATFS_H_
#define _SDFATFS_H_

#include "armcmx.h"

typedef 
struct SDFatFs {
  GPIOPin cs;
  GPIOPin detect;
  GPIOPin led;
  
  FATFS Fatfs;		/* File system object */
  FIL Fil;			/* File object */
  uint8_t buff[32];
} 
SDFatFs;

#endif
