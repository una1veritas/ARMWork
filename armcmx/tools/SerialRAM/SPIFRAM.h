/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 23K256/640
 modified by Sin

 */

#ifndef SerialFRAM_H
#define SerialFRAM_H

#include <Arduino.h>
#include <SPI.h>
//#include <inttypes.h>

class SerialFRAM {
private:
	const byte _cspin;
	uint32_t _address;

	static const uint32_t ADDRESS_BUS_MASK = 0x7fff;

// Op-codes
	enum OPCODE {
		FM25_WREN = B00000110, //Set Write Enable Latch 0000 0110b
		FM25_WRDI = B00000100, //Write Disable 0000 0100b
		FM25_RDSR = B00000101, //Read Status Register 0000 0101b
		FM25_WRSR = B00000001, //Write Status Register 0000 0001b
		FM25_READ = B00000011, //Read Memory Data 0000 0011b
		FM25_WRITE = B00000010, //Write Memory Data 0000 0010b
	};

	enum STATUSBIT {
		FM25_WEL = 1<<1,
		FM25_WPEN = 1<<7,
		FM25_BP1 = 1<<3,
		FM25_BP0 = 1<<2
	};

public:

	SerialFRAM(const byte cs) : _cspin(cs), _address(0) {}

	void init();
	void begin() { init(); }

	void setSPIMode(void) {
		SPI.setBitOrder(MSBFIRST);
		SPI.setClockDivider(SPI_CLOCK_DIV2);
		SPI.setDataMode(SPI_MODE0);
	}

	byte status();
	byte status(byte val);

	void read(const uint16_t & addr, byte *buffer, const uint16_t & size);
	byte read(const uint16_t & addr);
	void write(const uint16_t & address, byte *buffer, const uint16_t & size);
	void write(const uint16_t & address, byte data);


	inline void csLow() { digitalWrite(_cspin, LOW); }
	inline void csHigh() { digitalWrite(_cspin, HIGH); }
	inline void select(void) {
		setSPIMode();
		csLow();
	}
	inline void deselect(void) { csHigh(); }

};

#endif
