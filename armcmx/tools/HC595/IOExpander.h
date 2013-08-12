/*
 *  IOExpander.h
 *  
 *
 *  Created by Sin Shimozono on 11/07/13.
 *  Copyright 2011 . All rights reserved.
 *
 */
#ifndef IOExpander_h
#define IOExpander_h

//#include <inttypes.h>
#if ARDUINO >= 100
  #include "Arduino.h"
#else
   #include "WPrograms.h"
#endif

class IOExpander {
public:
	IOExpander() {}
	IOExpander(const byte address) {}

	virtual void init() = 0;
	void begin() { init(); }

	virtual byte read() = 0;
	virtual byte read(const byte addr) { return read(); }
	virtual byte write(const byte data) = 0;
	virtual byte write(const byte addr, const byte data) { return write(data); }
	virtual byte transfer(const byte data) = 0;

//	virtual void pinMode(const byte bitaddr, const byte mode) = 0;

	inline
	virtual byte digitalRead(const byte bitaddr) {
		return bitRead(read(bitaddr>>3), bitaddr&0x07);
	}

	inline
	virtual void digitalWrite(const byte bitaddr, const byte bitval) {
		byte val = read(bitaddr>>3);
		write(bitaddr>>3, bitWrite(val, bitaddr&0x07, bitval));
	}
};

#endif // IOExpander_h
