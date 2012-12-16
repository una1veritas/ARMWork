/*
 * Print.cpp
 *
 *  Created on: 2012/12/15
 *      Author: sin
 */

#include "Print.h"

size_t Print::write(const uint8_t * ptr, size_t n) {
	for (size_t i = 0; i < n; i++)
		write(ptr[n]);
	return n;
}

size_t Print::write(const char * str) {
	size_t n;
	for (n = 0; str[n]; n++) {
		write(str[n]);
	}
	return n;
}
/*
size_t Print::print(int32_t n, uint8_t base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      return printNumber(n, 10) + t;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}
*/
// Private Methods /////////////////////////////////////////////////////////////
/*
size_t Print::printNumber(uint32_t n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write(str);
}

size_t Print::printFloat(double number, uint8_t digits)
{
  size_t n = 0;

  // Handle negative numbers
  if (number < 0.0)
  {
     n += print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += print(".");
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int32_t toPrint = int(remainder);
    n += print(toPrint);
    remainder -= toPrint;
  }

  return n;
}
*/
