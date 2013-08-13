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

static uint32_t FatFsTimeStamp;

// volume
class SDFatFs {
  SPIBus & spibus;
  //
  FATFS fatfs;		/* File system object */
  
public:
  FRESULT rescode;
  
  static uint32_t fattime(uint32_t cal, uint32_t time);

public:
  SDFatFs(SPIBus & bus) : spibus(bus) {}
  
  void begin(void) {
    f_mount(0, &fatfs);		/* Register volume work area (never fails) */
  }
  
};

class SDFatFile : public Stream {
  FIL file;			/* File object */
  SDFatFs & sdfs;
  
  int16_t rbuf;
  boolean peeked;
  
//  uint8_t readbuff[], writebuff[];
public:
  const static BYTE FILE_READ =  FA_OPEN_EXISTING | FA_READ;
  const static BYTE FILE_WRITE = FA_OPEN_ALWAYS | FA_READ | FA_WRITE;

public:
  SDFatFile(SDFatFs & sdfs) : sdfs(sdfs) {}

    FRESULT result(void) { return sdfs.rescode; }
    
  void open(const char * fname, const uint8_t mode = FILE_READ) {
    peeked = false;
  	sdfs.rescode = f_open(&file, fname, mode);
    if (mode == FILE_WRITE && !sdfs.rescode ) {
      sdfs.rescode = f_lseek(&file, f_size(&file));
    }
  }
  
  char * gets(char * buff, size_t sz) {
    if ( peeked ) {
      peeked = false;
      *buff = rbuf;
      return f_gets((TCHAR*)buff+1, sz-1, &file);      
    } else {
      return f_gets((TCHAR*)buff, sz, &file);
    }
  }
  
  void close(void) {
    sdfs.rescode = f_close(&file);
  }
  
  inline virtual size_t write(uint8_t c) {
    return write(&c, 1);
  }
  virtual size_t write(const uint8_t * buf, size_t num);
  inline virtual size_t write(char * str) {
    return write((const uint8_t *) str, strlen(str));
  }
  using Print::write;
  
  virtual size_t available() {
    if ( f_eof(&file) )
      return 0;
    return f_size(&file) - f_tell(&file);
  }

  virtual int16_t read() {
    UINT n = 0;
    if ( !peeked )
      sdfs.rescode = f_read(&file, &rbuf, 1, &n);
    peeked = false;
    return rbuf;
  }
  virtual int16_t peek() {
    UINT n = 0;
    if ( !peeked ) 
      sdfs.rescode = f_read(&file, &rbuf, 1, &n);
    peeked = true;
    return rbuf;
  }

  virtual void flush() {
    sdfs.rescode = f_sync(&file);
  }
};

#endif
