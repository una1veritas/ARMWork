/**
  ******************************************************************************
  * @file    stm32f2xx_cryp.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file provides all the Cryptographic processor (CRYP) firmware
  *          functions.
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
#include "stm32f2xx_cryp.h"
#include "stm32f2xx_rcc.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @defgroup CRYP 
  * @brief CRYP driver modules
  * @{
  */ 

/** @defgroup CRYP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup CRYP_Private_Defines
  * @{
  */ 
#define FLAG_MASK    ((uint8_t)0x1F)

/**
  * @}
  */ 


/** @defgroup CRYP_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup CRYP_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup CRYP_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup CRYP_Private_Functions
  * @{
  */ 


/**
  * @brief  Deinitializes the CRYP peripheral registers to their default reset values
  * @param  None
  * @retval None
  */
void CRYP_DeInit(void)
{
  /* Enable CRYP reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_CRYP, ENABLE);

  /* Release CRYP from reset state */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_CRYP, DISABLE);
}

/**
  * @brief  Initializes the CRYP peripheral according to the specified parameters
  *   in the CRYP_InitStruct.
  * @param  CRYP_InitStruct: pointer to a CRYP_InitTypeDef structure that contains
  *   the configuration information for the CRYP peripheral.
  * @retval None
  */
void CRYP_Init(CRYP_InitTypeDef* CRYP_InitStruct)
{
  /* Check the parameters */
  assert_param(IS_CRYP_ALGOMODE(CRYP_InitStruct->CRYP_AlgoMode));
  assert_param(IS_CRYP_DATATYPE(CRYP_InitStruct->CRYP_DataType));
  assert_param(IS_CRYP_ALGODIR(CRYP_InitStruct->CRYP_AlgoDir));
	
  /* Select Algorithm mode*/  
  CRYP->CR &= ~CRYP_CR_ALGOMODE;
  CRYP->CR |= CRYP_InitStruct->CRYP_AlgoMode;
  
  /* Select dataType */ 
  CRYP->CR &= ~CRYP_CR_DATATYPE;
  CRYP->CR |= CRYP_InitStruct->CRYP_DataType;
  
  /* select Key size (used only with AES algorithm) */
  if ((CRYP_InitStruct->CRYP_AlgoMode == CRYP_AlgoMode_AES_ECB) ||
      (CRYP_InitStruct->CRYP_AlgoMode == CRYP_AlgoMode_AES_CBC) ||
      (CRYP_InitStruct->CRYP_AlgoMode == CRYP_AlgoMode_AES_CTR) ||
      (CRYP_InitStruct->CRYP_AlgoMode == CRYP_AlgoMode_AES_Key))
  {
    assert_param(IS_CRYP_KEYSIZE(CRYP_InitStruct->CRYP_KeySize));
    CRYP->CR &= ~CRYP_CR_KEYSIZE;
    CRYP->CR |= CRYP_InitStruct->CRYP_KeySize; /* Key size and value must be configured once the key has been prepared */
  }
  
  /* Select data Direction */ 
  CRYP->CR &= ~CRYP_CR_ALGODIR;
  CRYP->CR |= CRYP_InitStruct->CRYP_AlgoDir;
}

/**
  * @brief  Initializes the CRYP Initialization Vectors(IV) according to the
  *   specified parameters in the CRYP_IVInitStruct.
  * @param  CRYP_IVInitStruct: pointer to a CRYP_IVInitTypeDef structure that
  *   contains the configuration information for the CRYP Initialization Vectors(IV).
  * @retval None
  */
void CRYP_IVInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct)
{
  CRYP->IV0LR = CRYP_IVInitStruct->CRYP_IV0Left;
  CRYP->IV0RR = CRYP_IVInitStruct->CRYP_IV0Right;
  CRYP->IV1LR = CRYP_IVInitStruct->CRYP_IV1Left;
  CRYP->IV1RR = CRYP_IVInitStruct->CRYP_IV1Right;
}

/**
  * @brief  Initializes the CRYP Keys according to the specified parameters in
  *   the CRYP_KeyInitStruct.
  * @param  CRYP_KeyInitStruct: pointer to a CRYP_KeyInitTypeDef structure that
  *   contains the configuration information for the CRYP Keys.
  * @retval None
  */
