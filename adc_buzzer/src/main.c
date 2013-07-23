/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	buzzer_adc : PWM audio with frequency controlled via ADC
 * File:	main.c
 * Description:
 * 			Read ADC and adjust PWM frequency accordingly. Uses
 * 			SystemTick driven timer to modulate PWM duty cycle.
 * 			When used with LPCXpresso Baseboard PWM can drive
 * 			the speaker when connected with a jumper wire.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "freq.h"
#include "adc.h"
#include "debug.h"
#include "small_systick.h"

#include <stdint.h>

volatile uint32_t SysTickCount;		/* counts 10ms timeTicks */
uint32_t DelayStart_SysTickCount;

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    SysTickCount++; 				/* increment counter necessary in Delay() */
}

int main(void)
{
	uint32_t trimToFreq;
	uint8_t dutyCycle;
	uint8_t i=0;
	uint32_t DelayStart_SysTickCount;
	volatile uint16_t adcCount = 1000;

	const unsigned int k = ((MAX_FREQ-MIN_FREQ)/4096)<<16; //Q16 fixed point alleviates need for floating point math
	//Sinusoidal between 11% and 90% duty cycle
    const unsigned char dutyMod[64] = {
    		50	,
    		54	,
    		58	,
    		61	,
    		65	,
    		68	,
    		72	,
    		75	,
    		78	,
    		80	,
    		82	,
    		84	,
    		86	,
    		87	,
    		88	,
    		89	,
    		89	,
    		89	,
    		88	,
    		87	,
    		86	,
    		84	,
    		82	,
    		80	,
    		78	,
    		75	,
    		72	,
    		68	,
    		65	,
    		61	,
    		58	,
    		54	,
    		50	,
    		46	,
    		42	,
    		39	,
    		35	,
    		32	,
    		28	,
    		25	,
    		22	,
    		20	,
    		18	,
    		16	,
    		14	,
    		13	,
    		12	,
    		11	,
    		11	,
    		11	,
    		12	,
    		13	,
    		14	,
    		16	,
    		18	,
    		20	,
    		22	,
    		25	,
    		28	,
    		32	,
    		35	,
    		39	,
    		42	,
    		46	};

    //SystemInit(); is called by startup code
    ADCInit(ADC_CLK);
    initFreq();
    InitSysTick(100);

    while(1)
    {
    	//Low frequency modulation of PWM duty cycle and ADC updates @ 100Hz
    	//This results in a PWM cycle time of 0.64 seconds.
    	//Note - print statements with semi-hosting will decrease performance
    	DelayStart_SysTickCount = SysTickCount;
    	while((SysTickCount - DelayStart_SysTickCount) < 1 )
    	{
    		__WFI();
    	}

    	adcCount = ADC0Read(0);	//Convert AD to Frequency
    	trimToFreq = (adcCount*k)>>16; 			//scale ADC
    	trimToFreq += MIN_FREQ; 				//Offset by min
    	trimToFreq -= 100/2;    				//ADC background ~100 counts, so subtract 1/2 to minimally impact dynamic range

    	dutyCycle = dutyMod[ (i++ & 0x3F) ];	//Get next duty cycle from look up table
    	updateFreq(trimToFreq,dutyCycle);

#ifdef DEBUG
    	debug_printf("ADC:%d FREQ:%d DUTY:%d\n",adcCount,trimToFreq,dutyCycle);
    	debug_printf_flush();
#endif
    }
    return 0;
}
