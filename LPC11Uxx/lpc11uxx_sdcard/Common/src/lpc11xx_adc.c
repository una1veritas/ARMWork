/***********************************************************************//**
 * @file	: lpc11xx_adc.h
 * @brief	: Contains all macro definitions and function prototypes
 * 				support for ADC firmware library on LPC11xx
 * @version	: 1.0
 * @date	: 28. Jan. 2010
 * @author	: Coocox
 **************************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup ADC
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc11xx_adc.h"
#include "lpc11xx_syscon.h"
#include "lpc11xx_libcfg.h"


#if _ADC

/* Public Functions ----------------------------------------------------------- */
/** @addtogroup ADC_Public_Functions
 * @{
 */

/*********************************************************************//**
 * @brief 		Initial for ADC
 * 					- Set bit PCADC
 * 					- Set clock for ADC
 * 					- Set Clock Frequency
 * 
 * @param[in]	ConvFreq Clock frequency
 * @return 		None
 ************************************************************************/
void ADC_Init(uint32_t ConvFreq)

{
	uint32_t temp, tmp;

    tmp = 0;
	
	CHECK_PARAM(PARAM_ADC_FREQUENCY(ConvFreq));

	// Turn on power
	SYSCON_PowerCon(SYSCON_ABLOCK_ADC, ENABLE);
    
    // Turn on clock
    SYSCON_AHBPeriphClockCmd(SYSCON_AHBPeriph_ADC, ENABLE);
	
	LPC_ADC->CR = 0;

	// The A/D converter is in power-down mode
	//SYSCON_DeepSleepPowerCon(SYSCON_ABLOCK_ADC, ENABLE);
	
    // Set clock frequency
	temp = SystemAHBFrequency;
	temp = (temp /ConvFreq) - 1;
	tmp |=  ADC_CR_CLKDIV(temp);

	LPC_ADC->CR = tmp;
}


/*********************************************************************//**
 * @brief 		Close ADC
 * @param[in]	None
 * @return 		None
 *
 ************************************************************************/
void ADC_DeInit(void)
{
	// Turn off clock
	SYSCON_AHBPeriphClockCmd(SYSCON_AHBPeriph_ADC, DISABLE);

    // Turn off power 
    SYSCON_PowerCon(SYSCON_ABLOCK_ADC, DISABLE);
}


/*********************************************************************//**
 * @brief 		Get Result conversion from A/D data register
 * @param[in]	channel number which want to read back the result
 * @return 		Result of conversion
 ************************************************************************/
uint32_t ADC_GetData(uint32_t channel)
{
	uint32_t adc_value;

	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(channel));

	adc_value = *(uint32_t *)((&LPC_ADC->DR0) + channel);
	return ADC_GDR_RESULT(adc_value);
}


/*********************************************************************//**
 * @brief 		Set start mode for ADC
 * @param[in]	start_mode Start mode choose one of modes in
 * 							'ADC_START_OPT' enumeration type definition
 * @return 		None
 ************************************************************************/
void ADC_StartCmd(uint8_t start_mode)
{
	CHECK_PARAM(PARAM_ADC_START_OPT(start_mode));

	LPC_ADC->CR &= ~ADC_CR_START_MASK;
	LPC_ADC->CR |=ADC_CR_START_MODE_SEL((uint32_t)start_mode);
}


/*********************************************************************//**
 * @brief 		ADC Burst mode setting
 * 
 * @param[in]	NewState
 * 				-	1: Set Burst mode
 * 				-	0: reset Burst mode
 * @return 		None
 *
 ************************************************************************/
void ADC_BurstCmd(FunctionalState NewState)
{
	LPC_ADC->CR &= ~ADC_CR_BURST;
	if (NewState){
		LPC_ADC->CR |= ADC_CR_BURST;
	}

}

/*********************************************************************//**
 * @brief 		 Selects the number of clocks used for each conversion 
 *                 in Burst mode
 * 
 * @param[in]	clks the number of clocks used for each conversion in 
 *                   Burst mode, it can be:
 * 				-	ADC_CLOCK_NUM_10
 * 				-	ADC_CLOCK_NUM_9
 * 				-	ADC_CLOCK_NUM_8
 * 				-	ADC_CLOCK_NUM_7
 * 				-	ADC_CLOCK_NUM_6
 * 				-	ADC_CLOCK_NUM_5
 * 				-	ADC_CLOCK_NUM_4
 * 				-	ADC_CLOCK_NUM_3
 * @return 		None
 *
 ************************************************************************/
