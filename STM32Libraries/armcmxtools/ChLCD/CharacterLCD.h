#ifndef CharacterLCD_h
#define CharacterLCD_h

#include <inttypes.h>
#if defined ARDUINO
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "Wiring.h"
#endif
#elif defined ARMCMX
#include <armcmx.h>
#include <delay.h>
#include <gpio.h>
#endif
#include <Print.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class CharacterLCD : public Print {
public:
//  virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);


  void clear() {
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
  }

  void home()
  {
    command(LCD_RETURNHOME);  // set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
  }


  void setCursor(uint8_t c, uint8_t r)
  {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if ( r >= _numlines ) {
      r %= _numlines;    // we count rows starting w/ 0
    }
    _position = c + row_offsets[r];
    command(LCD_SETDDRAMADDR | _position);
  }
  /*
  void setCursor(int c, int r) {
  	position = (r % rows) * 0x40 + (c % 0x27);
  	command(LCD_SETDDRAMADDR | position);
  }
  */

  void noDisplay()
  {
   _displaycontrol &= ~LCD_DISPLAYON;
   command(LCD_DISPLAYCONTROL | _displaycontrol);
 }

  void display()
  {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
  }

  void noBlink()
  {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
  }

  void blink()
  {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
  }

  void noCursor()
  {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
  }

  void cursor()
  {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
  }

  void scrollDisplayLeft()
  {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
  }

  void scrollDisplayRight()
  {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
  }

  void leftToRight()
  {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
  }

  void rightToLeft()
  {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
  }

  void autoscroll()
  {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
  }

  void noAutoscroll()
  {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
  }


  void createChar(uint8_t location, uint8_t charmap[])
  {
    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i=0; i<8; i++) {
      write(charmap[i]);
    }
  }

  virtual inline size_t write(uint8_t value) {
	  send(value, HIGH);
	  return 1; // assume success
	}

  virtual inline void command(uint8_t value) {
	  send(value, LOW);
	}
  
  using Print::write;

  virtual void send(uint8_t value, uint8_t dcswitch) = 0;

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _initialized;
  uint8_t _numlines, _numcolumns;
  uint8_t _position;
};

#endif
