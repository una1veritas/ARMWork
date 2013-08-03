#ifndef _GPIO_H_
#define _GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "armcmx.h"

#define OUTPUT 1
#define INPUT  0
#define HIGH   1
#define LOW    0

enum PIONameDef {
	Not_A_Port = 0,
	PIO0 = 1<<0,
	PIO1 = 1<<1,
  PORT_NOT_DEFINED = 0xff,
};

static uint8_t PIONumber[] = { Not_A_Port, 0, 1, PORT_NOT_DEFINED }; 

enum _GPIOPin_Def {
	NOT_A_PIN = 0,
  PIO0_0 = (PIO0 <<5) | ((uint8_t)0),
  PIO0_1 = (PIO0 <<5) | ((uint8_t)1),
  PIO0_2 = (PIO0 <<5) | ((uint8_t)2),
  PIO0_3 = (PIO0 <<5) | ((uint8_t)3),
  PIO0_4 = (PIO0 <<5) | ((uint8_t)4),
  PIO0_5 = (PIO0 <<5) | ((uint8_t)5),
  PIO0_6 = (PIO0 <<5) | ((uint8_t)6),
  PIO0_7 = (PIO0 <<5) | ((uint8_t)7),
  PIO0_8 = (PIO0 <<5) | ((uint8_t)8),
  PIO0_9 = (PIO0 <<5) | ((uint8_t)9),
  PIO0_10 = (PIO0 <<5) | ((uint8_t)10),

  PIO0_18 = (PIO0 <<5) | ((uint8_t)18),
  PIO0_19 = (PIO0 <<5) | ((uint8_t)19),

  PIO0_21 = (PIO0 <<5) | ((uint8_t)21),
  PIO0_22 = (PIO0 <<5) | ((uint8_t)22),
  PIO0_23 = (PIO0 <<5) | ((uint8_t)23),

  PIO1_0 = (PIO1 <<5) | ((uint8_t)0),
  PIO1_1 = (PIO1 <<5) | ((uint8_t)1),
  PIO1_3 = (PIO1 <<5) | ((uint8_t)3),
  PIO1_4 = (PIO1 <<5) | ((uint8_t)4),
  PIO1_5 = (PIO1 <<5) | ((uint8_t)5),
  PIO1_6 = (PIO1 <<5) | ((uint8_t)6),
  PIO1_7 = (PIO1 <<5) | ((uint8_t)7),
  PIO1_8 = (PIO1 <<5) | ((uint8_t)8),
  PIO1_9 = (PIO1 <<5) | ((uint8_t)9),
  PIO1_10 = (PIO1 <<5) | ((uint8_t)10),
  PIO1_11 = (PIO1 <<5) | ((uint8_t)11),
  PIO1_12 = (PIO1 <<5) | ((uint8_t)12),
  PIO1_13 = (PIO1 <<5) | ((uint8_t)13),
  PIO1_14 = (PIO1 <<5) | ((uint8_t)14),
  PIO1_15 = (PIO1 <<5) | ((uint8_t)15),
  PIO1_16 = (PIO1 <<5) | ((uint8_t)16),
  PIO1_17 = (PIO1 <<5) | ((uint8_t)17),
  PIO1_18 = (PIO1 <<5) | ((uint8_t)18),
  PIO1_19 = (PIO1 <<5) | ((uint8_t)19),
  PIO1_20 = (PIO1 <<5) | ((uint8_t)20),
  PIO1_21 = (PIO1 <<5) | ((uint8_t)21),
  PIO1_22 = (PIO1 <<5) | ((uint8_t)22),
  PIO1_23 = (PIO1 <<5) | ((uint8_t)23),
  PIO1_24 = (PIO1 <<5) | ((uint8_t)24),
  PIO1_25 = (PIO1 <<5) | ((uint8_t)25),
  
  PIO1_29 = (PIO1 <<5) | ((uint8_t)29),

	PIN_NOT_DEFINED = (uint8_t) 0xff,
};

#define PIO_ClearFunc(x)      LPC_IOCON->x &= ~0x07
#define PIO_SetFunc(x, mode)  LPC_IOCON->x |= (mode & 0x07)
#define PIO_Reset(x)          LPC_IOCON->x = 0x90

#define PIO_MODE_INACTIVE 0x00
#define PIO_MODE_PULLDOWN 0x01
#define PIO_MODE_PULLUP   0x02
#define PIO_MODE_REPEATER 0x03
#define PIO_SetMode(x, mode) LPC_IOCON->x |= ((mode & 0x3)<<3)


void GPIOInit( void );

void pinMode( GPIOPin pin, uint8_t dir );
void digitalWrite(GPIOPin pin, uint8_t bitVal );
uint8_t digitalRead(GPIOPin pin);
uint8_t digitalToggle(GPIOPin pin);

#ifdef __cplusplus
}
#endif

#endif
