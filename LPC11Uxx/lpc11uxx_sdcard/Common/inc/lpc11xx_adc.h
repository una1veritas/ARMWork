/***********************************************************************//**
 * @file	: lpc11xx_adc.h
 * @brief	: Contains all macro definitions and function prototypes
 * 				support for ADC firmware library on LPC11xx
 * @version	: 1.0
 * @date	: 28. Jan. 2010
 * @author	: Coocox
 **************************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup ADC
 * @ingroup LPC1100CMSIS_FwLib_Drivers
 * @{
 */

#ifndef __LPC11XX_ADC_H_
#define __LPC11XX_ADC_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC11Uxx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* Private Macros ------------------------------------------------------------- */
/** @defgroup ADC_Private_Macros
 * @{
 */


/** @defgroup ADC_REGISTER_BIT_DEFINITIONS
 * @{
 */

/*********************************************************************//**
 * Macro defines for ADC  control register
 **********************************************************************/
/**  Selects which of the AD0.0:7 pins is (are) to be sampled and converted */
#define ADC_CR_CH_SEL(n)	((1UL << n))
/**  The APB clock (PCLK) is divided by (this value plus one)
* to produce the clock for the A/D */
#define ADC_CR_CLKDIV(n)	((n<<8))
/**  Repeated conversions A/D enable bit */
#define ADC_CR_BURST		((1UL<<16))
#define ADC_CR_CLKS_MASK    ((7UL)<<17)
#define ADC_CR_CLKS(SEL)    ((SEL)<<17)
/**  Start mask bits */
#define ADC_CR_START_MASK	((7UL<<24))
/**  Select Start Mode */
#define ADC_CR_START_MODE_SEL(SEL)	((SEL<<24))
/**  Start conversion now */
#define ADC_CR_START_NOW	((1UL<<24))
/**  Start conversion when the edge selected by bit 27 occurs on P2.10/EINT0 */
#define ADC_CR_START_EINT0	((2UL<<24))
/** Start conversion when the edge selected by bit 27 occurs on P1.27/CAP0.1 */
#define ADC_CR_START_CAP01	((3UL<<24))
/**  Start conversion when the edge selected by bit 27 occurs on MAT0.1 */
#define ADC_CR_START_MAT01	((4UL<<24))
/**  Start conversion when the edge selected by bit 27 occurs on MAT0.3 */
#define ADC_CR_START_MAT03	((5UL<<24))
/**  Start conversion when the edge selected by bit 27 occurs on MAT1.0 */
#define ADC_CR_START_MAT10	((6UL<<24))
/**  Start conversion when the edge selected by bit 27 occurs on MAT1.1 */
#define ADC_CR_START_MAT11	((7UL<<24))
/**  Start conversion on a falling edge on the selected CAP/MAT signal */
#define ADC_CR_EDGE			((1UL<<27))

/*********************************************************************//**
 * Macro defines for ADC Global Data register
 **********************************************************************/
/** When DONE is 1, this field contains result value of ADC conversion */
#define ADC_GDR_RESULT(n)		(((n>>6)&0x3FF))
/** These bits contain the channel from which the LS bits were converted */
#define ADC_GDR_CH(n)			(((n>>24)&0x7))
/** This bit is 1 in burst mode if the results of one or
 * more conversions was (were) lost */
#define ADC_GDR_OVERRUN_FLAG	((1UL<<30))
/** This bit is set to 1 when an A/D conversion completes */
#define ADC_GDR_DONE_FLAG		((1UL<<31))

/** This bits is used to mask for Channel */
#define ADC_GDR_CH_MASK		((7UL<<24))
/*********************************************************************//**
 * Macro defines for ADC Interrupt register
 **********************************************************************/
/** These bits allow control over which A/D channels generate
 * interrupts for conversion completion */
#define ADC_INTEN_CH(n)			((1UL<<n))
/** When 1, enables the global DONE flag in ADDR to generate an interrupt */
#define ADC_INTEN_GLOBAL		((1UL<<8))

/*********************************************************************//**
 * Macro defines for ADC Data register
 **********************************************************************/
/** When DONE is 1, this field contains result value of ADC conversion */
#define ADC_DR_RESULT(n)		(((n>>6)&0x3FF))
/** These bits mirror the OVERRRUN status flags that appear in the
 * result register for each A/D channel */
#define ADC_DR_OVERRUN_FLAG		((1UL<<30))
/** This bit is set to 1 when an A/D conversion completes. It is cleared
 * when this register is read */
#define ADC_DR_DONE_FLAG		((1UL<<31))

/*********************************************************************//**
 * Macro defines for ADC Status register
**********************************************************************/
/** These bits mirror the DONE status flags that appear in the result
 * register for each A/D channel */
