/**
  ******************************************************************************
  * @file    stm32f2xx_rng.h
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file contains all the functions prototypes for the Random Number
  *          Generator(RNG) firmware library.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_RNG_H
#define __STM32F2xx_RNG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @addtogroup RNG
  * @{
  */ 



/** @defgroup RNG_Exported_Types
  * @{
  */ 

/** @defgroup HASH_Exported_Constants
  * @{
  */ 
  
/** 
  * @brief   RNG flags definition  
  */ 
#define RNG_FLAG_DRDY               ((uint8_t)0x0001)
#define RNG_FLAG_CECS               ((uint8_t)0x0002)
#define RNG_FLAG_SECS               ((uint8_t)0x0004)
#define IS_RNG_GET_FLAG(RNG_FLAG) (((RNG_FLAG) == RNG_FLAG_DRDY) || \
                                   ((RNG_FLAG) == RNG_FLAG_CECS) || \
                                   ((RNG_FLAG) == RNG_FLAG_SECS))
/** 
  * @brief   RNG interrupts definition  
  */ 
#define RNG_IT_CEI                  ((uint8_t)0x20)
#define RNG_IT_SEI                  ((uint8_t)0x40)
#define IS_RNG_IT(IT) ((((IT) & (uint8_t)0x9F) == 0x00) && ((IT) != 0x00))
#define IS_RNG_GET_IT(RNG_IT) (((RNG_IT) == RNG_IT_CEI) || ((RNG_IT) == RNG_IT_SEI))
/**
  * @}
  */ 


/**
  * @}
  */ 



/** @defgroup RNG_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup RNG_Exported_Functions
  * @{
  */ 
void RNG_DeInit(void);
void RNG_Cmd(FunctionalState NewState);
void RNG_ITConfig(FunctionalState NewState);
uint32_t RNG_GetRandomNumber(void);
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG);
ITStatus RNG_GetITStatus(uint8_t RNG_IT);
void RNG_ClearITPendingBit(uint8_t RNG_IT);
#endif /*__STM32F2xx_RNG_H */
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
