
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

class ST7032i {
	static const byte DEFAULT_I2C_ADDRESS = 0b0111110;

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

public:

	void init(I2CBus * wirex);
	void begin();

    uint16_t write(const uint8_t);
    uint16_t write(uint8_t * a, uint16_t n);

    uint16_t print(const char c) { return write((uint8_t)c); }
    uint16_t print(const char * str);

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
