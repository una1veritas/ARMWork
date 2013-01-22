/*
 * i2cstruct.h
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#ifndef _I2CSTRUCT_H_
#define _I2CSTRUCT_H_

#include "armcore.h"
#include <stm32f4xx_i2c.h>

#include "armcore.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum __I2C_Mode {
	I2C_MODE_IDLE = 0,
	I2C_MODE_MASTER_TX,
	I2C_MODE_MASTER_RQ,
	I2C_MODE_MASTER_RX,
	I2C_MODE_SLAVE_TX,
	I2C_MODE_SLAVE_RQ,
	I2C_MODE_SLAVE_RX
} I2C_Mode;

#define I2C_BUFFER_SIZE 256
typedef struct __I2CStruct {
	I2C_TypeDef * I2Cx;
//	GPIOPin scl, sda;
	uint8_t address;
	boolean irqmode;
//	boolean waitfinish;
	__IO I2C_Mode mode;
	__IO uint16_t position;
	__IO uint16_t count;
	uint8_t buffer[I2C_BUFFER_SIZE];
	__IO uint32 flagstatus;
	__IO uint32 watch;
} I2CStruct;

extern I2CStruct I2C1Buffer, I2C2Buffer, I2C3Buffer;

boolean i2c_begin(I2CStruct * I2Cbuf, I2C_TypeDef * I2Cx, GPIOPin scl, GPIOPin sda, uint32_t clk);
//I2C_TypeDef * I2Cx, uint32_t clk);

void i2c_setup_comm(I2CStruct * I2Cbuf, I2C_Mode mode, uint8_t dstaddr, uint8_t * databuffer, uint16_t length);
boolean i2c_start_send(I2CStruct * I2Cbuf);
boolean i2c_start_receive(I2CStruct * I2Cbuf);

boolean i2c_transmit(I2CStruct * I2Cbuf, uint8_t addr, uint8_t * data, uint16_t length);
boolean i2c_request(I2CStruct * I2Cbuf, uint8_t addr, uint8_t * data, uint16_t len);
boolean i2c_receive(I2CStruct * I2Cbuf, uint8_t * data, uint16_t lim);

void I2C1_EV_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _I2CSTRUCT_H_ */
