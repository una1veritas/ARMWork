/*
 * gpio_digital.h
 *
 *  Created on: 2012/09/30
 *      Author: sin
 */

#ifndef _GPIO_DIGITAL_H_
#define _GPIO_DIGITAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

static GPIO_TypeDef * Port[] =
		{ 0, GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF };

/*
typedef enum
{ GPIO_Mode_AIN = 0x0,
  GPIO_Mode_IN_FLOATING = 0x04,
  GPIO_Mode_IPD = 0x28,
  GPIO_Mode_IPU = 0x48,
  GPIO_Mode_Out_OD = 0x14,
  GPIO_Mode_Out_PP = 0x10,
  GPIO_Mode_AF_OD = 0x1C,
  GPIO_Mode_AF_PP = 0x18
} GPIOMode_TypeDef;

typedef enum
{
  GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
}GPIOPuPd_TypeDef;

typedef enum
{ 
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
}GPIOOType_TypeDef;

typedef enum
{ 
  GPIO_Speed_2MHz   = 0x00, // Low speed
  GPIO_Speed_25MHz  = 0x01, // Medium speed
  GPIO_Speed_50MHz  = 0x02, // Fast speed
  GPIO_Speed_100MHz = 0x03  // High speed on 30 pF (80 MHz Output max speed on 15 pF) 
}GPIOSpeed_TypeDef;
*/

// bit 0-1: IO direction
#define INPUT			0  // 0
#define OUTPUT		1 // 1
#define ALTFUNC		2  // 2  AN 3
#define ANALOGIN	3  // 2  AN 3
// bit 2: type 
#define PUSHPULL 		(0<<2)
#define OPENDRAIN 	(1<<2)
// bit 3-4: internal pull-up
#define NOPULL		(0<<3)
#define PULLUP		(1<<3)
#define PULLDOWN	(2<<3)
// bit 5-6: clock speed
#define LOWSPEED 	(0<<5)
#define MEDSPEED 	(1<<5)
#define FASTSPEED (2<<5)
#define HIGHSPEED (3<<5)

#define HIGH		SET
#define LOW			RESET

#define PinSource(p) 		((p) & 0x0f)
#define PinPort(p) 			(Port[(p) >> 4 & 0x0f])
#define PinBit(p) 			((uint16_t)1<<((p) & 0x0f))


enum PortNameDef {
	Not_A_Port = 0,
	PortA = 1,
	PortB,
	PortC,
	PortD,
	PortE,
	PortF,
	PortG,
	PortH,
	PortI,
	PORT_NOT_DEFINED = 0xff
};

