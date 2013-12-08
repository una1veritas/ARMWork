/**
  ******************************************************************************
  * @file    stm32f2xx_rcc.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file provides all the RCC firmware functions.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_rcc.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */ 

/** @defgroup RCC_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RCC_Private_Defines
  * @{
  */ 
/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
/* --- CR Register ---*/
/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))
/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x13
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))
/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))
/* Alias word address of PLLI2SON bit */
#define PLLI2SON_BitNumber        0x1A
#define CR_PLLI2SON_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLI2SON_BitNumber * 4))

/* --- CFGR Register ---*/
/* Alias word address of I2SSRC bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x08)
#define I2SSRC_BitNumber          0x17
#define CFGR_I2SSRC_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (I2SSRC_BitNumber * 4))

/* --- BDCR Register ---*/
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x70)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))
/* --- CSR Register ---*/
/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x74)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))
/* ---------------------- RCC registers bit mask ------------------------ */
/* CR register bit mask */
#define CR_HSEBYP_Reset           ((uint32_t)0xFFFBFFFF)
#define CR_HSEBYP_Set             ((uint32_t)0x00040000)
#define CR_HSEON_Reset            ((uint32_t)0xFFFEFFFF)
#define CR_HSEON_Set              ((uint32_t)0x00010000)
#define CR_HSITRIM_Mask           ((uint32_t)0xFFFFFF07)
/* CFGR register bit mask */
#define CFGR_MCO2_Reset_Mask      ((uint32_t)0xC77FFFFF)
#define CFGR_MCO1_Reset_Mask      ((uint32_t)0xF89FFFFF)
#define CFGR_RTCPRE_Reset_Mask	  ((uint32_t)0xFFE0FFFF)
#define CFGR_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)
#define CFGR_HPRE_Set_Mask        ((uint32_t)0x000000F0)
#define CFGR_PPRE1_Reset_Mask     ((uint32_t)0xFFFFE3FF)
#define CFGR_PPRE1_Set_Mask       ((uint32_t)0x00001C00)
#define CFGR_PPRE2_Reset_Mask     ((uint32_t)0xFFFF1FFF)
#define CFGR_PPRE2_Set_Mask       ((uint32_t)0x0000E000)
/* PLLCFGR register bit mask */
#define PLLCFGR_PPLP_Mask         ((uint32_t)0x00030000)
#define PLLCFGR_PPLN_Mask         ((uint32_t)0x00007FC0)
/* CSR register bit mask */
#define CSR_RMVF_Set              ((uint32_t)0x01000000)
/* RCC Flag Mask */
#define FLAG_Mask                 ((uint8_t)0x1F)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x01))
/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE3_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x02))
/* BDCR register base address */
#define BDCR_ADDRESS              (PERIPH_BASE + BDCR_OFFSET)


/**
  * @}
  */ 


/** @defgroup RCC_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RCC_Private_Variables
  * @{
  */ 
static const uint8_t APBAHBPrescTable[16] =
  {
    0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9
  };
/**
  * @}
  */ 


/** @defgroup RCC_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RCC_Private_Functions
  * @{
  */ 


/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @param  None
  * @retval None
  */
void RCC_DeInit(void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;
  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;
  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x00000000;
  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;
  /* Disable all interrupts */
  RCC->CIR = 0x00000000;
}


/**
  * @brief  Configures the External High Speed oscillator (HSE).
  * @note   HSE can not be stopped if it is used directly or through the main PLL
  *         as system clock.
  * @param  RCC_HSE: specifies the new state of the HSE.
  *   This parameter can be one of the following values:
  *     @arg RCC_HSE_OFF: HSE oscillator OFF
  *     @arg RCC_HSE_ON: HSE oscillator ON
  *     @arg RCC_HSE_Bypass: HSE oscillator bypassed with external clock
  * @retval None
  */
void RCC_HSEConfig(uint32_t RCC_HSE)
{
  /* Check the parameters */
  assert_param(IS_RCC_HSE(RCC_HSE));
  /* Reset HSEON and HSEBYP bits before configuring the HSE ------------------*/
  /* Reset HSEON bit */
  RCC->CR &= CR_HSEON_Reset;
  /* Reset HSEBYP bit */
  RCC->CR &= CR_HSEBYP_Reset;
  /* Configure HSE (RCC_HSE_OFF is already covered by the code section above) */
  switch (RCC_HSE)
  {
    case RCC_HSE_ON:
      /* Set HSEON bit */
      RCC->CR |= CR_HSEON_Set;
      break;
    case RCC_HSE_Bypass:
      /* Set HSEBYP and HSEON bits */
      RCC->CR |= CR_HSEBYP_Set | CR_HSEON_Set;
      break;
    default:
      break;
  }
}


