/*
 *  HC595iox.h
 *  
 *
 *  Created by Sin Shimozono on 11/07/13.
 *
 */
#ifndef HC595iox_h
#define HC595iox_h

#include <SPI.h>
#include "IOExpander.h"

class HC595iox : public IOExpander
{
private:
  byte cs_pin;
  byte cache;
  
public:
  HC595iox(byte pin) {
	cs_pin = pin;
	deselect();
  }
  
  void init();
  void begin() { init(); }
  
  void select();
  void deselect();

  virtual byte transfer(const byte d) { return SPI.transfer(d); }
  virtual byte write(const byte data);
  virtual byte write(const byte data[], const int length);
  virtual byte read() { return cache; }

  virtual byte IOMode(const byte addr) { return 0xff; }
  virtual byte IOMode(const byte addr, const byte val) { return 0xff; }
  virtual byte pullup(const byte addr) { return 0x00; }
  virtual byte pullup(const byte addr, const byte val) { return 0x00; }

  void bitclear(int bit) {
	  bitClear(cache, bit);
	  select();
	  transfer(cache);
	  deselect();
  }
  void bitset(int bit) {
	  bitSet(cache, bit);
	  select();
	  transfer(cache);
	  deselect();
  }
  const boolean bitread(int bit) {
	  return bitRead(cache, bit);
  }
  void bitwrite(int bit, boolean val) {
	  bitWrite(cache, bit, val);
	  select();
	  transfer(cache);
	  deselect();
  }

};

#endif // HC595iox_h