void CRYP_KeyInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
{
  /* Key Initialisation */
  CRYP->K0LR = CRYP_KeyInitStruct->CRYP_Key0Left;
  CRYP->K0RR = CRYP_KeyInitStruct->CRYP_Key0Right;
  CRYP->K1LR = CRYP_KeyInitStruct->CRYP_Key1Left;
  CRYP->K1RR = CRYP_KeyInitStruct->CRYP_Key1Right;
  CRYP->K2LR = CRYP_KeyInitStruct->CRYP_Key2Left;
  CRYP->K2RR = CRYP_KeyInitStruct->CRYP_Key2Right;
  CRYP->K3LR = CRYP_KeyInitStruct->CRYP_Key3Left;
  CRYP->K3RR = CRYP_KeyInitStruct->CRYP_Key3Right;
}

/**
  * @brief  Fills each CRYP_InitStruct member with its default value.
  * @param  CRYP_InitStruct: pointer to a CRYP_InitTypeDef structure which will
  *   be initialized.
  * @retval None
  */
void CRYP_StructInit(CRYP_InitTypeDef* CRYP_InitStruct)
{
  /* Initialize the CRYP_AlgoDir member */
  CRYP_InitStruct->CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
  
  /* initialize the CRYP_AlgoMode member */
  CRYP_InitStruct->CRYP_AlgoMode = CRYP_AlgoMode_TDES_ECB;
  
  /* initialize the CRYP_DataType member */
  CRYP_InitStruct->CRYP_DataType = CRYP_DataType_32b;
  
  /* Initialize the CRYP_KeySize member */
  CRYP_InitStruct->CRYP_KeySize = CRYP_KeySize_128b;
}

/**
  * @brief  Fills each CRYP_IVInitStruct member with its default value.
  * @param  CRYP_IVInitStruct: pointer to a CRYP_IVInitTypeDef Initialization Vectors(IV) 
  *   structure which will be initialized.
  * @retval None
  */
void CRYP_IVStructInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct)
{
  CRYP_IVInitStruct->CRYP_IV0Left = 0;
  CRYP_IVInitStruct->CRYP_IV0Right = 0;
  CRYP_IVInitStruct->CRYP_IV1Left = 0;
  CRYP_IVInitStruct->CRYP_IV1Right = 0;
}

/**
  * @brief  Fills each CRYP_KeyInitStruct member with its default value.
  * @param  CRYP_KeyInitStruct: pointer to a CRYP_KeyInitTypeDef structure which
  *   will be initialized.
  * @retval None
  */
void CRYP_KeyStructInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
{
  CRYP_KeyInitStruct->CRYP_Key0Left = 0;
  CRYP_KeyInitStruct->CRYP_Key0Right = 0;
  CRYP_KeyInitStruct->CRYP_Key1Left = 0;
  CRYP_KeyInitStruct->CRYP_Key1Right = 0;
  CRYP_KeyInitStruct->CRYP_Key2Left = 0;
  CRYP_KeyInitStruct->CRYP_Key2Right = 0;
  CRYP_KeyInitStruct->CRYP_Key3Left = 0;
  CRYP_KeyInitStruct->CRYP_Key3Right = 0;
}

/**
  * @brief  Enables or disables the CRYP peripheral.
  * @param  NewState: new state of the CRYP peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CRYP_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Cryptographic processor */
    CRYP->CR |= CRYP_CR_CRYPEN;
  }
  else
  {
    /* Disable the Cryptographic processor */
    CRYP->CR &= ~CRYP_CR_CRYPEN;
  }
}

/**
  * @brief  Enables or disables the CRYP DMA interface.
  * @param  CRYP_DMAReq: specifies the CRYP DMA transfer request to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg CRYP_DMAReq_Tx: DMA for outgoing(Tx) data transfer
  *     @arg CRYP_DMAReq_Rx: DMA for incoming(Rx) data transfer
  * @param  NewState: new state of the selected CRYP DMA transfer request.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CRYP_DMACmd(uint8_t CRYP_DMAReq, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_CRYP_DMAREQ(CRYP_DMAReq));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
	
  if (NewState != DISABLE)
  {
    /* Enable the selected CRYP DMA request */
    CRYP->DMACR |= CRYP_DMAReq;
  }
  else
  {
    /* Disable the selected CRYP DMA request */
    CRYP->DMACR &= (uint16_t)~CRYP_DMAReq;
  }
}

