/**
  ******************************************************************************
  * @file    stm32f2xx_hash.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file provides all the HASH firmware functions.
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
#include "stm32f2xx_hash.h"
#include "stm32f2xx_rcc.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @defgroup HASH 
  * @brief HASH driver modules
  * @{
  */ 

/** @defgroup HASH_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup HASH_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup HASH_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup HASH_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup HASH_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup HASH_Private_Functions
  * @{
  */ 


/**
  * @brief  Deinitializes the HASH peripheral registers to their default reset values
  * @param  None
  * @retval None
  */
void HASH_DeInit(void)
{
  /* Enable HASH reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_HASH, ENABLE);
  /* Release HASH from reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_HASH, DISABLE);
}

/**
  * @brief  Initializes the HASH peripheral according to the specified parameters 
  *   in the HASH_InitStruct.
  * @param  HASH_InitStruct: pointer to a HASH_InitTypeDef structure that contains
  *   the configuration information for the HASH peripheral.
  * @retval None
  */
void HASH_Init(HASH_InitTypeDef* HASH_InitStruct)
{
  assert_param(IS_HASH_ALGOSELECTION(HASH_InitStruct->HASH_AlgoSelection));
  assert_param(IS_HASH_DATATYPE(HASH_InitStruct->HASH_DataType));
  assert_param(IS_HASH_ALGOMODE(HASH_InitStruct->HASH_AlgoMode));
  
  HASH->CR &= ~ (HASH_CR_ALGO | HASH_CR_DATATYPE | HASH_CR_MODE);
  HASH->CR |= (HASH_InitStruct->HASH_AlgoSelection | HASH_InitStruct->HASH_DataType | \
               HASH_InitStruct->HASH_AlgoMode);
    
  if(HASH_InitStruct->HASH_AlgoMode == HASH_AlgoMode_HMAC) 
  {
    assert_param(IS_HASH_KEYTYPE(HASH_InitStruct->HASH_KeyType));
    HASH->CR &= ~HASH_CR_LKEY;
    HASH->CR |= HASH_InitStruct->HASH_KeyType;
  }

  /* Reset the HASH processor core, so that the HASH will be ready to compute 
     the message digest of a new message */
  HASH->CR |= HASH_CR_INIT;  
}

/**
  * @brief  Fills each HASH_InitStruct member with its default value.
  * @param  HASH_InitStruct : pointer to a HASH_InitTypeDef structure which will
  *   be initialized.
  * @retval None
  */
void HASH_StructInit(HASH_InitTypeDef* HASH_InitStruct)
{
  /* Initialize the HASH_AlgoSelection member */
  HASH_InitStruct->HASH_AlgoSelection = HASH_AlgoSelection_SHA1;

  /* Initialize the HASH_AlgoMode member */
  HASH_InitStruct->HASH_AlgoMode = HASH_AlgoMode_HASH;

  /* Initialize the HASH_DataType member */
  HASH_InitStruct->HASH_DataType = HASH_DataType_32b;

  /* Initialize the HASH_KeyType member */
  HASH_InitStruct->HASH_KeyType = HASH_KeyType_ShortKey;
}

/**
  * @brief  Starts the message padding and calculation of the final message
  * @param  None
  * @retval None
  */
void HASH_Start(void)
{
  /* Start the Digest calculation */
  HASH->STR |= HASH_STR_DCAL;
}

/**
  * @brief  Resets the HASH processor core, so that the HASH will be ready
  *   to compute the message digest of a new message.
  * @param  None
  * @retval None
  */
void HASH_Reset(void)
{
  /* Reset the HASH processor core */
  HASH->CR |= HASH_CR_INIT;
}

/**
  * @brief  Configure the Number of valid bits in last word of the message
  * @param  HASH_ValidNumber: Number of valid bits in last word of the message.
  *   This parameter must be a number between 0 and 0x1F.
  * @retval None
  */
void HASH_NbValidBits(uint16_t HASH_ValidNumber)
{
  assert_param(IS_HASH_VALIDBITSNUMBER(HASH_ValidNumber));
  
  /* Configure the Number of valid bits in last word of the message */
  HASH->STR &= ~(HASH_STR_NBW);
  HASH->STR |= HASH_ValidNumber;
}

/**
  * @brief  Enables or disables the HASH DMA interface.
  * @note   The DMA is disabled by hardware after the end of transfer of message data.  
  * @param  NewState: new state of the selected HASH DMA transfer request.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void HASH_DMACmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the HASH DMA request */
    HASH->CR |= HASH_CR_DMAE;
  }
  else
  {
    /* Disable the HASH DMA request */
    HASH->CR &= ~HASH_CR_DMAE;
  }
}

/**
  * @brief  Writes data in the Data Input regiset (DIN).
  * @param  Data: new data of the message to be processed.
  * @retval None
  */
