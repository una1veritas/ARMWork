/*
 * usart.c
 *
 *  Created on: 2012/10/24
 *      Author: sin
 */

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <misc.h>
#include <stm32f4xx.h>

#include "gpio.h"
#include "delay.h"
#include "usart.h"

enum {
	USART1Serial = 0,
	USART2Serial,
	USART3Serial,
	UART4Serial,
	UART5Serial,
	USART6Serial
};

// for irq handlers
USARTRing * rxring[6], * txring[6];

void ring_clear(USARTRing * r) {
	r->head = 0;
	r->tail = 0;
	r->count = 0;
}

uint16_t ring_count(USARTRing * r) {
	return r->count;
}

uint8_t ring_is_full(USARTRing * r) {
	if ((r->head == r->tail) && (r->count > 0)) {
		return 1;
	}
	return 0;
}

uint16_t ring_enque(USARTRing * r, uint16_t w) {
	if (ring_is_full(r))
		return 0xffff;
	r->buf[r->head++] = w;
	r->count++;
	r->head %= USART_BUFFER_SIZE;
	return w;
}

uint16_t ring_deque(USARTRing * r) {
	uint16_t w;
	if (ring_count(r) == 0)
		return 0xffff;
	w = r->buf[r->tail++];
	r->count--;
	r->tail %= USART_BUFFER_SIZE;
	return w;
}

uint16_t ring_peek(USARTRing * r) {
	if (ring_count(r) == 0)
		return 0xffff;
	return r->buf[r->tail];
}

void usart_init(USART * usx, USART_TypeDef * usartx, GPIOPin rx, GPIOPin tx, uint32_t baud) {
	USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
	NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)
	//
	uint8_t af = GPIO_AF_USART1;
	IRQn_Type irq = USART1_IRQn;
	usx->USARTx = usartx;
	//usx->USARTx = usartx;

	if ( usx->USARTx == USART1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		af = GPIO_AF_USART1;
		irq = USART1_IRQn;
//		usx->usid = USART1Serial;
		rxring[USART1Serial] = &usx->rxring;
		txring[USART1Serial] = &usx->txring;
	} else if (usx->USARTx == USART2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		af = GPIO_AF_USART2;
		irq = USART2_IRQn;
//		usx->usid = USART2Serial;
//		usx->USARTx = USART2;
		rxring[USART2Serial] = &usx->rxring;
		txring[USART2Serial] = &usx->txring;
	} else if (usx->USARTx == USART3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		af = GPIO_AF_USART3;
		irq = USART3_IRQn;
//		usx->usid = USART3Serial;
//		usx->USARTx = USART3;
		rxring[USART3Serial] = &usx->rxring;
		txring[USART3Serial] = &usx->txring;
	} else if (usx->USARTx == UART4) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		af = GPIO_AF_UART4;
		irq = UART4_IRQn;
//		usx->usid = UART4Serial;
//		usx->USARTx = UART4;
		rxring[UART4Serial] = &usx->rxring;
		txring[UART4Serial] = &usx->txring;
	} else if (usx->USARTx == UART5) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		af = GPIO_AF_UART5;
		irq = UART5_IRQn;
