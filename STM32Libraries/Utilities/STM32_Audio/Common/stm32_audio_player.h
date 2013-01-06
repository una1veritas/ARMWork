/**
  ******************************************************************************
  * @file    stm32_audio_player.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   Header file of the stm32_audio_player module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __STM32_AUDIO_PLAYER_H__
#define  __STM32_AUDIO_PLAYER_H__


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "audio_app_conf.h"
#include "stm32_audio.h"
#include "stm32_audio_out_if.h"
   
#include "songutilities.h"
#include "stm32_audio_decoders.h"

#include "ff.h" /* File System */
   
/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup STM32_AUDIO_PLAYER
  * @brief This file is the header file for the stm32 audio player module 
  * @{
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Exported_Defines
  * @{
  */   
   
#define AUDIO_FWD_RWD_STEP                        5
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_TypesDefinitions
  * @{
  */ 
typedef enum
{
  PLAYER_IDLE = 0,
  PLAYER_PLAYING,
  PLAYER_PAUSED,
  PLAYER_ERROR,
}AudioPlayerState_TypeDef;

/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_FunctionsPrototype
  * @{
  */ 
uint32_t AudioPlayer_Init(void);
uint32_t AudioPlayer_DeInit(void);
uint32_t AudioPlayer_Open(uint8_t *FilePath);
uint32_t AudioPlayer_Close(void);
uint32_t AudioPlayer_Play(uint8_t *FilePath);
uint32_t AudioPlayer_Pause(void);
uint32_t AudioPlayer_Stop(void);
uint32_t AudioPlayer_Forward(void);
uint32_t AudioPlayer_Rewind(void);
uint32_t AudioPlayer_VolumeCtrl(uint8_t Volume);

TAGS_TypeDef* AudioPlayer_GetFileInfo(void);
uint32_t AudioPlayer_GetTimeInfo(uint32_t* Length, uint32_t* Elapsed);
uint32_t AudioPlayer_SetPosition(uint32_t pos);
uint32_t AudioPlayer_GetFileLength(void);
AudioPlayerState_TypeDef AudioPlayer_GetState(void);

void AudioPlayer_TaskInit();
void AudioPlayer_TaskInit();
void AudioPlayer_TaskDeInit();
void EXTI4_IRQHandler(void);

unsigned int Dec_ReadDataCallback(
                      void * pCompressedData,        /* [OUT] Bitbuffer */
                      unsigned int nDataSizeInChars, /* sizeof(Bitbuffer) */
                      void * pUserData               /* Application-supplied parameter */
                      );

void AudioPlayer_Task(void * pvParameters);
/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif /* __STM32_AUDIO_PLAYER_H__ */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
