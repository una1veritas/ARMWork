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

I2CContext I2C1Buffer, I2C2Buffer, I2C3Buffer;
//uint32 swatch[8];

boolean i2c_begin(I2CContext * wirebuf, I2C_TypeDef * i2cx, GPIOPin sda, GPIOPin scl, uint32_t clk) {
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	uint8_t gpio_af = GPIO_AF_I2C1;
	uint32_t periph_i2c = RCC_APB1Periph_I2C1;
	
	wirebuf->I2Cx = i2cx;
	wirebuf->sda = sda; //PB9;
	wirebuf->scl = scl; //PB8;

	if ( wirebuf->I2Cx == I2C2 ) {
		gpio_af = GPIO_AF_I2C2;
		periph_i2c = RCC_APB1Periph_I2C2;
	} else if ( wirebuf->I2Cx == I2C3 ) {
		gpio_af = GPIO_AF_I2C3;
		periph_i2c = RCC_APB1Periph_I2C3;
	} else { //I2C1, use initiali values set to the variables
		wirebuf->I2Cx = I2C1;
		gpio_af = GPIO_AF_I2C1;
		periph_i2c = RCC_APB1Periph_I2C1;
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
	
//	wirebuf->status = NOT_READY;
	wirebuf->mode = I2C_MODE_MASTER_IDLE;
//	wirebuf->irqmode = false;
//	wirebuf->address = (uint8_t)I2C_AcknowledgedAddress_7bit;

	return true;
}

boolean i2c_start_send(I2CContext * wire) {
	uint16_t i;
	uint16_t wc;
	uint8 * sendp;

	//polling mode	
	for (wc=0; I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY) == SET ; wc++ ){
			if (wc > 8)
				return false;
			delay_us(67);
	}

	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ); wc++) {
		if (wc > 8)
			return false;
		delay_us(67);
	} // wc = 1
	/* Send address for write */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ); wc++) {
	if (wc > 8)
		return false;
		delay_us(67);
	} // wc = 5
	
	sendp = wire->buffer;
	for (i = 0; i < wire->limlen; i++) { //i > 0; i--) {
		I2C_SendData(wire->I2Cx, *sendp++);
		for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ; wc++) {
//		if ( I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY) == RESET ) break;
			if (wc > 8)
				return false;
			delay_us(67);
		}

	}
	if ( wire->mode == I2C_MODE_MASTER_TX ) {
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	}

	return true;
}

