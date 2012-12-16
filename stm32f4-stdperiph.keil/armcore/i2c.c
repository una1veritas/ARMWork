/*
 * i2c.c
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#include "stm32f4xx_rcc.h"

#include "gpio.h"
#include "delay.h"
#include "i2c.h"


typedef enum __CommDirection {
	NOT_DEFINED = 0, 
	TRANSMITTER, 
	RECEIVER,
} CommDirection;

//I2C_Status i2c1_status;
//CommDirection i2c1_direction;

boolean i2c_begin(I2CBus * wire, I2C_TypeDef * I2Cx, GPIOPin sda, GPIOPin scl, uint32_t clk) {
	I2C_InitTypeDef I2C_InitStructure;
	// set default to I2C1
	uint8_t gpio_af = GPIO_AF_I2C1;
	uint32_t periph_i2c = RCC_APB1Periph_I2C1;

	wire->I2Cx = I2Cx;
	wire->sda = sda; //PB9;
	wire->scl = scl; //PB8;

	if ( I2Cx == I2C2 ) {
		gpio_af = GPIO_AF_I2C2;
		periph_i2c = RCC_APB1Periph_I2C2;
	} else if ( I2Cx == I2C3 ) {
		gpio_af = GPIO_AF_I2C3;
		periph_i2c = RCC_APB1Periph_I2C3;
	} else {
		gpio_af = GPIO_AF_I2C1;
		periph_i2c = RCC_APB1Periph_I2C1;
	}
	
	/* I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(periph_i2c, ENABLE); //  RCC_APB1PeriphClockCmd(I2C1_RCC, ENABLE);
	/* GPIO Periph clock enable */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // PB5 (SMBA), PB6 (SCL), PB9 (SDA)  // RCC_APB2PeriphClockCmd(I2C1_GPIO_RCC, ENABLE);
	GPIOMode(PinPort(wire->scl), PinBit(wire->scl), GPIO_Mode_AF,
			GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIOMode(PinPort(wire->sda), PinBit(wire->sda), GPIO_Mode_AF,
			GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);

	/* Configure I2C pins: SCL and SDA */
	GPIO_PinAFConfig(PinPort(wire->scl), PinSource(wire->scl), gpio_af); //GPIO_AF_I2C1 );
	GPIO_PinAFConfig(PinPort(wire->sda), PinSource(wire->sda), gpio_af); //GPIO_AF_I2C1 );

	switch (clk) {
	case 100000:
	case 400000:
		break;
	default:
		clk = 100000;
		break;
	}
	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = clk;

	/* Apply I2C configuration after enabling it */
	I2C_Init(wire->I2Cx, &I2C_InitStructure);
	/* I2C Peripheral Enable */
	I2C_Cmd(wire->I2Cx, ENABLE);

	wire->status = NOT_READY;
	wire->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_transmit(I2CBus * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	uint16_t i;
	uint16_t wc;

	wire->mode = I2C_MODE_MASTERTRANSMITTER;
	//
	if (!i2c_start(wire, addr))
		return false;

	for (i = 0; i < length; i++) {
		I2C_SendData(wire->I2Cx, data[i]);
		wire->status = BYTE_TRANSMITTING;
		// Test on EV8 and clear it
		for (wc = 5;
				!I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
				wc--) {
			if (wc == 0)
				return false;
			delay_us(667);
		}
		wire->status = BYTE_TRANSMITTED;
	}

	wire->status = TRANSMISSION_COMPLETED;

	// generate stop condition
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	wire->status = NOT_READY;
	wire->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_receive(I2CBus * wire, uint8_t addr, uint8_t req, uint8_t * recv,
		uint16_t lim) {
	uint16_t i;
	uint16_t wc;

	wire->mode = I2C_MODE_MASTERRECEIVER;
	//
	if (!i2c_start(wire, addr))
		return false;

	/* Send the EEPROM's internal address to read from: MSB of the address first */
	I2C_SendData(wire->I2Cx, req);
	wire->status = BYTE_TRANSMITTING;
	/* Test on EV8 and clear it */

	for (wc = 5;
			!I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = TRANSMISSION_COMPLETED;

	/* Send STRAT condition a second time */
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = RESTART_ISSUED;

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(wire->I2Cx, addr << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	for (wc = 5;
			!I2C_CheckEvent(wire->I2Cx,
					I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = SRC_ADDRESS_SENT;
	for (i = 1; i < lim; i++) {
		wire->status = RECEIVE_BYTE_READY;
		for (wc = 5;
				!I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED );
				wc--) {
			if (wc == 0)
				return false;
			delay_us(667);

		}
		/* Read a byte from the EEPROM */
		*recv++ = I2C_ReceiveData(wire->I2Cx);
		wire->status = BYTE_RECEIVED;
	}
	wire->status = BEFORELAST_BYTE_RECEIVED;

	/* Disable Acknowledgement */
	I2C_AcknowledgeConfig(wire->I2Cx, DISABLE);
	/* Send STOP Condition */
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	wire->status = LAST_BYTE_READY;

	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	/* Read a byte from the EEPROM */
	*recv = I2C_ReceiveData(wire->I2Cx);
	wire->status = RECEIVE_BYTE_COMPLETED;

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(wire->I2Cx, ENABLE);
	wire->status = NOT_READY;
	wire->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_start(I2CBus * wire, uint8_t addr) {
	uint16_t wc;
	//
	wire->status = NOT_READY;
	for (wc = 5; I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = READY;

	/* Send STRAT condition */
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = START_ISSUED;

	/* Send address for write */
	I2C_Send7bitAddress(wire->I2Cx, addr << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	for (wc = 5;
			!I2C_CheckEvent(wire->I2Cx,
					I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = DST_ADDRESS_SENT;
	return true;
}
