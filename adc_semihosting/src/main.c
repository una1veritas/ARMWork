/*****************************************************************************
 *   adctest.c:  main C entry file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.25  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#include "LPC17xx.h"
#include "type.h"
#include "adc.h"
#include "debug.h"

extern volatile uint32_t ADC0Value[ADC_NUM];
extern volatile uint32_t ADC0IntDone;
extern void TargetResetInit(void);

#define SEMIHOSTED_ADC_OUTPUT
#define OUTPUT_ONLY_CH0

///////////////////////////////////////////////////////////////////////////////////////
//Project only uses Semi-hosted output, so running without a debugger makes no sense.//
#if defined(SEMIHOSTED_ADC_OUTPUT) && defined(DEBUG)///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#define BAR_OUTPUT_LENGTH 60
#define ADC_COUNT_MAX 4095
#define CHANNEL_WIDTH 2
#define VOLT_WIDTH 4
#define MAX_BAR_LENGTH (BAR_OUTPUT_LENGTH-CHANNEL_WIDTH-1-VOLT_WIDTH-1-2)
#define SUPPLY_VOLTAGE 3.30

/*
 * FillString:	Replace 'len' characters of a string.
 *
 * Arguments:	s - destination string
 * 				c  - character to replace
 * 				len - byte count
 *
 */
void FillString(char *s, char c, uint32_t len)
{
	while(len)
		*s = c, s++, len--;

	*s = 0;
}

/*
 * ADCBar:		Display 'bar graph' representing ADC reading
 *
 * Arguments:	Channel - ADC channel
 * 				Counts - ADC reading
 *
 */
void ADCBar(int32_t Channel, uint32_t Counts)
{
	uint32_t ValueBarLen = ((Counts * (MAX_BAR_LENGTH+1)) - (ADC_COUNT_MAX/2 - 1)) / ADC_COUNT_MAX;
	char ValueBarBuf[BAR_OUTPUT_LENGTH+1];
	double ValueVolts = Counts*SUPPLY_VOLTAGE/ADC_COUNT_MAX;

	if(Channel >= 0)
	{
		if(ValueBarLen > MAX_BAR_LENGTH) ValueBarLen = MAX_BAR_LENGTH;
		FillString(ValueBarBuf, '#', ValueBarLen);
		debug_printf("%2d %d.%02dV |%s%*s|\n", Channel,
				(int)ValueVolts, (int)((ValueVolts-(int)ValueVolts)*100),
				ValueBarBuf, MAX_BAR_LENGTH-ValueBarLen, "");
	}
	else
	{
		FillString(ValueBarBuf, '-', MAX_BAR_LENGTH+2);
		debug_printf("%-9s%s\n", "", ValueBarBuf);
	}
}
#endif


/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  uint32_t i;
  
  /* Initialize ADC  */
  ADCInit( ADC_CLK );

  debug_puts("If you are using the Embedded Artists baseboard, you can turn\n"
		  "the potentiometer and display a graph of the ADC CH0 voltage.\n");

  while(1)
  {
#if BURST_MODE				/* Interrupt driven only */
	ADCBurstRead();
	while ( !ADC0IntDone );
	ADC0IntDone = 0;
#else						/* Not burst mode */
#if ADC_INTERRUPT_FLAG		/* Interrupt driven */
	for ( i = 0; i < ADC_NUM; i++ )
	{
	  ADC0Read( i );
	  while ( !ADC0IntDone );
	  ADC0IntDone = 0;
	}
#else  						/* Polling */
	for ( i = 0; i < ADC_NUM; i++ )
	{
	  ADC0Value[i] = ADC0Read( i );
	}
#endif	/* Endif interrupt driven */
#endif	/* Endif BURST mode */
#ifdef SEMIHOSTED_ADC_OUTPUT
#ifndef OUTPUT_ONLY_CH0
	for ( i = 0; i < ADC_NUM; i++ )
	{
		ADCBar(i, ADC0Value[i]);
	}
#else
	ADCBar(0, ADC0Value[0]);
#endif
#endif
	}
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