/**
  * @brief  Writes data in the Data Input register (DIN).
  * @note   After the DIN register has been read once or several times, the FIFO 
  *         must be flushed (using CRYP_FIFOFlush() function).  
  * @param  Data: data to write in Data Input register
  * @retval None
  */
void CRYP_DataIn(uint32_t Data)
{
  CRYP->DR = Data;
}

/**
  * @brief  Returns the last data entered into the output FIFO.
  * @param  None
  * @retval Last data entered into the output FIFO.
  */
uint32_t CRYP_DataOut(void)
{
  return CRYP->DOUT;
}

/**
  * @brief  Flushes the IN and OUT FIFOs (that is read and write pointers of the 
  *   FIFOs are reset)
  * @note   The FIFOs must be flushed only when BUSY flag is reset.  
  * @param  None
  * @retval None
  */
void CRYP_FIFOFlush(void)
{
  /* Reset the read and write pointers of the FIFOs */
  CRYP->CR |= CRYP_CR_FFLUSH;
}

/**
  * @brief  Enables or disables the specified CRYP interrupts.
  * @param  CRYP_IT: specifies the CRYP interrupt source to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg CRYP_IT_INI: Input FIFO interrupt
  *     @arg CRYP_IT_OUTI: Output FIFO interrupt
  * @param  NewState: new state of the specified CRYP interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CRYP_ITConfig(uint8_t CRYP_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_CRYP_CONFIG_IT(CRYP_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected CRYP interrupt */
    CRYP->IMSCR |= CRYP_IT;
  }
  else
  {
    /* Disable the selected CRYP interrupt */
    CRYP->IMSCR &= ~CRYP_IT;
  }
}

/**
  * @brief  Checks whether the specified CRYP interrupt has occurred or not.
  * @note   This function checks the status of the masked interrupt (i.e the 
  *         interrupt should be previously enabled).     
  * @param  CRYP_IT: specifies the CRYP (masked) interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg CRYP_IT_INI: Input FIFO interrupt
  *     @arg CRYP_IT_OUTI: Output FIFO interrupt
  * @retval The new state of CRYP_IT (SET or RESET).
  */
ITStatus CRYP_GetITStatus(uint8_t CRYP_IT)
{
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_CRYP_GET_IT(CRYP_IT));

  /* Check the status of the specified CRYP interrupt */
  if ((CRYP->MISR & CRYP_IT) != (uint8_t)RESET)
  {
    /* CRYP_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* CRYP_IT is reset */
    bitstatus = RESET;
  }
  /* Return the CRYP_IT status */
  return bitstatus;
}

/**
  * @brief  Checks whether the specified CRYP flag is set or not.
  * @param  CRYP_FLAG: specifies the CRYP flag to check.
  *   This parameter can be one of the following values:
  *     @arg CRYP_FLAG_IFEM: Input FIFO Empty flag.
  *     @arg CRYP_FLAG_IFNF: Input FIFO Not Full flag.
  *     @arg CRYP_FLAG_OFNE: Output FIFO Not Empty flag.
  *     @arg CRYP_FLAG_OFFU: Output FIFO Full flag.
  *     @arg CRYP_FLAG_BUSY: Busy flag.
  *     @arg CRYP_FLAG_OUTRIS: Output FIFO raw interrupt flag.
  *     @arg CRYP_FLAG_INRIS: Input FIFO raw interrupt flag.
  * @retval The new state of CRYP_FLAG (SET or RESET).
  */
FlagStatus CRYP_GetFlagStatus(uint8_t CRYP_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmp, tempreg = 0;

  /* Check the parameters */
  assert_param(IS_CRYP_GET_FLAG(CRYP_FLAG));

  /* Get the register index */
  tmp = (((uint8_t)CRYP_FLAG) >> 5);

  if (tmp == 0x00) /* The FLAG is in SR register */
  {
    tempreg = CRYP->SR;
  }
  else /* The FLAG is in RISR register */
  {
    tempreg = CRYP->RISR;
  }

  /* Get the flag position */
  tmp = CRYP_FLAG & FLAG_MASK;
 
  /* Check the status of the specified CRYP flag */
  if ((tempreg & tmp) != (uint8_t)RESET)
  {
    /* CRYP_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* CRYP_FLAG is reset */
    bitstatus = RESET;
  }

  /* Return the CRYP_FLAG status */
  return  bitstatus;
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