/**
  * @brief  Waits for HSE start-up.
  * @param  None
  * @retval An ErrorStatus enumuration value:
  * - SUCCESS: HSE oscillator is stable and ready to use
  * - ERROR: HSE oscillator not yet ready
  */
ErrorStatus RCC_WaitForHSEStartUp(void)
{
  __IO uint32_t startupcounter = 0;
  ErrorStatus status = ERROR;
  FlagStatus hsestatus = RESET;
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    hsestatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
    startupcounter++;
  }
  while ((hsestatus == RESET) && (startupcounter != HSE_STARTUP_TIMEOUT));
  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
  {
    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }
  return (status);
}

/**
  * @brief  Adjusts the Internal High Speed oscillator (HSI) calibration value.
  * @param  HSICalibrationValue: specifies the calibration trimming value.
  *   This parameter must be a number between 0 and 0x1F.
  * @retval None
  */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_CALIBRATION_VALUE(HSICalibrationValue));
  tmpreg = RCC->CR;
  /* Clear HSITRIM[4:0] bits */
  tmpreg &= CR_HSITRIM_Mask;
  /* Set the HSITRIM[4:0] bits according to HSICalibrationValue value */
  tmpreg |= (uint32_t)HSICalibrationValue << 3;
  /* Store the new value */
  RCC->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Internal High Speed oscillator (HSI).
  * @note   HSI can not be stopped if it is used directly or through the main PLL
  *   as system clock.
  * @param  NewState: new state of the HSI. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_HSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_HSION_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the main PLL.
  * @note   The main PLL can not be disabled if it is used as system clock.
  * @param  NewState: new state of the main PLL. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PLLON_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the main PLL clock source, multiplication and division factors.
  * @note   This function must be used only when the main PLL is disabled.
  * @param  RCC_PLLSource: specifies the PLL entry clock source.
  *   This parameter can be one of the following values:
  *     @arg RCC_PLLSource_HSI: HSI oscillator clock selected as PLL clock entry
  *     @arg RCC_PLLSource_HSE: HSE oscillator clock selected as PLL clock entry
  *   This clock source is common for the main PLL and PLLI2S.  
  * @param  PLLM: specifies the division factor for PLL VCO input clock
  *   This parameter must be a number between 0 and 63.
  * @param  PLLN: specifies the multiplication factor for PLL VCO output clock
  *   This parameter must be a number between 192 and 432.
  * @param  PLLP: specifies the division factor for main system clock (SYSCLK)
  *   This parameter must be a number in the range {2, 4, 6, or 8}.
  * @param  PLLQ: specifies the division factor for OTG FS, SDIO and RNG clocks
  *   This parameter must be a number between 4 and 15.
  * @param  PLLR: specifies the division factor for I2S clock
  *   This parameter must be a number between 2 and 7.  
  *   This parameter is not applicable for Silicon RevB  
  * @retval None
  */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ, uint32_t PLLR)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLL_SOURCE(RCC_PLLSource));
  assert_param(IS_RCC_PLLM_VALUE(PLLM));
  assert_param(IS_RCC_PLLN_VALUE(PLLN));
  assert_param(IS_RCC_PLLP_VALUE(PLLP));
  assert_param(IS_RCC_PLLQ_VALUE(PLLQ));
  assert_param(IS_RCC_PLLR_VALUE(PLLR));
  RCC->PLLCFGR = PLLM | (PLLN << 6) | (((PLLP >> 1) -1) << 16) | (RCC_PLLSource) |
                 (PLLQ << 24) | (PLLR << 28);
}

