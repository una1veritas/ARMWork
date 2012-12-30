/**
  ******************************************************************************
  * @file    stm32f2xx_rng.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file provides all the Random Number Generator(RNG) firmware functions.
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
#include "stm32f2xx_rng.h"
#include "stm32f2xx_rcc.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @defgroup RNG 
  * @brief RNG driver modules
  * @{
  */ 

/** @defgroup RNG_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RNG_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RNG_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RNG_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RNG_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup RNG_Private_Functions
  * @{
  */ 


/**
  * @brief  Deinitializes the RNG peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void RNG_DeInit()
{
  /* Enable RNG reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_RNG, ENABLE);

  /* Release RNG from reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_RNG, DISABLE);
}

/**
  * @brief  Enables or disables the RNG peripheral.
  * @param  NewState: new state of the RNG peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RNG_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the RNG */
    RNG->CR |= RNG_CR_RNGEN;
  }
  else
  {
    /* Disable the RNG */
    RNG->CR &= RNG_CR_RNGEN;
  }
}

/**
  * @brief  Enables or disables the RNG interrupt.
  * @note   The RNG has two interrupt sources; Seed error interrupt (SEI) and 
  *         Clock error interrupt (CEI), both interrupts are enabled by setting 
  *         the IM bit in CR register. However, each interrupt have its specific 
  *         status bit (see RNG_GetITStatus() function) and clear bit (see 
  *         RNG_ClearITPendingBit() function).
  * @param  NewState: new state of the RNG interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RNG_ITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the RNG interrupt */
    RNG->CR |= RNG_CR_IM;
  }
  else
  {
    /* Disable the RNG interrupt */
    RNG->CR &= ~RNG_CR_IM;
  }
}

/**
  * @brief  Returns a 32-bit random number.
  * @note   Before to call this function you have to wait till DRDY flag is set,
  *         using RNG_GetFlagStatus(RNG_FLAG_DRDY) function. 
  * @param  None
  * @retval 32-bit random number.
  */
uint32_t RNG_GetRandomNumber(void)
{
  /* Return the random number from the DR register */
  return RNG->DR;
}


/**
  * @brief  Checks whether the specified RNG flag is set or not.
  * @param  RNG_FLAG: specifies the RNG flag to check.
  *   This parameter can be one of the following values:
  *     @arg RNG_FLAG_DRDY: Data Ready flag.
  *     @arg RNG_FLAG_CECS: Clock Error Current flag.
  *     @arg RNG_FLAG_SECS: Seed Error Current flag.
  * @retval The new state of RNG_FLAG (SET or RESET).
  */
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_RNG_GET_FLAG(RNG_FLAG));

  /* Check the status of the specified RNG flag */
  if ((RNG->SR & RNG_FLAG) != (uint8_t)RESET)
  {
    /* RNG_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* RNG_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the RNG_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Checks whether the specified RNG interrupt has occurred or not.
  * @param  RNG_IT: specifies the RNG interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg RNG_IT_CEI: Clock Error Interrupt.
  *     @arg RNG_IT_SEI: Seed Error Interrupt.
  * @retval The new state of RNG_IT (SET or RESET).
  */
ITStatus RNG_GetITStatus(uint8_t RNG_IT)
{
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_RNG_GET_IT(RNG_IT));

  /* Check the status of the specified RNG interrupt */
  if ((RNG->SR & RNG_IT) != (uint8_t)RESET)
  {
    /* RNG_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* RNG_IT is reset */
    bitstatus = RESET;
  }
  /* Return the RNG_IT status */
  return bitstatus;
}


/**
  * @brief  Clears the RNG interrupt pending bit(s).
  * @param  RNG_IT: specifies the RNG interrupt pending bit(s) to clear.
  *   This parameter can be any combination of the following values:
  *     @arg RNG_IT_CEI: Clock Error Interrupt.
  *     @arg RNG_IT_SEI: Seed Error Interrupt.
  * @retval None
  */
void RNG_ClearITPendingBit(uint8_t RNG_IT)
{
  /* Check the parameters */
  assert_param(IS_RNG_IT(RNG_IT));

  /* Clear the selected RNG interrupt pending bit */
  RNG->SR = (uint8_t)~RNG_IT;
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
