/**
  ******************************************************************************
  * @file    stm322xg_eval_resampling.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for the resampling
  *          driver.        
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
#ifndef __STM322xG_RESAMPLING_H
#define __STM322xG_RESAMPLING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "audio_app_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Resampling ratio, can be different for each  HQresampleLR32t() call 
   Note: Size of  N (number of input samples) must be >= number of taps e.g. 32 */
#define RATIO_48_TO_44_1    246625075 //= 44.1/48 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_44      246065834 //=   44/48 * 2^28  = Fin/Fout*2^28
#define RATIO_44_TO_48      292838679 //=   44/48 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_96      134217728 //=   48/96 * 2^28  = Fin/Fout*2^28


#define RATIO_48_TO_48_1    267877378 //=   48/48.1 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_47_8    269558617 //=   48/47.8 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_49      262957181 //=   48/49 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_48      268435456 //=   48/49 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_24      536870912 //=   48/24 * 2^28  = Fin/Fout*2^28
#define RATIO_48_TO_8      1610612736 //=   48/24 * 2^28  = Fin/Fout*2^28


 /* I2S DMA Stream definitions */
 #define SRC_DMA_CLOCK            RCC_AHB1Periph_DMA2
 #define SRC_DMA_STREAM           DMA2_Stream7
 #define SRC_DMA_CHANNEL          DMA_Channel_0

 #define SRC_DMA_FLAG_TC          DMA_FLAG_TCIF7
 #define SRC_DMA_FLAG_HT          DMA_FLAG_HTIF7
 #define SRC_DMA_FLAG_FE          DMA_FLAG_FEIF7
 #define SRC_DMA_FLAG_TE          DMA_FLAG_TEIF7
 #define SRC_DMA_FLAG_DME         DMA_FLAG_DMEIF7
 #define SRC_DMA_FLAG_ALL         (uint32_t)(SRC_DMA_FLAG_TC | SRC_DMA_FLAG_HT |\
                                                   SRC_DMA_FLAG_FE | SRC_DMA_FLAG_DME)
 #define SRC_DMA_PERIPH_DATA_SIZE DMA_PeripheralDataSize_Word
 #define SRC_DMA_MEM_DATA_SIZE    DMA_MemoryDataSize_Word
 #define SRC_DMA                  DMA2
 #define SRC_DMA_IFCR             HIFCR


 #define SRC_IRQHandler           DMA2_Stream7_IRQHandler
 #define SRC_IRQ                  DMA2_Stream7_IRQn
/* #define SRC_IRQ_PREPRIO        0xFF */
/* #define SRC_IRQ_SUBRIO 	  0xFF */


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint32_t STM32_Resample_Init(uint32_t opt0, uint32_t opt1, uint32_t opt2, uint32_t opt3);
uint32_t STM32_Resample_Process(int* pSrcBuf, uint32_t Size, int* pDstBuf, uint32_t Ratio, uint32_t* pOutsize);

#endif /* __STM322xG_RESAMPLING_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