/**
  * @brief  Enables or disables the PLLI2S.
  * @note   PLLI2S is available only in Silicon RevB
  * @param  NewState: new state of the PLLI2S. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLI2SCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PLLI2SON_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the PLLI2S clock multiplication and division factors.
  * @note   - PLLI2S is available only in Silicon RevB   
  *         - This function must be used only when the PLLI2S is disabled.
  *         - PLLI2S clock source is common with the main PLL (configured in 
  *           RCC_PLLConfig function )  
  * @param  PLLN: specifies the multiplication factor for PLLI2S VCO output clock
  *   This parameter must be a number between 192 and 432.
  * @param  PLLR: specifies the division factor for I2S clock
  *   This parameter must be a number between 2 and 7.  
  * @retval None
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR)
{
  /* Check the parameters */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN));
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR));

  RCC->PLLI2SCFGR = (PLLI2SN << 6) | (PLLI2SR << 28);
}

/**
  * @brief  Configures the system clock (SYSCLK).
  * @param  RCC_SYSCLKSource: specifies the clock source used as system clock.
  *   This parameter can be one of the following values:
  *     @arg RCC_SYSCLKSource_HSI: HSI selected as system clock
  *     @arg RCC_SYSCLKSource_HSE: HSE selected as system clock
  *     @arg RCC_SYSCLKSource_PLLCLK: PLL selected as system clock
  * @retval None
  */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_SYSCLK_SOURCE(RCC_SYSCLKSource));
  tmpreg = RCC->CFGR;
  /* Clear SW[1:0] bits */
  tmpreg &= CFGR_SW_Mask;
  /* Set SW[1:0] bits according to RCC_SYSCLKSource value */
  tmpreg |= RCC_SYSCLKSource;
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Returns the clock source used as system clock.
  * @param  None
  * @retval The clock source used as system clock. The returned value can
  *   be one of the following:
  *     - 0x00: HSI used as system clock
  *     - 0x04: HSE used as system clock
  *     - 0x08: PLL used as system clock
  */
uint8_t RCC_GetSYSCLKSource(void)
{
  return ((uint8_t)(RCC->CFGR & CFGR_SWS_Mask));
}

/**
  * @brief  Configures the AHB clock (HCLK).
  * @param  RCC_SYSCLK: defines the AHB clock divider. This clock is derived from 
  *   the system clock (SYSCLK).
  *   This parameter can be one of the following values:
  *     @arg RCC_SYSCLK_Div1: AHB clock = SYSCLK
  *     @arg RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
  *     @arg RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
  *     @arg RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
  *     @arg RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
  *     @arg RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
  *     @arg RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
  *     @arg RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
  *     @arg RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
  * @retval None
  */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_HCLK(RCC_SYSCLK));
  tmpreg = RCC->CFGR;
  /* Clear HPRE[3:0] bits */
  tmpreg &= CFGR_HPRE_Reset_Mask;
  /* Set HPRE[3:0] bits according to RCC_SYSCLK value */
  tmpreg |= RCC_SYSCLK;
  /* Store the new value */
  RCC->CFGR = tmpreg;
}


/**
  * @brief  Configures the Low Speed APB clock (PCLK1).
  * @param  RCC_HCLK: defines the APB1 clock divider. This clock is derived from 
  *   the AHB clock (HCLK).
  *   This parameter can be one of the following values:
  *     @arg RCC_HCLK_Div1: APB1 clock = HCLK
  *     @arg RCC_HCLK_Div2: APB1 clock = HCLK/2
  *     @arg RCC_HCLK_Div4: APB1 clock = HCLK/4
  *     @arg RCC_HCLK_Div8: APB1 clock = HCLK/8
  *     @arg RCC_HCLK_Div16: APB1 clock = HCLK/16
  * @retval None
  */
void RCC_PCLK1Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_PCLK(RCC_HCLK));
  tmpreg = RCC->CFGR;
  /* Clear PPRE1[2:0] bits */
  tmpreg &= CFGR_PPRE1_Reset_Mask;
  /* Set PPRE1[2:0] bits according to RCC_HCLK value */
  tmpreg |= RCC_HCLK;
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Configures the High Speed APB clock (PCLK2).
  * @param  RCC_HCLK: defines the APB2 clock divider. This clock is derived from 
  *   the AHB clock (HCLK).
  *   This parameter can be one of the following values:
  *     @arg RCC_HCLK_Div1: APB2 clock = HCLK
  *     @arg RCC_HCLK_Div2: APB2 clock = HCLK/2
  *     @arg RCC_HCLK_Div4: APB2 clock = HCLK/4
  *     @arg RCC_HCLK_Div8: APB2 clock = HCLK/8
  *     @arg RCC_HCLK_Div16: APB2 clock = HCLK/16
  * @retval None
  */
