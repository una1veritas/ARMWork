/**
  ******************************************************************************
  * @file    stm32f2xx_hash.h
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file contains all the functions prototypes for the HASH firmware library.
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
#ifndef __STM32F2xx_HASH_H
#define __STM32F2xx_HASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @addtogroup HASH
  * @{
  */ 


/** @defgroup HASH_Exported_Types
  * @{
  */ 

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @addtogroup HASH
  * @{
  */ 


/** @defgroup HASH_Exported_Types
  * @{
  */ 
/** 
  * @brief   HASH Init structure definition
  */ 
typedef struct
{
  uint32_t HASH_AlgoSelection; /* SHA-1 or MD5 */
  uint32_t HASH_AlgoMode;      /* HASH or HMAC */
  uint32_t HASH_DataType;      /* 32-bit data, 16-bit data, 8-bit data or bit-string */
  uint32_t HASH_KeyType;       /* Short key or Long Key */
}HASH_InitTypeDef;

/** 
  * @brief  HASH message digest result structure definition  
  */ 
typedef struct
{
  uint32_t Data[5];
} HASH_MsgDigest; 

/**
  * @}
  */ 



/** @defgroup HASH_Exported_Constants
  * @{
  */ 
/** 
  * @brief   HASH Algo Selection  
  */ 
#define HASH_AlgoSelection_SHA1    ((uint16_t)0x0000) 
#define HASH_AlgoSelection_MD5     ((uint16_t)0x0080)

#define IS_HASH_ALGOSELECTION(ALGOSELECTION) (((ALGOSELECTION) == HASH_AlgoSelection_SHA1) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_MD5))
/** 
  * @brief   HASH Algorithm Mode  
  */ 
#define HASH_AlgoMode_HASH         ((uint16_t)0x0000) 
#define HASH_AlgoMode_HMAC         ((uint16_t)0x0040)

#define IS_HASH_ALGOMODE(ALGOMODE) (((ALGOMODE) == HASH_AlgoMode_HASH) || \
                                    ((ALGOMODE) == HASH_AlgoMode_HMAC))

/** 
  * @brief   HASH Data Type  
  */ 
#define HASH_DataType_32b          ((uint16_t)0x0000)
#define HASH_DataType_16b          ((uint16_t)0x0010)
#define HASH_DataType_8b           ((uint16_t)0x0020)
#define HASH_DataType_1b           ((uint16_t)0x0030)

#define IS_HASH_DATATYPE(DATATYPE) (((DATATYPE) == HASH_DataType_32b)|| \
                                    ((DATATYPE) == HASH_DataType_16b)|| \
                                    ((DATATYPE) == HASH_DataType_8b)|| \
                                    ((DATATYPE) == HASH_DataType_1b))

/** 
  * @brief   HMAC Long key   (only for HMAC mode)
  */ 
#define HASH_KeyType_ShortKey      ((uint32_t)0x00000000)
#define HASH_KeyType_LongKey       ((uint32_t)0x00010000)

#define IS_HASH_KEYTYPE(KEYTYPE) (((KEYTYPE) == HASH_KeyType_ShortKey) || \
                                  ((KEYTYPE) == HASH_KeyType_LongKey))
/** 
  * @brief * Number of valid bits in last word of the message  
  */ 
#define IS_HASH_VALIDBITSNUMBER(VALIDBITS) ((VALIDBITS) <= 0x1F)
/** 
  * @brief   HASH interrupts definition  
  */ 
#define HASH_IT_DINI               ((uint8_t)0x01)
#define HASH_IT_DCI                ((uint8_t)0x02)
#define IS_HASH_IT(IT) ((((IT) & (uint8_t)0xFC) == 0x00) && ((IT) != 0x00))
#define IS_HASH_GET_IT(IT) (((IT) == HASH_IT_DINI) || ((IT) == HASH_IT_DCI))
				   
/** 
  * @brief   HASH flags definition  
  */ 
#define HASH_FLAG_DINIS            ((uint16_t)0x0001)
#define HASH_FLAG_DCIS             ((uint16_t)0x0002)
#define HASH_FLAG_DMAS             ((uint16_t)0x0004)
#define HASH_FLAG_BUSY             ((uint16_t)0x0008)
#define HASH_FLAG_DINNE            ((uint16_t)0x1000)
#define IS_HASH_GET_FLAG(FLAG) (((FLAG) == HASH_FLAG_DINIS) || ((FLAG) == HASH_FLAG_DCIS)|| \
                                ((FLAG) == HASH_FLAG_DMAS) || ((FLAG) == HASH_FLAG_BUSY)|| \
                                ((FLAG) == HASH_FLAG_DINNE)) 

/**
  * @}
  */ 



/** @defgroup HASH_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup HASH_Exported_Functions
  * @{
  */ 
void HASH_DeInit(void);
void HASH_Init(HASH_InitTypeDef* HASH_InitStruct);
void HASH_StructInit(HASH_InitTypeDef* HASH_InitStruct);
void HASH_Start(void);
void HASH_Reset(void);
void HASH_NbValidBits(uint16_t HASH_ValidNumber);
void HASH_DMACmd(FunctionalState NewState);
void HASH_DataIn(uint32_t Data);
void HASH_DataOut(HASH_MsgDigest* HASH_MessageDigest);
void HASH_ITConfig(uint8_t HASH_IT, FunctionalState NewState);
ITStatus HASH_GetITStatus(uint8_t HASH_IT);
void HASH_ClearITPendingBit(uint8_t HASH_IT);
FlagStatus HASH_GetFlagStatus(uint16_t HASH_FLAG);
uint8_t HASH_GetFIFOWordsNumber(void);

#endif /*__STM32F2xx_HASH_H */
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
