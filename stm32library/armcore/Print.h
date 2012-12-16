/*
 * Print.h
 *
 *  Created on: 2012/12/15
 *      Author: sin
 */

#ifndef PRINT_H_
#define PRINT_H_

#include <stdint.h>
#include <stddef.h>
#include "armcore.h"

class Print {
private:
//    size_t printNumber(uint32_t, uint8_t);
 //   size_t printFloat(double, uint8_t);

public:
	virtual size_t write(const uint8_t c) = 0;
	virtual size_t write(const uint8_t * ptr, size_t n);
	virtual size_t write(const char * str);
/*
	size_t print(const char c) { return write(c); }
	size_t print(const char * str) { return write(str); }
	size_t print(const int32_t n, uint8_t base = DEC);
	size_t print(const uint32_t val, uint8_t base = DEC) { return printNumber(val, base); }
	*/
};


#endif /* PRINT_H_ */
