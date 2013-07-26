/***************************************************************************
 * @file	: lpc11xx_syscon.h
 * @brief	: Contains all macro definitions and function prototypes
 * 				support for system configuration firmware library on LPC11xx
 * @version	: 1.0
 * @date	: 20. Jan. 2010
 * @author	: Coocox
 **************************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup SYSCON
 * @ingroup LPC1100CMSIS_FwLib_Drivers
 * @{
 */

#ifndef __LPC11XX_SYSCON_H_
#define __LPC11XX_SYSCON_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC11Uxx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* Public Types --------------------------------------------------------------- */
/** @defgroup SYSCON_Public_Types
 * @{
 */

typedef enum
{
    START_LOGIC_EDGE_Falling = 0,  /** Falling Edge select for start logic input */
    START_LOGIC_EDGE_Rising,       /** Rising Edge select for start logic input */    
}START_LOGIC_INPUT_EDGE;
 /**
 * @}
 */


 /* Public Macros -------------------------------------------------------------- */
/** @defgroup SYSCON_Public_Macros
 * @{
 */

#define  SYSCON_GetAHBClockDiv()          (LPC_SYSCON->SYSAHBCLKDIV & 0xFF)
#define  SYSCON_GetSPI0ClockDiv()         (LPC_SYSCON->SSP0CLKDIV & 0xFF)
#define  SYSCON_GetSPI1ClockDiv()         (LPC_SYSCON->SSP1CLKDIV & 0xFF)
#define  SYSCON_GetUARTClockDiv()         (LPC_SYSCON->UARTCLKDIV & 0xFF)
#define  SYSCON_GetWDTClockDiv()          (LPC_SYSCON->WDTCLKDIV & 0xFF) 
#define  SYSCON_GetCLKOUTClockDiv()       (LPC_SYSCON->CLKOUTCLKDIV & 0xFF)    


#define IRC_OSC_FREQ      12000000UL


/********************************************************************
* Power Control for Peripherals Definitions
**********************************************************************/
/** AHB to APB bridge clock control bit */
#define	 SYSCON_AHBPeriph_SYS          ((uint32_t)(1<<0))
/** ROM clock control bit */
#define	 SYSCON_AHBPeriph_ROM          ((uint32_t)(1<<1))
/** RAM clock control bit */
#define	 SYSCON_AHBPeriph_RAM          ((uint32_t)(1<<2))
/** Flash register interface clock control bit */
#define	 SYSCON_AHBPeriph_FLASHREG     ((uint32_t)(1<<3))
/** Flash array access clock control bit */
#define	 SYSCON_AHBPeriph_FLASHARRAY   ((uint32_t)(1<<4))
/** I2C clock control bit */
#define	 SYSCON_AHBPeriph_I2C          ((uint32_t)(1<<5))
/** GPIO clock control bit */
#define	 SYSCON_AHBPeriph_GPIO         ((uint32_t)(1<<6))
/** 16-bit counter/timer 0 clock control bit */
#define	 SYSCON_AHBPeriph_CT16B0       ((uint32_t)(1<<7))
/** 16-bit counter/timer 1 clock control bit */
#define	 SYSCON_AHBPeriph_CT16B1       ((uint32_t)(1<<8))
/** 32-bit counter/timer 0 clock control bit */
#define	 SYSCON_AHBPeriph_CT32B0       ((uint32_t)(1<<9))
/** 32-bit counter/timer 1 clock control bit */
#define	 SYSCON_AHBPeriph_CT32B1       ((uint32_t)(1<<10))
/** SSP0 clock control bit */
#define	 SYSCON_AHBPeriph_SSP0         ((uint32_t)(1<<11))
/** UART clock control bit */
#define	 SYSCON_AHBPeriph_UART         ((uint32_t)(1<<12))
/** ADC clock control bit */
#define	 SYSCON_AHBPeriph_ADC          ((uint32_t)(1<<13))
/** WDT clock control bit */
#define	 SYSCON_AHBPeriph_WDT          ((uint32_t)(1<<15))
/** IO configuration block clock control bit */
#define	 SYSCON_AHBPeriph_IOCON        ((uint32_t)(1<<16))
/** SSP1 configuration block clock control bit */
#define	 SYSCON_AHBPeriph_SSP1         ((uint32_t)(1<<18))

/** Power Control for Peripherals bit mask */
#define  SYSCON_AHBPeriph_BITMASK      0x5BFFF

/********************************************************************
* System Memory Remap Mode Definitions
**********************************************************************/
#define  MAP_BootLoader_MODE       0
#define  MAP_RAM_MODE              1
#define  MAP_FLASH_MODE            2

