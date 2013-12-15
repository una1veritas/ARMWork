// IMPORTANT: SEE COMMENTS @ LINE 15 REGARDING SHIELD VS BREAKOUT BOARD USAGE.

// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

#ifndef _TFTLCD_H_
#define _TFTLCD_H_

#if defined(ARDUINO)
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#elif defined(ARMCMX)
#include "armcmx.h"
#endif

#include <Adafruit_GFX.h>

#define TFTWIDTH   240
#define TFTHEIGHT  320

// LCD controller chip identifiers
#define ID_932X    0
#define ID_7575    1
#define ID_UNKNOWN 0xFF

#define	C565_BLACK   0x0000
#define	C565_BLUE    0x001F
#define	C565_RED     0xF800
#define	C565_GREEN   0x07E0
#define C565_CYAN    0x07FF
#define C565_MAGENTA 0xF81F
#define C565_YELLOW  0xFFE0
#define C565_WHITE   0xFFFF

// **** IF USING THE LCD BREAKOUT BOARD, COMMENT OUT THIS NEXT LINE. ****
// **** IF USING THE LCD SHIELD, LEAVE THE LINE ENABLED:             ****
//#define USE_ADAFRUIT_SHIELD_PINOUT

class TFTLCD : public Adafruit_GFX {
private:
  GPIOPin _CS, _WR, _CD, _RD, _RST;
  uint8_t  driver;

  void     init();
           // These items may have previously been defined as macros
           // in pin_magic.h.  If not, function versions are declared:
  void write8(uint8_t value);
  void setWriteDir(void);
  void setReadDir(void);
  void writeRegister8(uint8_t a, uint8_t d);
  void writeRegister16(uint16_t a, uint16_t d);
  void writeRegisterPair(uint8_t aH, uint8_t aL, uint16_t d);
  void setLR(void);
  void flood(uint16_t color, uint32_t len);
  uint8_t  read8(uint8_t & val);

 public:

//  TFTLCD(void);
  TFTLCD(const GPIOPin & cs, const GPIOPin & cd, const GPIOPin & wr, const GPIOPin & rd, const GPIOPin & rst);

  boolean     begin(uint16_t id = 0x9325);
  virtual void     drawPixel(int16_t x, int16_t y, uint16_t color);
  void     drawPixel(uint16_t color);
  virtual void     drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
  virtual void     drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
  virtual void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
  virtual void     fillScreen(uint16_t color);
  void     reset(void);
  void     setRegisters8(uint8_t *ptr, uint8_t n);
  void     setRegisters16(uint16_t *ptr, uint8_t n);
  void     setRotation(uint8_t x);
       // These methods are public in order for BMP examples to work:
  void     setAddrWindow(int x1, int y1, int x2, int y2);
  void     pushColors(uint16_t *data, uint8_t len, boolean first);

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b),
           readPixel(int16_t x, int16_t y),
           readID(void);

  uint16_t driverID(void) { return driver; }
};

// For compatibility with sketches written for older versions of library.
// Color function name was changed to 'color565' for parity with 2.2" LCD
// library.
#define Color565 color565


#endif
