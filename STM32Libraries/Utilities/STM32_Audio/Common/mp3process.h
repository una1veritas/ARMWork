/**
  ******************************************************************************
  * @file    mp3process.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes MP3 processing layer headers
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
#ifndef __MP3PROCESS_H_
#define __MP3PROCESS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_audio.h"
   
#include "spiritMP3Dec.h"
#include <stdlib.h>
#include "songutilities.h"
#include "global_includes.h"
 
   
/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup MP3PROCESS
  * @brief This file is the Header file for mp3process.c
  * @{
  */
  
/** @defgroup MP3PROCESS_Exported_Defines
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */ 

/** @defgroup AUDIO_SYNCHPROCESS_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 
  
/** @defgroup AUDIO_SYNCHPROCESS_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup AUDIO_SYNCHPROCESS_Exported_Functions
  * @{
  */    
TSpiritMP3Info * MP3Process_GetFileInfoStruct(void);

#ifdef __MP3_DECODER__
uint32_t Mp3Process_DecoderInit(uint8_t* pHeader, 
                                fnReadCallback_TypeDef* pReadCallback, 
                                fnSetPositionCallback_TypeDef* pSetPosCallback);
uint32_t Mp3Process_DecoderDeInit(void);
uint32_t Mp3Process_DecodeData(__IO int16_t* OutPutBuffer,uint32_t NbSamples,void * Usr);
uint32_t Mp3Process_RetrieveIDTAGS(TAGS_TypeDef * pIDTAG, FIL* file);
uint32_t Mp3Process_GetStreamLenght(uint32_t fLength);
uint32_t Mp3Process_GetElapsedTime(uint32_t currpos);
uint32_t Mp3Process_GetSamplingRate(void);
uint32_t Mp3Process_StreamBackward(void);
uint32_t Mp3Process_StreamForward(void);
uint32_t Mp3Process_StopMode(void);
uint32_t Mp3Process_VBRDetected(void);
#endif /* __MP3_DECODER__ */

#ifdef __MP3_ENCODER__
uint32_t Mp3Process_EncoderInit            (uint32_t Freq, uint8_t* Param);
uint32_t Mp3Process_EncoderDeInit          (void);
uint32_t Mp3Process_EncoderStopMode        (void);
uint32_t Mp3Process_EncodeData             (int8_t *pBufIn, int8_t *pBufOut, uint32_t* nEnc, void* pOpt);
uint32_t Mp3Process_SetTags                (TAGS_TypeDef* IdTags, void *file);
uint32_t Mp3Process_EncGetSampleRate       (void);
uint32_t Mp3Process_EncGetStreamLength     (void);
uint32_t Mp3Process_EncGetElapsedTime      (void);
#endif /* __MP3_ENCODER__ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 
#endif /* __MP3PROCESS_H_ */

#ifdef __cplusplus
}
#endif

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
