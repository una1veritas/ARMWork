/*
 * armcore.h
 *
 *  Created on: 2012/10/23
 *      Author: sin
 */

#ifndef ARMCORE_H_
#define ARMCORE_H_

#include <stdio.h>
#include <stdint.h>

#define HEX 16
#define DEC 10
#define BIN 2

#ifdef abs
#undef abs
#define abs(x)  ((x) < 0 ? -(x) : (x))
#endif

typedef uint8_t uint8;
typedef uint8_t byte;
typedef uint16_t uint16;
typedef uint16_t word;
typedef uint32_t uint32;
typedef uint64_t uint64;

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t boolean;
#define true 0xff
#define false 0x00

#define UINT8(x)  ((uint8_t)(x))
#define UINT16(x)  ((uint16_t)(x))
#define HIGHLOW(x, y)  (((uint16_t)(x))<< 8 | y)

size_t strlen(const char * s);

#ifdef __cplusplus
}
#endif
#endif /* ARMCORE_H_ */
