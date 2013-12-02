/*

 Uses I2c Wires interface

 Uses Analog pin 4 - SDA
 Uses Analog pin 5 - SCL


 Usage:

 */

//#include <stdio.h>
//#include <stdlib.h>

/*
 #if ARDUINO >= 100
 #include <Arduino.h>
 #else
 #include <WProgram.h>
 #endif
 */
#include "armcmx.h"
#include "I2CBus.h"
#include "ST7032i.h"

// private constants

#define CMDDELAY 50        // Delay to wait after sending commands;
#define DATADELAY 50        // Delay to wait after sending data;
#define DEFAULTCONTRAST  0x27 //42

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
//
// private functions
//

//
ST7032i::ST7032i(I2CBus & bus,  GPIOPin bklt, GPIOPin rst) : wire(bus), pin_bklight(bklt), pin_reset(rst){
	_numlines = 2;
	_numcolumns = 16;
	_position = 0;
	i2c_address = DEFAULT_I2C_ADDRESS;
	contrast = DEFAULTCONTRAST;

//	wirex = wx;
	/*
	 // for some 1 line displays you can select a 10 pixel high font
	 if ((dotsize != 0) && (lines == 1)) {
	 _displayfunction |= LCD_5x10DOTS;
	 }
	 */
	if (pin_bklight != NOT_A_PIN) {
		pinMode(pin_bklight, OUTPUT);
	}
	if (pin_reset != NOT_A_PIN) {
		pinMode(pin_reset, OUTPUT);
    digitalWrite(pin_reset, HIGH);
	}
}
//

void ST7032i::send(uint8_t value, uint8_t dcswitch) {
	if ( dcswitch == LOW ) {
		wire.beginTransmission(i2c_address);
		wire.write((uint8)0x00);
		wire.write(value);
		wire.endTransmission();
		// delay_us(CMDDELAY);
	} else {
		wire.beginTransmission(i2c_address);
		wire.write((uint8)B01000000);
		wire.write((uint8)value);
		wire.endTransmission();
		// delay_us(CMDDELAY);
		_position++;
	}
}

void ST7032i::reset() {
	if (pin_reset != NOT_A_PIN) {
    digitalWrite(pin_reset, LOW);
    delay(10);
    digitalWrite(pin_reset, HIGH);
  }
}

bool ST7032i::begin() {

  reset();

	delay(40);
	command(B00111000); //function set
	command(B00111001); // function set
//	delay_ms(2);
	command(B00010100); // interval osc
	command(B01110000 | (contrast & 0xf)); // contrast Low 4 bits
//	delay_ms(2);

	command(B01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	command(B01101100); // follower control
	delay_ms(300);

	command(B00111000); // function set
	command(B00001100); // Display On
//	delay_ms(2);

	command(B00000001); // Clear Display
	delay_ms(2); // Clear Display needs additional wait
  
  /*
//	command(B00000010); // home, but does not work
//	delay_ms(2);

	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);
*/
	// turn the display on with no cursor or blinking default
  
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
//	display();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
command(LCD_ENTRYMODESET | _displaymode);
  
  return true;
}

void ST7032i::setContrast(byte val) {
	contrast = 0x7f & val;
	command( B00111000); //function set
	command(B00111001); // function set
	delay_ms(2);
	command(B01110000 | (contrast & 0xf)); // contrast Low 4 bits
	delay_ms(2);
	command(B01011100 | ((contrast >> 4) & 0x3)); // contast High/icon/power
	command(B00111000); // function set
	delay_ms(2);
}

/* ------------------ */

void ST7032i::clear() {
	command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	_position = 0;
	delay_ms(200);  // this command takes a long time!
}

void ST7032i::home() {
	command(LCD_RETURNHOME);  // set cursor position to zero
	_position = 0;
	delay_ms(200);  // this command takes a long time!
}

void ST7032i::setCursor(uint8_t c, uint8_t r) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (r >= _numlines) {
		r %= _numlines;    // we count rows starting w/ 0
	}
	_position = c + row_offsets[r];
	command(LCD_SETDDRAMADDR | _position);
}

/*
// LCD_DISPLAYON, LCD_BLINKON, LCD_CURSORON
void ST7032i::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::showCursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void ST7032i::scrollDisplayLeft() {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void ST7032i::scrollDisplayRight() {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void ST7032i::leftToRight() {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

void ST7032i::rightToLeft() {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

void ST7032i::autoscroll() {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

void ST7032i::noAutoscroll() {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}
*/
void ST7032i::createChar(uint8_t location, uint8_t charmap[]) {
	int i;
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (i = 0; i < 8; i++) {
		write(charmap[i]);
	}
}


