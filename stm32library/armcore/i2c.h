/*
 * i2c.h
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#ifndef I2C_H_
#define I2C_H_

#include "armcore.h"
#include <stm32f4xx_i2c.h>

#include "armcore.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum __I2C_Status {
	NOT_READY = 0xff,
	READY = 0,
	STARTING,
	ADDRESS_SENDING,
	BYTE_TRANSFERRING,
	TRANSMISSION_COMPLETED,
	
	RESTARTING,
	/*
	RECEIVE_BYTE_READY,
	BEFORELAST_BYTE_RECEIVED,
	LAST_BYTE_READY,
	RECEIVE_BYTE_COMPLETED,
	RECEIVE_COMPLETED,
	*/
} I2C_Status;

typedef enum __I2C_CommMode {
	I2C_MODE_MASTER = 0,
	I2C_MODE_MASTER_TX,
	I2C_MODE_MASTER_RX,
	I2C_MODE_SLAVE,
	I2C_MODE_SLAVE_TX,
	I2C_MODE_SLAVE_RX
} I2C_CommMode;

#define I2C_BUFFER_SIZE 256
typedef struct __I2CBus {
	I2C_TypeDef * I2Cx;
	GPIOPin sda, scl;
	uint8_t address;
	I2C_CommMode mode;
	boolean irqmode;
	__IO I2C_Status status;
	__IO uint16_t position;
	__IO uint16_t length;
	uint8_t databytes[I2C_BUFFER_SIZE];
} I2CBus;

extern I2CBus Wire1, Wire2, Wire3;

boolean i2c_begin(/*I2CBus * wire,*/ I2C_TypeDef * i2cx, GPIOPin sda, GPIOPin scl, uint32_t clk); //I2C_TypeDef * I2Cx, uint32_t clk);
boolean i2c_start(I2CBus * wire, uint8_t addr);
boolean i2c_transmit(I2CBus * wire, uint8_t addr, uint8_t * data, uint16_t length);
//void i2c_receive(uint8_t addr, uint8_t * data, uint16_t nlimit);
boolean i2c_receive(I2CBus * wire, uint8_t addr, uint8_t req, uint8_t * recv, uint16_t lim);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
