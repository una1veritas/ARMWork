#ifndef _STRINGSTREAM_H_
#define _STRINGSTREAM_H_

#include <stdint.h>
#include <string.h>
#include "armcmx.h"
#include "Printable.h"
#include "Stream.h"

class StringStream : public Printable {
  char * _string;
  uint16_t _size;
  uint16_t _count;
  
  uint16_t _readhead;
  
public:
  
  StringStream(char * buf, uint16_t size);

  void clear(void);
  void reset(void);

  char * string(void) { return _string; }

  size_t write(uint8_t c) ;
  size_t write(const char * str);
  
  inline boolean is_full(void) { return _count + 1 < _size; }
  inline int length(void) { return _count; }
  inline int available(void) { return _count - _readhead; }
  int read(void);
  int peek(void) { return _string[_readhead]; }
  void flush(void) { return clear(); }
  size_t getToken(char * dst, size_t maxlen);

  virtual size_t printTo(Print & pr) const {
    return pr.print(_string + _readhead);
  }
};

#endif  /* _STRINGSTREAM_H_ */
