/*
 * StringStream.cpp
 *
 *  Created on: 2013/05/11
 *      Author: sin
 */

#include <ctype.h>
#include "StringStream.h"

void StringStream::init(char str[], uint16_t n) {
  int16_t i;
  buffer_array = str;
  buffer_size = n;
  for(i = 0; i < n && buffer_array[i]; i++);
  count = i;
}

StringStream::StringStream(char buf[], size_t n) {
  init(buf, n);
	setTimeout(0);
}

uint8_t StringStream::is_full() {
	return !(count < buffer_size);
}

size_t StringStream::write(uint8_t b) {
	if ( is_full() ) 
		return 0;
	buffer_array[count++] = b;
	return 1;
}

int StringStream::available() {
	return count;
}

int StringStream::read() {
	int c;
	if ( count == 0 )
		return -1;
	c = buffer_array[0];
  for(count = 0; count < buffer_size; count++) {
    buffer_array[count] = buffer_array[c+1];
    if ( !buffer_array[count] )
      break;
  }
	return c;
}

int StringStream::peek() {
	if ( count == 0 )
		return -1;
	return buffer_array[0];
}

size_t StringStream::readLineFrom(Stream & src, size_t maxsize) {
	int c;
	size_t n = 0;
	for (; n < maxsize; n++) {
		c = src.read();
		if (c == -1) // stream ended without neither cr nor endl
			break;
		if ( is_full() )
			break;
		if (c == cr) {
			c = char( src.peek() );
			if (c == nl)
				src.read(); // read to skip
			//dst[n] = 0;
			break;
		} else if (c == nl) {
			//dst[n] = 0;
			break;
		} else {
			write((char) c);
		}
	}
	//dst[n] = 0;
	return n;
}


size_t StringStream::readLineFrom(char * src, size_t maxsize) {
	char c;
	size_t n = 0;
	for (; n < maxsize - 1; n++) {
		c = src[n];
    if ( c == 0 )
      break;
		if ( is_full() )
			break;
		if (c == cr) {
			if (src[n+1] == nl)
        n++;
			break;
		} else if (c == nl) {
			break;
		} else {
			write(c);
		}
	}
	return n;
}


size_t StringStream::getString(char * dst, size_t maxlen) {
	int i;
	for (i = 0; i < maxlen && count > 0; i++) {
		dst[i] = read();
	}
	dst[i] = 0;
	return count;
}

size_t StringStream::getToken(char * dst, size_t maxlen) {
	int c;
	size_t n = 0;
	do {
		c = peek();
		if (iscntrl(c) or isspace(c)) {
			read(); // skip control chars and space chars
			continue;
		}
		break;
	} while (c != -1);
	for (; n < maxlen; n++) {
		c = read();
		if (c == -1)
			break;
		dst[n] = c & 0xff;
		if ( iscntrl(c) or isspace(c) ) {
			if (c == '\r') {
				c = peek() & 0xff;
				if (c == '\n')
					read();
			}
			break;
		}
	}
	dst[n] = 0;
	return n;
}

boolean ishexdigit(const char c) {
	return isdigit(c) or ('a' <= c && c <= 'f') or ('A' <= c && c<= 'F');
}

uint32_t StringStream::parseHex() {
	uint32_t tmp = 0;
	int c;

	while ( available() && !ishexdigit(peek()) ) {
		read();
	}
	if ( !available() )
		return 0;
	while (available()) {
		c = read();
		if ( c == -1 ) // not available()
			break;
//		Serial.print((char)c);
		c = toupper(c);
		if ( c >= '0' and c <= '9' ) {
			tmp = (tmp<<4) + c - '0';
		} else if ( c >= 'A' and c <= 'F' ) {
			tmp = (tmp<<4) + c - 'A' + 10;
		} else {
			break;
		}
//		Serial.print(t);
//		Serial.print(", ");
	}
	return tmp;
}

void StringStream::flush() {
	count = 0;
  buffer_array[count] = 0;
}

size_t StringStream::printTo(Print& p) const {
  p.print(buffer_array);
	return count;
}

