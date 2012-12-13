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

USARTSerial Serial1(USART1Serial), Serial2(USART2Serial),
Serial3(USART3Serial), Serial4(UART4Serial); //, Serial5, Serial6;
USARTRing rxring[6], txring[6];

void buffer_clear(USARTRing * r) {
	r->head = 0;
	r->tail = 0;
	r->count = 0;
}

uint16_t buffer_count(USARTRing * r) {
	return r->count;
}

uint8_t buffer_is_full(USARTRing * r) {
	if ((r->head == r->tail) && (r->count > 0)) {
		return 1;
	}
	return 0;
}

uint16_t buffer_enque(USARTRing * r, uint16_t w) {
	if (buffer_is_full(r))
		return 0xffff;
	r->buf[r->head++] = w;
	r->count++;
	r->head %= USART_BUFFER_SIZE;
	return w;
}

uint16_t buffer_deque(USARTRing * r) {
	uint16_t w;
	if (buffer_count(r) == 0)
		return 0xffff;
	w = r->buf[r->tail++];
	r->count--;
	r->tail %= USART_BUFFER_SIZE;
	return w;
}

uint16_t buffer_peek(USARTRing * r) {
	if (buffer_count(r) == 0)
		return 0xffff;
	return r->buf[r->tail];
}

void USARTSerial::begin(//Serial * usx,
		GPIOPin rx, GPIOPin tx, uint32_t baud) {
	USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
	NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)
	//
	uint8_t af = GPIO_AF_USART1;
	IRQn_Type irq = USART1_IRQn;

	if (usx == USART1Serial ) { //&Serial1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		af = GPIO_AF_USART1;
		irq = USART1_IRQn;
//		usx->usid = USART1Serial;
		USARTx = USART1;
		rxring = &rxring[USART1Serial];
		txring = &txring[USART1Serial];
	} else if (usx == USART2Serial ) { //&Serial2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		af = GPIO_AF_USART2;
		irq = USART2_IRQn;
	//  usx->usid = USART2Serial;
		USARTx = USART2;
		rxring = &rxring[USART2Serial];
		txring = &txring[USART2Serial];
	} else if (usx == USART3Serial ) { //&Serial3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		af = GPIO_AF_USART3;
		irq = USART3_IRQn;
		//usx->usid = USART3Serial;
		USARTx = USART3;
		rxring = &rxring[USART3Serial];
		txring = &txring[USART3Serial];
	}
	else if (usx == UART4Serial) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		af = GPIO_AF_UART4;
		irq = UART4_IRQn;
		//usx->usid = UART4Serial;
		USARTx = UART4;
		rxring = &rxring[UART4Serial];
		txring = &txring[UART4Serial];
	} /* else if (usx == &Serial5) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		af = GPIO_AF_UART5;
		irq = UART5_IRQn;
		usx->usid = UART5Serial;
		usx->USARTx = UART5;
		usx->rxring = &rxring[UART5Serial];
		usx->txring = &txring[UART5Serial];
	} else { // Serial6
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		af = GPIO_AF_USART6;
		irq = USART6_IRQn;
		usx->usid = USART6Serial;
		usx->USARTx = USART6;
		usx->rxring = &rxring[USART6Serial];
		usx->txring = &txring[USART6Serial];
	}
	*/

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

	USART_Init(USARTx, &USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt
	USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);

	NVIC_InitStructure.NVIC_IRQChannel = irq;
	// we want to configure the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART3 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);	// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	//
	buffer_clear(rxring); //&rxring[usx->usid]);
	buffer_clear(txring); //&txring[usx->usid]);
	// finally this enables the complete USART3 peripheral
	USART_Cmd(USARTx, ENABLE);
}

void USARTSerial::bare_write(//Serial * usx,
		const uint16_t w) {
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE ) == RESET)
		;
	USART_SendData(USARTx, w);
//	while (USART_GetFlagStatus(USART3, USART_FLAG_TC ) == RESET) ;
}

