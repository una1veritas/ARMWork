/*
 * TextStream.h
 *
 *  Created on: 2012/08/07
 *      Author: sin
 */

#ifndef TEXTSTREAM_H_
#define TEXTSTREAM_H_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wiring.h>
#endif
#include <Stream.h>
// stdlib.h is included in Arduino.h

const char endl = '\n';
const char cr = '\r';
const char tab = '\t';

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

class TextStream : public Stream {
	Stream & stream;
//	char * sbuf;

	void printHex(const byte b);

public:
    virtual inline size_t write(uint8_t b) { return stream.write(b); }
    using Print::write;

    virtual int available() { return stream.available(); }
    virtual inline int read() { return stream.read(); }
    virtual inline int peek() { return stream.peek(); }
    virtual void flush() { stream.flush(); }

    TextStream(Stream & s) : stream(s) {
	}
    ~TextStream() { }

	using Print::print;

	void printBytes(const byte * a, const int length, char gap = ' ', byte base = HEX);
	void printBytes(const char * s, const int length, char gap = 0x00);
	void printWords(const word * a, const int length, char gap = ' ');

	int readToken(char buf[], long timeout = 200);
	boolean readLine(char buf[], const int startidx, const int maxlen, const long wait = 10);
	inline boolean readLine(char buf[], const int maxlen, const long wait = 10) {
		return readLine(buf, 0 , maxlen, wait);
	}
	boolean concateLine(char buf[], const int maxlen, const long wait = 10);

	static int scanBytes(char * str, byte * result, const int maxlen, const byte base = HEX);
	static int ithToken(const char buf[], const int item, int & fromindex);
};

#endif /* TEXTSTREAM_H_ */
