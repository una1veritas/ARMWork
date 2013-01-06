/**
  ******************************************************************************
  * @file    stm32_audio_in_if.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   header file for the stm32_audio_in_if.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#ifndef __STM32_AUDIO_IN_IF_H_
#define __STM32_AUDIO_IN_IF_H_


/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup AUDIO_IN_IF
  * @brief This file is the Header file for stm32_audio_in_if.c
  * @{
  */ 


/** @defgroup AUDIO_IN_IF_Exported_Defines
  * @{
  */ 
/* Audio Commands enmueration */
typedef enum
{
  AUDIO_IN_CMD_START = 1,
  AUDIO_IN_CMD_STOP,
}AUDIO_IN_CMD_TypeDef;

/* Mute commands */
#define AUDIO_IN_MUTE                   0x01
#define AUDIO_IN_UNMUTE                 0x00

/* Functions return value */
#define AUDIO_OK                        0x00
#define AUDIO_FAIL                      0xFF

/* Audio Machine States */
#define AUDIO_IN_STATE_INACTIVE         0x00
#define AUDIO_IN_STATE_ACTIVE           0x01
#define AUDIO_IN_STATE_RECORDING        0x02
#define AUDIO_IN_STATE_STOPPED          0x03
#define AUDIO_IN_STATE_ERROR            0x04

#define MAX_AUDIO_IN_FREQ               DEFAULT_IN_AUDIO_FREQ
#define MIN_AUDIO_IN_FREQ               8000

/**
  * @}
  */ 


/** @defgroup AUDIO_IN_IF_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup AUDIO_IN_IF_Exported_Macros
  * @{
  */ 
#define stm32_audio_in_XferCplt       STM32_AudioRec_Update

/**
  * @}
  */ 

/** @defgroup AUDIO_IN_IF_Exported_Variables
  * @{
  */ 

extern AUDIO_FOPS_TypeDef  AUDIO_IN_fops;

/**
  * @}
  */ 

/** @defgroup AUDIO_IN_IF_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif  /* __STM32_AUDIO_IN_IF_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
