
#ifndef LCD_ST7032i_h
#define LCD_ST7032i_h

/*
#include <inttypes.h>
#if ARDUINO >= 100
#include <stdio.h>
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include "Print.h"
*/

#include "CharacterLCD.h"

class LCD_ST7032i : public CharacterLCD {
	static const byte DEFAULT_I2C_ADDRESS = 0b0111110;

	byte contrast;
//	int id;
	byte i2c_address;
	byte pin_bklight;
	
	void initST7032i();
	virtual void send(byte value, byte dcswitch);

public:
	LCD_ST7032i(byte pin = 0xff);
	
  void begin(uint8_t c = 16, uint8_t r = 2);
  inline void start() { initST7032i(); home(); }

#if ARDUINO >= 100
//  virtual size_t println(void) { wrap(); return 1; }
#else
  virtual void write(uint8_t);
//  virtual void println(void) { wrap(); }
#endif
//	void wrap();
	void clearLine();

	void display();
	void noDisplay();
	void setContrast(byte );

	void backlightOn() { digitalWrite(pin_bklight, LOW); }
	void backlightOff() { digitalWrite(pin_bklight, HIGH); }

};

#endif
