/*
 * i2c.c
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#include <string.h>

#include "i2c.h"

// for IRQ Handler
I2CBuffer * I2CBuf[3];

void i2c_init(i2c * i2cptr, I2C_TypeDef * I2Cx, GPIOPin scl, GPIOPin sda) {
	if ( I2Cx == I2C2 ) {
		i2cptr->I2Cx = I2C2;
		i2cptr->scl = scl;
		i2cptr->sda = sda;
		I2CBuf[1] = & i2cptr->I2CBuf;
	} else if ( I2Cx == I2C3 ) {
		i2cptr->I2Cx = I2C3;
		i2cptr->scl = scl;
		i2cptr->sda = sda;
		I2CBuf[2] = & i2cptr->I2CBuf;
	} else { // default 
		i2cptr->I2Cx = I2C1;
		i2cptr->scl = scl;
		i2cptr->sda = sda;
		I2CBuf[0] = & i2cptr->I2CBuf;
	}
}

boolean i2c_begin(i2c * wire, uint32_t clk) {
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	uint8_t pinaf;// = GPIO_AF_I2C1;
	uint32_t periph; // = RCC_APB1Periph_I2C1;
	IRQn_Type irqn_ev, irqn_er;
	
	if ( wire->I2Cx == I2C2 ) {
			pinaf = GPIO_AF_I2C2;
			periph = RCC_APB1Periph_I2C2;
		irqn_ev = I2C2_EV_IRQn;
		irqn_er = I2C2_ER_IRQn;
	} else if ( wire->I2Cx == I2C3 ) {
			pinaf = GPIO_AF_I2C3;
			periph = RCC_APB1Periph_I2C3;
		irqn_ev = I2C3_EV_IRQn;
		irqn_er = I2C3_ER_IRQn;
	} else { // default 
			pinaf = GPIO_AF_I2C1;
			periph = RCC_APB1Periph_I2C1;
		irqn_ev = I2C1_EV_IRQn;
		irqn_er = I2C1_ER_IRQn;
	}

	/* I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(periph, ENABLE); //  RCC_APB1PeriphClockCmd(I2C1_RCC, ENABLE);
	/* GPIO Periph clock enable */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // PB5 (SMBA), PB6 (SCL), PB9 (SDA)  // RCC_APB2PeriphClockCmd(I2C1_GPIO_RCC, ENABLE);
	GPIOMode(PinPort(wire->scl), PinBit(wire->scl), GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIOMode(PinPort(wire->sda), PinBit(wire->sda), GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);

	/* Configure I2C pins: SCL and SDA */
	GPIO_PinAFConfig(PinPort(wire->scl), PinSource(wire->scl), pinaf); //GPIO_AF_I2C1 );
	GPIO_PinAFConfig(PinPort(wire->sda), PinSource(wire->sda), pinaf); //GPIO_AF_I2C1 );

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
	
	NVIC_InitStructure.NVIC_IRQChannel = irqn_ev;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);	// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	NVIC_Init(&NVIC_InitStructure); 
	NVIC_InitStructure.NVIC_IRQChannel = irqn_er; 
	NVIC_Init(&NVIC_InitStructure); 
	//
	/* I2C Peripheral Enable */
	I2C_Cmd(wire->I2Cx, ENABLE);
	
	wire->I2CBuf.flagstatus = 0;
	wire->I2CBuf.mode = I2C_MODE_IDLE;
	wire->irqmode = false;
//	wirebuf->waitfinish = true;
	I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
	return true;
}

void i2c_setup_comm(i2c * wire, I2C_Mode mode, uint8_t dstaddr, uint8_t * data, uint16_t length) {
	wire->I2CBuf.mode = mode;
	wire->address = dstaddr;
	wire->I2CBuf.position = 0;
	if ( data != NULL && (wire->I2CBuf.count = length) > 0 )
		memcpy(wire->I2CBuf.tx, data, wire->I2CBuf.count);
}

