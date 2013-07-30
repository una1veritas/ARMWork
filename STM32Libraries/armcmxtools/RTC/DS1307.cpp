/*
#if ARDUINO >= 100
#include "Wire.h"
#else
extern "C" {
#include "Wire.h"
}
#endif
 */
#include "DS1307.h"
#include "i2c.h"
//#include <Wire.h>


//PROGMEM 
const char DS1307::NameOfDay[]=
"Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
//PROGMEM 
const char DS1307::NameOfMonth[]=
"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";

// PRIVATE FUNCTIONS

// Aquire data from the RTC chip in BCD format
// refresh the buffer
void DS1307::readRegisters(uint8_t addr, uint8_t * regvals, uint8_t num) {
	// use the Wire lib to connect to tho rtc
	// reset the resgiter pointer to zero

	wire.beginTransmission(DS1307_CTRL_ID);
//#if ARDUINO >= 100
//    WIRE_WRITE(addr); //Wire.send(addr);
 	wire.write(addr+chipID);
//#else
 //	Wire.send(addr);
//#endif
	wire.endRequest();
	// request num bytes of data
	wire.receiveFrom((uint8_t)DS1307_CTRL_ID, num);
	for(int i = 0; i < num; i++) {
		// store data in raw bcd format
//#if ARDUINO >= 100
		*regvals++ = wire.read(); //Wire.receive();
//#else
//		*regvals++ = Wire.receive(); //Wire.receive();
//#endif
	}
	
//	i2c_request(&i2cx, DS1307_CTRL_ID, &addr, 1);
//	i2c_receive(&i2cx, regvals, num);
}

// update the data on the IC from the bcd formatted data in the buffer
void DS1307::writeRegisters(uint8_t addr, uint8_t *regvals, uint8_t num)
{
	wire.beginTransmission(DS1307_CTRL_ID);
//#if ARDUINO >= 100
	wire.write(addr+chipID); // reset register pointer
//#else
//	Wire.send(addr); // reset register pointer
//#endif
	for(int i=0; i<num; i++) {
//#if ARDUINO >= 100
		wire.write(*regvals++);
//#else
//		Wire.send(*regvals++);
//#endif
	}
	wire.endTransmission();

//	uint8 buf[8];
//	buf[0] = addr;
//	memcpy(buf+1, regvals, num);
//	i2c_transmit(&i2cx, DS1307_CTRL_ID, buf, num+1);
}


boolean DS1307::updateTime() {
	uint32 tmp = time;
	readRegisters((byte) DS1307_SEC, (byte *) &tmp, 3);
	if (tmp != time) {
		time = tmp & ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC);
		return true;
	}
	return false;
}

boolean DS1307::updateCalendar() {
  uint32 tmp = cal;
	readRegisters((byte) DS1307_DATE, (byte *) &cal, 3);
	cal &= ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE);
  if ( tmp != cal ) 
    return true;
  return false;
}

uint8 DS1307::getSeconds() {
	uint8 sec;
	readRegisters((byte) DS1307_SEC, (byte *) &sec, 1);
	return sec & BITS_SEC;
}

void DS1307::setTime(const long & p) {
//	writeRegisters((byte *) &(p ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC)),
//			(byte) DS1307_SEC, 3);
	writeRegisters((byte) DS1307_SEC, (byte *) &p, 3);
}

void DS1307::setCalendar(const long & p) {
	// YYMMDD
//	writeRegisters((byte*) &(p & ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE)), (uint8_t) DS1307_DOW, 4);
	writeRegisters((uint8_t) DS1307_DATE, (byte*) &p, 3);
}

byte DS1307::dayOfWeek() {
	return  (JD2000(cal) + 1) % 7;
}

long DS1307::JD2000(const long & yymmdd) {
	byte y = yymmdd>>16 & 0xff;
	byte m = yymmdd>>8 & 0xff;
	byte d = yymmdd & 0xff;
	y = 10* (y>>4&0x0f) + (y&0x0f);
	m = 10 * (m>>4&0x0f) + (m&0x0f);
	d = 10 * (d>>4&0x0f) + (d & 0x0f);
	return JD2000(y,m,d);
}

long DS1307::JD2000(byte year, byte month, byte day) {
	/* year must be after the millenium 2000 */
	/*
	Serial.println(year, DEC);
	Serial.println(month, DEC);
	Serial.println(day, DEC);
	*/
	if (month <= 2) {
		year--;
		month += 12;
	}
	int a = int(year / 100);
	int b = 2 - a + int(a / 4);
	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
			+ 2451479.5 + 0.5 + b); // + 0.5 for getting the day of that day's noon
//	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
//			+ 1720994.5 + b);
}

long integerPart(float df) {
	return (long int) df;
}

float fractionalPart(float df) {
	return df - ((long int) df);
}

long sign(float d) {
	if ( d < 0 )
		return -1;
	else
		return 1;
}

float DS1307::CalendarDate(float jd) {
	jd += 0.5f;
	long z = integerPart(jd);
	long a = z;
	float f = fractionalPart(jd);
	if ( z >= 2299161 ) {
		long alpha = integerPart( (z-1867216.25f)/36524.25f );
		a += 1 + alpha - integerPart(alpha/4);
	}
	long b = a + 1524;
	long c = integerPart( (b-122.1f)/365.25f );
	long d = integerPart(365.25f * c);
	long e = integerPart( (b-d)/30.6001f );
	long day = b - d - integerPart(30.6001f * e) + f;
	int month;
	if ( e < 13.5f ) {
		month = e - 1;
	} else {
		month = e-13;
	}
	long year;
	if ( month > 2.5f) {
		year = c - 4716;
	} else {
		year = c - 4715;
	}
  if ( year > 0 )
    return year*10000 + month*100 + day + f;
  else {
    year = -year;
    return -1*(year*10000 + month*100 + day + f);
  }
}


void DS1307::writeRegister(byte rg, byte val) {
	writeRegisters(rg % 0x40, (uint8_t *) &val, 1);
}

byte DS1307::readRegister(byte rg) {
	byte val;
	readRegisters(rg % 0x40, (uint8_t *) &val, 1);
	return val;
}


void DS1307::stop(void)
{
	// set the ClockHalt bit high to stop the rtc
	// this bit is part of the seconds byte
  uint8_t r;
	readRegisters((uint8_t) DS1307_SEC, &r, 1);
	r |= DS1307_CLOCKHALT;
  writeRegisters(DS1307_SEC, &r, 1);
}

void DS1307::start(void)
{
	// unset the ClockHalt bit to start the rtc
	// TODO : preserve existing seconds
  uint8_t r;
	readRegisters((uint8_t) DS1307_SEC, &r, 1);
	r &= ~DS1307_CLOCKHALT;
	writeRegisters(DS1307_SEC, &r, 1);
}
