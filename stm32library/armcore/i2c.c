/*
 * i2c.c
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#include <string.h>

#include "stm32f4xx_rcc.h"

#include "gpio.h"
#include "delay.h"
#include "i2c.h"

/*
typedef enum __CommDirection {
	NOT_DEFINED = 0, 
	TRANSMITTER, 
	RECEIVER,
} CommDirection;
*/

I2CBuffer I2CBuffer1 = { I2C1 }, I2CBuffer2 = { I2C2 }, I2CBuffer3 = { I2C3 };

boolean i2c_begin(I2CBuffer * wirebuf, GPIOPin sda, GPIOPin scl, uint32_t clk) {
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	// set default to I2C1
	uint8_t gpio_af = GPIO_AF_I2C1;
	uint32_t periph_i2c = RCC_APB1Periph_I2C1;
	
	wirebuf->sda = sda; //PB9;
	wirebuf->scl = scl; //PB8;

	if ( wirebuf->I2Cx == I2C2 ) {
		gpio_af = GPIO_AF_I2C2;
		periph_i2c = RCC_APB1Periph_I2C2;
	} else if ( wirebuf->I2Cx == I2C3 ) {
		gpio_af = GPIO_AF_I2C3;
		periph_i2c = RCC_APB1Periph_I2C3;
	//} else { I2C1, use initiali values set to the variables
	}
	
	/* I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(periph_i2c, ENABLE); //  RCC_APB1PeriphClockCmd(I2C1_RCC, ENABLE);
	/* GPIO Periph clock enable */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // PB5 (SMBA), PB6 (SCL), PB9 (SDA)  // RCC_APB2PeriphClockCmd(I2C1_GPIO_RCC, ENABLE);
	GPIOMode(PinPort(scl), PinBit(scl), GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIOMode(PinPort(sda), PinBit(sda), GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);

	/* Configure I2C pins: SCL and SDA */
	GPIO_PinAFConfig(PinPort(scl), PinSource(scl), gpio_af); //GPIO_AF_I2C1 );
	GPIO_PinAFConfig(PinPort(sda), PinSource(sda), gpio_af); //GPIO_AF_I2C1 );

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
	I2C_Init(wirebuf->I2Cx, &I2C_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);	// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	NVIC_Init(&NVIC_InitStructure); 
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn; 
	NVIC_Init(&NVIC_InitStructure); 
	//
	/* I2C Peripheral Enable */
	I2C_Cmd(wirebuf->I2Cx, ENABLE);

//	wire->status = NOT_READY;
	wirebuf->mode = I2C_MODE_MASTER;
	wirebuf->irqmode = true;
	wirebuf->address = (uint8_t)I2C_AcknowledgedAddress_7bit;

	return true;
}

boolean i2c_transmit(I2CBuffer * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	uint16_t i;
	uint16_t wc;
	uint32_t temp;
	
	wire->mode = I2C_MODE_MASTER_TX;
	
	for (wc = 5; I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->status = READY;
	if ( wire->irqmode ) {
		wire->address = addr;
		memcpy(wire->databytes, data, length);
		wire->length = length;
		wire->status = STARTING;
		I2C_ITConfig(wire->I2Cx, I2C_IT_EVT, ENABLE);
		I2C_GenerateSTART(wire->I2Cx, ENABLE);
	} else {
		//polling mode
		if (!i2c_start(wire, addr))
			return false;

		for (i = 0; i < length; i++) {
			I2C_SendData(wire->I2Cx, data[i]);
			// Test on EV8 and clear it
			for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ); wc--) {
				if (wc == 0)
					return false;
				delay_us(667);
			}
		}
		
		// generate stop condition
		__disable_irq();
		// Clear ADDR flag
//		temp = wire->I2Cx->SR2;
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
		__enable_irq();
	}
	wire->mode = I2C_MODE_MASTER;
	return true;
}

