// IMPORTANT: SEE COMMENTS @ LINE 15 REGARDING SHIELD VS BREAKOUT BOARD USAGE.

// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

#ifndef _ADAFRUIT_TFTLCD_H_
#define _ADAFRUIT_TFTLCD_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Adafruit_GFX.h>

// **** IF USING THE LCD BREAKOUT BOARD, COMMENT OUT THIS NEXT LINE. ****
// **** IF USING THE LCD SHIELD, LEAVE THE LINE ENABLED:             ****
#define USE_ADAFRUIT_SHIELD_PINOUT

class Adafruit_TFTLCD : public Adafruit_GFX {

 public:

  Adafruit_TFTLCD(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t rst);
  Adafruit_TFTLCD(void);

  void     begin(uint16_t id = 0x9325),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color),
           drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c),
           fillScreen(uint16_t color),
           reset(void),
           setRegisters8(uint8_t *ptr, uint8_t n),
           setRegisters16(uint16_t *ptr, uint8_t n),
           setRotation(uint8_t x),
           // These methods are public in order for BMP examples to work:
           setAddrWindow(int x1, int y1, int x2, int y2),
           pushColors(uint16_t *data, uint8_t len, boolean first);

#ifndef read8
  uint8_t  read8(void); // See notes below re: macros
#endif

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b),
           readPixel(int16_t x, int16_t y),
           readID(void);

 private:

  void     init(),
           // These items may have previously been defined as macros
           // in pin_magic.h.  If not, function versions are declared:
#ifndef write8
           write8(uint8_t value),
#endif
#ifndef setWriteDir
           setWriteDir(void),
#endif
#ifndef setReadDir
           setReadDir(void),
#endif
#ifndef writeRegister8
           writeRegister8(uint8_t a, uint8_t d),
#endif
#ifndef writeRegister16
           writeRegister16(uint16_t a, uint16_t d),
#endif
#ifndef writeRegisterPair
           writeRegisterPair(uint8_t aH, uint8_t aL, uint16_t d),
#endif
           setLR(void),
           flood(uint16_t color, uint32_t len);
  uint8_t  driver;
#ifndef USE_ADAFRUIT_SHIELD_PINOUT
  volatile uint8_t *csPort    , *cdPort    , *wrPort    , *rdPort;
  uint8_t           csPinSet  ,  cdPinSet  ,  wrPinSet  ,  rdPinSet  ,
                    csPinUnset,  cdPinUnset,  wrPinUnset,  rdPinUnset,
                   _reset;
#endif
};

// For compatibility with sketches written for older versions of library.
// Color function name was changed to 'color565' for parity with 2.2" LCD
// library.
#define Color565 color565

#endif