void HASH_DataIn(uint32_t Data)
{
  /* Write in the DIN register a new data */
  HASH->DIN = Data;
}

/**
  * @brief  Provides the message digest result.
  * @note   In MD5 mode, Data[4] filed of HASH_MsgDigest structure is not used
  *         and is read as zero.  
  * @param  HASH_MessageDigest: pointer to a HASH_MsgDigest structure which will hold
  *   the message digest result 
  * @retval None
  */
void HASH_DataOut(HASH_MsgDigest* HASH_MessageDigest)
{
  /* Get the data field */
  HASH_MessageDigest->Data[0] = HASH->HR[0];
  HASH_MessageDigest->Data[1] = HASH->HR[1];
  HASH_MessageDigest->Data[2] = HASH->HR[2];
  HASH_MessageDigest->Data[3] = HASH->HR[3];
  HASH_MessageDigest->Data[4] = HASH->HR[4];
}

/**
  * @brief  Enables or disables the specified HASH interrupts.
  * @param  HASH_IT: specifies the HASH interrupt source to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg HASH_IT_DINI: Data Input interrupt
  *     @arg HASH_IT_DCI: Digest Calculation Completion Interrupt
  * @param  NewState: new state of the specified HASH interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void HASH_ITConfig(uint8_t HASH_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_HASH_IT(HASH_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected HASH interrupt */
    HASH->IMR |= HASH_IT;
  }
  else
  {
    /* Disable the selected HASH interrupt */
    HASH->IMR &= ~HASH_IT;
  }
}

/**
  * @brief  Checks whether the specified HASH interrupt has occurred or not.
  * @param  HASH_IT: specifies the HASH interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg HASH_IT_DINI: Data Input interrupt
  *     @arg HASH_IT_DCI: Digest Calculation Completion Interrupt
  * @retval The new state of HASH_IT (SET or RESET).
  */
ITStatus HASH_GetITStatus(uint8_t HASH_IT)
{
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_HASH_GET_IT(HASH_IT));

  /* Check the status of the specified HASH interrupt */
  if ((HASH->SR & HASH_IT) != (uint8_t)RESET)
  {
    /* HASH_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* HASH_IT is reset */
    bitstatus = RESET;
  }
  /* Return the HASH_IT status */
  return bitstatus;
}

/**
  * @brief  Clears the HASH interrupt pending bit(s).
  * @param  HASH_IT: specifies the HASH interrupt pending bit(s) to clear.
  *   This parameter can be any combination of the following values:
  *     @arg HASH_IT_DINI: Data Input interrupt
  *     @arg HASH_IT_DCI: Digest Calculation Completion Interrupt
  * @retval None
  */
void HASH_ClearITPendingBit(uint8_t HASH_IT)
{
  /* Check the parameters */
  assert_param(IS_HASH_IT(HASH_IT));

  /* Clear the selected HASH interrupt pending bit */
  HASH->SR = (uint8_t)~HASH_IT;
}

/**
  * @brief  Checks whether the specified HASH flag is set or not.
  * @param  HASH_FLAG: specifies the HASH flag to check.
  *   This parameter can be one of the following values:
  *     @arg HASH_FLAG_DINIS: Data input interrupt status flag
  *     @arg HASH_FLAG_DCIS: Digest calculation completion interrupt status flag
  *     @arg HASH_FLAG_BUSY: Busy flag
  *     @arg HASH_FLAG_DMAS: DMAS Status flag
  *     @arg HASH_FLAG_DINNE: Data Input regiset (DIN) not empty status flag
  * @retval The new state of HASH_FLAG (SET or RESET)
  */
FlagStatus HASH_GetFlagStatus(uint16_t HASH_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t hashreg, tempreg = 0;

  /* Check the parameters */
  assert_param(IS_HASH_GET_FLAG(HASH_FLAG));

  /* Get the HASH register index */
  hashreg = (((uint16_t)HASH_FLAG) >> 12);

  if (hashreg == 0x01) /* The FLAG is in CR register */
  {
    tempreg = HASH->CR;
  }
  else /* The FLAG is in SR register */
  {
    tempreg = HASH->SR;
  }

  /* Check the status of the specified HASH flag */
  if ((tempreg & HASH_FLAG) != (uint8_t)RESET)
  {
    /* HASH is set */
    bitstatus = SET;
  }
  else
  {
    /* HASH_FLAG is reset */
    bitstatus = RESET;
  }

  /* Return the HASH_FLAG status */
  return  bitstatus;
}


/**
  * @brief  Returns the number of words already pushed into the IN FIFO.
  * @param  None
  * @retval The value of words already pushed into the IN FIFO.
  */
uint8_t HASH_GetFIFOWordsNumber(void)
{
  /* Return the value of NBW bits */
  return ((HASH->CR & HASH_CR_NBW) >> 8);
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