void RCC_PCLK2Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_PCLK(RCC_HCLK));
  tmpreg = RCC->CFGR;
  /* Clear PPRE2[2:0] bits */
  tmpreg &= CFGR_PPRE2_Reset_Mask;
  /* Set PPRE2[2:0] bits according to RCC_HCLK value */
  tmpreg |= RCC_HCLK << 3;
  /* Store the new value */
  RCC->CFGR = tmpreg;
}


/**
  * @brief  Enables or disables the specified RCC interrupts.
  * @param  RCC_IT: specifies the RCC interrupt sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg RCC_IT_LSIRDY: LSI ready interrupt
  *     @arg RCC_IT_LSERDY: LSE ready interrupt
  *     @arg RCC_IT_HSIRDY: HSI ready interrupt
  *     @arg RCC_IT_HSERDY: HSE ready interrupt
  *     @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *     @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt  
  * @param  NewState: new state of the specified RCC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_IT(RCC_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to enable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS |= RCC_IT;
  }
  else
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to disable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS &= (uint8_t)~RCC_IT;
  }
}

/**
  * @brief  Configures the External Low Speed oscillator (LSE).
  * @param  RCC_LSE: specifies the new state of the LSE.
  *   This parameter can be one of the following values:
  *     @arg RCC_LSE_OFF: LSE oscillator OFF
  *     @arg RCC_LSE_ON: LSE oscillator ON
  *     @arg RCC_LSE_Bypass: LSE oscillator bypassed with external clock
  * @retval None
  */
void RCC_LSEConfig(uint8_t RCC_LSE)
{
  /* Check the parameters */
  assert_param(IS_RCC_LSE(RCC_LSE));
  /* Reset LSEON and LSEBYP bits before configuring the LSE ------------------*/
  /* Reset LSEON bit */
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
  /* Reset LSEBYP bit */
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
  /* Configure LSE (RCC_LSE_OFF is already covered by the code section above) */
  switch (RCC_LSE)
  {
    case RCC_LSE_ON:
      /* Set LSEON bit */
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_ON;
      break;
    case RCC_LSE_Bypass:
      /* Set LSEBYP and LSEON bits */
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_Bypass | RCC_LSE_ON;
      break;
    default:
      break;
  }
}


/**
  * @brief  Enables or disables the Internal Low Speed oscillator (LSI).
  * @note   LSI can not be disabled if the IWDG is running.
  * @param  NewState: new state of the LSI. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_LSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CSR_LSION_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the RTC clock (RTCCLK).
  * @note   Once the RTC clock is selected it can’t be changed unless the Backup domain is reset.
  * @param  RCC_RTCCLKSource: specifies the RTC clock source.
  *   This parameter can be one of the following values:
  *     @arg RCC_RTCCLKSource_LSE: LSE selected as RTC clock
  *     @arg RCC_RTCCLKSource_LSI: LSI selected as RTC clock
  *     @arg RCC_RTCCLKSource_HSE_Divx: HSE clock divided by x selected as RTC clock, where x:[2,31]
  * @retval None
  */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_RTCCLK_SOURCE(RCC_RTCCLKSource));
  if ((RCC_RTCCLKSource & 0x00000300) == 0x00000300)
  { /* If HSE is selected as RTC clock source, configure HSE division factor for RTC clock */
    tmpreg = RCC->CFGR;
    /* Clear RTCPRE[4:0] bits */
    tmpreg &= CFGR_RTCPRE_Reset_Mask;
    /* Configure HSE division factor for RTC clock */
    tmpreg |= (RCC_RTCCLKSource & 0xFFFFCFF);
    /* Store the new value */
    RCC->CFGR = tmpreg;
  }
    
  /* Select the RTC clock source */
  RCC->BDCR |= (RCC_RTCCLKSource & 0x00000FFF);
}

/**
  * @brief  Enables or disables the RTC clock.
  * @note   This function must be used only after the RTC clock was selected using the RCC_RTCCLKConfig function.
  * @param  NewState: new state of the RTC clock. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) BDCR_RTCEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the I2S clock source (I2SCLK).
  * @note   - This function must be called before enabling the I2S APB clock.
  *         - This function applies only to Silicon RevB
  * @param  RCC_I2SCLKSource: specifies the I2S clock source.
  *   This parameter can be one of the following values:
  *     @arg RCC_I2S2CLKSource_PLLI2S: PLLI2S clock used as I2S clock source
  *     @arg RCC_I2S2CLKSource_Ext: External clock mapped on the I2S_CKIN pin
  *                                 used as I2S clock source
  * @retval None
  */
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource)
{
  /* Check the parameters */
  assert_param(IS_RCC_I2SCLK_SOURCE(RCC_I2SCLKSource));

  *(__IO uint32_t *) CFGR_I2SSRC_BB = RCC_I2SCLKSource;
}