boolean i2c_transmit(I2CContext * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	boolean t;
	wire->mode = I2C_MODE_MASTER_TX;
	wire->address = addr;
	wire->limlen = length;
	wire->buffer = data;
	t = i2c_start_send(wire);
	// generate stop cond. inside of start_send
	wire->mode = I2C_MODE_MASTER_IDLE;
	if ( t ) {
		wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	return false;
}

boolean i2c_request(I2CContext * wire, uint8_t addr, uint8_t * data, uint16_t len, uint16_t lim) {
	boolean t;
	wire->mode = I2C_MODE_MASTER_RX;
	wire->address = addr;
	wire->limlen = len;
	wire->buffer = data;
	t = i2c_start_send(wire);
	if ( !t ) {
			wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
			I2C_GenerateSTOP(wire->I2Cx, ENABLE);
			return false;
	}
	wire->limlen = lim;
	if ( t && i2c_receive(&I2C1Buffer) ) {
		wire->mode = I2C_MODE_MASTER_IDLE;
		wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	return false;
}


/*
boolean i2c_irq_transmit(I2CContext * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	uint16_t i;
	uint16_t wc;
	uint32_t temp;
	
	for (wc = 32; I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY ) 
	//&& wire->mode != I2C_MODE_MASTER_IDLE
	; wc--) {
		if (wc == 0)
			return false;
		delay_us(667);
	}
	wire->mode = I2C_MODE_MASTER_TX;
	wire->status = READY;
	wire->address = addr;
	if ( wire->irqmode ) {
		memcpy(wire->databytes, data, length);
		wire->length = length;
		wire->status = STARTING;
//		swatch[STARTING] = micros();
		I2C_ITConfig(wire->I2Cx, I2C_IT_EVT, ENABLE);
		I2C_GenerateSTART(wire->I2Cx, ENABLE);

	} else {
		//polling mode
		if (!i2c_start(wire, I2C_Direction_Transmitter))
			return false;

		for (i = 0; i < length; i++) {
			I2C_SendData(wire->I2Cx, data[i]);
			// Test on EV8 and clear it
			for (wc = 5; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ); wc--) {
				if (wc == 0)
					return false;
				delay_us(67);
			}
		}
		// generate stop condition
		__disable_irq();
		temp = wire->I2Cx->SR2;	// Clear ADDR flag
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
		__enable_irq();
		wire->mode = I2C_MODE_MASTER_IDLE;
	}
	return true;
}
*/

boolean i2c_receive(I2CContext * wire) {
	uint16_t i;
	uint16_t wc;
	uint8 * recv;
	
	//polling mode
	wire->mode = I2C_MODE_MASTER_RX;
//	wire->status = READY;
	/* Send STRAT condition as restart */
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
	/* Test on EV5 and clear it */
	for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ); wc++) {
		if (wc > 8)
			return false;
		delay_us(67);
	} 
	/* Send address for read */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ); wc++) {
		if (wc > 8)
			return false;
		delay_us(67);
	} 
	
	recv = wire->buffer;
	for (i = wire->limlen; i > 0; ) {
		// Test on EV7 and clear it
		for (wc = 0; !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED); wc++) {
			if (wc > 72)
				return false;
			delay_us(67);
		}
		/* Read a byte from the slave */
		*recv++ = I2C_ReceiveData(wire->I2Cx);
		i--;
		if ( i > 1 ) 
			continue;
		/* Disable Acknowledgement */
		I2C_AcknowledgeConfig(wire->I2Cx, DISABLE);
		/* Send STOP Condition */
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	}
	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(wire->I2Cx, ENABLE);
	wire->mode = I2C_MODE_MASTER_IDLE;
	return true;
}


