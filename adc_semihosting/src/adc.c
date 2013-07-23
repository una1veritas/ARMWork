/*****************************************************************************
 *   adc.c:  ADC module file for NXP LPC17xx Family Microprocessors
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

volatile uint32_t ADC0Value[ADC_NUM];
volatile uint32_t ADC0IntDone = 0;

#if BURST_MODE
volatile uint32_t channel_flag; 
#endif

#if ADC_INTERRUPT_FLAG
/******************************************************************************
** Function name:		ADC_IRQHandler
**
** Descriptions:		ADC interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ADC_IRQHandler (void)  
{
  uint32_t regVal;
  
  regVal = LPC_ADC->ADSTAT;		/* Read ADC will clear the interrupt */
  if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
  {
	regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
	switch ( regVal )
	{
	  case 0x01:
		regVal = LPC_ADC->ADDR0;
	  break;
	  case 0x02:
		regVal = LPC_ADC->ADDR1;
	  break;
	  case 0x04:
		regVal = LPC_ADC->ADDR2;
	  break;
	  case 0x08:
		regVal = LPC_ADC->ADDR3;
	  break;
	  case 0x10:
		regVal = LPC_ADC->ADDR4;
	  break;
	  case 0x20:
		regVal = LPC_ADC->ADDR5;
	  break;
	  case 0x40:
		regVal = LPC_ADC->ADDR6;
	  break;
	  case 0x80:
		regVal = LPC_ADC->ADDR7;
	  break;
	  default:
	  break;
	}
	LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC0IntDone = 1;
	return;
  }
    
  if ( regVal & ADC_ADINT )
  {
	switch ( regVal & 0xFF )	/* check DONE bit */
	{
	  case 0x01:
		ADC0Value[0] = ( LPC_ADC->ADDR0 >> 4 ) & 0xFFF;
	  break;
	  case 0x02:
		ADC0Value[1] = ( LPC_ADC->ADDR1 >> 4 ) & 0xFFF;
	  break;
	  case 0x04:
		ADC0Value[2] = ( LPC_ADC->ADDR2 >> 4 ) & 0xFFF;
	  break;
	  case 0x08:
		ADC0Value[3] = ( LPC_ADC->ADDR3 >> 4 ) & 0xFFF;
	  break;
	  case 0x10:
		ADC0Value[4] = ( LPC_ADC->ADDR4 >> 4 ) & 0xFFF;
	  break;
	  case 0x20:
		ADC0Value[5] = ( LPC_ADC->ADDR5 >> 4 ) & 0xFFF;
	  break;
	  case 0x40:
		ADC0Value[6] = ( LPC_ADC->ADDR6 >> 4 ) & 0xFFF;
	  break;
	  case 0x80:
		ADC0Value[7] = ( LPC_ADC->ADDR7 >> 4 ) & 0xFFF;
	  break;		
	  default:
	  break;
	}
#if BURST_MODE
	channel_flag |= (regVal & 0xFF);
	if ( (channel_flag & 0xFF) == 0xFF )
	{
	  /* All the bits in have been set, it indicates all the ADC 
	  channels have been converted. */
	  LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */ 
	  ADC0IntDone = 1;
	}
#else
	LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */ 
	ADC0IntDone = 1;
#endif
  }
  return;
}
#endif

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		true or false
** 
*****************************************************************************/
uint32_t ADCInit( uint32_t ADC_Clk )
{
  uint32_t pclkdiv, pclk;

  /* Enable CLOCK into ADC controller */
  LPC_SC->PCONP |= (1 << 12);

  /* all the related pins are set to ADC inputs, AD0.0~7 */
  LPC_PINCON->PINSEL0 |= 0x0F000000;	/* P0.12~13, A0.6~7, function 11 */	
  LPC_PINCON->PINSEL1 &= ~0x003FC000;	/* P0.23~26, A0.0~3, function 01 */
  LPC_PINCON->PINSEL1 |= 0x00154000;
  LPC_PINCON->PINSEL3 |= 0xF0000000;	/* P1.30~31, A0.4~5, function 11 */

  /* By default, the PCLKSELx value is zero, thus, the PCLK for
  all the peripherals is 1/4 of the SystemFrequency. */
  /* Bit 6~7 is for UART0 */
  pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
  switch ( pclkdiv )
  {
	case 0x00:
	default:
	  pclk = SystemCoreClock/4;
	break;
	case 0x01:
	  pclk = SystemCoreClock;
	break; 
	case 0x02:
	  pclk = SystemCoreClock/2;
	break; 
	case 0x03:
	  pclk = SystemCoreClock/8;
	break;
  }

  LPC_ADC->ADCR = ( 0x01 << 0 ) | 	/* SEL=1,select channel 0 on ADC0 */
		( ( pclk  / ADC_Clk - 1 ) << 8 ) |  /* CLKDIV = Fpclk / ADC_Clk - 1 */ 
		( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
		( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
		( 1 << 21 ) |  		/* PDN = 1, normal operation */
		( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
		( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 

  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
  LPC_ADC->ADINTEN = 0x1FF;		/* Enable all interrupts */
  NVIC_EnableIRQ(ADC_IRQn);
#endif
  return (TRUE);
}

/*****************************************************************************
** Function name:		ADC0Read
**
** Descriptions:		Read ADC0 channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
uint32_t ADC0Read( uint8_t channelNum )
{
#if !ADC_INTERRUPT_FLAG
  uint32_t regVal, ADC_Data;
#endif

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  LPC_ADC->ADCR &= 0xFFFFFF00;
  LPC_ADC->ADCR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(LPC_ADC_BASE 
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	if ( regVal & ADC_DONE )
	{
	  break;
	}
  }	
        
  LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 4 ) & 0xFFF;
  return ( ADC_Data );	/* return A/D conversion value */
#else
  return ( channelNum );	/* if it's interrupt driven, the ADC reading is 
							done inside the handler. so, return channel number */
#endif
}

/*****************************************************************************
** Function name:		ADC0BurstRead
**
** Descriptions:		Use burst mode to convert multiple channels once.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void ADCBurstRead( void )
{
  if ( LPC_ADC->ADCR & (0x7<<24) )
  {
	LPC_ADC->ADCR &= ~(0x7<<24);
  }
  /* Test channel 5,6,7 using burst mode because they are not shared
  with the JTAG pins. */
  LPC_ADC->ADCR &= ~0xFF;
  /* Read all channels, 0 through 7. */
  LPC_ADC->ADCR |= (0xFF);
  LPC_ADC->ADCR |= (0x1<<16);	/* Set burst mode and start A/D convert */
  return;						/* the ADC reading is done inside the 
								handler, return 0. */
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