typedef enum _GPIOPin_Def {
	NOT_A_PIN = 0,
	PA0 = (PortA <<4) | ((uint8_t)0),
	PA1 = (PortA <<4) | ((uint8_t)1),
	PA2 = (PortA <<4) | ((uint8_t)2),
	PA3 = (PortA <<4) | ((uint8_t)3),
	PA4 = (PortA <<4) | ((uint8_t) 4),
	PA5 = (PortA <<4) | ((uint8_t) 5),
	PA6 = (PortA <<4) | ((uint8_t) 6),
	PA7 = (PortA <<4) | ((uint8_t) 7),
	PA8 = (PortA <<4) | ((uint8_t)8),
	PA9 = (PortA <<4) | ((uint8_t)9),
	PA10 = (PortA <<4) | ((uint8_t)10),
	PA11 = (PortA <<4) | ((uint8_t) 11),
	PA12 = (PortA <<4) | ((uint8_t)12),
	PA13 = (PortA <<4) | ((uint8_t)13),
	PA14 = (PortA <<4) | ((uint8_t)14),
	PA15 = (PortA <<4) | ((uint8_t) 15),
	PA_ALL = (PortA <<4) | ((uint8_t) 0x0f),

	PB0 = (PortB <<4) | ((uint8_t)0),
	PB1 = (PortB <<4) | ((uint8_t)1),
	PB2 = (PortB <<4) | ((uint8_t)2),
	PB3 = (PortB <<4) | ((uint8_t)3),
	PB4 = (PortB <<4) | ((uint8_t)4),
	PB5 = (PortB <<4) | ((uint8_t)5),
	PB6 = (PortB <<4) | ((uint8_t)6),
	PB7 = (PortB <<4) | ((uint8_t)7),
	PB8 = (PortB <<4) | ((uint8_t)8),
	PB9 = (PortB <<4) | ((uint8_t)9),
	PB10 = (PortB <<4) | ((uint8_t)10),
	PB11 = (PortB <<4) | ((uint8_t)11),
	PB12 = (PortB <<4) | ((uint8_t)12),
	PB13 = (PortB <<4) | ((uint8_t)13),
	PB14 = (PortB <<4) | ((uint8_t)14),
	PB15 = (PortB <<4) | ((uint8_t)15),
	PB_ALL = (PortB <<4) | ((uint8_t) 0x0f),

	PC0 = (PortC <<4) | ((uint8_t)0),
	PC1 = (PortC <<4) | ((uint8_t)1),
	PC2 = (PortC <<4) | ((uint8_t)2),
	PC3 = (PortC <<4) | ((uint8_t)3),
	PC4 = (PortC <<4) | ((uint8_t)4),
	PC5 = (PortC <<4) | ((uint8_t)5),
	PC6 = (PortC <<4) | ((uint8_t)6),
	PC7 = (PortC <<4) | ((uint8_t)7),
	PC8 = (PortC <<4) | ((uint8_t)8),
	PC9 = (PortC <<4) | ((uint8_t)9),
	PC10 = (PortC <<4) | ((uint8_t)10),
	PC11 = (PortC <<4) | ((uint8_t)11),
	PC12 = (PortC <<4) | ((uint8_t)12),
	PC13 = (PortC <<4) | ((uint8_t)13),
	PC14 = (PortC <<4) | ((uint8_t)14),
	PC15 = (PortC <<4) | ((uint8_t)15),
	PC_ALL = (PortC <<4) | ((uint8_t) 0x0f),

	PD0 = (PortD <<4) | ((uint8_t)0),
	PD1 = (PortD <<4) | ((uint8_t)1),
	PD2 = (PortD <<4) | ((uint8_t)2),
	PD3 = (PortD <<4) | ((uint8_t)3),
	PD4 = (PortD <<4) | ((uint8_t)4),
	PD5 = (PortD <<4) | ((uint8_t)5),
	PD6 = (PortD <<4) | ((uint8_t)6),
	PD7 = (PortD <<4) | ((uint8_t)7),
	PD8 = (PortD <<4) | ((uint8_t)8),
	PD9 = (PortD <<4) | ((uint8_t)9),
	PD10 = (PortD <<4) | ((uint8_t)10),
	PD11 = (PortD <<4) | ((uint8_t)11),
	PD12 = (PortD <<4) | ((uint8_t)12),
	PD13 = (PortD <<4) | ((uint8_t)13),
	PD14 = (PortD <<4) | ((uint8_t)14),
	PD15 = (PortD <<4) | ((uint8_t)15),
	PD_ALL = (PortD <<4) | ((uint8_t) 0x0f),

	PE0 = (PortE <<4) | ((uint8_t)0),
	PE1 = (PortE <<4) | ((uint8_t)1),
	PE2 = (PortE <<4) | ((uint8_t)2),
	PE3 = (PortE <<4) | ((uint8_t)3),
	PE4 = (PortE <<4) | ((uint8_t)4),
	PE5 = (PortE <<4) | ((uint8_t)5),
	PE6 = (PortE <<4) | ((uint8_t)6),
	PE7 = (PortE <<4) | ((uint8_t)7),
	PE8 = (PortE <<4) | ((uint8_t)8),
	PE9 = (PortE <<4) | ((uint8_t)9),
	PE10 = (PortE <<4) | ((uint8_t)10),
	PE11 = (PortE <<4) | ((uint8_t)11),
	PE12 = (PortE <<4) | ((uint8_t)12),
	PE13 = (PortE <<4) | ((uint8_t)13),
	PE14 = (PortE <<4) | ((uint8_t)14),
	PE15 = (PortE <<4) | ((uint8_t)15),
	PE_ALL = (PortE <<4) | ((uint8_t) 0x0f),

	PF0 = (PortF <<4) | ((uint8_t)0),
	PF_ALL = (PortF <<4) | ((uint8_t) 0x0f),

	PIN_NOT_DEFINED = (uint8_t)0xff
} GPIOPin_Def;
typedef uint8_t 	GPIOPin;

void pinDisable(GPIOPin);
void pinMode(GPIOPin portpin, uint8_t mode);
void digitalWrite(GPIOPin portpin, uint8_t bit);
uint8_t digitalRead(GPIOPin portpin);

void GPIOAltFunc(GPIOPin portpins, uint16_t pinbits, GPIOMode_TypeDef GPIOType_AF);

void GPIOEnable(GPIOPin portpins);
void GPIODisable(GPIOPin portpins);
void GPIOMode(GPIOPin portpins, uint16_t pinbits, uint8_t mode);
uint16_t GPIORead(GPIOPin portpins);
void GPIOWrite(GPIOPin portpins, uint16_t bits);

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_DIGITAL_H_ */