/**
  * @brief  Returns the frequencies of different on chip clocks.
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold
  *   the clocks frequencies.
  * @retval None
  */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
  uint32_t tmp = 0, presc = 0, pllvco = 0, pllp = 2;
  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & CFGR_SWS_Mask;
  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock */
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock */
      RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock */
	  /* Note: on FPGA PLLVCO_CLK_OUT can be 240MHz, 288MHz, 384MHz or 432MHz and is 
	           defined by PLLN value (i.e PLLVCO_CLK_OUT = PLLN) */
      pllvco = (RCC->PLLCFGR & PLLCFGR_PPLN_Mask) >> 6;
	  pllp = (((RCC->PLLCFGR & PLLCFGR_PPLP_Mask) >>16) + 1 ) *2;
      RCC_Clocks->SYSCLK_Frequency = (pllvco/pllp) * 1000000;
      break;
    default:
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
      break;
  }
  /* Compute HCLK, PCLK1 and PCLK2 clocks frequencies ------------------------*/
  /* Get HCLK prescaler */
  tmp = RCC->CFGR & CFGR_HPRE_Set_Mask;
  tmp = tmp >> 4;
  presc = APBAHBPrescTable[tmp];
  /* HCLK clock frequency */
  RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;
  /* Get PCLK1 prescaler */
  tmp = RCC->CFGR & CFGR_PPRE1_Set_Mask;
  tmp = tmp >> 10;
  presc = APBAHBPrescTable[tmp];
  /* PCLK1 clock frequency */
  RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
  /* Get PCLK2 prescaler */
  tmp = RCC->CFGR & CFGR_PPRE2_Set_Mask;
  tmp = tmp >> 13;
  presc = APBAHBPrescTable[tmp];
  /* PCLK2 clock frequency */
  RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
}

