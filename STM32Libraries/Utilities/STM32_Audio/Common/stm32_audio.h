/**
  ******************************************************************************
  * @file    stm32_audio.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   Common header file for all audio processing.
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

#ifndef __STM32_AUDIO_H_
#define __STM32_AUDIO_H_

/* Includes ------------------------------------------------------------------*/
#ifdef STM32F2XX
 #include "stm32f2xx.h"
#elif defined(STM32F4XX)
 #include "stm32f4xx.h"
#elif defined(STM32F10X_CL)
 #include "stm32f10x.h"
#endif /* STM32F2XX */

/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup STM32_AUDIO
  * @brief This file is the Header file for audio processor
  * @{
  */ 


/** @defgroup STM32_AUDIO_Exported_Defines
  * @{
  */ 

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define DEFAULT_OUT_BIT_RESOLUTION                    16
#define DEFAULT_OUT_CHANNEL_NBR                       2 /* Mono = 1, Stereo = 2 */ 

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define DEFAULT_IN_BIT_RESOLUTION                     16
#define DEFAULT_IN_CHANNEL_NBR                        1 /* Mono = 1, Stereo = 2 */

#define DIRECTION_OUT                                 0
#define DIRECTION_IN                                  1

#ifndef NULL
 #define NULL                                         0
#endif

/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_Exported_TypesDefinitions
  * @{
  */
typedef uint32_t (fnReadCallback_TypeDef)(
            void *       pCompressedData,             /* [OUT] Pointer to buffer to fill with coded MP3 data */
            uint32_t     nDataSizeInBytes,            /* Buffer size in Bytes */
            void *       pUser                        /* User pointer: can be used to pass additional parameters */
    );

typedef void (fnXFerCpltCallback_TypeDef)( uint8_t Direction, 
                                           uint8_t** pbuf, 
                                           uint32_t* pSize);

typedef uint32_t (fnSetPositionCallback_TypeDef)( uint32_t Pos);

typedef struct _Audio_Fops
{
    uint8_t   (*Init)         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
    uint8_t   (*DeInit)       (uint32_t options);
    uint8_t   (*AudioCmd)     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
    uint8_t   (*VolumeCtl)    (uint8_t vol);
    uint8_t   (*MuteCtl)      (uint8_t cmd);
    uint32_t  (*OptionCtrl)   (uint8_t* cmd, uint32_t size);
    uint8_t   (*GetState)     (void);
    void      (*SetXferCpltCallback)  (fnXFerCpltCallback_TypeDef* Clbck);
    __IO uint32_t* pOptions;
}AUDIO_FOPS_TypeDef;




/**
  * @}
  */ 



/** @defgroup STM32_AUDIO_Exported_Macros
  * @{
  */ 
#define BYTE_0(val)                    (uint8_t)(val & 0xFF)
#define BYTE_1(val)                    (uint8_t)((val & 0xFF00) >> 8)
#define BYTE_2(val)                    (uint8_t)((val & 0xFF0000) >> 16)
#define BYTE_3(val)                    (uint8_t)((val & 0xFF000000) >> 24)

/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_Exported_Functions
  * @{
  */

/* This function should be called by low layer functions when current buffer transfer is complete */
uint8_t  stm32_audio_BuffXferCplt (uint8_t Direction, uint8_t** pbuf, uint32_t* pSize);

/**
  * @}
  */ 

#endif  /* __STM32_AUDIO_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
