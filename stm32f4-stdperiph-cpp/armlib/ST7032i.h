
#ifndef ST7032i_h
#define ST7032i_h

#include <stdint.h>

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

static const byte DEFAULT_I2C_ADDRESS = 0b0111110;

class ST7032i {
	  uint8_t _displayfunction;
	  uint8_t _displaycontrol;
	  uint8_t _displaymode;
	  uint8_t _initialized;
	  uint8_t _numlines, _numcolumns;
	  uint8_t _position;
//
	  I2CBus * wirex;
	byte contrast;
	byte i2c_address;
	GPIOPin pin_bklight;


	void command(byte value);
	size_t write(byte value);

public:

	void init(I2CBus * wirex);
	void begin();
	
	size_t print(const char * str);

	//#if ARDUINO >= 100
//  virtual size_t println(void) { wrap(); return 1; }
//#else
//  virtual void write(uint8_t);
//  virtual void println(void) { wrap(); }
//#endif
//	void wrap();
//	void ST7032i_clearLine(ST7032i * lcd);

	void home();
	void clear();
	void setCursor(uint8_t c, uint8_t r);
	void display();
	void noDisplay();
	void setContrast(byte c);
//	void ST7032i_backlightOn(ST7032i * lcd); // { digitalWrite(pin_bklight, LOW); }
//	void ST7032i_backlightOff(ST7032i * lcd); // { digitalWrite(pin_bklight, HIGH); }

};

#endif
