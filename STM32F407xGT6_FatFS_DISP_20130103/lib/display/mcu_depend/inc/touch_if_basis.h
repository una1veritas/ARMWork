/********************************************************************************/
/*!
	@file			touch_if_basis.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2011.05.30
	@brief          Interface of Touch Panel Hardware Depend Layer			@n
					Based On "BlueScreen"'s Touch Driver Thunks !			@n 
					Currentry ADS7843 ONLY!!!

    @section HISTORY
		2010.05.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __TOUCH_IF_BASIS_H
#define __TOUCH_IF_BASIS_H	0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* Basic Includes */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/* Device or MCU Depend Includes */
#include "stm32f4xx.h"
#include "display_if.h"
#include "display_if_support.h"
#include "ads7843.h"

/* Touch Variable MCU Depend */
#define CHK_CALIBED()	((RTC_ReadBackupRegister(RTC_BKP_DR6) == 0xA5A5A5A5))
extern long ccx, cm1x, cm2x;
extern long ccy, cm1y, cm2y;


#ifndef BSRR
#define BSRR BSRRL
#define BRR  BSRRH
#endif

/* Touch PenIOPort Definitions */
#if defined(STM32F407VGT6)
/* PA5:DCLK   PB7:CS  PA7:DIN  PA6:DOUT  PB6:PENIRQ */
/* Device Dependent */
#define TOUCH_DCLK				GPIO_Pin_5
#define TOUCH_CS				GPIO_Pin_7
#define TOUCH_DIN				GPIO_Pin_7
#define TOUCH_DOUT				GPIO_Pin_6
#define TOUCH_PENIRQ			GPIO_Pin_6

#define TOUCH_PORT_DCLK			GPIOA
#define TOUCH_PORT_CS			GPIOB
#define TOUCH_PORT_DIN			GPIOA
#define TOUCH_PORT_DOUT			GPIOA
#define TOUCH_PORT_PENIRQ		GPIOB

#define TOUCH_CLK_DCLK			RCC_AHB1Periph_GPIOA
#define TOUCH_CLK_CS			RCC_AHB1Periph_GPIOB
#define TOUCH_CLK_DIN			RCC_AHB1Periph_GPIOA
#define TOUCH_CLK_DOUT			RCC_AHB1Periph_GPIOA
#define TOUCH_CLK_PENIRQ		RCC_AHB1Periph_GPIOB
#else /* #elif defined(STM32F407ZGT6) */
/* Device Dependent */
/* PB13:DCLK  PB12:CS  PB15:DIN  PB14:DOUT  PG7:PENIRQ */
#define TOUCH_DCLK				GPIO_Pin_13
#define TOUCH_CS				GPIO_Pin_12
#define TOUCH_DOUT				GPIO_Pin_14
#define TOUCH_DIN				GPIO_Pin_15
#define TOUCH_PENIRQ			GPIO_Pin_7

#define TOUCH_PORT_DCLK			GPIOB
#define TOUCH_PORT_CS			GPIOB
#define TOUCH_PORT_DIN			GPIOB
#define TOUCH_PORT_DOUT			GPIOB
#define TOUCH_PORT_PENIRQ		GPIOG

#define TOUCH_CLK_DCLK			RCC_AHB1Periph_GPIOB
#define TOUCH_CLK_CS			RCC_AHB1Periph_GPIOB
#define TOUCH_CLK_DIN			RCC_AHB1Periph_GPIOB
#define TOUCH_CLK_DOUT			RCC_AHB1Periph_GPIOB
#define TOUCH_CLK_PENIRQ		RCC_AHB1Periph_GPIOB
#endif

#define TOUCH_DCLK_SET()		(TOUCH_PORT_DCLK->BSRR   = TOUCH_DCLK)
#define TOUCH_DCLK_CLR()		(TOUCH_PORT_DCLK->BRR	 = TOUCH_DCLK)
#define TOUCH_CS_SET()			(TOUCH_PORT_CS->BSRR     = TOUCH_CS)
#define TOUCH_CS_CLR()        	(TOUCH_PORT_CS->BRR      = TOUCH_CS)
#define TOUCH_DIN_SET()			(TOUCH_PORT_DIN->BSRR    = TOUCH_DIN)
#define TOUCH_DIN_CLR()        	(TOUCH_PORT_DIN->BRR     = TOUCH_DIN)
#define TOUCH_DOUT_SET()		(TOUCH_PORT_DOUT->BSRR   = TOUCH_DOUT)
#define TOUCH_DOUT_CLR()     	(TOUCH_PORT_DOUT->BRR    = TOUCH_DOUT)
#define TOUCH_PENIRQ_SET()		(TOUCH_PORT_PENIRQ->BSRR = TOUCH_PENIRQ)
#define TOUCH_PENIRQ_CLR()      (TOUCH_PORT_PENIRQ->BRR  = TOUCH_PENIRQ)

#define ASSART_CS_TOUCH()		TOUCH_CS_CLR()
#define NEGATE_CS_TOUCH()		TOUCH_CS_SET()

#define DIN_HI()				TOUCH_DIN_SET()	
#define DIN_LO()				TOUCH_DIN_CLR()

/* 200nSec Stupid Wait */
#define WAIT()    \
    for(volatile uint32_t Count=0;Count<=38;Count++){ 	\
			__asm__ __volatile__("nop"); 				\
						}

#define TOUCH_CLK()   			WAIT();				\
								TOUCH_DCLK_SET();	\
								WAIT();				\
								TOUCH_DCLK_CLR();	\
								WAIT();


#define DOUT_CHK() 			((TOUCH_PORT_DOUT->IDR & TOUCH_DOUT))

#define PENIRQ_CHK()		((TOUCH_PORT_PENIRQ->IDR & TOUCH_PENIRQ))
#define PEN_PRESSED			(0)
#define PEN_RELEASED		(TOUCH_PENIRQ)


/* Macro Functions */
inline static uint8_t TouchSpi_Rcvr(){
	/* Sampled Falling Edge of DCLK */
	uint8_t r;

	r = 0;   TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D7 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D6 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D5 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D4 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D3 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D2 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D1 */
	r <<= 1; TOUCH_CLK();
	if (DOUT_CHK()) r++;	/* Sample D0 */

	return r;
}

inline static void TouchSpi_Xmit(uint8_t x){

	(0x80 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x40 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x20 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x10 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x08 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x04 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x02 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
	(0x01 & x) ? DIN_HI() : DIN_LO();
	TOUCH_CLK();
}


/* Externs */
extern void TC_IoInit_If(void);
extern void TC_Restore_Calivalue(void);
extern void TC_Store_Calivalue(void);


#ifdef __cplusplus
}
#endif

#endif /* __TOUCH_IF_H */
