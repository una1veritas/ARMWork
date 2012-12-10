/*
 * i2c.c
 *
 *  Created on: 2012/11/03
 *      Author: sin
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include <stm32f4xx.h>

#include "gpio.h"
#include "delay.h"
#include "i2c.h"

/*
enum I2CPortNumber {
	I2C_1 = 0, I2C_2, I2C_3,
};

uint8_t i2c_id(I2C_TypeDef * I2Cx) {
	if (I2Cx == I2C1 )
		return I2C_1;
	if (I2Cx == I2C2 )
		return I2C_2;
	if (I2Cx == I2C3 )
		return I2C_3;
	return 0;
}

struct {
	GPIOPin_Type sda, scl;
	uint32_t periph;
	GPIO_TypeDef * gpio_sda;
	GPIO_TypeDef * gpio_scl;
	uint8_t sdasrc, sclsrc;
	uint8_t af;
} I2CPortInfo[] = {
		{ PB7, PB8,
				((uint32_t) RCC_APB1Periph_I2C1 ),
				GPIOB, GPIOB,
				GPIO_PinSource7, GPIO_PinSource8,
				GPIO_AF_I2C1
		},
		{ PB11, PB10, ((uint32_t) RCC_APB1Periph_I2C2 ),
				GPIOB, GPIOB,
				GPIO_PinSource11, GPIO_PinSource10, GPIO_AF_I2C2 //, USART1_IRQn
		},
		{ PC9, PA8, ((uint32_t) RCC_APB1Periph_I2C3 ),
				GPIOC, GPIOA,
				GPIO_PinSource9, GPIO_PinSource8, GPIO_AF_I2C3 //, USART1_IRQn
		},
};
*/

/**
 * @brief  I2C Configuration
 * @param  None
 * @retval None
 */
void i2c_begin(I2C_TypeDef * I2Cx, uint32_t clkspeed) {
//  GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	// Preset I2C configuration to the structure.
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = clkspeed;

	// startt he initialization sequence.
	/*
	if ( I2Cx == I2C2 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	} else if ( I2Cx == I2C3 ) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
	} else {
	*/
		// I2C1 as default
		// I2C Periph Clock enable.
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		// SDA -- GPIOB 7, 9 / SCL -- GPIOB 6, 8
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		// Set Pins as AF
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
		portMode(GPIOB, GPIO_Pin_9 | GPIO_Pin_6,
				GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_50MHz);
		/* Apply I2C configuration after enabling it */
		I2C_Init(I2C1, &I2C_InitStructure);
		// I2C_ITConfig
		// DMA_Init
		// I2C_DMACmd
		// I2C_DMALastTransferCmd
		I2C_Cmd(I2C1, ENABLE);
		// DMA_Cmd
		/*
	}
	*/

}

/**
 * @brief  Write Command to ST7032i
 * @param  Data : Command Data
 * @retval None
 */
void i2c_transmit(I2C_TypeDef * I2Cx, uint8_t addr, uint8_t * data, uint16_t n) {
	uint16_t i;

	/* Send STRAT condition */
//	I2C_GenerateSTART(I2Cx, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on EV5 and clear it */
//	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT ))
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT ))
		;
	/* Send EEPROM address for write */
//		I2C_Send7bitAddress(I2Cx, addr << 1, I2C_Direction_Transmitter );
		I2C_Send7bitAddress(I2C1, addr << 1, I2C_Direction_Transmitter );
	/* Test on EV6 and clear it */
//		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
			while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
		;

	for (i = 0; i < n; i++) {
//		I2C_SendData(I2Cx, data[i]);
		I2C_SendData(I2C1, data[i]);
		/* Test on EV8 and clear it */
//		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
			while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
			;
	}
//	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
}

