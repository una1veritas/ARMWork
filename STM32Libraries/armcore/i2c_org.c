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

I2CBus Wire1 = { I2C1, 0, 0, 0, 0, 0 }, Wire2 = { I2C2, 0, 0, 0, 0, 0 }, Wire3 =
		{ I2C3, 0, 0, 0, 0, 0 };

typedef enum __CommDirection {
	NOT_DEFINED = 0, TRANSMITTER, RECEIVER,
} CommDirection;

//I2C_Status i2c1_status;
//CommDirection i2c1_direction;

boolean i2c_begin(I2CBus * wirex, GPIOPin sda, GPIOPin scl, uint32_t clkspeed) {
	I2C_InitTypeDef I2C_InitStructure;
	// set default to I2C1
	uint8_t gpio_af = GPIO_AF_I2C1;
	uint32_t periph_i2c = RCC_APB1Periph_I2C1;

	wirex->sda = sda; //PB9;
	wirex->scl = scl; //PB8;

	if (wirex->I2Cx == I2C2 ) {
		gpio_af = GPIO_AF_I2C2;
		periph_i2c = RCC_APB1Periph_I2C2;
	} else if (wirex->I2Cx == I2C3 ) {
		gpio_af = GPIO_AF_I2C3;
		periph_i2c = RCC_APB1Periph_I2C3;
	} /* else if (wirex->I2Cx == I2C1 ) {
		gpio_af = GPIO_AF_I2C1;
		periph_i2c = RCC_APB1Periph_I2C1;
	} */

	/* I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(periph_i2c, ENABLE); //  RCC_APB1PeriphClockCmd(I2C1_RCC, ENABLE);
	/* GPIO Periph clock enable */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // PB5 (SMBA), PB6 (SCL), PB9 (SDA)  // RCC_APB2PeriphClockCmd(I2C1_GPIO_RCC, ENABLE);
	GPIOMode(PinPort(wirex->scl), PinBit(wirex->scl), GPIO_Mode_AF,
			GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIOMode(PinPort(wirex->sda), PinBit(wirex->sda), GPIO_Mode_AF,
			GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);

	/* Configure I2C pins: SCL and SDA */
	GPIO_PinAFConfig(PinPort(wirex->scl), PinSource(wirex->scl), gpio_af); //GPIO_AF_I2C1 );
	GPIO_PinAFConfig(PinPort(wirex->sda), PinSource(wirex->sda), gpio_af); //GPIO_AF_I2C1 );

//#if defined (REMAP_I2C1)
//Remap_I2C1_Configuration();
//#endif
	switch (clkspeed) {
	case 100000:
	case 400000:
		break;
	default:
		clkspeed = 100000;
		break;
	}
	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = clkspeed;

	/* Apply I2C configuration after enabling it */
	I2C_Init(wirex->I2Cx, &I2C_InitStructure);
	/* I2C Peripheral Enable */
	I2C_Cmd(wirex->I2Cx, ENABLE);

	wirex->status = NOT_READY;
	wirex->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_transmit(I2CBus * wirex, uint8_t addr, uint8_t * data,
		uint16_t length) {
	uint16_t i;
	uint16_t wc;

	wirex->mode = I2C_MODE_MASTERTRANSMITTER;
	//
	if (!i2c_start(wirex, addr))
		return false;

	for (i = 0; i < length; i++) {
		I2C_SendData(wirex->I2Cx, data[i]);
		wirex->status = BYTE_TRANSMITTING;
		// Test on EV8 and clear it
		for (wc = 5;
				!I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
				wc--) {
			if (wc == 0)
				return false;
			delay_us(667);
		}
		wirex->status = BYTE_TRANSMITTED;
	}

	wirex->status = TRANSMISSION_COMPLETED;

	// generate stop condition
	I2C_GenerateSTOP(wirex->I2Cx, ENABLE);
	wirex->status = NOT_READY;
	wirex->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_receive(I2CBus * wirex, uint8_t addr, uint8_t req, uint8_t * recv,
		uint16_t lim) {
	uint16_t i;
	uint16_t wc;

	wirex->mode = I2C_MODE_MASTERRECEIVER;
	//
	if (!i2c_start(wirex, addr))
		return false;

	/* Send the EEPROM's internal address to read from: MSB of the address first */
	I2C_SendData(wirex->I2Cx, req);
	wirex->status = BYTE_TRANSMITTING;
	/* Test on EV8 and clear it */

	for (wc = 5;
			!I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = TRANSMISSION_COMPLETED;

	/* Send STRAT condition a second time */
	I2C_GenerateSTART(wirex->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_MODE_SELECT );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = RESTART_ISSUED;

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(wirex->I2Cx, addr << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	for (wc = 5;
			!I2C_CheckEvent(wirex->I2Cx,
					I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = SRC_ADDRESS_SENT;
	for (i = 1; i < lim; i++) {
		wirex->status = RECEIVE_BYTE_READY;
		for (wc = 5;
				!I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED );
				wc--) {
			if (wc == 0)
				return false;
			delay_us(667);

		}
		/* Read a byte from the EEPROM */
		*recv++ = I2C_ReceiveData(wirex->I2Cx);
		wirex->status = BYTE_RECEIVED;
	}
	wirex->status = BEFORELAST_BYTE_RECEIVED;

	/* Disable Acknowledgement */
	I2C_AcknowledgeConfig(wirex->I2Cx, DISABLE);
	/* Send STOP Condition */
	I2C_GenerateSTOP(wirex->I2Cx, ENABLE);
	wirex->status = LAST_BYTE_READY;

	for (wc = 5; !I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	/* Read a byte from the EEPROM */
	*recv = I2C_ReceiveData(wirex->I2Cx);
	wirex->status = RECEIVE_BYTE_COMPLETED;

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(wirex->I2Cx, ENABLE);
	wirex->status = NOT_READY;
	wirex->mode = I2C_MODE_NOTDEFINED;

	return true;
}

boolean i2c_start(I2CBus * wirex, uint8_t addr) {
	uint16_t wc;
	//
	wirex->status = NOT_READY;
	for (wc = 5; I2C_GetFlagStatus(wirex->I2Cx, I2C_FLAG_BUSY ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = READY;

	/* Send STRAT condition */
	I2C_GenerateSTART(wirex->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wirex->I2Cx, I2C_EVENT_MASTER_MODE_SELECT );
			wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = START_ISSUED;

	/* Send address for write */
	I2C_Send7bitAddress(wirex->I2Cx, addr << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	for (wc = 5;
			!I2C_CheckEvent(wirex->I2Cx,
					I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wirex->status = DST_ADDRESS_SENT;
	return true;

}
