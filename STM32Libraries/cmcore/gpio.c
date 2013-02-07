/*
 * gpio_digital.cpp
 *
 *  Created on: 2012/09/30
 *      Author: sin
 */

#include "gpio.h"

uint32_t PortPeriph[] = {
		0, // Arduino Headers
		RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC,
		RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF,
		RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI };

GPIO_TypeDef * Port[] =
		{ 0, GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI };

uint16_t Pin[] = { GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3,
		GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9,
		GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14,
		GPIO_Pin_15, GPIO_Pin_All };

GPIO_TypeDef * PinPort(GPIOPin portpin) {
	return Port[portpin >> 4 & 0x0f];
}

uint16_t PinBit(GPIOPin portpin) {
	return ((uint16_t)1)<<(portpin &0x0f);
}

uint8_t PinSource(GPIOPin portpin) {
	return portpin & 0x0f;
}

void pinMode(GPIOPin portpin, GPIOMode_TypeDef mode) {

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(PortPeriph[portpin>>4 & 0x0f], ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PinBit(portpin);
	GPIO_InitStructure.GPIO_Mode = mode;
	//
	GPIO_Init(Port[portpin>>4 & 0x0f], &GPIO_InitStructure);
}

void digitalWrite(GPIOPin portpin, uint8_t bit) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef * port = Port[portpin>>4 & 0x0f];
	uint8_t mode = (port->MODER) >> (PinSource(portpin) * 2) & 0x03;
	if (mode == GPIO_Mode_OUT) {
		if (bit) {
		//? Bit_SET : Bit_RESET ));
			GPIO_SetBits(port, PinBit(portpin));
		} else {
			GPIO_ResetBits(port, PinBit(portpin));
		}
	} else {
		GPIO_StructInit(&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = PinBit(portpin);
		GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef) (mode & 0x3);
		GPIO_InitStructure.GPIO_PuPd = (GPIOPuPd_TypeDef)(bit & 0x3);
	//
		GPIO_Init(port, &GPIO_InitStructure);
	}
}

uint8_t digitalRead(GPIOPin portpin) {
	GPIO_TypeDef * port = Port[portpin>>4 & 0x0f];
	uint8_t mode = (port->MODER) >> (PinSource(portpin) * 2);
	if (mode == GPIO_Mode_OUT)
		return (GPIO_ReadOutputDataBit(port, PinBit(portpin)) ? SET : RESET);
	return (GPIO_ReadInputDataBit(port, PinBit(portpin)) ? SET : RESET);
}

void GPIOMode(GPIO_TypeDef * port, uint16_t pinbit, GPIOMode_TypeDef mode,
              GPIOSpeed_TypeDef clk, GPIOOType_TypeDef otype, GPIOPuPd_TypeDef pupd) {
		GPIO_InitTypeDef GPIO_InitStructure;
		
	if ( port == GPIOA ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	} else if ( port == GPIOB ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	} else if ( port == GPIOC ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	} else if ( port == GPIOD ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	} else if ( port == GPIOE ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	} else if ( port == GPIOF ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	} else if ( port == GPIOG ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	} else if ( port == GPIOH ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	} else if ( port == GPIOI ) {
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	} else {
	// else NOT_A_PORT
		return;
	}		
	// assumes port is already waked up.

	GPIO_InitStructure.GPIO_Pin = pinbit;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = otype;
	GPIO_InitStructure.GPIO_PuPd = pupd;
	GPIO_InitStructure.GPIO_Speed = clk;
	//
	GPIO_Init(port, &GPIO_InitStructure);
}


void GPIOAltFunc(GPIO_TypeDef * port, uint16_t pinbits, uint8_t pinaf) {
	int i;
	for(i = 0; (pinbits>>i) != 0 ; i++) {
		if ( pinbits>>i & 1)
			GPIO_PinAFConfig(port, PinSource(i), pinaf);
	}
}

void GPIOWrite(GPIO_TypeDef * port, uint16_t bits) {
	GPIO_Write(port, bits);
}

void digitalToggle(GPIOPin portpin) {
	GPIO_ToggleBits(PinPort(portpin), PinBit(portpin));
}

/*
 void ConfigPin(GPIO_TypeDef *myGPIO, uint32_t PIN, uint32_t MODE, uint32_t SPEED, uint32_t PUPD) {

 //         myGPIO: The GPIOx port for the selected pin
 //         MODE: 0 = INPUT .... 1 = OUTPUT .... 2 = ALTERNATE FUNCTION .... 3 = ANALOG
 //         SPEED: 0 = 2MHz (Low Speed) .... 1 = 25MHz (Med. Speed) .... 2 = 50MHz (Fast Speed) .... 3 = 100MHz/80MHz (High Speed)(100MHz(30pf) - 80MHz(15pf))
 //         PUPD: 0 = No Pull-Up / No Pull-Down .... 1 = Pull-Up Enabled .... 2 = Pull-Down Enabled .... 3 = Reserved

 myGPIO->MODER |= (MODE << (PIN * 2));//OUTPUT
 myGPIO->OSPEEDR |= (SPEED << (PIN * 2));//50MHz
 myGPIO->PUPDR |= (PUPD << (PIN * 2)); //Set it for NO PUPD
 }
 */