void ADC_SetConvClock(ADC_CONV_NUM clks)
{
    LPC_ADC->CR &= ADC_CR_CLKS_MASK;
    LPC_ADC->CR |= ADC_CR_CLKS(clks) & ADC_CR_CLKS_MASK;  
}


/*********************************************************************//**
 * @brief 		Set Edge start configuration
 *
 * @param[in]	EdgeOption is ADC_START_ON_RISING and ADC_START_ON_FALLING
 * 					0:ADC_START_ON_RISING
 * 					1:ADC_START_ON_FALLING 
 *
 * @return 		None
 *
 ************************************************************************/
void ADC_EdgeStartConfig(uint8_t EdgeOption)
{	
	CHECK_PARAM(PARAM_ADC_START_ON_EDGE_OPT(EdgeOption));

	LPC_ADC->CR &= ~ADC_CR_EDGE;
	if (EdgeOption){
		LPC_ADC->CR |= ADC_CR_EDGE;
	}
}


/*********************************************************************//**
 * @brief 		ADC interrupt configuration 
 * @param[in]	IntType
 * @param[in]	NewState:
 * 					- SET : enable ADC interrupt
 * 					- RESET: disable ADC interrupt
 *
 * @return 		None
 *
 ************************************************************************/
void ADC_IntConfig (ADC_TYPE_INT_OPT IntType, FunctionalState NewState)
{	
	CHECK_PARAM(PARAM_ADC_TYPE_INT_OPT(IntType));

	LPC_ADC->INTEN &= ~ADC_INTEN_CH(IntType);
	if (NewState){
		LPC_ADC->INTEN |= ADC_INTEN_CH(IntType);
	}
}

/*********************************************************************//**
 * @brief 		Enable/Disable ADC channel number 
 * @param[in]	Channel channel number
 * @param[in]	NewState Enable or Disable
 *
 * @return 		None
 *
 ************************************************************************/
void ADC_ChannelCmd (uint8_t Channel, FunctionalState NewState)
{
	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(Channel));
       
	if (NewState == ENABLE) {
		LPC_ADC->CR |= ADC_CR_CH_SEL(Channel);
	} else {
		LPC_ADC->CR &= ~ADC_CR_CH_SEL(Channel);
	}
}


/********************************************************************//**
 * @brief 		Init ADC AD input pins, pins assign:
 *                AD0 - PIO0_11
 *                AD1 - PIO1_0
 *                AD2 - PIO1_1
 *                AD3 - PIO1_2
 *                AD4 - PIO1_3
 *                AD5 - PIO1_4
 *                AD6 - PIO1_10
 *                AD7 - PIO1_11
 *              NOTE:AD0,AD1,AD2,AD3,AD4 are multiplexed with JTAG pins(SW)
 *
 * @param[in]	* @param[in]	Channel channel number           
 *
 * @return		None
 *********************************************************************/