boolean i2c_transmit(i2c * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	// setup
	wire->I2CBuf.mode = I2C_MODE_MASTER_TX;
	wire->address = addr;
	wire->I2CBuf.position = 0;
	wire->I2CBuf.count = length;
	memcpy(wire->I2CBuf.tx, data, length);
	wire->I2CBuf.flagstatus = 0;
	//
	if ( i2c_start_send(wire) ) {
		return true;
	}
	wire->I2CBuf.flagstatus = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	return false;
}

boolean i2c_request(i2c * wire, uint8_t addr, uint8_t * data, uint16_t length) {
	// setup
	wire->I2CBuf.mode = I2C_MODE_MASTER_RQ;
	wire->address = addr;
	wire->I2CBuf.position = 0;
	wire->I2CBuf.count = length;
	memcpy(wire->I2CBuf.tx, data, length); // request command bytes
	wire->I2CBuf.flagstatus = 0;
	//
	if ( i2c_start_send(wire) ) {
		//wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->I2CBuf.flagstatus = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	return false;
}

boolean i2c_receive(i2c * wire, uint8_t * data, uint16_t lim) {
	boolean t;
	// setup
	wire->I2CBuf.mode = I2C_MODE_MASTER_RX;
//	wire->address = addr;
	wire->I2CBuf.position = 0;
	wire->I2CBuf.count = lim;
//	memcpy(wire->buffer, data, length);
	wire->I2CBuf.flagstatus = 0;
	t = i2c_start_receive(wire);
	memcpy(data, wire->I2CBuf.rx, wire->I2CBuf.count) ;
	if ( t ) {
		//wire->status = I2C_GetLastEvent(wire->I2Cx);
		return true;
	}
	wire->I2CBuf.flagstatus = 0x80000000 | I2C_GetLastEvent(wire->I2Cx);
	I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	return false;
}


boolean i2c_start_send(i2c * wire) {
//	uint16_t i;
//	uint8 * sendp;
	const uint32 wait = 100;
	wire->I2CBuf.watch = millis();

	//polling mode	
	while ( I2C_GetFlagStatus(wire->I2Cx, I2C_FLAG_BUSY) == SET ){
		if ( millis() > wire->I2CBuf.watch + wait )
			return false;
	}

	wire->I2CBuf.watch = millis();
//	I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE );
	I2C_GenerateSTART(wire->I2Cx, ENABLE);
//	while (wire->mode != I2C_MODE_IDLE);
//	I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
//	return true;
	delay_us(15);
	/* Test on EV5 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) ) {
		if ( millis() > wire->I2CBuf.watch + wait )
			return false;
	}
	/* Send address for write */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) {
		if ( millis() > wire->I2CBuf.watch + wait )
			return false;
	}
	
	if ( wire->irqmode && wire->I2CBuf.count > 1 ) {
		wire->I2CBuf.watch = millis();
		I2C_SendData(wire->I2Cx, wire->I2CBuf.tx[wire->I2CBuf.position++]);
		I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE );
		while ( wire->I2CBuf.position < wire->I2CBuf.count ) {
			if ( millis() > wire->I2CBuf.watch + wait )
				return false;
		}
		// done. //I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
	} else {
		while ( wire->I2CBuf.position < wire->I2CBuf.count ) { 
			I2C_SendData(wire->I2Cx, wire->I2CBuf.tx[wire->I2CBuf.position++]);
			wire->I2CBuf.watch = millis();
			while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ) {
				if ( millis() > wire->I2CBuf.watch + wait )
					return false;
			}
		}
	}
	if ( wire->I2CBuf.mode == I2C_MODE_MASTER_TX )
		I2C_GenerateSTOP(wire->I2Cx, ENABLE);
	wire->I2CBuf.mode = I2C_MODE_IDLE;
	return true;
}

