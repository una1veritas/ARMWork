/*

 Uses I2c Wires interface

 Uses Analog pin 4 - SDA
 Uses Analog pin 5 - SCL


 Usage:

 */

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Wire.h>
#include "ST7032i.h"

#define CMDDELAY 50        // Delay to wait after sending commands;
#define DATADELAY 50        // Delay to wait after sending data;
#define DEFAULTCONTRAST  40

#define LCDI2C_MAX_STRLEN			40
#define LCDI2C_PRINT_STR_DELAY		50
/*
 static unsigned char icon_data[]={
 0x00, 0b10000,
 0x02, 0b10000,
 0x04, 0b10000,
 0x06, 0b10000,

 0x07, 0b10000,
 0x07, 0b01000,
 0x09, 0b10000,
 0x0B, 0b10000,

 0x0D, 0b01000,
 0x0D, 0b00100,
 0x0D, 0b00010,
 0x0D, 0b10000,

 0x0F, 0b10000, //
 };
 */

LCD_ST7032i::LCD_ST7032i(byte pin) {
	_numlines = 2;
	_numcolumns = 16;
	_position = 0;
	i2c_address = DEFAULT_I2C_ADDRESS;
	contrast = DEFAULTCONTRAST;

	pin_bklight = pin;
}

void LCD_ST7032i::initST7032i() {
//	delay(40);
	command(0b00111000); //function set
	command(0b00111001); // function set 
	delay(2);

	command(0b00010100); // interval osc
	command(0b01110000 | (contrast & 0xf)); // contrast Low 4 bits
	delay(2);

	command(0b01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	command(0b01101100); // follower control
	delay(300);

	command(0b00111000); // function set
	command(0b00001100); // Display On
	delay(2);

	command(0b00000001); // Clear Display
	delay(2); // Clear Display needs additional wait
	command(0b00000010); // home, but does not work
	delay(2);

	pinMode(pin_bklight, OUTPUT);
}

void LCD_ST7032i::begin(uint8_t cols, uint8_t lines) {
	_numlines=lines;
	_numcolumns=cols;
	initST7032i();
	  if (lines > 1) {
	    _displayfunction |= LCD_2LINE;
	  }
	  _numlines = lines;
	  _position = 0;
/*
	  // for some 1 line displays you can select a 10 pixel high font
	  if ((dotsize != 0) && (lines == 1)) {
	    _displayfunction |= LCD_5x10DOTS;
	  }
*/
	  // finally, set # lines, font size, etc.
	  command(LCD_FUNCTIONSET | _displayfunction);

	  // turn the display on with no cursor or blinking default
	  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	  display();

	  // clear it off
//	  clear();
	  home();

	  // Initialize to default text direction (for romance languages)
	  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	  // set the entry mode
	  command(LCD_ENTRYMODESET | _displaymode);

}

void LCD_ST7032i::send(byte value, byte dcswitch) {
	Wire.beginTransmission(i2c_address);
	switch (dcswitch) {
	case LOW: // write
#if ARDUINO >= 100
		Wire.write((byte) 0x00);
		Wire.write(value);
#else
		Wire.send(0x00);
		Wire.send(value);
#endif
		Wire.endTransmission();
		delayMicroseconds(CMDDELAY);
		break;
	case HIGH: // data
#if ARDUINO >= 100
		Wire.write(0b01000000);
		Wire.write(value & 0xff);
#else
		Wire.send(0b01000000);
		Wire.send(value & 0xff);
#endif
		Wire.endTransmission();
		delayMicroseconds(DATADELAY);
		break;
	}
}

/*
void LCD_ST7032i::wrap() {
	if (position < 0x40)
		position = 0x40;
	else
		position = 0;
	command(0b10000000 | position);
}
*/

//Turn the LCD ON

void LCD_ST7032i::display() {
	command(0b00111000); // function set
	command(LCD_DISPLAYCONTROL | LCD_DISPLAYON); // Display On
}

// Turn the LCD OFF

 void LCD_ST7032i::noDisplay(){
	 command(0xFE);
	 command(0x0B);
 }

void LCD_ST7032i::setContrast(byte val) {
	contrast = 0x7f & val;
	command(0b00111000); //function set
	command(0b00111001); // function set 
	delay(2);
	command(0b01110000 | (contrast & 0xf)); // contrast Low 4 bits
	delay(2);
	command(0b01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	command(0b00111000); // function set
	delay(2);
}