#define ADC_STAT_CH_DONE_FLAG(n)		((n&0xFF))
/** These bits mirror the OVERRRUN status flags that appear in the
 * result register for each A/D channel */
#define ADC_STAT_CH_OVERRUN_FLAG(n)		(((n>>8)&0xFF))
/** This bit is the A/D interrupt flag */
#define ADC_STAT_INT_FLAG				((1UL<<16))

/*********************************************************************//**
 * Macro defines for ADC Trim register
**********************************************************************/
/** Offset trim bits for ADC operation */
#define ADC_ADCOFFS(n)		(((n&0xF)<<4))
/** Written to boot code*/
#define ADC_TRIM(n)		    (((n&0xF)<<8))

/**
 * @}
 */

/**
 * @}
 */


/* Public Types --------------------------------------------------------------- */
/** @defgroup ADC_Public_Types
 * @{
 */

/*********************************************************************//**
 * @brief ADC enumeration
 **********************************************************************/
/** @brief Select Analog/Digital mode of ADC pins */
#define ADC_PINS_MODE_MASK     ((uint32_t)0x1<<7)


/** @brief Channel Selection */
typedef enum
{
	ADC_CHANNEL_0  = 0, /*!<  Channel 0 */
	ADC_CHANNEL_1,		/*!<  Channel 1 */
	ADC_CHANNEL_2,		/*!<  Channel 2 */
	ADC_CHANNEL_3,		/*!<  Channel 3 */
	ADC_CHANNEL_4,		/*!<  Channel 4 */
	ADC_CHANNEL_5,		/*!<  Channel 5 */
	ADC_CHANNEL_6,		/*!<  Channel 6 */
	ADC_CHANNEL_7		/*!<  Channel 7 */
}ADC_CHANNEL_SELECTION;



/** @brief Type of start option */

/** @brief Type of start option */

typedef enum
{
	ADC_START_CONTINUOUS =0,	/*!< Continuous mode */
	ADC_START_NOW,				/*!< Start conversion now */
	ADC_START_ON_PIO0_2,	    /*!< Start conversion when the edge selected
								 * by bit 27 occurs on PIO0_2/SSEL/CT16B0_CAP0 */
	ADC_START_ON_PIO1_5,	    /*!< Start conversion when the edge selected
								 * by bit 27 occurs on PIO1_5/DIR/CT32B0_CAP0 */
	ADC_START_ON_CT32B0_MAT0,	/*!< Start conversion when the edge selected
								 * by bit 27 occurs on CT32B0_MAT0 */
	ADC_START_ON_CT32B0_MAT1,	/*!< Start conversion when the edge selected
								 * by bit 27 occurs on CT32B0_MAT1 */
	ADC_START_ON_CT16B0_MAT0,	/*!< Start conversion when the edge selected
								  * by bit 27 occurs on CT16B0_MAT0 */
	ADC_START_ON_CT16B0_MAT1,	/*!< Start conversion when the edge selected
								  * by bit 27 occurs on CT16B0_MAT1 */
} ADC_START_OPT;


/** @brief Type of edge when start conversion on the selected CAP/MAT signal */

typedef enum
{
	ADC_START_ON_RISING = 0,	/*!< Start conversion on a rising edge
								*on the selected CAP/MAT signal */
	ADC_START_ON_FALLING		/*!< Start conversion on a falling edge
								*on the selected CAP/MAT signal */
} ADC_START_ON_EDGE_OPT;

/** @brief* ADC type interrupt enum */
typedef enum
{
	ADC_ADINTEN0 = 0,		/*!< Interrupt channel 0 */
	ADC_ADINTEN1,			/*!< Interrupt channel 1 */
	ADC_ADINTEN2,			/*!< Interrupt channel 2 */
	ADC_ADINTEN3,			/*!< Interrupt channel 3 */
	ADC_ADINTEN4,			/*!< Interrupt channel 4 */
	ADC_ADINTEN5,			/*!< Interrupt channel 5 */
	ADC_ADINTEN6,			/*!< Interrupt channel 6 */
	ADC_ADINTEN7,			/*!< Interrupt channel 7 */
	ADC_ADGINTEN			/*!< Individual channel/global flag done generate an interrupt */
}ADC_TYPE_INT_OPT;

/** Macro to determine if it is valid interrupt type */
#define PARAM_ADC_TYPE_INT_OPT(OPT) ((OPT == ADC_ADINTEN0)||(OPT == ADC_ADINTEN1)\
										||(OPT == ADC_ADINTEN2)||(OPT == ADC_ADINTEN3)\
										||(OPT == ADC_ADINTEN4)||(OPT == ADC_ADINTEN5)\
										||(OPT == ADC_ADINTEN6)||(OPT == ADC_ADINTEN7)\
										||(OPT == ADC_ADGINTEN))


