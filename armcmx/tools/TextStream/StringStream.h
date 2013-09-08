#ifndef _STRINGSTREAM_H_
#define _STRINGSTREAM_H_

#include <stdint.h>
#include <string.h>
#include "armcmx.h"

class StringStream {
  char * _string;
  uint16_t _size;
  uint16_t _count;
  
  uint16_t _readhead;
  
public:
  
  StringStream(char * buf, uint16_t size);

  void clear(void);
  void reset(void);

  size_t write(const char c) ;
  size_t write(const char * str);
  
  inline boolean is_full(void) { return _count + 1 < _size; }
  inline uint16_t length(void) { return _count; }
  inline uint16_t available(void) { return _count; }
  int read(void);

  size_t getToken(char * dst, size_t maxlen);

};

#endif  /* _STRINGSTREAM_H_ */