boolean i2c_start_receive(i2c * wire) {
//	uint16_t i;
	const uint32 wait = 100;
	wire->I2CBuf.watch = millis();
	
	/* Send STRAT condition as restart */
	I2C_GenerateSTART(wire->I2Cx, ENABLE); 
	delay_us(15);
	/* Test on EV5 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ) ) {
		if ( millis() > wire->I2CBuf.watch + wait )
			return false;
	} 
	/* Send address for read */
	I2C_Send7bitAddress(wire->I2Cx, wire->address << 1, I2C_Direction_Receiver );
	/* Test on EV6 and clear it */
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ) {
		if ( millis() > wire->I2CBuf.watch + wait )
			return false;
	} 
	
	if ( wire->irqmode && wire->I2CBuf.count > 1 && false) {
				I2C_ITConfig(wire->I2Cx, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE );
				while ( wire->I2CBuf.position < wire->I2CBuf.count ) {
					if ( millis() > wire->I2CBuf.watch + wait )
						return false;
				}
				// done. //I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
	} else {
		while ( wire->I2CBuf.position < wire->I2CBuf.count ) {
			wire->I2CBuf.watch = millis();
			if ( wire->I2CBuf.position + 1 == wire->I2CBuf.count ) {
				/* Disable Acknowledgement */
				I2C_AcknowledgeConfig(wire->I2Cx, DISABLE);
				/* Send STOP Condition */
				I2C_GenerateSTOP(wire->I2Cx, ENABLE);
			}
			// Test on EV7 and clear it
			while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) {
			if ( millis() > wire->I2CBuf.watch + wait )
				return false;
			}
			/* Read a byte from the slave */
			wire->I2CBuf.rx[wire->I2CBuf.position++] = I2C_ReceiveData(wire->I2Cx);
			if (wire->I2CBuf.position == wire->I2CBuf.count) {
				/* Enable Acknowledgement to be ready for another reception */
				I2C_AcknowledgeConfig(wire->I2Cx, ENABLE);
			}
		}
	}
	/*
	wire->watch = millis();
	while ( !I2C_CheckEvent(wire->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) {
		if ( millis() > watch + wait )
			return false;
	}
	// Read a byte from the slave
	*recv = I2C_ReceiveData(wire->I2Cx);
*/	

	delay_us(15);
	wire->I2CBuf.mode = I2C_MODE_IDLE;
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
	I2CBuf[0]->flagstatus = I2C_GetLastEvent(I2C1);
	switch(I2CBuf[0]->flagstatus) {
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			if ( I2CBuf[0]->position < I2CBuf[0]->count ) {
				I2C_SendData(I2C1, I2CBuf[0]->tx[I2CBuf[0]->position++]);
				I2CBuf[0]->watch = millis();
				//I2C1Buffer.mode = I2C_MODE_IDLE;
			} else {
				// done.
				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
			}
			break;
			
		case I2C_EVENT_MASTER_BYTE_RECEIVED:
			if ( I2CBuf[0]->position + 1 == I2CBuf[0]->count ) {
				// Disable Acknowledgement 
				I2C_AcknowledgeConfig(I2C1, DISABLE);
				// Send STOP Condition 
				I2C_GenerateSTOP(I2C1, ENABLE);
			}
			I2CBuf[0]->rx[I2CBuf[0]->position++] = I2C_ReceiveData(I2C1);
			I2CBuf[0]->watch = millis();
			if (I2CBuf[0]->position == I2CBuf[0]->count) {
				// done.
				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE );
				// Enable Acknowledgement to be ready for another reception 
				I2C_AcknowledgeConfig(I2C1, ENABLE);
			}
			break;
//		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
//			I2C_ClearFlag(I2C1, I2C_FLAG_TXE);
		default:
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
	uint32 levt;
  if(I2C_EVENT_SLAVE_ACK_FAILURE & (levt = I2C_GetLastEvent(I2C1)) ) {
    // Generate Stop condition (return back to slave mode)
    I2C_GenerateSTOP(I2C1,ENABLE);
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
  }
  I2CBuf[0]->mode = I2C_MODE_IDLE;
  I2CBuf[0]->flagstatus = levt | 0x80000000;
}
