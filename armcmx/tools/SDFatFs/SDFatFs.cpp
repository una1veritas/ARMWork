/*
*
*
*/
#include <ctype.h>
#include "SDFatFs.h"

/*
#if defined (CAPPUCCINO)
#include "cappuccino.h"
SDFatFs SD(SPI0, PIO0_2, SD_DETECT, SD_BUSYLED);

#elif defined (LPCLCD)
#include "lpclcd.h"
SDFatFs SD(SPI0, PIO0_2);

#endif
*/

FRESULT SDFile::open(const TCHAR * fpath, BYTE mode) {
  error = f_open(&file, fpath, mode);
  fatfs.busyOn();
  return error;
}

FRESULT SDFile::close() {
  flush();
  error = f_close(&file);
  fatfs.busyOff();
  return error;
}

/*
size_t SDFile::readBlock(void) {
  UINT n;
  ferr = f_read(&file, ring, BUFFER_SIZE, &n);
  rhead = 0;
  count = n;
  return (size_t) n;
}
*/

int SDFile::read(void) {
  UINT n;
  char buf;
  error = f_read(&file, &buf, 1, &n);
  if ( n == 1 ) {
    return (int) buf;
  }
  return -1;
}

int SDFile::peek(void) {
  int c = read();
  f_lseek(&file, f_tell(&file)-1);
  return c;
}


size_t SDFile::write(const uint8_t c) {
  return ( f_putc((TCHAR)c, &file) == 1 ? 1 : 0); 
}

size_t SDFile::write(const uint8_t * p, size_t n) {
  UINT wn;
  error = f_write(&file, p, n, &wn);
  return wn;
}

bool SDFile::eof(void) {
  // both cache and file is at the end.
  return f_eof(&file);
}

int SDFile::available(void) {
  if ( f_eof(&file) ) 
    return 0;
  return f_size(&file) - f_tell(&file);
}

