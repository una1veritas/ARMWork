/*

 Uses I2c Wires interface

 Uses Analog pin 4 - SDA
 Uses Analog pin 5 - SCL


 Usage:

 */

/*
 #if ARDUINO >= 100
 #include <Arduino.h>
 #else
 #include <WProgram.h>
 #endif
 */
#include "delay.h"
#include "i2c.h"
#include "ST7032i.h"

// private constants

#define CMDDELAY 50        // Delay to wait after sending commands;
#define DATADELAY 50        // Delay to wait after sending data;
#define DEFAULTCONTRAST  42

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
void ST7032i_command(ST7032i * lcd, byte value);
size_t ST7032i_write(ST7032i * lcd, byte value);

//
void ST7032i_init(ST7032i * lcd, I2CBus * wx) {
	lcd->_numlines = 2;
	lcd->_numcolumns = 16;
	lcd->_position = 0;
	lcd->i2c_address = DEFAULT_I2C_ADDRESS;
	lcd->contrast = DEFAULTCONTRAST;
	lcd->pin_bklight = PIN_NOT_DEFINED;

	lcd->wirex = wx;
	/*
	 // for some 1 line displays you can select a 10 pixel high font
	 if ((dotsize != 0) && (lines == 1)) {
	 _displayfunction |= LCD_5x10DOTS;
	 }
	 */
	if (lcd->pin_bklight != PIN_NOT_DEFINED) {
		pinMode(lcd->pin_bklight, OUTPUT);
	}
}
//

void ST7032i_command(ST7032i * lcd, uint8_t value) {
	uint8_t buf[2];
	buf[0] = (byte) 0x00;
	buf[1] = value;
	i2c_transmit(lcd->wirex, lcd->i2c_address, buf, 2);
	delay_us(CMDDELAY);
}

//
size_t ST7032i_write(ST7032i * lcd, uint8_t value) {
	uint8_t buf[2];
	buf[0] = 0b01000000;
	buf[1] = value & 0xff;
	i2c_transmit(lcd->wirex,lcd->i2c_address, buf, 2);
	delay_us(CMDDELAY);
	return 1; // assume success
}

void ST7032i_begin(ST7032i * lcd) {
//	delay(40);
	ST7032i_command(lcd, 0b00111000); //function set
	ST7032i_command(lcd, 0b00111001); // function set
	delay_ms(2);

	ST7032i_command(lcd, 0b00010100); // interval osc
	ST7032i_command(lcd, 0b01110000 | (lcd->contrast & 0xf)); // contrast Low 4 bits
	delay_ms(2);

	ST7032i_command(lcd, 0b01011100 | ((lcd->contrast >> 4) & 0x3)); // contast High/icon/power
	ST7032i_command(lcd, 0b01101100); // follower control
	delay_ms(300);

	ST7032i_command(lcd, 0b00111000); // function set
	ST7032i_command(lcd, 0b00001100); // Display On
	delay_ms(2);

	ST7032i_command(lcd, 0b00000001); // Clear Display
	delay_ms(2); // Clear Display needs additional wait
	ST7032i_command(lcd, 0b00000010); // home, but does not work
	delay_ms(2);

	// finally, set # lines, font size, etc.
	ST7032i_command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);

	// turn the display on with no cursor or blinking default
	lcd->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	ST7032i_display(lcd);

	// clear it off
	ST7032i_clear(lcd);
	ST7032i_home(lcd);

	// Initialize to default text direction (for romance languages)
	lcd->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	ST7032i_command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

size_t ST7032i_print(ST7032i * lcd, const char * str) {
	uint16_t i;
	for (i = 0; str[i]; i++)
		ST7032i_write(lcd, str[i]);
	return i; // assume success
}

void ST7032i_setContrast(ST7032i * lcd, byte val) {
	lcd->contrast = 0x7f & val;
	ST7032i_command(lcd, 0b00111000); //function set
	ST7032i_command(lcd, 0b00111001); // function set
	delay_ms(2);
	ST7032i_command(lcd, 0b01110000 | (lcd->contrast & 0xf)); // contrast Low 4 bits
	delay_ms(2);
	ST7032i_command(lcd, 0b01011100 | ((lcd->contrast >> 4) & 0x3)); // contast High/icon/power
	ST7032i_command(lcd, 0b00111000); // function set
	delay_ms(2);
}

/* ------------------ */

void ST7032i_clear(ST7032i * lcd) {
	ST7032i_command(lcd, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	delay_ms(200);  // this command takes a long time!
}

void ST7032i_home(ST7032i * lcd) {
	ST7032i_command(lcd, LCD_RETURNHOME);  // set cursor position to zero
	delay_ms(200);  // this command takes a long time!
}

void ST7032i_setCursor(ST7032i * lcd, uint8_t c, uint8_t r) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (r >= lcd->_numlines) {
		r %= lcd->_numlines;    // we count rows starting w/ 0
	}
	lcd->_position = c + row_offsets[r];
	ST7032i_command(lcd, LCD_SETDDRAMADDR | lcd->_position);
}

// LCD_DISPLAYON, LCD_BLINKON, LCD_CURSORON
void ST7032i_noDisplay(ST7032i * this) {
	this->_displaycontrol &= ~LCD_DISPLAYON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_display(ST7032i * this) {
	this->_displaycontrol |= LCD_DISPLAYON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_noBlink(ST7032i * this) {
	this->_displaycontrol &= ~LCD_BLINKON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_blink(ST7032i * this) {
	this->_displaycontrol |= LCD_BLINKON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_noCursor(ST7032i * this) {
	this->_displaycontrol &= ~LCD_CURSORON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_showCursor(ST7032i * this) {
	this->_displaycontrol |= LCD_CURSORON;
	ST7032i_command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

void ST7032i_scrollDisplayLeft(ST7032i * this) {
	ST7032i_command(this, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void ST7032i_scrollDisplayRight(ST7032i * this) {
	ST7032i_command(this, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void ST7032i_leftToRight(ST7032i * this) {
	this->_displaymode |= LCD_ENTRYLEFT;
	ST7032i_command(this, LCD_ENTRYMODESET | this->_displaymode);
}

void ST7032i_rightToLeft(ST7032i * this) {
	this->_displaymode &= ~LCD_ENTRYLEFT;
	ST7032i_command(this, LCD_ENTRYMODESET | this->_displaymode);
}

void ST7032i_autoscroll(ST7032i * this) {
	this->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	ST7032i_command(this, LCD_ENTRYMODESET | this->_displaymode);
}

void ST7032i_noAutoscroll(ST7032i * this) {
	this->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	ST7032i_command(this, LCD_ENTRYMODESET | this->_displaymode);
}

void ST7032i_createChar(ST7032i * this, uint8_t location, uint8_t charmap[]) {
	int i;
	location &= 0x7; // we only have 8 locations 0-7
	ST7032i_command(this, LCD_SETCGRAMADDR | (location << 3));
	for (i = 0; i < 8; i++) {
		ST7032i_write(this, charmap[i]);
	}
}

