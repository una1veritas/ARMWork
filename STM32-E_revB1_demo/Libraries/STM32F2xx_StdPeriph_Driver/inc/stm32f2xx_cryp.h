/**
  ******************************************************************************
  * @file    stm32f2xx_cryp.h
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file contains all the functions prototypes for the Cryptographic
  *          processor(CRYP) firmware library.
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
#ifndef __STM32F2xx_CRYP_H
#define __STM32F2xx_CRYP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @addtogroup CRYP
  * @{
  */ 


/** @defgroup CRYP_Exported_Types
  * @{
  */ 

/** 
  * @brief   CRYP Init structure definition  
  */ 
typedef struct
{
  uint16_t CRYP_AlgoDir;   /* Encrypt or Decrypt */
  uint16_t CRYP_AlgoMode;  /* TDES-ECB, TDES-CBC, DES-ECB, DES-CBC, AES-ECB, AES-CBC, AES-CTR, AES-Key */
  uint16_t CRYP_DataType;  /* 32-bit data, 16-bit data, bit data or bit-string*/ 
  uint16_t CRYP_KeySize;   /* Used only in AES mode only : 128, 192 or 256 bit key length.*/
}CRYP_InitTypeDef;

/** 
  * @brief   CRYP Key(s) structure definition  
  */ 
typedef struct
{
  uint32_t CRYP_Key0Left;  /* Key 0 Left */
  uint32_t CRYP_Key0Right; /* Key 0 Right */
  uint32_t CRYP_Key1Left;  /* Key 1 left */
  uint32_t CRYP_Key1Right; /* Key 1 Right */
  uint32_t CRYP_Key2Left;  /* Key 2 left */
  uint32_t CRYP_Key2Right; /* Key 2 Right */
  uint32_t CRYP_Key3Left;  /* Key 3 left */
  uint32_t CRYP_Key3Right; /* Key 3 Right */
}CRYP_KeyInitTypeDef;
/** 
  * @brief   CRYP Initialization Vectors (IV) structure definition  
  */ 
typedef struct
{
  uint32_t CRYP_IV0Left;  /* Init Vector 0 Left */
  uint32_t CRYP_IV0Right; /* Init Vector 0 Right */
  uint32_t CRYP_IV1Left;  /* Init Vector 1 left */
  uint32_t CRYP_IV1Right; /* Init Vector 1 Right */
}CRYP_IVInitTypeDef;
/**
  * @}
  */ 



/** @defgroup CRYP_Exported_Constants
  * @{
  */ 
/** 
  * @brief   CRYP_Algorithm Direction  
  */ 
#define CRYP_AlgoDir_Encrypt      ((uint16_t)0x0000)
#define CRYP_AlgoDir_Decrypt      ((uint16_t)0x0004)
#define IS_CRYP_ALGODIR(ALGODIR) (((ALGODIR) == CRYP_AlgoDir_Encrypt) || \
                                  ((ALGODIR) == CRYP_AlgoDir_Decrypt))
                                     
/** 
  * @brief   CRYP_Algorithm Mode  
  */ 

/* TDES Modes */
#define CRYP_AlgoMode_TDES_ECB    ((uint16_t)0x0000)
#define CRYP_AlgoMode_TDES_CBC    ((uint16_t)0x0008)

/* DES Modes */
#define CRYP_AlgoMode_DES_ECB     ((uint16_t)0x0010)
#define CRYP_AlgoMode_DES_CBC     ((uint16_t)0x0018)

/* AES Modes */
#define CRYP_AlgoMode_AES_ECB     ((uint16_t)0x0020)
#define CRYP_AlgoMode_AES_CBC     ((uint16_t)0x0028)
#define CRYP_AlgoMode_AES_CTR     ((uint16_t)0x0030)
#define CRYP_AlgoMode_AES_Key     ((uint16_t)0x0038)

#define IS_CRYP_ALGOMODE(ALGOMODE) (((ALGOMODE) == CRYP_AlgoMode_TDES_ECB)||((ALGOMODE) == CRYP_AlgoMode_TDES_CBC)|| \
                                    ((ALGOMODE) == CRYP_AlgoMode_DES_ECB) ||((ALGOMODE) == CRYP_AlgoMode_DES_CBC)|| \
                                    ((ALGOMODE) == CRYP_AlgoMode_AES_ECB) ||((ALGOMODE) == CRYP_AlgoMode_AES_CBC)|| \
                                    ((ALGOMODE) == CRYP_AlgoMode_AES_CTR) ||((ALGOMODE) == CRYP_AlgoMode_AES_Key)) 
/** 
  * @brief   CRYP Data Type  
  */ 
