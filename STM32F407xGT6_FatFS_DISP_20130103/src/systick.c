/********************************************************************************/
/*!
	@file			systick.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        3.00
    @date           2012.04.25
	@brief          Based on ST Microelectronics's Sample Thanks!
					For STM32F2/F4 Only!

    @section HISTORY
		2011.06.12	V1.00	Start Here.
		2012.03.20  V2.00	Fixed Timer Settings in _delay_us();
		2012.04.25  V3.00	ReFixed Timer Settings for _delay_us();

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
/* check header file version for fool proof */
#if __SYSTICK_H!= 0x0300
#error "header file version is not correspond!"
#endif

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
__IO uint32_t TimingDelay;

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    @brief	SysTickTimer Initialize(every 1mSec)
	@param	interval: Set Systick timer interval ratio.
    @retval	none
*/
/**************************************************************************/
void SysTickInit(__IO uint32_t interval)
{
	uint32_t cal_usec_divide = SystemCoreClock;		/* :default value (HCLK/1)*1(APB1Div is 1)  */
	
	/* Making MilliSecond-Order Timer */
	/* Select Clock Source  */
	SystemCoreClockUpdate();
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock/ interval))
	{ 
		/* Capture error */ 
		while (1);
	}

	/* Making MicroSecond-Order Timer */
	/* Enable timer clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	/* calculate TIM5 Prescaler clock */
	if(RCC->CFGR & RCC_CFGR_PPRE1){
		if((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV2){
			cal_usec_divide = SystemCoreClock;   	/* (HCLK/2)*2(APB1Div is not 1) */
		}
		else if((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV4){
			cal_usec_divide = SystemCoreClock/2; 	/* (HCLK/4)*2(APB1Div is not 1) */
		}
		else if((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV8){
			cal_usec_divide = SystemCoreClock/4; 	/* (HCLK/8)*2(APB1Div is not 1) */
		}
		else if((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV16){
			cal_usec_divide = SystemCoreClock/8; 	/* (HCLK/16)*2(APB1Div is not 1) */
		}
	}

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Prescaler 	= ((cal_usec_divide)/USEC_INTERVAL) - 1;
	TIM_TimeBaseStructure.TIM_Period 		= UINT32_MAX; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/* Enable counter */
	TIM_Cmd(TIM5, ENABLE);

}


/**************************************************************************/
/*! 
    @brief	Delay Millisecond Order (upto 16777214mSec!)
	@param	mSec: 24-bit value due to systick downcount timer
    @retval	none
*/
/**************************************************************************/
void _delay_ms(__IO uint32_t mSec)
{ 
	TimingDelay = mSec;

	while(TimingDelay != 0);
}

/**************************************************************************/
/*! 
    @brief	Delay Microsecond Order (upto 4294967296uSec!)
	@param	uSec: 32-bit value due to TIM5 upcount timer
    @retval	none
*/
/**************************************************************************/
void _delay_us(__IO uint32_t uSec)
{

#if 1
	if(uSec){
		TIM5->CNT=0;
		/* use 32 bit count wrap around */
		while((uint32_t)(TIM5->CNT) <= uSec);
	}

#else
	/* This is the stupid method */
	while(uSec--){ 
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					}
#endif
}

/**************************************************************************/
/*! 
    @brief	Delay Stupid.
	@param	nCount
    @retval	none
*/
/**************************************************************************/
void Delay(__IO uint32_t nCount)
{
	while(nCount--){ 
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					}
}

/* End Of File ---------------------------------------------------------------*/
