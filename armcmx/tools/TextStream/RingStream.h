/*
 * RingStream.h
 *
 *  Created on: 2013/05/11
 *      Author: sin
 */

#ifndef RINGSTREAM_H_
#define RINGSTREAM_H_

#if defined (ARDUINO)
#include <Arduino.h>
#elif defined (ARMCMX)
#include "armcmx.h"
#endif
#include <Stream.h>
#include <Printable.h>

class RingStream: public Stream, public Printable {

	// members
private:
	uint8_t * buffer_array;
	uint16_t buffer_size;
	uint16_t windex, rindex;
	uint16_t count;

public:

  RingStream(char buf[], size_t lim);
//	virtual ~StringStream() {}

  void clear();

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
