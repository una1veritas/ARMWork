#include "lpc11xx_adc.h"
#include "lcd.h"
#include "stdio.h"


void ADCExp()
{
	uint32_t adc_value;
	char buf[6];
	/* Configuration for ADC :
	 * 	Frequency at 1Mhz
	 *  ADC channel 2, no Interrupt
	 */
	ADC_Init(1000000);
	//ADC_IntConfig(ADC_ADINTEN5,ENABLE);
    ADC_PinsInit(ADC_CHANNEL_5);
	ADC_ChannelCmd(ADC_CHANNEL_5,ENABLE);
    
	while(1)
	{
		/* Start conversion */
		ADC_StartCmd(ADC_START_NOW);
		/* Wait conversion complete */
		while (!(ADC_ChannelGetStatus(ADC_CHANNEL_5,ADC_DATA_DONE)));
		adc_value = ADC_ChannelGetData(ADC_CHANNEL_5);            	        
		sprintf((char *)buf,"ADC Ch-5:%.4d",(int)adc_value);
		lcd_putstring(LINE2,(char *)buf);
		// printf("AD value : %d", adc_value);      
	}  
}