/**
  * @brief  Enables or disables the AHB1 peripheral clock.
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC,
  *     RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF,
  *     RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI,
  *     RCC_AHB1Periph_CRC, RCC_AHB1Periph_BKPSRAM, RCC_AHB1Periph_DMA1,
  *     RCC_AHB1Periph_DMA2, RCC_AHB1Periph_ETH_MAC,
  *     RCC_AHB1Periph_ETH_MAC_Tx, RCC_AHB1Periph_ETH_MAC_Rx,
  *     RCC_AHB1Periph_ETH_MAC_PTP, RCC_AHB1Periph_OTG_HS,
  *     RCC_AHB1Periph_OTG_HS_ULPI
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_CLOCK_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1ENR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1ENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock.
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_AHB2Periph_DCMI, RCC_AHB2Periph_CRYP, RCC_AHB2Periph_HASH,
  *     RCC_AHB2Periph_RNG, RCC_AHB2Periph_OTG_FS
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB2ENR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2ENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock.
  * @param  RCC_AHBPeriph: specifies the AHB3 peripheral to gates its clock.
  *   This parameter must be: RCC_AHB3Periph_FSMC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB3ENR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3ENR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Enables or disables the Low Speed APB (APB1) peripheral clock.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
  *     RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7,
  *     RCC_APB1Periph_TIM12, RCC_APB1Periph_TIM13, RCC_APB1Periph_TIM14,
  *     RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3,
  *     RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4,
  *     RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2,
  *     RCC_APB1Periph_I2C3, RCC_APB1Periph_CAN11, RCC_APB1Periph_CAN12,
  *     RCC_APB1Periph_PWR, RCC_APB1Periph_DAC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1ENR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1ENR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_APB2Periph_TIM1, RCC_APB2Periph_TIM8, RCC_APB2Periph_USART1,
  *     RCC_APB2Periph_USART6, RCC_APB2Periph_ADC1, RCC_APB2Periph_ADC2,
  *     RCC_APB2Periph_ADC3, RCC_APB2Periph_SDIO, RCC_APB2Periph_SPI1,
  *     RCC_APB2Periph_SYSCFG, RCC_APB2Periph_TIM9, RCC_APB2Periph_TIM10,
  *     RCC_APB2Periph_TIM11
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB2ENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2ENR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Forces or releases AHB1 peripheral reset.
  * @param  RCC_AHB1Periph: specifies the AHB1 peripheral to reset.
  *   This parameter can be any combination of the following values:
  *     RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC,
  *     RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF,
  *     RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI,
  *     RCC_AHB1Periph_CRC, RCC_AHB1Periph_DMA1, RCC_AHB1Periph_DMA2,
  *     RCC_AHB1Periph_ETH_MAC, RCC_AHB1Periph_OTG_HS
  * @param  NewState: new state of the specified peripheral reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_RESET_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1RSTR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1RSTR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Forces or releases AHB2 peripheral reset.
  * @param  RCC_AHB2Periph: specifies the AHB2 peripheral to reset.
  *   This parameter can be any combination of the following values:
  *     RCC_AHB2Periph_DCMI, RCC_AHB2Periph_CRYP, RCC_AHB2Periph_HASH,
  *     RCC_AHB2Periph_RNG, RCC_AHB2Periph_OTG_FS
  * @param  NewState: new state of the specified peripheral reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB2RSTR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2RSTR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Forces or releases AHB3 peripheral reset.
  * @param  RCC_AHB3Periph: specifies the AHB3 peripheral to reset.
  *   This parameter must be: RCC_AHB3Periph_FSMC
  * @param  NewState: new state of the specified peripheral reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB3RSTR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3RSTR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
  *   This parameter can be any combination of the following values:
  *     RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
  *     RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7,
  *     RCC_APB1Periph_TIM12, RCC_APB1Periph_TIM13, RCC_APB1Periph_TIM14,
  *     RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3,
  *     RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4,
  *     RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2,
  *     RCC_APB1Periph_I2C3, RCC_APB1Periph_CAN11, RCC_APB1Periph_CAN12,
  *     RCC_APB1Periph_PWR, RCC_APB1Periph_DAC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1RSTR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1RSTR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Forces or releases High Speed APB (APB2) peripheral reset.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to reset.
  *   This parameter can be any combination of the following values:
  *     RCC_APB2Periph_TIM1, RCC_APB2Periph_TIM8, RCC_APB2Periph_USART1,
  *     RCC_APB2Periph_USART6, RCC_APB2Periph_ADC, RCC_APB2Periph_SDIO,
  *     RCC_APB2Periph_SPI1, RCC_APB2Periph_SYSCFG, RCC_APB2Periph_TIM9,
  *     RCC_APB2Periph_TIM10, RCC_APB2Periph_TIM11
  * @param  NewState: new state of the specified peripheral reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_RESET_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB2RSTR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2RSTR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  * @param  RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC,
  *     RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOF,
  *     RCC_AHB1Periph_GPIOG, RCC_AHB1Periph_GPIOH, RCC_AHB1Periph_GPIOI,
  *     RCC_AHB1Periph_CRC, RCC_AHB1Periph_FLITF, RCC_AHB1Periph_BKPSRAM,
  *     RCC_AHB1Periph_DMA1, RCC_AHB1Periph_DMA2, RCC_AHB1Periph_ETH_MAC,
  *     RCC_AHB1Periph_ETH_MAC_Tx, RCC_AHB1Periph_ETH_MAC_Rx,
  *     RCC_AHB1Periph_ETH_MAC_PTP, RCC_AHB1Periph_OTG_HS,
  *     RCC_AHB1Periph_OTG_HS_ULPI
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphClockLPModeCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB1_LPMODE_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB1LPENR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1LPENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_AHB2Periph_DCMI, RCC_AHB2Periph_CRYP, RCC_AHB2Periph_HASH,
  *     RCC_AHB2Periph_RNG, RCC_AHB2Periph_OTG_FS
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB2PeriphClockLPModeCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB2LPENR |= RCC_AHB2Periph;
  }
  else
  {
    RCC->AHB2LPENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock during Low Power (Sleep) mode.
  * @param  RCC_AHBPeriph: specifies the AHB3 peripheral to gates its clock.
  *   This parameter must be: RCC_AHB3Periph_FSMC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB3PeriphClockLPModeCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->AHB3LPENR |= RCC_AHB3Periph;
  }
  else
  {
    RCC->AHB3LPENR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Enables or disables the APB1 peripheral clock during Low Power (Sleep) mode.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
  *     RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7,
  *     RCC_APB1Periph_TIM12, RCC_APB1Periph_TIM13, RCC_APB1Periph_TIM14,
  *     RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3,
  *     RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4,
  *     RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2,
  *     RCC_APB1Periph_I2C3, RCC_APB1Periph_CAN11, RCC_APB1Periph_CAN12,
  *     RCC_APB1Periph_PWR, RCC_APB1Periph_DAC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphClockLPModeCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1LPENR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1LPENR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Enables or disables the APB2 peripheral clock during Low Power (Sleep) mode.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *   This parameter can be any combination of the following values:
  *     RCC_APB2Periph_TIM1, RCC_APB2Periph_TIM8, RCC_APB2Periph_USART1,
  *     RCC_APB2Periph_USART6, RCC_APB2Periph_ADC1, RCC_APB2Periph_ADC2,
  *     RCC_APB2Periph_ADC3, RCC_APB2Periph_SDIO, RCC_APB2Periph_SPI1,
  *     RCC_APB2Periph_SYSCFG, RCC_APB2Periph_TIM9, RCC_APB2Periph_TIM10,
  *     RCC_APB2Periph_TIM11
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB2PeriphClockLPModeCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB2LPENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2LPENR &= ~RCC_APB2Periph;
  }
}

/**
  * @brief  Forces or releases the Backup domain reset.
  * @param  NewState: new state of the Backup domain reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_BackupResetCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) BDCR_BDRST_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Clock Security System.
  * @param  NewState: new state of the Clock Security System..
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_CSSON_BB = (uint32_t)NewState;
}

/**
  * @brief  Selects the clock source to output on MCO1 pin(PA8).
  * @param  RCC_MCO1Source: specifies the clock source to output.
  *   This parameter can be one of the following values:
  *     @arg RCC_MCO1Source_HSI: HSI clock selected as MCO1 source
  *     @arg RCC_MCO1Source_LSE: LSE clock selected as MCO1 source
  *     @arg RCC_MCO1Source_HSE: HSE clock selected as MCO1 source
  *     @arg RCC_MCO1Source_PLLCLK: main PLL clock selected as MCO1 source
  * @param  RCC_MCO1Div: specifies the MCO1 prescaler.
  *   This parameter can be one of the following values:
  *     @arg RCC_MCO1Div_1: no division applied to MCO1 clock
  *     @arg RCC_MCO1Div_2: division by 2 applied to MCO1 clock
  *     @arg RCC_MCO1Div_3: division by 3 applied to MCO1 clock
  *     @arg RCC_MCO1Div_4: division by 4 applied to MCO1 clock
  *     @arg RCC_MCO1Div_5: division by 5 applied to MCO1 clock
  * @retval None
  */
