/**
  ******************************************************************************
  * @file    stm32_audio_out_if.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   header file for the stm32_audio_out_if.c file.
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

#ifndef __USB_AUDIO_OUT_IF_H_
#define __USB_AUDIO_OUT_IF_H_

#include "stm32_audio.h"

#ifdef USE_STM322xG_EVAL 
 #include "stm322xg_audio_codec.h"
#elif defined USE_STM324xG_EVAL
 #include "stm324xg_audio_codec.h"
#elif defined(USE_STM3210C_EVAL)
 #include "stm3210c_audio_codec.h"
#elif defined(USE_STM32F4XX_DISCOVERY)
 #include "stm32f4_discovery_audio_codec.h"
#endif /* USE_STM322xG_EVAL */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup usbd_audio
  * @brief This file is the Header file for USBD_audio.c
  * @{
  */ 


/** @defgroup stm32_audio_Exported_Defines
  * @{
  */ 
/* Audio Commands enmueration */
typedef enum
{
  AUDIO_CMD_PLAY = 1,
  AUDIO_CMD_PAUSE,
  AUDIO_CMD_STOP,
}AUDIO_CMD_TypeDef;

/* Mute commands */
#define AUDIO_MUTE                      0x01
#define AUDIO_UNMUTE                    0x00

/* Functions return value */
#define AUDIO_OK                        0x00
#define AUDIO_FAIL                      0xFF

/* Audio Machine States */
#define AUDIO_STATE_INACTIVE            0x00
#define AUDIO_STATE_ACTIVE              0x01
#define AUDIO_STATE_PLAYING             0x02
#define AUDIO_STATE_PAUSED              0x03
#define AUDIO_STATE_STOPPED             0x04
#define AUDIO_STATE_ERROR               0x05

#define MAX_AUDIO_FREQ                  DEFAULT_OUT_AUDIO_FREQ
#define MIN_AUDIO_FREQ                  32000

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

#define XferCplt     Audio_MAL_IRQHandler

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern AUDIO_FOPS_TypeDef  AUDIO_OUT_fops;

/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif  /* __USB_AUDIO_OUT_IF_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
