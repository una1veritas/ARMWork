
#include "armcore.h"

static USARTPort stdserial;

void armcore_init() {
	// starting delay/millis system by timer2
	TIM2_timer_start();
	usart_init(&stdserial, DEFAULTUSART, PC7, PC6, 115200);
}
