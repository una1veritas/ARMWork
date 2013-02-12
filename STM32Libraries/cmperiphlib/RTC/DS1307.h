/*
 DS1307.h - library for DS1307/DS3231 i2c rtc
 */

// ensure this library description is only included once
#ifndef DS1307_h
#define DS1307_h

//#include <avr/pgmspace.h>
#include <string.h>
/*
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Wire.h>
*/
#include "cmcore.h"
#include "I2CWire.h"

// library interface description
class DS1307 {
	// library-accessible "private" interface
private:
//	I2CBuffer & i2cx;
	I2CWire & wire;

	void readRegisters(byte reg, uint8_t *, byte);
	void writeRegisters(byte reg, uint8_t *, byte);

	enum {
		DS1307_SEC = 0,
		DS1307_MIN,
		DS1307_HR,
		DS1307_DOW,
		DS1307_DATE,
		DS1307_MTH,
		DS1307_YR
	};

	static const uint8_t DS1307_CTRL_ID = B01101000; //DS1307

	// Define register bit masks
	static const uint8_t DS1307_CLOCKHALT = B10000000;

	static const uint8_t BITS_SEC = B01111111;
	static const uint8_t BITS_HR =  B00111111;
	static const uint8_t BITS_MIN = B01111111;
	static const uint8_t BITS_DOW = B00000111;
	static const uint8_t BITS_DATE =B00111111;
	static const uint8_t BITS_MTH = B00111111;
	static const uint8_t BITS_YR =  B11111111;

	static const int DS1307_BASE_YR = 2000;

//		uint8_t bcdToDec(uint8_t b) { return (b>>4)*10 + (b&0x0f); }
//		uint8_t decToBcd(uint8_t d) { return ((d/10)<<4) + (d%10); }

	// user-accessible "public" interface

public:
	//PROGMEM 
	const static char NameOfDay[36];
	//PROGMEM 
	const static char NameOfMonth[60];
	enum DAYINDEX {
		NA = 0, SUN = 1, MON, TUE, WED, THU, FRI, SAT,
	};

public:
	uint32 time, cal;

//	DS1307(I2CBuffer & wire) : i2cx(wire), time(0), cal(0) {}
	DS1307(I2CWire & i2cwire) : wire(i2cwire), time(0), cal(0) {}

	void init() {
		start();
	}
	void begin() {
		start();
	}
//		static int base_year() { return DS1307_BASE_YR; }

	boolean updateTime();
	void updateCalendar();
	uint8 getSeconds();
//		byte* getTimestamp(byte* );
	void setTime(const long &);
	void setCalendar(const long &);
//		long time();
//		long calendar();
//		void tweakMinutes(int);
	void writeRegister(byte, byte);
	byte readRegister(byte);

	void start(void);
	void stop(void);

	byte dayOfWeek();
	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);


	static char * copyNameOfDay(char * buf, byte d) {
		strcpy(buf, NameOfDay + d * 4);
		return buf;
	}

	static char * copyNameOfMonth(char * buf, byte m) {
		strcpy(buf, NameOfMonth + ((m+11) % 12) * 4);
		return buf;
	}

};

#endif

