
#include "armcore.h"

void armcore_init() {
	// starting delay/millis system by timer2
	TIM2_timer_start();
#ifdef DEFAULTUSART
	usart_init(&stdserial, DEFAULTUSART, PC7, PC6, 57600);
#endif
}
