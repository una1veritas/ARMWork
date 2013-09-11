#ifndef _SDFATFS_H_
#define _SDFATFS_H_

 /*
  * Wrapper for FatFs by ChaN
  */

#include "armcmx.h"
#include "Print.h"
#include "Stream.h"

#include "SPI.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "FatFs/diskio.h"
#include "FatFs/ff.h"

#ifdef __cplusplus
}
#endif

// volume
class SDFatFs {
  SPIBus & spibus;

//  static volatile DSTATUS Stat;	/* Physical drive status */
//  static volatile UINT Timer1, Timer2;	/* 1kHz decrement timer stopped at zero (disk_timerproc()) */
//  static BYTE CardType;			/* Card type flags */

  GPIOPin pin_cs;
  GPIOPin pin_detect;
  GPIOPin pin_busyled;
  
  //
  FATFS fatfs;		/* File system object */

  static uint32 time;
  static uint32 cal;
  
public:
  FRESULT rescode;
  
  static uint32 fattime();
  static void settime(uint32 t, uint32 c) { time = t; cal = c; }

public:
  SDFatFs(SPIBus & bus, GPIOPin cs, GPIOPin detect = PIN_NOT_DEFINED, GPIOPin led = PIN_NOT_DEFINED) 
    : spibus(bus), pin_cs(cs), pin_detect(detect), pin_busyled(led) {}
  
  void begin(void);
  void end() { spibus.end(); }
      
  inline void setClockDivider(uint8 div) { spibus.setClockDivider(div); }
  inline uint16 transfer(uint16 d) { return spibus.transfer(d); }
  
  int wait_ready (uint32 wt) {
    wt += millis();
    while ( wt > millis() ) {
      if ( transfer(0xff) == 0xff )
        return 1;
    }
    return 0;
  }

  inline void deselect() { 
    digitalWrite(pin_cs, HIGH);
    spibus.transfer(0xFF);
  }
  inline uint8 select() { 
    digitalWrite(pin_cs, LOW);
    spibus.setClockDivider(SPI_CLOCK_DIV4);
    spibus.transfer(0xFF);	/* Dummy clock (force DO enabled) */
    if ( wait_ready(500) ) //wait_ready(500)) 
      return 1;	/* OK */
    deselect();
    return 0;	/* Timeout */
  }
  inline void setDataSize(uint32 dss) { spibus.setDataSize(dss); } 
};


class SDFatFile : public Stream {
  FIL file;			/* File object */
  SDFatFs & sdfs;
  
  int16_t rbuf;
  boolean peeked;
  
//  uint8_t readbuff[], writebuff[];

public:
  SDFatFile(SDFatFs & sdfs) : sdfs(sdfs) {}

  FRESULT result(void) { return sdfs.rescode; }
    
  void open(const char * fname, const uint8_t mode);
  char * gets(char * buff, size_t sz);
  void close(void);
  void seek(uint16_t offset);
  
  inline virtual size_t write(uint8_t c) {
    return write(&c, 1);
  }
  virtual size_t write(const uint8_t * buf, size_t num);
  inline virtual size_t write(char * str) {
    return write((const uint8_t *) str, strlen(str));
  }
  using Print::write;
  
  virtual int available() {
    if ( f_eof(&file) )
      return 0;
    return f_size(&file) - f_tell(&file);
  }
  virtual int read(void);
  virtual int peek(void);

  virtual void flush() {
    sdfs.rescode = f_sync(&file);
  }
};

extern SDFatFs SD;

#endif