//		usx->usid = UART5Serial;
//		usx->USARTx = UART5;
		rxring[UART5Serial] = &usx->rxring;
		txring[UART5Serial] = &usx->txring;
	} else { // Serial6
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		af = GPIO_AF_USART6;
		irq = USART6_IRQn;
//		usx->usid = USART6Serial;
//		usx->USARTx = USART6;
		rxring[USART6Serial] = &usx->rxring;
		txring[USART6Serial] = &usx->txring;
	}

	GPIOMode(PinPort(rx), PinBit(rx), GPIO_Mode_AF, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIOMode(PinPort(tx), PinBit(tx), GPIO_Mode_AF, GPIO_Speed_50MHz,
			GPIO_OType_PP, GPIO_PuPd_NOPULL);

	GPIO_PinAFConfig(PinPort(rx), PinSource(rx), af);
	GPIO_PinAFConfig(PinPort(tx), PinSource(tx), af);

	USART_InitStruct.USART_BaudRate = baud;	// the baudrate is set to the value we passed into this init function
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
	USART_InitStruct.USART_StopBits = USART_StopBits_1;	// we want 1 stop bit (standard)
	USART_InitStruct.USART_Parity = USART_Parity_No;// we don't want a parity bit (standard)
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver

	USART_Init(usx->USARTx, &USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

	USART_ITConfig(usx->USARTx, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt
	USART_ITConfig(usx->USARTx, USART_IT_TXE, DISABLE);

	NVIC_InitStructure.NVIC_IRQChannel = irq;
	// we want to configure the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // this sets the priority group of the USART interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);	// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	//
	ring_clear(&usx->rxring); //rxring[usx->usid]);
	ring_clear(&usx->txring); //txring[usx->usid]);
	// finally this enables the complete USART peripheral
	USART_Cmd(usx->USARTx, ENABLE);
}

void usart_polling_write(USART * usx, const uint16_t w) {
	while (USART_GetFlagStatus(usx->USARTx, USART_FLAG_TXE ) == RESET)
		;
	USART_SendData(usx->USARTx, w);
//	while (USART_GetFlagStatus(USART3, USART_FLAG_TC ) == RESET) ;
}

size_t usart_write(USART * usx, const uint16_t w) {
//	static uint16 lastchar; for two or more conseq. returns
	uint16_t waitcount = 7;
	while ( waitcount > 0 &&
				((ring_count(&usx->txring) > 0 && iscntrl(w))
				|| ring_is_full(&usx->txring) ) ) {
		delay_us(667);
		waitcount--;
	}
	USART_ITConfig(usx->USARTx, USART_IT_TXE, DISABLE);
	ring_enque(&usx->txring, w); //&txring[usx->usid], w);
	USART_ITConfig(usx->USARTx, USART_IT_TXE, ENABLE);
	return 1;
}

size_t usart_print(USART * usx, const char * s) {
	size_t n = 0;
	while (*s) {
		usart_write(usx, (uint16_t) *s++);
		n++;
	}
	return n;
}

uint16_t usart_polling_read(USART_TypeDef * USARTx /*usartx[usx]*/) {
	return USART_ReceiveData(USARTx);
}

uint16_t usart_read(USART * usx) {
	uint16_t w = ring_deque(&usx->rxring); //&rxring[usx->usid]);
	if (w == 0xffff)
		return 0; // buffer is empty
	return w;
}

void usart_flush(USART * usx) {
	uint32_t wtill = millis() + 100;
	while (ring_count(&usx->txring) > 0) {
		if (millis() > wtill)
			break;
	}
	/*
	 USART_ITConfig(usx->USARTx, USART_IT_RXNE, DISABLE); // disable the USART3 receive interrupt
	 buffer_clear(usx->rxring); //&rxring[usx->usid]);
	 USART_ClearITPendingBit(usx->USARTx, USART_IT_RXNE );
	 USART_ITConfig(usx->USARTx, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt
	 USART_ITConfig(usx->USARTx, USART_IT_TXE, DISABLE);
	 while ( buffer_count(usx->txring //&txring[usx->usid]
	 ) > 0 ) {
	 while (USART_GetFlagStatus(usx->USARTx, USART_FLAG_TXE ) == RESET);
	 USART_SendData(usx->USARTx, buffer_deque(usx->txring)); //&txring[usx->usid]));
	 while (USART_GetFlagStatus(usx->USARTx, USART_FLAG_TC ) == RESET);
	 }
	 USART_ClearITPendingBit(usx->USARTx, USART_IT_TXE );
	 */
	ring_clear(&usx->rxring); //&txring[usx->usid]);
}

uint16_t usart_peek(USART * usx) {
//	if ( buffer_count(&(usx->rxring)) == 0 )
//		return 0xffff;
	return ring_peek(&usx->rxring); //	rxring[usx->usid].buf[rxring[usx->usid].tail];
}

uint16_t usart_available(USART * usx) {
	return ring_count(&usx->rxring);
	//return buffer_count(&rxring[usx->usid]);
}

// this is the interrupt request handler (IRQ) for ALL USART3 interrupts

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE )) {
		ring_enque(rxring[USART1Serial],
				USART_ReceiveData(USART1 ));
	}

	if (USART_GetITStatus(USART1, USART_IT_TXE )) {
		if (txring[USART1Serial]->count
				== 0) {
			USART_ITConfig(USART1, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART1, USART_IT_TXE );
		} else {
			USART_SendData(USART1, ring_deque(txring[USART1Serial]) );
		}
	}
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE )) {
		ring_enque(rxring[USART2Serial], USART_ReceiveData(USART2 ));
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE )) {
		if (txring[USART2Serial]->count
		== 0) {
			USART_ITConfig(USART2, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART2, USART_IT_TXE );
		} else {
			USART_SendData(USART2, ring_deque(txring[USART2Serial]));
		}
	}
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE )) {
		ring_enque(rxring[USART3Serial],
				USART_ReceiveData(USART3 ));
	}

	if (USART_GetITStatus(USART3, USART_IT_TXE )) {
		if (txring[USART3Serial]->count == 0) {
			USART_ITConfig(USART3, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART3, USART_IT_TXE );
		} else {
			USART_SendData(USART3,
					ring_deque(txring[USART3Serial]));
		}
	}
}

void UART4_IRQHandler(void) {
	if (USART_GetITStatus(UART4, USART_IT_RXNE )) {
		ring_enque(txring[UART4Serial], USART_ReceiveData(UART4 ));
	}

	if (USART_GetITStatus(UART4, USART_IT_TXE )) {
		if (txring[UART4Serial]->count == 0) {
			USART_ITConfig(UART4, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(UART4, USART_IT_TXE );
		} else {
			USART_SendData(UART4, ring_deque(txring[UART4Serial]));
		}
	}
}
/*
 void UART5_IRQHandler(void) {
 if (USART_GetITStatus(UART5, USART_IT_RXNE )) {
 buffer_enque(&rxring[UART5Serial], USART_ReceiveData(UART5) );
 }

 if (USART_GetITStatus(USART3, USART_IT_TXE )) {
 if (txring[UART5Serial].count == 0) {
 USART_ITConfig(UART5, USART_IT_TXE, (FunctionalState) DISABLE);
 USART_ClearITPendingBit(UART5, USART_IT_TXE );
 } else {
 USART_SendData(UART5, buffer_deque(&txring[UART5Serial]));
 }
 }
 }
*/
void USART6_IRQHandler(void) {
	if (USART_GetITStatus(USART6, USART_IT_RXNE )) {
		ring_enque(rxring[USART6Serial],
				USART_ReceiveData(USART6 ));
	}

	if (USART_GetITStatus(USART6, USART_IT_TXE )) {
		if (txring[USART6Serial]->count == 0) {
			USART_ITConfig(USART6, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART6, USART_IT_TXE );
		} else {
			USART_SendData(USART6,
					ring_deque(txring[USART6Serial]));
		}
	}
}


