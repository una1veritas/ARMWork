
#include "cmcore.h"
#include "usart.h"


#if defined (STDSERIAL)
#if (STDSERIAL == 3)
#define STDUSART 	USART3
#define STDRX 		PB11
#define STDTX 		PB10
#define STDBAUD 	57600
#warning "using SERIAL 3"
#elif (STDSERIAL == 6)
#define STDUSART 	USART6
#define STDRX 		PC7
#define STDTX 		PC6
#define STDBAUD 	57600
#warning "using SERIAL 6"
#endif
#endif

void cmcore_init() {
	// starting delay/millis system by timer2
	TIM2_delay_start();
#ifdef STDSERIAL
	usart_init(&stdserial, STDUSART, STDRX, STDTX);
	usart_begin(&stdserial, STDBAUD);
#else
	#warning "No STDSERIAL defined."
#endif
}
