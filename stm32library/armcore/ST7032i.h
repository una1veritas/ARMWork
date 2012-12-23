
#ifndef ST7032i_h
#define ST7032i_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#if ARDUINO >= 100
#include <stdio.h>
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include "Print.h"
*/
#include "armcore.h"
#include "gpio.h"

#include "CharacterLCD.h"
#include "Stream.h"

class ST7032i : public Print {
	static const byte DEFAULT_I2C_ADDRESS = B0111110;

	uint8_t _displayfunction;
	  uint8_t _displaycontrol;
	  uint8_t _displaymode;
	  uint8_t _initialized;
	  uint8_t _numlines, _numcolumns;
	  uint8_t _position;
//
	I2CBuffer * wirex;
	byte contrast;
	byte i2c_address;
	GPIOPin pin_bklight;

	void command(byte value);

public:

	void init(I2CBuffer * );
	void begin();

   virtual size_t write(const uint8_t);
	using Print::write;
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
	void noDisplay();
	void setContrast(byte c);

	void display();
	void noBlink();
	void blink();
	void noCursor();
	void showCursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void autoscroll();
	void noAutoscroll();
	void createChar(uint8_t location, uint8_t charmap[]);

	//	void ST7032i_backlightOn(ST7032i * lcd); // { digitalWrite(pin_bklight, LOW); }
	//	void ST7032i_backlightOff(ST7032i * lcd); // { digitalWrite(pin_bklight, HIGH); }
};

#endif
