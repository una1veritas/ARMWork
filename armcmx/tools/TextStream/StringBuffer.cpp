
#include <ctype.h>
#include "StringBuffer.h"

StringBuffer::StringBuffer(char * buf, int size) {
  _string = buf;
  _size = (uint16_t) size;
  clear();
}

void StringBuffer::clear(void) {
  _count = 0;
  _readhead = 0;
  _string[0] = 0;
}

void StringBuffer::reset(void) {
  _count = strlen(_string);
  _readhead = 0;
}

int StringBuffer::read(void) {
  if ( _readhead < _count )
    return _string[_readhead++];
  return -1;
}

size_t StringBuffer::write(uint8_t c) {
  if ( _count + 1 < _size )
    _string[_count++] = c;
  _string[_count] = 0;
  return 1;
}

size_t StringBuffer::write(const char * str) {
  size_t n = 0;
  while ( str[n] && _count + 1 < _size ) 
    _string[_count++] = str[n++];
  _string[_count] = 0;
  return n;
}

/*
 *
 */

size_t StringBuffer::getToken(char * dst, size_t maxlen) {
	int c;
	size_t n = 0;
  
	while (1) {
		c = _string[_readhead];
		if (iscntrl(c) or isspace(c)) {
			_readhead++; // skip control chars and space chars
			continue;
		}
		break;
	};
	for (; n < maxlen && _readhead < _count ; n++) {
		c = _string[_readhead++];
		if (c == 0)
			break;
		dst[n] = c & 0xff;
		if ( iscntrl(c) or isspace(c) ) {
			if (c == '\r') {
				c = _string[_readhead] & 0xff;
				if (c == '\n')
					_readhead++;
			}
			break;
		}
	}
	dst[n] = 0;
	return n;
}

