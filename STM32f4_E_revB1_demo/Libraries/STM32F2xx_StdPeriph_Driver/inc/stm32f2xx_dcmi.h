/**
  ******************************************************************************
  * @file    stm32f2xx_dcmi.h
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file contains all the functions prototypes for the DCMI firmware library.
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
#ifndef __STM32F2xx_DCMI_H
#define __STM32F2xx_DCMI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DCMI
  * @{
  */ 

/** @defgroup DCMI_Exported_Types
  * @{
  */ 
/** 
  * @brief   DCMI Init structure definition  
  */ 
typedef struct
{
  uint16_t DCMI_CaptureMode;
  uint16_t DCMI_SynchroMode;
  uint16_t DCMI_PCKPolarity;
  uint16_t DCMI_VSPolarity;
  uint16_t DCMI_HSPolarity;
  uint16_t DCMI_CaptureRate; /* DCMI_CaptureRate is used only in Continuous mode*/
  uint16_t DCMI_ExtendedDataMode;
} DCMI_InitTypeDef;
/** 
  * @brief   DCMI CROP Init structure definition  
  */ 
typedef struct
{
  uint16_t DCMI_VerticalStartLine;
  uint16_t DCMI_HorizontalOffsetCount;
  uint16_t DCMI_VerticalLineCount;
  uint16_t DCMI_CaptureCount;
} DCMI_CROPInitTypeDef;
/** 
  * @brief   DCMI Embedded Synchronisation CODE Init structure definition  
  */ 
typedef struct
{
  uint8_t DCMI_FrameStartCode;
  uint8_t DCMI_LineStartCode;
  uint8_t DCMI_LineEndCode;
  uint8_t DCMI_FrameEndCode;
} DCMI_CodesInitTypeDef;
/**
  * @}
  */ 

/** @defgroup DCMI_Exported_Constants
  * @{
  */ 
/** @defgroup DCMI_Capture_Mode 
  * @{
  */ 
#define DCMI_CaptureMode_Continuous    ((uint16_t)0x0000)
#define DCMI_CaptureMode_SnapShot      ((uint16_t)0x0002)
#define IS_DCMI_CAPTURE_MODE(MODE)(((MODE) == DCMI_CaptureMode_Continuous) || \
                                   ((MODE) == DCMI_CaptureMode_SnapShot))
/**
  * @}
  */ 


/** @defgroup DCMI_Synchronisation_Mode_ 
  * @{
  */ 
#define DCMI_SynchroMode_Hardware    ((uint16_t)0x0000)
#define DCMI_SynchroMode_Embedded    ((uint16_t)0x0010)
#define IS_DCMI_SYNCHRO(MODE)(((MODE) == DCMI_SynchroMode_Hardware) || \
                              ((MODE) == DCMI_SynchroMode_Embedded))
/**
  * @}
  */ 


/** @defgroup DCMI_PIXCK_Polarity 
  * @{
  */ 
#define DCMI_PCKPolarity_Falling    ((uint16_t)0x0000)
#define DCMI_PCKPolarity_Rising     ((uint16_t)0x0020)
#define IS_DCMI_PCKPOLARITY(POLARITY)(((POLARITY) == DCMI_PCKPolarity_Falling) || \
                                      ((POLARITY) == DCMI_PCKPolarity_Rising))
/**
  * @}
  */ 


/** @defgroup DCMI_VSYNC_Polarity 
  * @{
  */ 
#define DCMI_VSPolarity_Low     ((uint16_t)0x0000)
#define DCMI_VSPolarity_High    ((uint16_t)0x0080)
#define IS_DCMI_VSPOLARITY(POLARITY)(((POLARITY) == DCMI_VSPolarity_Low) || \
                                    ((POLARITY) == DCMI_VSPolarity_High))
/**
  * @}
  */ 


/** @defgroup DCMI_HSYNC_Polarity 
  * @{
  */ 
#define DCMI_HSPolarity_Low     ((uint16_t)0x0000)
#define DCMI_HSPolarity_High    ((uint16_t)0x0040)
#define IS_DCMI_HSPOLARITY(POLARITY)(((POLARITY) == DCMI_HSPolarity_Low) || \
                                     ((POLARITY) == DCMI_HSPolarity_High))
/**
  * @}
  */ 


/** @defgroup DCMI_Capture_Rate 
  * @{
  */ 
#define DCMI_CaptureRate_All_Frame     ((uint16_t)0x0000)
#define DCMI_CaptureRate_1of2_Frame    ((uint16_t)0x0100)
#define DCMI_CaptureRate_1of4_Frame    ((uint16_t)0x0200)
#define IS_DCMI_CAPTURE_RATE(RATE) (((RATE) == DCMI_CaptureRate_All_Frame) || \
                                   ((RATE) == DCMI_CaptureRate_1of2_Frame) ||\
                                   ((RATE) == DCMI_CaptureRate_1of4_Frame))
/**
  * @}
  */ 


/** @defgroup DCMI_Extended_Data_Mode 
  * @{
  */ 
