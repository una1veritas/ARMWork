/*
 * adc3dma.h
 *
 *  Created on: 2012/12/16
 *      Author: Sin
 */

#ifndef ADC3DMA_H_
#define ADC3DMA_H_

#include <stm32f4xx_adc.h>

extern __IO uint16_t ADC3ConvertedValue;
extern __IO uint32_t ADC3ConvertedVoltage;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void ADC3_CH12_DMA_Config(void);


#endif /* ADC3DMA_H_ */
