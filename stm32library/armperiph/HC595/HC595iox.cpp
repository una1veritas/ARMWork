/*
 *  HC595iox.cpp
 *  
 *
 *  Created by Sin Shimozono on 11/07/13.
 *
 */

#include <SPI.h>
#include "HC595iox.h"

void HC595iox::init() {
  if ( cs_pin != 0xff) {
	  pinMode(cs_pin, OUTPUT);
  }
  write(0);
}

void HC595iox::select() { 
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  if ( cs_pin != 0xff ) {
	  digitalWrite(cs_pin, LOW);
  }
}

void HC595iox::deselect() {
	if ( cs_pin != 0xff )
	  digitalWrite(cs_pin, HIGH);
}


byte HC595iox::write(const byte data) {
  select();
  SPI.transfer(cache = data);
  deselect();
  return 1;
}

byte HC595iox::write(const byte data[], const int length) {
  select();
  for (int i = 0; i < length; i++)
	  SPI.transfer(cache = data[i]);
  deselect();
  return length;
}