void RCC_MCO1Config(uint32_t RCC_MCO1Source, uint32_t RCC_MCO1Div)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO1SOURCE(RCC_MCO1Source));
  assert_param(IS_RCC_MCO1DIV(RCC_MCO1Div));  
  tmpreg = RCC->CFGR;
  /* Clear MCO1[1:0] and MCO1PRE[2:0] bits */
  tmpreg &= CFGR_MCO1_Reset_Mask;
  /* Select MCO1 clock source and prescaler */
  tmpreg |= RCC_MCO1Source | RCC_MCO1Div;
  /* Store the new value */
  RCC->CFGR = tmpreg;  
}


/**
  * @brief  Selects the clock source to output on MCO2 pin(PC9).
  * @param  RCC_MCO2Source: specifies the clock source to output.
  *   This parameter can be one of the following values:
  *     @arg RCC_MCO2Source_SYSCLK: System clock (SYSCLK) selected as MCO2 source
  *     @arg RCC_MCO2Source_PLLI2SCLK: PLLI2S clock selected as MCO2 source
  *     @arg RCC_MCO2Source_HSE: HSE clock selected as MCO2 source
  *     @arg RCC_MCO2Source_PLLCLK: main PLL clock selected as MCO2 source
  * @param  RCC_MCO2Div: specifies the MCO2 prescaler.
  *   This parameter can be one of the following values:
  *     @arg RCC_MCO2Div_1: no division applied to MCO2 clock
  *     @arg RCC_MCO2Div_2: division by 2 applied to MCO2 clock
  *     @arg RCC_MCO2Div_3: division by 3 applied to MCO2 clock
  *     @arg RCC_MCO2Div_4: division by 4 applied to MCO2 clock
  *     @arg RCC_MCO2Div_5: division by 5 applied to MCO2 clock
  * @retval None
  */