/********************************************************************
* Peripherals Reset Definitions
**********************************************************************/
#define  SYSCON_RSTPeriph_SSP0    ((uint32_t)(1<<0))  
#define  SYSCON_RSTPeriph_I2C     ((uint32_t)(1<<1))  
#define  SYSCON_RSTPeriph_SSP1    ((uint32_t)(1<<2))  

/********************************************************************
* PLL Clock Source Definitions
**********************************************************************/
#define  SYSCON_PLL_SOURCE_IRC     0
#define  SYSCON_PLL_SOURCE_SYSOSC  1
#define  SYSCON_PLL_SOURCE_RES     2

#define  SYSCON_PLL_P_1            1
#define  SYSCON_PLL_P_2            2
#define  SYSCON_PLL_P_4            4
#define  SYSCON_PLL_P_8            8

/********************************************************************
* Main Clock Source Definitions
**********************************************************************/
/** IRC oscillator */
#define  SYSCON_MCK_IRC           0 
/** Input clock to system PLL */
#define  SYSCON_MCK_PLLINPUT      1
/** WDT oscillator */
#define  SYSCON_MCK_WDTOSC        2
/** SYSCON_MCLK_PLL */
#define  SYSCON_MCK_PLL           3    

/********************************************************************
* Main Oscillator Frequency Range Definitions
**********************************************************************/
/** 1 - 20 MHz frequency range */
#define SYSCON_MOSC_FREQRANG_1_20      ((uint32_t)0 << 1)
/** 15 - 25 MHz frequency range */
#define SYSCON_MOSC_FREQRANG_15_25     ((uint32_t)1 << 1)

/********************************************************************
* Watchdog Oscillator Analog Output Frequency Definitions
**********************************************************************/
#define  SYSCON_WDT_FREQ_0_5  1
#define  SYSCON_WDT_FREQ_0_8  2
#define  SYSCON_WDT_FREQ_1_1  3
#define  SYSCON_WDT_FREQ_1_4  4
#define  SYSCON_WDT_FREQ_1_6  5
#define  SYSCON_WDT_FREQ_1_8  6
#define  SYSCON_WDT_FREQ_2_0  7
#define  SYSCON_WDT_FREQ_2_2  8
#define  SYSCON_WDT_FREQ_2_4  9
#define  SYSCON_WDT_FREQ_2_6  10
#define  SYSCON_WDT_FREQ_2_7  11
#define  SYSCON_WDT_FREQ_2_9  12
#define  SYSCON_WDT_FREQ_3_1  13
#define  SYSCON_WDT_FREQ_3_2  14
#define  SYSCON_WDT_FREQ_3_4  15

/********************************************************************
* CLKOUT Clock Source Definitions
**********************************************************************/
#define  SYSCON_CLKOUT_CLK_IRC      0
#define  SYSCON_CLKOUT_CLK_SYSOSC   1
#define  SYSCON_CLKOUT_CLK_WDTOSC   2
#define  SYSCON_CLKOUT_CLK_MCK      3


/********************************************************************
* Analog Block Definitions
**********************************************************************/
/** IRC oscillator output wake-up configuration */
#define  SYSCON_ABLOCK_IRCOUT       ((uint32_t)1 << 0)
/** IRC oscillator power-down wake-up configuration */
#define  SYSCON_ABLOCK_IRC          ((uint32_t)1 << 1)
/** Flash wake-up configuration */
#define  SYSCON_ABLOCK_FLASH        ((uint32_t)1 << 2)
/** BOD wake-up configuration */
#define  SYSCON_ABLOCK_BOD          ((uint32_t)1 << 3)
/** ADC wake-up configuration */
#define  SYSCON_ABLOCK_ADC          ((uint32_t)1 << 4)
/** System oscillator wake-up configuration */
#define  SYSCON_ABLOCK_SYSOSC       ((uint32_t)1 << 5)
/** Watchdog oscillator wake-up configuration */
#define  SYSCON_ABLOCK_WDTOSC       ((uint32_t)1 << 6)
/** System PLL wake-up configuration */
#define  SYSCON_ABLOCK_SYSPLL       ((uint32_t)1 << 7)

/********************************************************************
* WDT Clock Source Definitions
**********************************************************************/
/** IRC oscillator */
#define SYSCON_WDT_CLK_IRC          0
/** Main clock */                   
#define SYSCON_WDT_CLK_MCK          1
/** Watchdog oscillator */
#define SYSCON_WDT_CLK_WDTOSC       2


#define  SYSCON_ABLOCK_BITMASK      0xFF
#define SYSCONF_PLL_LOCK    ((uint32_t)(1<<0))


/********************************************************************
 * Start Logic Input Definitions
 ********************************************************************/
