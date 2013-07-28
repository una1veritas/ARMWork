/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 23K256/640
 modified by Sin

 */

#ifndef SPISRAM_H
#define SPISRAM_H

#if defined (ARDUINO)
#include <Arduino.h>
#include <SPI.h>
#elif defined (ARMCMX)
#include "armcmx.h"
#include "SPIBus.h"
#endif

class SPISRAM {
private:
  SPIBus & SPIx;
	const byte _csPin;
	const byte _addrbus;
//	volatile long addr;
//	byte clock_divider;
//	byte spi_mode;
//	byte status_cache;

// INSTRUCTION SET
	static const byte READ = 0x03; // Read data from memory
	static const byte WRITE = 0x02; // Write data to memory
	// EDIO
	// EQIO
	// RSTIO
	static const byte RDSR = 0x05; // Read Status register
	static const byte WRSR = 0x01; // Write Status register

	// STATUS REGISTER
	static const byte BYTE_MODE = 0x00;
	static const byte PAGE_MODE = 0x80;
	static const byte SEQ_MODE = 0x40;

	void set_access(const byte mode, const long & address) {
		SPIx.transfer(mode);
		if (_addrbus == BUS_MBITS)
			SPIx.transfer(address >> 16 & 0xff);
		SPIx.transfer(address >> 8 & 0xff);
		SPIx.transfer(address & 0xff);
	}

	void writeStatusRegister(byte stat) {
		SPIx.transfer(WRSR);
		SPIx.transfer(stat);
	}

	byte readStatusRegister() {
		return SPIx.transfer(RDSR);
	}

public:
	enum {
		BUS_23K256 = 16, // 23K256
		BUS_23K640 = 16,  // 23K640
		BUS_23LC1024 = 24, // 23A/LC1024
		BUS_KBITS = 16,
		BUS_MBITS = 24
	};

	SPISRAM(SPIBus & spi, const byte csPin, const byte addrwidth = BUS_23K256);

	void init();
	inline void begin() {
		init();
	}
	inline void setSPIMode();

	byte read(const long & address);
	void read(const long & address, byte *buffer, const long & size);
	void write(const long & address, byte data);
	void write(const long & address, byte *buffer, const long & size);

	inline void csLow();
	inline void csHigh();
	inline void select(void);
	inline void deselect(void);
};

#endif
