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

enum PortNameDef {
	Not_A_Port = 0xff,
	PIO0 = 0,
	PIO1 = 1,
};

enum _GPIOPin_Def {
	NOT_A_PIN = 0,
  PIO0_0 = (PIO0 <<4) | ((uint8_t)0),
  PIO0_1 = (PIO0 <<4) | ((uint8_t)1),
  PIO0_2 = (PIO0 <<4) | ((uint8_t)2),
  PIO0_3 = (PIO0 <<4) | ((uint8_t)3),
  PIO0_4 = (PIO0 <<4) | ((uint8_t)4),
  PIO0_5 = (PIO0 <<4) | ((uint8_t)5),
  PIO0_6 = (PIO0 <<4) | ((uint8_t)6),
  
  PIO0_18 = (PIO0 <<4) | ((uint8_t)18),
  PIO0_19 = (PIO0 <<4) | ((uint8_t)19),

  PIO1_0 = (PIO1 <<4) | ((uint8_t)0),
  PIO1_1 = (PIO1 <<4) | ((uint8_t)1),
  PIO1_3 = (PIO1 <<4) | ((uint8_t)3),
  PIO1_5 = (PIO1 <<4) | ((uint8_t)5),
  PIO1_6 = (PIO1 <<4) | ((uint8_t)6),
  PIO1_23 = (PIO1 <<4) | ((uint8_t)23),
  PIO1_25 = (PIO1 <<4) | ((uint8_t)25),
};

void GPIOInit( void );

void pinMode( GPIOPin pin, uint8_t dir );
void digitalWrite(GPIOPin pin, uint8_t bitVal );
uint8_t digitalRead(GPIOPin pin);
uint8_t digitalToggle(GPIOPin pin);

#ifdef __cplusplus
}
#endif

#endif
