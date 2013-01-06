/**
  ******************************************************************************
  * @file    WMAProcess.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes wma processing layer headers
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
#ifndef __WMAPROCESS_H_
#define __WMAPROCESS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_audio.h"
   
#include "spiritWMADec.h"
#include "SpiritASFReader.h"
#include "asf.h"
#include <stdlib.h>
#include "songutilities.h"
#include "global_includes.h"
   
/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @addtogroup Wmaprocess
  * @{
  */
   
/** @defgroup WmaProcess_Private_Defines
  * @{
  */
#define SEEKSTEP    0x05
/**
  * @}
  */


/** @defgroup WmaProcess_Exported_Functions
  * @{
  */   
uint32_t WmaProcess_DecoderInit(uint8_t* pHeader, 
                                fnReadCallback_TypeDef* pReadCallback, 
                                fnSetPositionCallback_TypeDef* pSetPosCallback);
uint32_t WmaProcess_DecoderDeInit(void);
uint32_t WmaProcess_DecodeData(__IO int16_t* OutPutBuffer, uint32_t _none, void * __none);
uint32_t WmaProcess_Forward(void);
uint32_t WmaProcess_Backward(void);
uint32_t WmaProcess_GetStreamLenght(uint32_t fLength);
uint32_t WmaProcess_GetElapsedTime(uint32_t CurrPos);
uint32_t WmaProcess_StopMode(void);
uint32_t WmaProcess_GetSamplingRate(void);
uint32_t WmaProcess_RetrieveIDTAGS(TAGS_TypeDef *, FIL* File);

#ifdef __cplusplus
}
#endif

#endif /* __WMAPROCESS_H_ */
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