void RCC_MCO2Config(uint32_t RCC_MCO2Source, uint32_t RCC_MCO2Div)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO2SOURCE(RCC_MCO2Source));
  assert_param(IS_RCC_MCO2DIV(RCC_MCO2Div));
  
  tmpreg = RCC->CFGR;
  /* Clear MCO2 and MCO2PRE[2:0] bits */
  tmpreg &= CFGR_MCO2_Reset_Mask;
  /* Select MCO2 clock source and prescaler */
  tmpreg |= RCC_MCO2Source | RCC_MCO2Div;
  /* Store the new value */
  RCC->CFGR = tmpreg;  
}


/**
  * @brief  Checks whether the specified RCC flag is set or not.
  * @param  RCC_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *     @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
  *     @arg RCC_FLAG_PLLRDY: main PLL clock ready
  *     @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready
  *     @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *     @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *     @arg RCC_FLAG_BORRST: POR/PDR or BOR reset
  *     @arg RCC_FLAG_PINRST: Pin reset
  *     @arg RCC_FLAG_PORRST: POR/PDR reset
  *     @arg RCC_FLAG_SFTRST: Software reset
  *     @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
  *     @arg RCC_FLAG_WWDGRST: Window Watchdog reset
  *     @arg RCC_FLAG_LPWRRST: Low Power reset
  * @retval The new state of RCC_FLAG (SET or RESET).
  */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
  uint32_t tmp = 0;
  uint32_t statusreg = 0;
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_RCC_FLAG(RCC_FLAG));
  /* Get the RCC register index */
  tmp = RCC_FLAG >> 5;
  if (tmp == 1)               /* The flag to check is in CR register */
  {
    statusreg = RCC->CR;
  }
  else if (tmp == 2)          /* The flag to check is in BDCR register */
  {
    statusreg = RCC->BDCR;
  }
  else                       /* The flag to check is in CSR register */
  {
    statusreg = RCC->CSR;
  }
  /* Get the flag position */
  tmp = RCC_FLAG & FLAG_Mask;
  if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}


/**
  * @brief  Clears the RCC reset flags.
  *   The reset flags are: RCC_FLAG_PINRST, RCC_FLAG_PORRST,  RCC_FLAG_SFTRST,
  *   RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST, RCC_FLAG_LPWRRST
  * @param  None
  * @retval None
  */
void RCC_ClearFlag(void)
{
  /* Set RMVF bit to clear the reset flags */
  RCC->CSR |= CSR_RMVF_Set;
}


/**
  * @brief  Checks whether the specified RCC interrupt has occurred or not.
  * @param  RCC_IT: specifies the RCC interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg RCC_IT_LSIRDY: LSI ready interrupt
  *     @arg RCC_IT_LSERDY: LSE ready interrupt
  *     @arg RCC_IT_HSIRDY: HSI ready interrupt
  *     @arg RCC_IT_HSERDY: HSE ready interrupt
  *     @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *     @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt  
  *     @arg RCC_IT_CSS: Clock Security System interrupt
  * @retval The new state of RCC_IT (SET or RESET).
  */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_RCC_GET_IT(RCC_IT));
  /* Check the status of the specified RCC interrupt */
  if ((RCC->CIR & RCC_IT) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the RCC_IT status */
  return  bitstatus;
}


/**
  * @brief  Clears the RCC’s interrupt pending bits.
  * @param  RCC_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg RCC_IT_LSIRDY: LSI ready interrupt
  *     @arg RCC_IT_LSERDY: LSE ready interrupt
  *     @arg RCC_IT_HSIRDY: HSI ready interrupt
  *     @arg RCC_IT_HSERDY: HSE ready interrupt
  *     @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *     @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt  
  *     @arg RCC_IT_CSS: Clock Security System interrupt
  * @retval None
  */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
  /* Check the parameters */
  assert_param(IS_RCC_CLEAR_IT(RCC_IT));
  /* Perform Byte access to RCC_CIR[23:16] bits to clear the selected interrupt
     pending bits */
  *(__IO uint8_t *) CIR_BYTE3_ADDRESS = RCC_IT;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