void USARTSerial::write(//Serial * usx,
		const uint16_t w) {
	uint16_t waitcount = 3;
	while (buffer_is_full(txring) && (waitcount > 0) ) {
		delay_us(667);
		waitcount--;
	}
	USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
	buffer_enque(txring, w); //&txring[usx->usid], w);
	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
}

void USARTSerial::print( const char * s) {
	while (*s)
		write((uint16_t) *s++);
}

uint16_t USARTSerial::bare_read() {
	return USART_ReceiveData(USARTx);
}

uint16_t USARTSerial::read() {
	uint16_t w = buffer_deque(rxring); //&rxring[usx->usid]);
	if (w == 0xffff)
		return 0; // buffer is empty
	return w;
}

void USARTSerial::flush() {
	uint32_t wtill = millis() + 100;
	while (buffer_count(txring) > 0) {
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
	buffer_clear(rxring); //&txring[usx->usid]);
}

uint16_t USARTSerial::peek() {
//	if ( buffer_count(&(usx->rxring)) == 0 )
//		return 0xffff;
	return buffer_peek(rxring); //	rxring[usx->usid].buf[rxring[usx->usid].tail];
}

uint16_t USARTSerial::available() {
	return buffer_count(rxring);
	//return buffer_count(&rxring[usx->usid]);
}

// this is the interrupt request handler (IRQ) for ALL USART3 interrupts

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE )) {
		buffer_enque(Serial1.rxring,
		//&rxring[USART1Serial],
				USART_ReceiveData(USART1 ));
	}

	if (USART_GetITStatus(USART1, USART_IT_TXE )) {
		if (Serial1.rxring->count
		//txring[USART1Serial].count
				== 0) {
			USART_ITConfig(USART1, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART1, USART_IT_TXE );
		} else {
			USART_SendData(USART1, buffer_deque(Serial1.txring)); // &txring[USART1Serial]) );
		}
	}
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE )) {
		buffer_enque(Serial2.rxring //&rxring[USART2Serial]/
				, USART_ReceiveData(USART2 ));
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE )) {
		if (Serial2.txring->count //txring[USART2Serial].count
		== 0) {
			USART_ITConfig(USART2, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART2, USART_IT_TXE );
		} else {
			USART_SendData(USART2, buffer_deque(Serial2.txring)); //&txring[USART2Serial]));
		}
	}
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE )) {
		buffer_enque(Serial3.rxring /*&rxring[USART3Serial]*/,
				USART_ReceiveData(USART3 ));
	}

	if (USART_GetITStatus(USART3, USART_IT_TXE )) {
		if (Serial3.txring->count == 0) {
			USART_ITConfig(USART3, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(USART3, USART_IT_TXE );
		} else {
			USART_SendData(USART3,
					buffer_deque(Serial3.txring /*&txring[USART3Serial]*/));
		}
	}
}

void UART4_IRQHandler(void) {
	if (USART_GetITStatus(UART4, USART_IT_RXNE )) {
		buffer_enque(Serial4.rxring, USART_ReceiveData(UART4 ));
	}

	if (USART_GetITStatus(UART4, USART_IT_TXE )) {
		if (Serial4.txring->count == 0) {
			USART_ITConfig(UART4, USART_IT_TXE, (FunctionalState) DISABLE);
			USART_ClearITPendingBit(UART4, USART_IT_TXE );
		} else {
			USART_SendData(UART4, buffer_deque(Serial4.txring));
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

 void USART6_IRQHandler(void) {
 if (USART_GetITStatus(USART6, USART_IT_RXNE )) {
 buffer_enque(&rxring[USART6Serial], USART_ReceiveData(USART6) );
 }

 if (USART_GetITStatus(USART6, USART_IT_TXE )) {
 if (txring[USART6Serial].count == 0) {
 USART_ITConfig(USART6, USART_IT_TXE, (FunctionalState) DISABLE);
 USART_ClearITPendingBit(USART6, USART_IT_TXE );
 } else {
 USART_SendData(USART6, buffer_deque(&txring[USART6Serial]));
 }
 }
 }
 */
