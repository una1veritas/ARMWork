/*
 * i2c.h
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "armcmx.h"
//#include <stm32f4xx_i2c.h>
#include <stm32f4xx_i2c.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	I2C_MODE_IDLE = 0,
	I2C_MODE_MASTER_TX,
	I2C_MODE_MASTER_RQ,
	I2C_MODE_MASTER_RX,
	I2C_MODE_SLAVE_TX,
	I2C_MODE_SLAVE_RQ,
	I2C_MODE_SLAVE_RX
} I2C_Mode;

#define I2C_BUFFER_SIZE 256
typedef struct {
	uint8 rx[I2C_BUFFER_SIZE];
	uint8 tx[I2C_BUFFER_SIZE];
	uint16 rxcount, txcount;
//	boolean waitfinish;
	__IO I2C_Mode mode;
	__IO uint16_t position;
	__IO uint16_t count;
	__IO uint32 flagstatus;
	__IO uint32 watch;
} I2CBuffer;

typedef struct {
	I2C_TypeDef * I2Cx;
	GPIOPin scl, sda;
	uint32 clkspeed;
	uint8_t address;
	boolean irqmode;
	
	I2CBuffer I2CBuf;
} i2c;


void i2c_init(i2c * i2cxptr, I2C_TypeDef * I2Cx, GPIOPin scl, GPIOPin sda);
boolean i2c_begin(i2c * i2cxptr, uint32 clk);
//I2C_TypeDef * I2Cx, uint32_t clk);

void i2c_setup_comm(i2c * i2cxptr, I2C_Mode mode, uint8_t dstaddr, uint8_t * databuffer, uint16_t length);
boolean i2c_start_send(i2c * i2cxptr);
boolean i2c_start_receive(i2c * i2cxptr);

boolean i2c_transmit(i2c * i2cxptr, uint8_t addr, uint8_t * data, uint16_t length);
boolean i2c_request(i2c * I2Cbuf, uint8_t addr, uint8_t * data, uint16_t len);
boolean i2c_receive(i2c * I2Cbuf, uint8_t * data, uint16_t lim);

void I2C1_EV_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _I2C_H_ */
