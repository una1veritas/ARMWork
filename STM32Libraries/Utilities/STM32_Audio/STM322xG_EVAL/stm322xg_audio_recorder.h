/**
  *******************************************************************************
  * @file    stm322xg_audio_recorder.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for the wave recorder
  *          firmware driver stm322xg_audio_recorder.c.
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
#ifndef __STM322XG_AUDIO_RECORDER_H
#define __STM322XG_AUDIO_RECORDER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm322xg_eval.h"

#include "audio_app_conf.h"

/** @addtogroup Audio_Utilities
  * @{
  */

/** @addtogroup AUDIO_RECORDER
  * @{
  */

/** @defgroup AUDIO_RECORDER_Exported_Types
  * @{
  */

/**
  * @}
  */



/** @defgroup AUDIO_RECORDER_Exported_Constants
  * @{
  */

/* ----------------------------------------------------------------------------

   All the commented configuration parameters are gathered into audio_app_conf.h 
   file. If you already set them in audio_app_conf.h file, there is no need 
   to reconfigure them here.

-----------------------------------------------------------------------------*/
/* Set the preemption and subpriority values for the audio recorder timer interrupt */
/* #define AUDIO_REC_TIM_PRE_PRIO            1 */
/* #define AUDIO_REC_TIM_SUB_PRIO            0 */

/* Uncomment this define to enable single data processing (just after aquiring from ADC) */
#define AUDIO_REC_USE_DATA_PROC_SINGLE
/* Uncomment this define to enable buffer data processing (just after a full buffer has 
   been filled in by ADC) */
//#define AUDIO_REC_USE_DATA_PROC_BUFFER

#define ADC_TIMEOUT_MAX                   1000 /* ~83µs @120MHz */

#define AUDIO_REC_TIM                     TIM3
#define AUDIO_REC_TIM_IRQn                TIM3_IRQn
#define AUDIO_REC_ADC                     ADC3
#define AUDIO_REC_ADC_CLK                 RCC_APB2Periph_ADC3
#define AUDIO_REC_ADC_CH                  ADC_Channel_8
#define AUDIO_REC_ADC_PORT_CLK            RCC_AHB1Periph_GPIOF
#define AUDIO_REC_ADC_PORT                GPIOF
#define AUDIO_REC_ADC_PIN                 GPIO_Pin_10

#define AUDIO_REC_TIM_IRQHANDLER          TIM3_IRQHandler

/* Audio recorder clock update direction */
#define AUDIO_REC_DIRECTION_DOWN        0
#define AUDIO_REC_DIRECTION_UP          1

//#define _MAX_SS                255

#define REC_FREQ               8000 /* Audio revording frequency in Hz */

#define  SAMPLE_RATE_8000                    8000
#define  SAMPLE_RATE_11025                   11025
#define  SAMPLE_RATE_22050                   22050
#define  SAMPLE_RATE_44100                   44100
#define  BITS_PER_SAMPLE_8                   8
#define  WAVE_DUMMY_BYTE                     0xA5   
#define  DAC_DHR8R1_Address                  0x40007410
#define  DAC_DHR12R1_Address                 0x40007408
#define  DAC_DHR12L1_Address                 0x4000740C
#define  DAC_DHR12L2_Address                 0x40007418
/**
  * @}
  */


/** @defgroup AUDIO_RECORDER_Exported_Macros
  * @{
  */
#if !defined (CRITICAL_SECTION_ON)
 #define CRITICAL_SECTION_ON  __NOP
#endif
#if !defined (CRITICAL_SECTION_OFF)
 #define CRITICAL_SECTION_OFF __NOP
#endif
  
/**
  * @}
  */



/** @defgroup AUDIO_RECORDER_Exported_functions
  * @{
  */

uint32_t STM32_AudioRec_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint32_t STM32_AudioRec_Start(uint8_t* pbuf, uint32_t size);
uint32_t STM32_AudioRec_Stop(void);
uint32_t STM32_AudioRec_HeaderInit(uint8_t* pHeadBuf);
uint32_t STM32_AudioRec_ClkUpdate(uint32_t Step, uint32_t Direction);

/* This function should be implemented by the user application.
   It is called into this driver when the current buffer is filled
   to prepare the next buffer pointer and its size. */
uint32_t STM32_AudioRec_Update(uint8_t** pbuf, uint32_t* pSize);

/* Optional data processing functions (Digital Filtering, Encoding ...).
   These functions' prototypes only are declared by the audio recorder module.
   Their implementation may be done (if needed) by the user into application modules. 
   These functions are enabled only when the relative defines:
   AUDIO_REC_USE_DATA_PROC_SINGLE and AUDIO_REC_USE_DATA_PROC_BUFFER are enabled in the
   file stm322xg_eval_audio_conf.h  */
#ifdef AUDIO_REC_USE_DATA_PROC_SINGLE
uint32_t STM32_AudioRec_DataProcessSingle(uint8_t* pdata, uint32_t fmt, uint32_t chnbr);
#endif
#ifdef AUDIO_REC_USE_DATA_PROC_BUFFER
uint32_t STM32_AudioRec_DataProcessBuffer(uint8_t * pbuf, uint32_t size, uint32_t fmt, uint32_t chnbr);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__STM322XG_AUDIO_RECORDER_H */

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