void ADC_PinsInit(uint8_t channel)
{    
//	uint32_t pin;
	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(Channel));
    CHECK_PARAM(PARAM_ADMODE(mod));  

    /* Set AD function */
    switch(channel) {
#if 0
    case 0:         /* AD Channel 0:  PIO0_11 */
        IOCON_SetPinFunc(IOCON_PIO0_11, PIO0_11_FUN_AD0);
        pin = IOCON_PIO0_11;
        break;
    case 1:         /* AD Channel 1:  PIO1_0  */
        IOCON_SetPinFunc(IOCON_PIO1_0, PIO1_0_FUN_AD1);
        pin = IOCON_PIO1_0;
        break;
    case 2:         /* AD Channel 2:  PIO1_1  */
        IOCON_SetPinFunc(IOCON_PIO1_1, PIO1_1_FUN_AD2);        
        pin = IOCON_PIO1_1;
        break;
    case 3:         /* AD Channel 3:  PIO1_2  */
        IOCON_SetPinFunc(IOCON_PIO1_2, PIO1_2_FUN_AD3);
        pin = IOCON_PIO1_2;
        break;
    case 4:         /* AD Channel 4:  PIO1_3  */
        IOCON_SetPinFunc(IOCON_PIO1_3, PIO1_3_FUN_AD4);
        pin = IOCON_PIO1_3;
        break;
#endif
    case 5:         /* AD Channel 5:  PIO1_4  */
        //IOCON_SetPinFunc(IOCON_PIO1_4, PIO1_4_FUN_AD5);
        //pin = IOCON_PIO1_4;
    	LPC_IOCON->PIO0_16 &= ~0x07;
    	LPC_IOCON->PIO0_16 |= 0x01;
    	LPC_IOCON->PIO0_16 &= ~(1<<0x07);
        break;
#if 0
    case 6:         /* AD Channel 6:  PIO1_10 */
        IOCON_SetPinFunc(IOCON_PIO1_10, PIO1_10_FUN_AD6);
        pin = IOCON_PIO1_10;
        break;
    case 7:         /* AD Channel 7:  PIO1_11  */
        IOCON_SetPinFunc(IOCON_PIO1_11, PIO1_11_FUN_AD7);
        pin = IOCON_PIO1_11;
        break;
#endif
    default: return;
    };

   	//*(uint32_t *)((uint32_t)&(LPC_IOCON->PIO2_6) + pin) &= ~ADC_PINS_MODE_MASK;
}


/*********************************************************************//**
 * @brief 		Get ADC result
 * @param[in]	channel channel number
 * @return 		Data conversion
 *
 ************************************************************************/
uint16_t ADC_ChannelGetData(uint8_t channel)
{
	uint32_t adc_value;
	
	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(channel));

	adc_value = *(uint32_t *) ((&LPC_ADC->DR0) + channel);
	return ADC_DR_RESULT(adc_value);
}


/*********************************************************************//**
 * @brief 		Get ADC Chanel status from ADC data register
 * @param[in]	channel channel number
 * @param[in]  	StatusType
 *              		 	0:Burst status
 *               		1:Done 	status
 * @return 		SET / RESET
 *
 ************************************************************************/
FlagStatus ADC_ChannelGetStatus(uint8_t channel, uint32_t StatusType)
{
	uint32_t temp;
	
	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(channel));
	CHECK_PARAM(PARAM_ADC_DATA_STATUS(StatusType));

	temp =  *(uint32_t *) ((&LPC_ADC->DR0) + channel);
	if (StatusType) {
		temp &= ADC_DR_DONE_FLAG;
	}else{
		temp &= ADC_DR_OVERRUN_FLAG;
	}
	if (temp) {
		return SET;
	} else {
		return RESET;
	}
}


/*********************************************************************//**
 * @brief 		Get ADC Data from AD Global register
 * @param[in]	channel channel number
 * @return 		Result of conversion
 *
 ************************************************************************/
uint16_t ADC_GlobalGetData(void)
{	
	CHECK_PARAM(PARAM_ADC_CHANNEL_SELECTION(channel));

	//ADCx->ADGDR &= ~ADC_GDR_CH_MASK;
	//ADCx->ADGDR |= ADC_GDR_CH(channel);
	return (uint16_t)(ADC_GDR_RESULT(LPC_ADC->GDR));
}


/*********************************************************************//**
 * @brief 		Get ADC Chanel status from AD global data register
 * @param[in]  	StatusType
 *              		 	0:Burst status
 *               		1:Done 	status
 * @return 		SET / RESET
 *
 ************************************************************************/
FlagStatus	ADC_GlobalGetStatus(uint32_t StatusType)
{
	uint32_t temp;
	
	CHECK_PARAM(PARAM_ADC_DATA_STATUS(StatusType));

	temp =  LPC_ADC->GDR;
	if (StatusType){
		temp &= ADC_DR_DONE_FLAG;
	}else{
		temp &= ADC_DR_OVERRUN_FLAG;
	}
	if (temp){
		return SET;
	}else{
		return RESET;
	}
}

/**
 * @}
 */

#endif /* _ADC */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */

