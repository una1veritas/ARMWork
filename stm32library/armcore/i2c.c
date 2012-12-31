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

I2CBuffer I2C1Buffer, I2C2Buffer, I2C3Buffer;
//uint32 swatch[8];

boolean i2c_begin(I2CBuffer * wirebuf, I2C_TypeDef * i2cx, GPIOPin sda, GPIOPin scl, uint32_t clk) {
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
	
	wirebuf->status = NOT_READY;
	wirebuf->mode = I2C_MODE_IDLE;
	wirebuf->irqmode = false;
//	wirebuf->address = (uint8_t)I2C_AcknowledgedAddress_7bit;
//I2C_ReadRegister(wirebuf->I2Cx, wirebuf->I2Cx->SR1);
//I2C_ReadRegister(wirebuf->I2Cx, wirebuf->I2Cx->SR2);
	I2C_ITConfig(wirebuf->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
	return true;
}

void i2c_setup_comm(I2CBuffer * i2cbuff, I2C_CommMode mode, uint8_t dstaddr, uint8_t * data, uint16_t length) {
	i2cbuff->mode = mode;
	i2cbuff->address = dstaddr;
	i2cbuff->position = 0;
	if ( (i2cbuff->limlen = length) > 0 )
		memcpy(i2cbuff->buffer, data, i2cbuff->limlen);
}

boolean i2c_transmit(I2CBuffer * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	// setup
	wire->mode = I2C_MODE_MASTER_TX;
	wire->address = addr;
	wire->position = 0;
	wire->limlen = length;
	memcpy(wire->buffer, data, length);
	wire->status = 0;
	//
	if ( i2c_start_send(wire) ) {
		wire->mode = I2C_MODE_IDLE;
		return true;
	}
	wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	return false;
}

boolean i2c_request(I2CBuffer * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	// setup
	wire->mode = I2C_MODE_MASTER_RQ;
	wire->address = addr;
	wire->position = 0;
	wire->limlen = length;
	memcpy(wire->buffer, data, length); // request command bytes
	wire->status = 0;
	//
	if ( i2c_start_send(wire) ) {
		wire->mode = I2C_MODE_IDLE;
		//wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	return false;
}

boolean i2c_receive(I2CBuffer * wire, uint8_t * data, uint16_t lim) {
	boolean t;
	wire->mode = I2C_MODE_MASTER_RX;
//	wire->address = addr;
	wire->position = 0;
	wire->limlen = lim;
//	memcpy(wire->buffer, data, length);
	wire->status = 0;
	t = i2c_start_receive(wire);
	memcpy(data, wire->buffer, wire->limlen) ;
	if ( t ) {
		wire->mode = I2C_MODE_IDLE;
		//wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->status = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	return false;
}


boolean i2c_start_send(I2CBuffer * wire) {
	uint16_t i;
	uint8 * sendp;
	uint32 watch = millis();
	const uint32 wait = 100;

	//polling mode	
	while ( I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY) == SET ){
		if ( millis() > watch + wait )
			return false;
	}

	watch = millis();
//	I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE );
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
//	while (wire->mode != I2C_MODE_IDLE);
//	I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
//	return true;
	delay_us(15);
	/* Test on EV5 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) ) {
		if ( millis() > watch + wait )
			return false;
	}
	/* Send address for write */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) {
		if ( millis() > watch + wait )
			return false;
	}
	
	sendp = wire->buffer;
	for (i = 0; i < wire->limlen; i++) { //i > 0; i--) {
		watch = millis();
		I2C_SendData(wire->I2Cx, *sendp++);
		while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ) {
			if ( millis() > watch + wait )
				return false;
		}
	}
	if ( wire->mode == I2C_MODE_MASTER_TX ) {
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	}
	return true;
}

boolean i2c_start_receive(I2CBuffer * wire) {
	uint16_t i;
	uint8 * recv;
	uint32 watch = millis();
	const uint32 wait = 100;
	
	//polling mode
	/* Send STRAT condition as restart */
	I2C_GenerateSTART(wire->I2Cx, ENABLE); 
	delay_us(15);
	/* Test on EV5 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ) ) {
		if ( millis() > watch + wait )
			return false;
	} 
	/* Send address for read */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ) {
		if ( millis() > watch + wait )
			return false;
	} 
	
	recv = wire->buffer;
	for (i = wire->limlen; i > 1; i--) {
		watch = millis();
		// Test on EV7 and clear it
		while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) {
		if ( millis() > watch + wait )
			return false;
		}
		/* Read a byte from the slave */
		*recv++ = I2C_ReceiveData(wire->I2Cx);
	}
	watch = millis();
	/* Disable Acknowledgement */
	I2C_AcknowledgeConfig(wire->I2Cx, DISABLE);
	/* Send STOP Condition */
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) {
		if ( millis() > watch + wait )
			return false;
	}
	/* Read a byte from the slave */
	*recv = I2C_ReceiveData(wire->I2Cx);
	
	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(wire->I2Cx, ENABLE);
	
	delay_us(15);
	return true;
}


/*************************************************************************
 * Function Name: I2C1_EvnIntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: I2C1 event interrupt handler
 *
 *************************************************************************/
void I2C1_EV_IRQHandler(void) {
	I2C1Buffer.status = I2C_GetLastEvent(I2C1);
	switch(I2C1Buffer.status) {
		case I2C_EVENT_MASTER_MODE_SELECT:
			I2C_Send7bitAddress(I2C1Buffer.I2Cx, I2C1Buffer.address << 1, I2C_Direction_Transmitter );
			I2C1Buffer.position = 0;
		break;
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			if ( I2C1Buffer.position < I2C1Buffer.limlen ) {
				I2C_SendData(I2C1, I2C1Buffer.position++);
				//I2C1Buffer.mode = I2C_MODE_IDLE;
			} else {
				if ( I2C1Buffer.mode == I2C_MODE_MASTER_TX ) {
					I2C_GenerateSTOP(I2C1, ENABLE);
				}	
				I2C1Buffer.mode = I2C_MODE_IDLE;
			}
			break;
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
			I2C1Buffer.position = 0;
			break;
	}

}

/*************************************************************************
 * Function Name: I2C1_ErrIntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: I2C1 error interrupt handler
 *
 *************************************************************************/
void I2C1_ER_IRQHandler(void) {
	I2CBuffer * wire = &I2C1Buffer;
	uint32 levt;
  if(I2C_EVENT_SLAVE_ACK_FAILURE & (levt = I2C_GetLastEvent(I2C1)) ) {
    // Generate Stop condition (return back to slave mode)
    I2C_GenerateSTOP(I2C1,ENABLE);
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
  }
  wire->mode = I2C_MODE_IDLE;
  wire->status = levt | 0x80000000;
}
