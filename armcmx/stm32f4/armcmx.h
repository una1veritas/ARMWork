/*
 * cmcore.h
 *
 *  Created on: 2012/10/23
 *      Author: sin
 */

#ifndef _ARMCMX_H_
#define _ARMCMX_H_

#ifndef ARMCMX
#define ARMCMX
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "binary.h"

#define HEX 16
#define DEC 10
#define BIN 2

#ifdef abs
#undef abs
#define abs(x)  ((x) < 0 ? -(x) : (x))
#endif

#ifndef max
#define max(x, y) 	((x) >= (y)? (x) : (y))
#endif
#ifndef min
#define min(x, y) 	((x) <= (y)? (x) : (y))
#endif

typedef uint8_t uint8;
typedef uint8_t byte;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint16_t word;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;

#define highByte(x) 	((x)>>8&0xff)
#define lowByte(x) 		((x)&0xff)

typedef uint8_t boolean;

#define true 0xff
#define false 0x00

#define UINT8(x)  ((uint8_t)(x))
#define UINT16(x)  ((uint16_t)(x))
#define HIGHLOW(x, y)  (((uint16_t)(x))<< 8 | y)

#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"

void armcmx_init(void);
//size_t strlen(const char * s);

#ifdef __cplusplus
}
#endif

#endif /* ARMCORE_H_ */