#define CRYP_DataType_32b         ((uint16_t)0x0000)
#define CRYP_DataType_16b         ((uint16_t)0x0040)
#define CRYP_DataType_8b          ((uint16_t)0x0080)
#define CRYP_DataType_1b          ((uint16_t)0x00C0)
#define IS_CRYP_DATATYPE(DATATYPE) (((DATATYPE) == CRYP_DataType_32b) || \
                                    ((DATATYPE) == CRYP_DataType_16b)|| \
                                    ((DATATYPE) == CRYP_DataType_8b)|| \
                                    ((DATATYPE) == CRYP_DataType_1b))  
                                     
/** 
  * @brief   CRYP Key Size  (for AES only)
  */ 
#define CRYP_KeySize_128b         ((uint16_t)0x0000)
#define CRYP_KeySize_192b         ((uint16_t)0x0100)
#define CRYP_KeySize_256b         ((uint16_t)0x0200)
#define IS_CRYP_KEYSIZE(KEYSIZE) (((KEYSIZE) == CRYP_KeySize_128b)|| \
                                  ((KEYSIZE) == CRYP_KeySize_192b)|| \
                                  ((KEYSIZE) == CRYP_KeySize_256b))
/** 
  * @brief   CRYP flags definition  
  */ 
#define CRYP_FLAG_IFEM            ((uint8_t)0x01)  /* Input Fifo Empty */
#define CRYP_FLAG_IFNF            ((uint8_t)0x02)
#define CRYP_FLAG_OFNE            ((uint8_t)0x04)  /* Output Fifo Not Empty */
#define CRYP_FLAG_OFFU            ((uint8_t)0x08)
#define CRYP_FLAG_BUSY            ((uint8_t)0x10)
#define CRYP_FLAG_OUTRIS          ((uint8_t)0x21)
#define CRYP_FLAG_INRIS           ((uint8_t)0x22)
#define IS_CRYP_GET_FLAG(FLAG) (((FLAG) == CRYP_FLAG_IFEM) || ((FLAG) == CRYP_FLAG_IFNF)|| \
                                ((FLAG) == CRYP_FLAG_OFNE) || ((FLAG) == CRYP_FLAG_OFFU)|| \
                                ((FLAG) == CRYP_FLAG_BUSY) || ((FLAG) == CRYP_FLAG_OUTRIS)|| \
                                ((FLAG) == CRYP_FLAG_INRIS))
/** 
  * @brief   CRYP interrupts definition  
  */ 
#define CRYP_IT_INI               ((uint8_t)0x01)
#define CRYP_IT_OUTI              ((uint8_t)0x02)
#define IS_CRYP_CONFIG_IT(IT) ((((IT) & (uint8_t)0xFC) == 0x00) && ((IT) != 0x00))
#define IS_CRYP_GET_IT(IT) (((IT) == CRYP_IT_INI) || ((IT) == CRYP_IT_OUTI))


/** 
  * @brief   CRYP DMA transfer requests  
  */ 
#define CRYP_DMAReq_Rx            ((uint8_t)0x01)
#define CRYP_DMAReq_Tx            ((uint8_t)0x02)
#define IS_CRYP_DMAREQ(DMAREQ) ((((DMAREQ) & (uint8_t)0xFC) == 0x00) && ((DMAREQ) != 0x00))
/**
  * @}
  */ 


/**
  * @}
  */ 

/** @defgroup CRYP_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup CRYP_Exported_Functions
  * @{
  */ 

/* CRYP initialization and configuration functions */
void CRYP_DeInit(void);
void CRYP_Init(CRYP_InitTypeDef* CRYP_InitStruct);
void CRYP_KeyInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct);
void CRYP_IVInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct);

/* Structures initialization functions */
void CRYP_StructInit(CRYP_InitTypeDef* CRYP_InitStruct);
void CRYP_KeyStructInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct);
void CRYP_IVStructInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct);

/* CRYP enable/disable function */
void CRYP_Cmd(FunctionalState NewState);

/* CRYP's DMA interface enable/disable function */
void CRYP_DMACmd(uint8_t CRYP_DMAReq, FunctionalState NewState);

/* Data writing/reading functions */
void CRYP_DataIn(uint32_t Data);
uint32_t CRYP_DataOut(void);

/* IN/OUT FIFOs flush function */
void CRYP_FIFOFlush(void);

/* Flags & Interrupts functions */
void CRYP_ITConfig(uint8_t CRYP_IT, FunctionalState NewState);
ITStatus CRYP_GetITStatus(uint8_t CRYP_IT);
FlagStatus CRYP_GetFlagStatus(uint8_t CRYP_FLAG);

#endif /*__STM32F2xx_CRYP_H */
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