#define DCMI_ExtendedDataMode_8b     ((uint16_t)0x0000)
#define DCMI_ExtendedDataMode_10b    ((uint16_t)0x0400)
#define DCMI_ExtendedDataMode_12b    ((uint16_t)0x0800)
#define DCMI_ExtendedDataMode_14b    ((uint16_t)0x0C00)
#define IS_DCMI_EXTENDED_DATA(DATA)(((DATA) == DCMI_ExtendedDataMode_8b) || \
                                    ((DATA) == DCMI_ExtendedDataMode_10b) ||\
                                    ((DATA) == DCMI_ExtendedDataMode_12b) ||\
                                    ((DATA) == DCMI_ExtendedDataMode_14b))
/**
  * @}
  */ 


/** @defgroup DCMI_interrupt_sources 
  * @{
  */ 
#define DCMI_IT_FRAME    ((uint16_t)0x0001)
#define DCMI_IT_OVF      ((uint16_t)0x0002)
#define DCMI_IT_ERR      ((uint16_t)0x0004)
#define DCMI_IT_VSYNC    ((uint16_t)0x0008)
#define DCMI_IT_LINE     ((uint16_t)0x0010)
#define IS_DCMI_CONFIG_IT(IT) ((((IT) & (uint16_t)0xFFE0) == 0x0000) && ((IT) != 0x0000))
#define IS_DCMI_GET_IT(IT) (((IT) == DCMI_IT_FRAME) || \
                            ((IT) == DCMI_IT_OVF) || \
                            ((IT) == DCMI_IT_ERR) || \
                            ((IT) == DCMI_IT_VSYNC) || \
                            ((IT) == DCMI_IT_LINE))
/**
  * @}
  */ 


/** @defgroup DCMI_Flags 
  * @{
  */ 
/** 
  * @brief   DCMI SR register  
  */ 
#define DCMI_FLAG_HSYNC     ((uint16_t)0x2001)
#define DCMI_FLAG_VSYNC     ((uint16_t)0x2002)
#define DCMI_FLAG_FNE       ((uint16_t)0x2004)
/** 
  * @brief   DCMI RISR register  
  */ 
#define DCMI_FLAG_FRAMERI    ((uint16_t)0x0001)
#define DCMI_FLAG_OVFRI      ((uint16_t)0x0002)
#define DCMI_FLAG_ERRRI      ((uint16_t)0x0004)
#define DCMI_FLAG_VSYNCRI    ((uint16_t)0x0008)
#define DCMI_FLAG_LINERI     ((uint16_t)0x0010)
/** 
  * @brief   DCMI MISR register  
  */ 
#define DCMI_FLAG_FRAMEMI    ((uint16_t)0x1001)
#define DCMI_FLAG_OVFMI      ((uint16_t)0x1002)
#define DCMI_FLAG_ERRMI      ((uint16_t)0x1004)
#define DCMI_FLAG_VSYNCMI    ((uint16_t)0x1008)
#define DCMI_FLAG_LINEMI     ((uint16_t)0x1010)
#define IS_DCMI_GET_FLAG(FLAG) (((FLAG) == DCMI_FLAG_HSYNC) || \
                                ((FLAG) == DCMI_FLAG_VSYNC) || \
                                ((FLAG) == DCMI_FLAG_FNE) || \
                                ((FLAG) == DCMI_FLAG_FRAMERI) || \
                                ((FLAG) == DCMI_FLAG_OVFRI) || \
                                ((FLAG) == DCMI_FLAG_ERRRI) || \
                                ((FLAG) == DCMI_FLAG_VSYNCRI) || \
                                ((FLAG) == DCMI_FLAG_LINERI) || \
                                ((FLAG) == DCMI_FLAG_FRAMEMI) || \
                                ((FLAG) == DCMI_FLAG_OVFMI) || \
                                ((FLAG) == DCMI_FLAG_ERRMI) || \
                                ((FLAG) == DCMI_FLAG_VSYNCMI) || \
                                ((FLAG) == DCMI_FLAG_LINEMI))
                                
#define IS_DCMI_CLEAR_FLAG(FLAG) ((((FLAG) & (uint16_t)0xFFE0) == 0x0000) && ((FLAG) != 0x0000))
/**
  * @}
  */ 


/**
  * @}
  */ 



/** @defgroup DCMI_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup DCMI_Exported_Functions
  * @{
  */ 
void DCMI_DeInit(void);
void DCMI_Init(DCMI_InitTypeDef* DCMI_InitStruct);
void DCMI_StructInit(DCMI_InitTypeDef* DCMI_InitStruct);
void DCMI_ITConfig(uint16_t DCMI_IT, FunctionalState NewState);
void DCMI_CROPConfig(DCMI_CROPInitTypeDef* DCMI_CROPInitStruct);
void DCMI_SetEmbeddedSynchroCodes(DCMI_CodesInitTypeDef* DCMI_CodesInitStruct);
void DCMI_Cmd(FunctionalState NewState);
void DCMI_CaptureCmd(FunctionalState NewState);
void DCMI_CROPCmd(FunctionalState NewState);
void DCMI_JPEGCmd(FunctionalState NewState);
uint32_t DCMI_ReadData(void);
FlagStatus DCMI_GetFlagStatus(uint16_t DCMI_FLAG);
void DCMI_ClearFlag(uint16_t DCMI_FLAG);
ITStatus DCMI_GetITStatus(uint16_t DCMI_IT);
void DCMI_ClearITPendingBit(uint16_t DCMI_IT);
#endif /*__STM32F2xx_DCMI_H */
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