#define START_LOGIC_PIO0_0      (1UL<<0)   /** Start logic input PIO0_0  */
#define START_LOGIC_PIO0_1      (1UL<<1)   /** Start logic input PIO0_1  */
#define START_LOGIC_PIO0_2      (1UL<<2)   /** Start logic input PIO0_2  */
#define START_LOGIC_PIO0_3      (1UL<<3)   /** Start logic input PIO0_3  */
#define START_LOGIC_PIO0_4      (1UL<<4)   /** Start logic input PIO0_4  */
#define START_LOGIC_PIO0_5      (1UL<<5)   /** Start logic input PIO0_5  */
#define START_LOGIC_PIO0_6      (1UL<<6)   /** Start logic input PIO0_6  */
#define START_LOGIC_PIO0_7      (1UL<<7)   /** Start logic input PIO0_7  */
#define START_LOGIC_PIO0_8      (1UL<<8)   /** Start logic input PIO0_8  */
#define START_LOGIC_PIO0_9      (1UL<<9)   /** Start logic input PIO0_9  */
#define START_LOGIC_PIO0_10     (1UL<<10)  /** Start logic input PIO0_10 */ 
#define START_LOGIC_PIO0_11     (1UL<<11)  /** Start logic input PIO0_11 */ 
#define START_LOGIC_PIO1_0      (1UL<<12)  /** Start logic input PIO1_0  */

#define PARAM_START_LOGIC_MASK   0x1FFF

/** Macro to determine if the parament is valid */
#define PARAM_MapMode(n)    (n<=3)
#define PARAM_ClkType(n)    (n<=3)
#define PARAM_MSEL(n)       ((n>=1) && (n<=32))
#define PARAM_PSEL(n)       ((n==1) || (n==2) || (n==4) || (n==8))
#define PARAM_DIVVAL(n)     (n<=0xFF)
#define PARAM_WDTDIV(n)     (n<=0x1F)
#define PARAM_FREQSEL(n)    (n<=0xF)
#define PARAM_MCKCLKSEL(n)  (n<=3)
#define PARAM_MCKSEL(n)     (n<=3)
#define PARAM_WDTCLKSEL(n)  ((n==SYSCON_WDT_CLK_IRC) || (n==SYSCON_WDT_CLK_MCK) || SYSCON_WDT_CLK_WDTOSC)

/**
 * @}
 */


/* Public Functions ----------------------------------------------------------- */
/** @defgroup GPIO_Public_Functions
 * @{
 */

void SYSCON_MemRemap (uint32_t MapMode);
void SYSCON_AHBPeriphClockCmd(uint32_t Periph, FunctionalState NewState);
void SYSCON_PeriphResetCmd (uint32_t Periph, FunctionalState NewState);

void SYSCON_SetAHBClockDiv (uint32_t DivVal);
void SYSCON_SetSPI0ClockDiv (uint32_t DivVal);
void SYSCON_SetSPI1ClockDiv (uint32_t DivVal);
void SYSCON_SetUARTClockDiv (uint32_t DivVal);
void SYSCON_SetCLKOUTClockDiv (uint32_t DivVal);

FlagStatus SYSCON_GetLockStatus (void);
void SYSCON_PLLClkSel (uint32_t ClkType);
void SYSCON_PLLConfig (uint32_t m, uint32_t p);
void SYSCON_PLLUpdate (void);

void SYSCON_MCKSel (uint32_t SYSCON_MCK_source);
void SYSCON_MCKUpdate (void);

void SysOscConfig (FunctionalState bypass, uint32_t SYSCON_MOSC_FREQRANG_x);

void SYSCON_WDTOscConfig (uint32_t div, uint32_t FreqSel);
void SYSCON_WDTClkSel (uint32_t ClkSel);
void SYSCON_SetWDTClockDiv (uint32_t DivVal);
void SYSCON_WDTClkUpdate (void);

void SYSCON_CLKOUTClkSel (uint32_t ClkSel);
void SYSCON_CLKOUTClkDiv (uint32_t DivVal);
void SYSCON_CLKOUTClkUpdate (void);

FlagStatus SYSCON_GetPIORstState(uint32_t portNum, uint32_t pinNum);

void SYSCON_PowerCon(uint32_t block, FunctionalState NewState);
FunctionalState SYSCON_GetPowerSta (uint32_t block);
void SYSCON_DeepSleepPowerCon (uint32_t block, FunctionalState NewState);
void SYSCON_WakeupPowerCon (uint32_t block, FunctionalState NewState);

void SYSCON_StartLogicEdge(uint32_t pins, START_LOGIC_INPUT_EDGE edge);
void SYSCON_StartLogicCmd(uint32_t pins, FunctionalState cmd);
void SYSCON_StartLogicReset(uint32_t pins);
uint32_t SYSCON_StartLogicStatus(void);

uint32_t SYSCON_GetWDTClock(void);
uint32_t SYSCON_GetSSP0Clock(uint8_t sspNum);
uint32_t SYSCON_GetUARTClock(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC11XX_SYSCONFIG_H_ */
