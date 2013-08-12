
#ifndef ST7032i_h
#define ST7032i_h

#include <stdio.h>
//#include <stdlib.h>
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
#include <armcmx.h>
#include <gpio.h>

#include <Print.h>
#include <I2CBus.h>
#include "HD44780.h"
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
	I2CBus & wire;
	byte contrast;
	byte i2c_address;
	GPIOPin pin_bklight;
  GPIOPin pin_reset;

public:

//	void init(I2CBuffer * );
	ST7032i(I2CBus &, GPIOPin bklt = NOT_A_PIN, GPIOPin rst = NOT_A_PIN);
	bool begin();
  void reset();

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


	virtual void home();
	virtual void clear();
	virtual void setCursor(uint8_t c, uint8_t r);
	void setContrast(byte c);

	void createChar(uint8_t location, uint8_t charmap[]);

	inline void backlightHigh() { digitalWrite(pin_bklight, HIGH); }
	inline void backlightLow() { digitalWrite(pin_bklight, LOW); }

};

#endif