/** @brief ADC Data  status */
typedef enum
{
	ADC_DATA_BURST = 0,		/*Burst bit*/
	ADC_DATA_DONE		 /*Done bit*/
}ADC_DATA_STATUS;


/** @brief  Number of clocks used for each conversion in Burst mode */
typedef enum
{	
	ADC_CLOCK_NUM_10 = 0,   /** 11 clocks / 10 bits */
    ADC_CLOCK_NUM_9,        /** 10 clocks / 9 bits  */
    ADC_CLOCK_NUM_8,        /** 9 clocks / 8 bits   */
    ADC_CLOCK_NUM_7,        /** 8 clocks / 7 bits   */                                  
    ADC_CLOCK_NUM_6,        /** 7 clocks / 6 bits   */  
    ADC_CLOCK_NUM_5,        /** 6 clocks / 5 bits   */
    ADC_CLOCK_NUM_4,        /** 5 clocks / 4 bits   */
    ADC_CLOCK_NUM_3,        /** 4 clocks / 3 bits   */
}ADC_CONV_NUM;

#define PARAM_CONVNUM(n)   ((n==ADC_CLOCK_NUM_10) || \
                            (n==ADC_CLOCK_NUM_9)  || \
                            (n==ADC_CLOCK_NUM_8)  || \
                            (n==ADC_CLOCK_NUM_7)  || \
                            (n==ADC_CLOCK_NUM_6)  || \
                            (n==ADC_CLOCK_NUM_5)  || \
                            (n==ADC_CLOCK_NUM_4)  || \
                            (n==ADC_CLOCK_NUM_3))


#define PARAM_ADC_START_ON_EDGE_OPT(OPT)	((OPT == ADC_START_ON_RISING)||(OPT == ADC_START_ON_FALLING))

#define PARAM_ADC_DATA_STATUS(OPT) ((OPT== ADC_DATA_BURST)||(OPT== ADC_DATA_DONE))

#define PARAM_ADC_FREQUENCY(FRE) (FRE <= 4500000 )

#define PARAM_ADC_CHANNEL_SELECTION(SEL)     ((SEL == ADC_CHANNEL_0)||(ADC_CHANNEL_1)\
												||(SEL == ADC_CHANNEL_2)|(ADC_CHANNEL_3)\
												||(SEL == ADC_CHANNEL_4)||(ADC_CHANNEL_5)\
												||(SEL == ADC_CHANNEL_6)||(	ADC_CHANNEL_7))

#define PARAM_ADC_START_OPT(OPT)	((OPT == ADC_START_CONTINUOUS)||(OPT == ADC_START_NOW)\
										||(OPT == ADC_START_ON_PIO0_2)||(OPT == ADC_START_ON_PIO1_5)\
										||(OPT == ADC_START_ON_CT32B0_MAT0)||(OPT == ADC_START_ON_CT32B0_MAT1)\
										||(OPT == ADC_START_ON_CT16B0_MAT0)||(OPT == ADC_START_ON_CT16B0_MAT1))

#define PARAM_ADC_TYPE_INT_OPT(OPT) ((OPT == ADC_ADINTEN0)||(OPT == ADC_ADINTEN1)\
										||(OPT == ADC_ADINTEN2)||(OPT == ADC_ADINTEN3)\
										||(OPT == ADC_ADINTEN4)||(OPT == ADC_ADINTEN5)\
										||(OPT == ADC_ADINTEN6)||(OPT == ADC_ADINTEN7)\
										||(OPT == ADC_ADGINTEN))


/**
 * @}
 */


/* Public Macros -------------------------------------------------------------- */
/** @defgroup ADC_Public_Macros
 * @{
 */

/**
 * @}
 */



/* Public Functions ----------------------------------------------------------- */
/** @defgroup ADC_Public_Functions
 * @{
 */

void ADC_Init(uint32_t ConvFreq);
void ADC_PinsInit(uint8_t channel);
void ADC_DeInit(void);
void ADC_BurstCmd(FunctionalState NewState);
void ADC_SetConvClock(ADC_CONV_NUM clks);
void ADC_StartCmd(uint8_t start_mode);
void ADC_EdgeStartConfig(uint8_t EdgeOption);
void ADC_IntConfig (ADC_TYPE_INT_OPT IntType, FunctionalState NewState);
void ADC_ChannelCmd (uint8_t Channel, FunctionalState NewState);
uint16_t ADC_ChannelGetData(uint8_t channel);
FlagStatus ADC_ChannelGetStatus(uint8_t channel, uint32_t StatusType);
uint16_t ADC_GlobalGetData(void);
FlagStatus	ADC_GlobalGetStatus(uint32_t StatusType);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif


#endif /* __LPC11XX_ADC_H_ */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
