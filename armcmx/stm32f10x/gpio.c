/*
 * gpio_digital.cpp
 *
 *  Created on: 2012/09/30
 *      Author: sin
 */

#include "armcmx.h"
#include "gpio.h"

uint32_t PortPeriph[] = {
		0, // Arduino Headers
		RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC,
		RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE, RCC_APB2Periph_GPIOF, RCC_APB2Periph_GPIOG,
 };

/*
GPIO_TypeDef * Port[] =
		{ 0, GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI };
*/

uint16_t Pin[] = { GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3,
		GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9,
		GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14,
		GPIO_Pin_15, GPIO_Pin_All };


void pinDisable(GPIOPin portpin) {
	RCC_APB2PeriphClockCmd(PortPeriph[portpin>>4 & 0x0f], DISABLE);
}

void pinMode(GPIOPin portpin, uint8_t modes) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(PortPeriph[portpin>>4 & 0x0f], ENABLE);
  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PinBit(portpin);
	if ( (modes & 0x03) == INPUT ) {
		switch (modes & (3<<3)) {
			case NOPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			break;
			case PULLUP:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			break;
			case PULLDOWN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
			break;
		}
	} else if ( (modes & 0x03) == OUTPUT ) {
		switch (modes & (1<<2)) {
			case PUSHPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			break;
			case OPENDRAIN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			break;
		}
	} else if ( (modes & 0x03) == ALTFUNC ) {
		switch (modes & (1<<2)) {
			case PUSHPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			break;
			case OPENDRAIN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
			break;
		}
	} else if ( (modes & 0x03) == ANALOGIN ) {
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	}
	
	switch (modes & (3<<5)) {
		case LOWSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		break;
		case MEDSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		break;
		case FASTSPEED:
		case HIGHSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		break;
	}
	GPIO_Init(PinPort(portpin), &GPIO_InitStructure);
}


void digitalWrite(GPIOPin portpin, uint8_t bit) {
	GPIO_TypeDef * port = PinPort(portpin); //[portpin>>4 & 0x0f];
#ifdef USE_DIGITALWRITE_PULLUP
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t mode = (port->MODER) >> (PinSource(portpin) * 2) & 0x03;
	if (mode == GPIO_Mode_OUT) {
#endif
    GPIO_WriteBit(port, PinBit(portpin), ( bit ? Bit_SET : Bit_RESET) );
    
#ifdef USE_DIGITALWRITE_PULLUP
	} else {
		GPIO_StructInit(&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = PinBit(portpin);
		GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef) (mode & 0x3);
		GPIO_InitStructure.GPIO_PuPd = (GPIOPuPd_TypeDef)(bit & 0x3);
	//
		GPIO_Init(port, &GPIO_InitStructure);
	}
#endif
}

uint8_t digitalRead(GPIOPin portpin) {
	GPIO_TypeDef * port = PinPort(portpin); //[portpin>>4 & 0x0f];
//	uint8_t mode = (port->MODER) >> (PinSource(portpin) * 2) & 0x03;
//	if (mode == GPIO_Mode_OUT)
//		return (GPIO_ReadOutputDataBit(port, PinBit(portpin)) ? SET : RESET);
	return (GPIO_ReadInputDataBit(port, PinBit(portpin)) ? SET : RESET);
}


void GPIOEnable(GPIOPin portpins) {
  RCC_APB2PeriphClockCmd(PortPeriph[portpins>>4 & 0x0f], ENABLE);
}

void GPIODisable(GPIOPin portpins) {
  RCC_APB2PeriphClockCmd(PortPeriph[portpins>>4 & 0x0f], DISABLE);
}

void GPIOMode(GPIOPin portpins, uint16_t pinbit, uint8_t modes) {
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(PortPeriph[portpins>>4 & 0x0f], ENABLE);

	GPIO_InitStructure.GPIO_Pin = pinbit;
//	GPIO_InitStructure.GPIO_Mode = mode;
//	GPIO_InitStructure.GPIO_OType = otype;
//	GPIO_InitStructure.GPIO_PuPd = pupd;
//	GPIO_InitStructure.GPIO_Speed = clk;
	if ( (modes & 0x03) == INPUT ) {
		switch (modes & (3<<3)) {
			case NOPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			break;
			case PULLUP:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			break;
			case PULLDOWN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
			break;
		}
	} else if ( (modes & 0x03) == OUTPUT ) {
		switch (modes & (1<<2)) {
			case PUSHPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			break;
			case OPENDRAIN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			break;
		}
	} else if ( (modes & 0x03) == ALTFUNC ) {
		switch (modes & (1<<2)) {
			case PUSHPULL:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			break;
			case OPENDRAIN:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
			break;
		}
	} else if ( (modes & 0x03) == ANALOGIN ) {
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	}
	
	switch (modes & (3<<5)) {
		case LOWSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		break;
		case MEDSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		break;
		case FASTSPEED:
		case HIGHSPEED:
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		break;
	}
	//
	GPIO_Init(PinPort(portpins), &GPIO_InitStructure);
}

uint16_t portRead(GPIOPin portpins) {
	return GPIO_ReadInputData(PinPort(portpins));
}

void portWrite(GPIOPin portpins, uint16_t bits) {
	GPIO_Write(PinPort(portpins), bits);
}

/*
void pinAltFunc(GPIOPin portpins, uint16_t pinbits, uint8_t pinaf) {
	int i;
	for(i = 0; (pinbits>>i) != 0 ; i++) {
		if ( pinbits>>i & 1)
			GPIO_PinAFConfig(PinPort(portpins), PinSource(i), pinaf);
	}
}
*/
/*
void digitalToggle(GPIOPin portpin) {
  if ( GPIO_ReadOutputDataBit(PinPort(portpin), PinBit(portpin)) == 0 )
    GPIO_WriteBit(PinPort(portpin),  PinBit(portpin), GPIO_ReadOutputDataBit(PinPort(portpin), Bit_SET);
  else
    GPIO_WriteBit(PinPort(portpin),  PinBit(portpin), GPIO_ReadOutputDataBit(PinPort(portpin), Bit_RESET);
}
*/
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
