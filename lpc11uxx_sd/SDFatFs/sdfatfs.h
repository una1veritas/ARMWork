#ifndef _SDFATFS_H_
#define _SDFATFS_H_

#include <stdint.h>
#include "ff.h"
#include "mmc_ssp.h"

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

  void begin(void) {
    pinMode(chipselect, OUTPUT);
    digitalWrite(chipselect, HIGH);
    if ( carddetect != PIN_NOT_DEFINED) 
      pinMode(carddetect, INPUT);
    if ( busyled != PIN_NOT_DEFINED) { 
      pinMode(busyled, OUTPUT);
      busyOff();
    }
  }
  
  void set_datetime(uint32_t d, uint32_t t) {
    date = d;
    time = t;
  }

  void mount(void) {
    f_mount(0, &fatfs);		/* Register volume work area (never fails) */

  }
  void unmount(void) {
    f_mount(0, NULL);
  }
  
  void busyOn(void) {
    if ( busyled != PIN_NOT_DEFINED ) 
      digitalWrite(busyled, LOW);
  }

  void busyOff(void) {
    if ( busyled != PIN_NOT_DEFINED ) 
      digitalWrite(busyled, HIGH);
  }

};

class SDFile {
  FIL file;
  FRESULT ferr;
  SDFatFs & fatfs;
  
  static const int BUFFER_SIZE = 64;
  char ring[BUFFER_SIZE];
  uint16 rhead, count;
  
  size_t readBlock(void);

  public:

  SDFile(SDFatFs & fs) : fatfs(fs) {
    rhead = 0;
    count = 0;
  }
  
  FIL * operator() (void) { return &file; }
  FRESULT error(void) { return ferr; }
  
  FRESULT open(const TCHAR * fpath, BYTE mode);
  FRESULT close();
 
  int read(void);
  int peek(void);
  
  size_t write(uint8_t * p, size_t n);
  
  void flush(void) { f_sync(&file); }
  
  void seek(int32_t ofs) { f_lseek(&file, (DWORD)ofs); }
  size_t size() { return f_size(&file); }
  /* *** */
  enum CHARCLASS {
    EOL_CRNL = 0,
    EOL_NL = 0x0A,
    EOL_CR = 0x0D,
    SPACE = 0x20,
  };
  size_t getToken(char * t, size_t lim, const CHARCLASS sep = SPACE);
  size_t getLine(char * t, size_t lim, const CHARCLASS sep = EOL_NL) { return getToken(t, lim, sep); }
  
  bool buffer_is_full() { return count == BUFFER_SIZE; }
  bool buffer_is_empty() { return !(rhead < count); }
};

extern SDFatFs SD;

#endif
