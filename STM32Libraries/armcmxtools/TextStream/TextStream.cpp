/*
 * TextStream.cpp
 *
 *  Created on: 2012/08/12
 *      Author: sin
 */

#include "TextStream.h"

void TextStream::printHex(const byte b) {
	write( ((b & 0xf0) < 0xa0 ? '0' : 'A'-10 ) + (b>>4) );
	write( ((b & 0x0f) < 0x0a ? '0' : 'A'-10 ) + (b&0x0f) );
}

void TextStream::printBytes(const byte * a, const int length, const char gap,
		byte base) {

	for (int i = 0; i < length;) {
		switch (base) {
		case HEX:
			printHex(a[i]);
			break;
		case BIN:
			print(a[i], BIN);
			break;
		default:
			print(a[i], DEC);
			break;
		}
		i++;
		if (gap && i < length)
			print(gap);
	}
	return;
}

void TextStream::printBytes(const char * s, const int length, const char gap) {
	for (int i = 0; i < length;) {
		if (isprint(s[i]))
			print(s[i]);
		else {
			print("$");
			print((byte) s[i], HEX);
		}
		i++;
		if (gap && i < length)
			print(gap);
	}
	return;
}

void TextStream::printWords(const word * a, const int length, const char gap) {
	for (int i = 0; i < length;) {
		printHex(a[i] >> 8);
		printHex(a[i]);
		i++;
		if (gap && i < length)
			print(gap);
	}
	return;
}
/*
 const char * printHex(const byte b) {
 sprintf(sbuf, "%02X", b);
 return sbuf;
 }

 const char * printHex(const word w) {
 sprintf(sbuf, "%04X", w);
 return sbuf;
 }
 */

int TextStream::scanBytes(char * str, byte * result, const int maxnum, const byte base) {
	char c;
	byte val;
	int n;
	for(n = 0; (*str != 0) && (n < maxnum); n++ ) {
		c = *str++;
		if ( !isxdigit(c) )
			continue;

		// lower nibble or upper nibble
		if ( c & 0x40 ) {
			val = (c & 0x4f) - 'A' + 10;
		} else {
			val = c - '0';
		}
		c = *str++;
		if ( isxdigit(c) ) {
			val <<= 4;
			if ( c & 0x40 ) {
				val |= (c & 0x4f) - 'A' + 10;
			} else {
				val |= c - '0';
			}
		}
		result[n] = val;
	}
	return n;
}


int TextStream::readToken(char buf[], long timeout) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	buf[bp] = 0;

	while (available() && (c = read())) {
		if (isspace(c)) {
			if (bp != 0)
				break;
		} else {
			msec = millis();
		}
		buf[bp++] = c;
		while (!available() && (millis() < timeout + msec))
			;
		if (!available())
			break;
	}
	buf[bp] = 0;
	return bp;
}

int TextStream::ithToken(const char buf[], const int item, int & fromix) {
	int tc, tend;
	for (tc = 0, tend = 0, fromix = 0; buf[fromix]; tc++) {
		for (fromix = tend; buf[fromix]; fromix++)
			if (!isspace(buf[fromix]))
				break;
		for (tend = fromix; buf[tend]; tend++)
			if (isspace(buf[tend]))
				break;
		if (item == tc)
			break;
	}
	return tend - fromix;
}

boolean TextStream::readLine(char buf[], const int startidx, const int maxlen, const long wait) {
	long msec = millis();
	int bp = startidx;
	char c;
	boolean lineEnded = false;

	while ( available() ) {
		c = read() & 0xff;
		if ( iscntrl(c) && (c != '\t') ) {
			lineEnded = true;
			break;
		}
		buf[bp++] = c;
		if ( !(bp < maxlen) ) {
			lineEnded = true;
			break;
		}
		//	buf[bp++] = (byte) c;
		if (millis() > wait + msec) {
			//Serial.print("cannot wait ");
			//Serial.println(millis() - msec);
			break;
		}
	}
	buf[bp] = 0;
	//   if the file is ending with non-control char, ...
	return lineEnded;
}

boolean TextStream::concateLine(char buf[], const int maxlen, const long wait) {
	int istart = 0;
	while (buf[istart] != 0 && istart < maxlen)
		istart++;
	return readLine(buf, istart, maxlen, wait);
}

