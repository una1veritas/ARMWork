/**
 *     GPIO/main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <math.h>

// USE_STDPERIPH_DRIVER is defined in both C and C++ at makefile(?)
#include "stm32f4xx.h"

#include "gpio_digital.h"
#include "stm32f4xx_it.h"
#include "delay.h"

int main(void) {

	uint8 c = 0;

	pinMode(PC13, OUTPUT);
	pinMode(PG8, OUTPUT);

	pinMode(PA0, INPUT);

	SysTick_init(1000);

	while (1) {
		if (digitalRead(PA0)) {
			c++;
		} else {
			c--;
		}
		switch ((c) % 4) {
		case 0:
			digitalWrite(PC13, HIGH);
			digitalWrite(PG8, HIGH);
			break;
		case 1:
			digitalWrite(PC13, LOW);
			digitalWrite(PG8, HIGH);
			break;
		case 2:
			digitalWrite(PC13, LOW);
			digitalWrite(PG8,  LOW);
			break;
		case 3:
			digitalWrite(PC13, HIGH);
			digitalWrite(PG8,  LOW);
			break;
		}
		SysTick_delay(50 + c);
	}

	return 0;
}

