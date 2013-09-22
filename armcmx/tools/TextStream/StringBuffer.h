#ifndef _STRINGBUFFER_H_
#define _STRINGBUFFER_H_

#include <stdint.h>
#include <string.h>
#include "armcmx.h"
#include "Printable.h"
#include "Stream.h"

class StringBuffer : public Print, public Printable {
  char * _string;
  uint16_t _size;
  uint16_t _count;
  
  uint16_t _readhead;
  
public:
  
  StringBuffer(char * buf, int size);

  void clear(void);
  void reset(void);

  char * string(void) { return _string; }

  virtual size_t write(uint8_t c) ;
  virtual size_t write(const char * str);
  using Print::write;

  inline boolean is_full(void) { return !(_count + 1 < _size); }
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

#endif  /* _STRINGBUFFER_H_ */
