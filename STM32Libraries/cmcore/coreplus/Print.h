/*
  Print.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t
#include <string.h>

//#include "WString.h"
//#include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print
{
  private:
    int write_error;
    size_t printNumber(uint32, uint8_t);
    size_t printFloat(double, uint8_t);

  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() : write_error(0) {}
  
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
  
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    
//    size_t print(const __FlashStringHelper *);
//    size_t print(const String &);
    size_t print(const char[]);
    size_t print(const char);
    size_t print(uint8, uint8 = DEC);
    size_t print(int16, uint8 = DEC);
    size_t print(uint16, uint8 = DEC);
    size_t print(int32, uint8 = DEC);
    size_t print(uint32, uint8 = DEC);
    size_t print(double, uint8 = 2);
//    size_t print(const Printable&);
		size_t printByte(uint8 * array, uint8 length, char sep = ' ');
		size_t printByte(uint8 val);
		size_t printByte(uint32 val);

//    size_t println(const __FlashStringHelper *);
//    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(const char);
    size_t println(uint8, uint8 = DEC);
    size_t println(int16, uint8 = DEC);
    size_t println(uint16, uint8 = DEC);
    size_t println(int32, uint8 = DEC);
    size_t println(uint32, uint8 = DEC);
    size_t println(double, uint8 = 2);
//    size_t println(const Printable&);
    size_t println(void);
};

#endif
