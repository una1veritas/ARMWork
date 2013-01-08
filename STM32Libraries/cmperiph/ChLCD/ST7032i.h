
#ifndef ST7032i_h
#define ST7032i_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef ARDUINO
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include "CharacterLCD.h"
#else
#include <armcore.h>
#include <gpio.h>

#include <Print.h>
#include <I2CWire.h>
#include "ChLCD/CharacterLCD.h"
#endif



class ST7032i : public CharacterLCD {
	static const byte DEFAULT_I2C_ADDRESS = B0111110;

	uint8_t _displayfunction;
	  uint8_t _displaycontrol;
	  uint8_t _displaymode;
	  uint8_t _initialized;
	  uint8_t _numlines, _numcolumns;
	  uint8_t _position;
//
//	I2CBuffer * wirex;
	I2CWire & wirex;
	byte contrast;
	byte i2c_address;
	GPIOPin pin_bklight;

public:

//	void init(I2CBuffer * );
	ST7032i(I2CWire &);
	void begin();

	using CharacterLCD::write;

  virtual void send(uint8_t value, uint8_t dcswitch);

	//#if ARDUINO >= 100
//  virtual size_t println(void) { wrap(); return 1; }
//#else
//  virtual void write(uint8_t);
//  virtual void println(void) { wrap(); }
//#endif
//	void wrap();
//	void ST7032i_clearLine(ST7032i * lcd);

	uint8_t row() {
		uint8_t tp = 0;
		if( _position & 0x40 )
			tp = 1;
		if ( (_position & ~0x40) >= 0x14 )
			tp += 2;
		return tp;
	}	
	uint8_t column() {
		uint8_t tp = _position & ~0x40;
		return tp % 0x14;
	}


	void home();
	void clear();
	void setCursor(uint8_t c, uint8_t r);
	void setContrast(byte c);

	void createChar(uint8_t location, uint8_t charmap[]);

	//	void ST7032i_backlightOn(ST7032i * lcd); // { digitalWrite(pin_bklight, LOW); }
	//	void ST7032i_backlightOff(ST7032i * lcd); // { digitalWrite(pin_bklight, HIGH); }
};

#endif
