/*
*
*
*/
#include <ctype.h>
#include "SDFatFs.h"

#if defined (CAPPUCCINO)
#include "cappuccino.h"
SDFatFs SD(SPI0, PIO0_2, SD_DETECT, SD_BUSYLED);

#elif defined (LPCLCD)
#include "lpclcd.h"
SDFatFs SD(SPI0, PIO0_2);

#endif

FRESULT SDFile::open(const TCHAR * fpath, BYTE mode) {
  ferr = f_open(&file, fpath, mode);
  fatfs.busyOn();
  return ferr;
}

FRESULT SDFile::close() {
  flush();
  ferr = f_close(&file);
  fatfs.busyOff();
  return ferr;
}

size_t SDFile::readBlock(void) {
  UINT n;
  ferr = f_read(&file, ring, BUFFER_SIZE, &n);
  rhead = 0;
  count = n;
  return (size_t) n;
}

int SDFile::read(void) {
  UINT n;
  if ( buffer_is_empty() ) {
    n = readBlock();
    if ( n == 0 )
      return -1;
  }
  return ring[rhead++];
}

int SDFile::peek(void) {
  UINT n;
  if ( buffer_is_empty() ) {
    n = readBlock();
    if ( n == 0 )
      return -1;
  }
  return ring[rhead];
}

size_t SDFile::write(uint8_t * p, size_t n) {
  UINT wn;
  ferr = f_write(&file, p, n, &wn);
  return wn;
}

size_t SDFile::getToken(char * t, size_t lim, const CHARCLASS sep) {
  size_t i = 0;
  bool isdelim;
  while ( i < lim ) {
    if ( buffer_is_empty() )
      readBlock();
    t[i] = ring[rhead++];
    switch(sep) {
      case SPACE:
        isdelim = isspace(t[i]);
        break;
      case EOL_NL:
      case EOL_CRNL:
        isdelim = (t[i] == '\n' ? true : false );
        if ( isdelim && sep == EOL_CRNL && (i > 0 && t[i-1] == '\r') )
          i--;
        break;
      case EOL_CR:
        isdelim = (t[i] == '\r' ? true : false );
        break;        
      default:
        isdelim = false;
      break;
    }
    if ( isdelim ) 
      break;
    i++;
  }
  t[i] = 0;
  return i;
}
