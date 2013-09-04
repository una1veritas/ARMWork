/*
 * StringStream.h
 *
 *  Created on: 2013/05/11
 *      Author: sin
 */

#ifndef STRINGSTREAM_H_
#define STRINGSTREAM_H_

#if defined (ARDUINO)
#include <Arduino.h>
#elif defined (ARMCMX)
#include "armcmx.h"
#endif
#include <Stream.h>
#include <Printable.h>

	// constants
static const char endl = '\n';
static const char cr = '\r';

class StringStream: public Stream, public Printable {

	// members
private:
	uint8_t * buffer_array;
	uint16_t buffer_size;
	uint16_t windex, rindex;
	uint16_t count;

public:
	StringStream(char buf[], size_t lim);
//	virtual ~StringStream() {}

	void set(char str[], uint16_t sz) {
		buffer_array = (uint8_t *) str;
		buffer_size = sz;
		windex = strlen(str); rindex = 0;
		count = windex;
	}
  
  using Stream::write;
  
  virtual size_t write(uint8_t b);
  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);

  size_t readLineFrom(Stream &, size_t);
  size_t readLineFrom(char *, size_t);

  uint8_t is_full();

  size_t getString(char * dst, size_t maxlen);
  size_t getToken(char * dst, size_t maxlen);

  uint32_t parseHex();

  virtual void flush();
  uint16_t writeindex() { return windex; }
  uint16_t readindex() { return rindex; }
  size_t length() { return count; }

  using Stream::print;

  virtual size_t printTo(Print& p) const;

//	size_t readLine(char *buffer, size_t length);

};


#endif /* STRINGSTREAM_H_ */