boolean i2c_receive(I2CBuffer * wire, uint8_t addr, uint8_t req, uint8_t * recv,
		uint16_t lim) {
	uint16_t i;
	uint16_t wc;

	wire->mode = I2C_MODE_MASTER_RX;
	//
	if (!i2c_start(wire, addr))
		return false;

	// Send the EEPROM's internal address to read from: MSB of the address first 
	I2C_SendData(wire->I2Cx, req);
//	wire->status = BYTE_TRANSMITTING;
	// Test on EV8 and clear it

	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
//	wire->status = TRANSMISSION_COMPLETED;

	/* Send STRAT condition a second time */
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
//	wire->status = RESTART_ISSUED;

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(wire->I2Cx, addr << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
//	wire->status = ADDRESS_SENT_MATCHED;
	for (i = 1; i < lim; i++) {
//		wire->status = RECEIVE_BYTE_READY;
		for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED ); wc--) {
			if (wc == 0)
				return false;
			delay_us(667);

		}
		/* Read a byte from the EEPROM */
		*recv++ = I2C_ReceiveData(wire->I2Cx);
//		wire->status = BYTE_RECEIVED;
	}
//	wire->status = BEFORELAST_BYTE_RECEIVED;

	/* Disable Acknowledgement */
	I2C_AcknowledgeConfig(wire->I2Cx, DISABLE);
	/* Send STOP Condition */
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
//	wire->status = LAST_BYTE_READY;

	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	/* Read a byte from the EEPROM */
	*recv = I2C_ReceiveData(wire->I2Cx);
//	wire->status = RECEIVE_BYTE_COMPLETED;

	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(wire->I2Cx, ENABLE);
//	wire->status = NOT_READY;
	wire->mode = I2C_MODE_MASTER;

	return true;
}

boolean i2c_start(I2CBuffer * wire, uint8_t addr) {
	uint16_t wc;
	//

/*	if ( wire->irq ) {
		I2C_ITConfig(wire->I2Cx, I2C_IT_EVT, ENABLE);
		I2C_GenerateSTART(I2C1, ENABLE);
		while ( wire->status != START_ISSUED );
		I2C_ITConfig(wire->I2Cx, I2C_IT_EVT, ENABLE);
		while ( wire->status != DST_ADDRESS_SENT );
	} else {
	*/
	/* Send STRAT condition */
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	/* Send address for write */
	I2C_Send7bitAddress(wire->I2Cx, addr << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ); wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}	
	return true;
}


void I2C1_EV_IRQHandler(void) {
	uint32_t temp;
	
	if ( I2C_GetITStatus(I2C1, I2C_IT_SB ) == SET
		&& I2CBuffer1.status == STARTING ) {
		// Test on EV5 and clear it 
		I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT );
		//
		I2C_Send7bitAddress(I2C1, (I2CBuffer1.address) << 1, I2C_Direction_Transmitter );
		I2CBuffer1.status = ADDRESS_SENDING;
	} else 
	if ( I2C_GetITStatus(I2C1, I2C_IT_ADDR ) == SET 
		&& I2CBuffer1.status == ADDRESS_SENDING ) {
		I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED );
		//	temp = I2Cx->SR2;
		//
		I2CBuffer1.position = 0;
		I2C_SendData(I2CBuffer1.I2Cx, I2CBuffer1.databytes[I2CBuffer1.position]);
		I2CBuffer1.status = BYTE_TRANSFERRING;
		// Test on EV8 and clear it
	} else 
	if ( I2C_GetITStatus(I2C1, I2C_IT_BTF ) == SET 
		&& I2CBuffer1.status == BYTE_TRANSFERRING ) {
		// Test on EV8 and clear it
		I2C_CheckEvent(I2CBuffer1.I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
		//
		I2CBuffer1.position++;
		if ( I2CBuffer1.position < I2CBuffer1.length ) {
			I2C_SendData(I2CBuffer1.I2Cx, I2CBuffer1.databytes[I2CBuffer1.position]);
			I2CBuffer1.status = BYTE_TRANSFERRING;
		} else {
			// generate stop condition
			// Clear ADDR flag
			temp = I2CBuffer1.I2Cx->SR2;
			I2C_GenerateSTOP(I2CBuffer1.I2Cx, ENABLE);
			I2CBuffer1.status = TRANSMISSION_COMPLETED;
			I2CBuffer1.mode = I2C_MODE_MASTER;
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
		}			
	}
	//I2C_ClearITPendingBit(I2C1, );
}