/*
void I2C1_EV_IRQHandler(void) {
	uint32_t temp;
	
	if ( I2C1Buffer.mode == I2C_MODE_MASTER_TX ) {
		if ( I2C_GetITStatus(I2C1, I2C_IT_SB ) == SET
			&& I2C1Buffer.status == STARTING ) {
			// read SR2 and clear it
			I2C_ReadRegister(I2C1, I2C_Register_SR2);  //	
			//	temp = I2C1->SR2;	//	
			//I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT );
			I2C_Send7bitAddress(I2C1, (I2C1Buffer.address) << 1, I2C_Direction_Transmitter );
			I2C1Buffer.status = ADDRESS_SENDING;
//						swatch[ADDRESS_SENDING] = micros();
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_ADDR ) == SET 
			&& I2C1Buffer.status == ADDRESS_SENDING ) {
			I2C_ReadRegister(I2C1, I2C_Register_SR2); //	
			//	temp = I2C1->SR2;	//	
			//I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED );
			//
			I2C1Buffer.position = 0;
			I2C_SendData(I2C1Buffer.I2Cx, I2C1Buffer.databytes[I2C1Buffer.position]);
			I2C1Buffer.status = BYTE_TRANSFERRING;
//						swatch[BYTE_TRANSFERRING] = micros();
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_BTF ) == SET 
			&& I2C1Buffer.status == BYTE_TRANSFERRING ) {
			// Test on EV8 and clear it
			I2C_ReadRegister(I2C1, I2C_Register_SR2); //	temp = I2C1->SR2;	//	I2C_CheckEvent(I2CContext1.I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
			//
			I2C1Buffer.position++;
			if ( I2C1Buffer.position < I2C1Buffer.length ) {
				I2C_SendData(I2C1Buffer.I2Cx, I2C1Buffer.databytes[I2C1Buffer.position]);
				I2C1Buffer.status = BYTE_TRANSFERRING;

			} else {
				I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);	
				// generate stop condition
				I2C_GenerateSTOP(I2C1Buffer.I2Cx, ENABLE);
				I2C1Buffer.status = TRANSMISSION_COMPLETED;
				I2C1Buffer.mode = I2C_MODE_MASTER_IDLE;
//				swatch[I2C_MODE_MASTER_IDLE] = micros();
			}			
		} else {
			// unexpected case
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			I2C1Buffer.status |= FAILURE;
		}
	} else 
	if (  I2C1Buffer.mode == I2C_MODE_MASTER_RX ) {
		if ( I2C_GetITStatus(I2C1, I2C_IT_SB ) == SET
			&& I2C1Buffer.status == STARTING ) {
				// read SR2 and clear it
				I2C_ReadRegister(I2C1, I2C_Register_SR2);  //	temp = I2C1->SR2;	//	I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT );
				I2C_Send7bitAddress(I2C1, (I2C1Buffer.address) << 1, I2C_Direction_Transmitter );
				I2C1Buffer.status = ADDRESS_SENDING;
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_ADDR ) == SET 
			&& I2C1Buffer.status == ADDRESS_SENDING ) {
				// Test on EV8 and clear it
				I2C_ReadRegister(I2C1, I2C_Register_SR2);  //	temp = I2C1->SR2;	//	I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED );
				I2C_SendData(I2C1, I2C1Buffer.databytes[0]);
				I2C1Buffer.status = REQUEST_SENDING;			
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_BTF ) == SET 
			&& I2C1Buffer.status == REQUEST_SENDING ) {
				I2C_ReadRegister(I2C1, I2C_Register_SR2);  //	temp = I2C1->SR2;	//	I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED );		
				// Send STRAT condition a second time 
				I2C_GenerateSTART(I2C1, ENABLE);
				I2C1Buffer.status = RESTARTING;
		} else 
		if (I2C_GetITStatus(I2C1, I2C_IT_SB ) == SET
			&& I2C1Buffer.status == RESTARTING ) {
				I2C_ReadRegister(I2C1, I2C_Register_SR2); //	temp = I2C1->SR2;	//	I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED );
				//
				I2C_Send7bitAddress(I2C1, I2C1Buffer.address << 1, I2C_Direction_Receiver );
				I2C1Buffer.status = RECEIVER_ADDRESS_SENDING;
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_ADDR ) == SET 
			&& I2C1Buffer.status == RECEIVER_ADDRESS_SENDING ) {
				I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED );
				I2C1Buffer.position = 0;
				I2C1Buffer.status = BYTE_RECEIVING;
		} else
		if ( I2C_GetITStatus(I2C1, I2C_IT_RXNE | I2C_IT_BTF ) == SET 
			&& I2C1Buffer.status == BYTE_RECEIVING ) {
				I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED );
				// Read a byte from the salve 
				I2C1Buffer.databytes[I2C1Buffer.position++] = I2C_ReceiveData(I2C1);
				if ( I2C1Buffer.position < I2C1Buffer.length ) {
					I2C1Buffer.status = BYTE_RECEIVING;
				} else {
					// Disable Acknowledgement 
					I2C_AcknowledgeConfig(I2C1, DISABLE);
					// Send STOP Condition 
					I2C_GenerateSTOP(I2C1, ENABLE);
					I2C1Buffer.status = END_RECEIVING;
				}
		} else 
		if ( I2C_GetITStatus(I2C1, I2C_IT_RXNE | I2C_IT_BTF ) == SET 
			&& I2C1Buffer.status == END_RECEIVING ) {
				I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED );
				I2C1Buffer.databytes[I2C1Buffer.position] = I2C_ReceiveData(I2C1);
				// Enable Acknowledgement to be ready for another reception 
				I2C_AcknowledgeConfig(I2C1, ENABLE);
				I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);	
				I2C1Buffer.status = TRANSMISSION_COMPLETED;
				I2C1Buffer.mode = I2C_MODE_MASTER_IDLE;
		} else {
			// unexpected case
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			I2C1Buffer.status |= FAILURE;
		}
	} 
	else {
		I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
		I2C1Buffer.status |= FAILURE;
	}
	
}
*/
