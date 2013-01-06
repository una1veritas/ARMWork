/**
  ******************************************************************************
  * @file    stm322xg_audio_recorder.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains the main function for audio recording using ADC
  *          and timer on STM322xG-EVAL evaluation board.
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
#include "stm322xg_audio_recorder.h"

/** @addtogroup Audio_Utilities
  * @{
  */

/** @defgroup AUDIO_RECORDER
  * @brief    This file contains the main function for audio recording using ADC
  *           and timer on STM322xG-EVAL evaluation board.
  * @{
  */

/** @defgroup AUDIO_RECORDER_Private_Types
  * @{
  */
typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Variables
  * @{
  */

/* Audio recording Samples format (from 8 to 16 bits) */
uint32_t AudioRecBitRes = 16; 

/* Audio recording number of channels (1 for Mono or 2 for Stereo) */
uint32_t AudioRecChnlNbr = 1;

/* Main buffer pointer for the recorded data storing */
uint8_t* pAudioRecBuf;

/* Current size of the recorded buffer */
uint32_t AudioRecCurrSize = 0; 

/* Number of data currently stored into the audio buffer */
uint32_t AudioRecCounter = 0;

/* Current Timer autoreload value */
uint32_t AutoReload     = 0xFFFFFFFF;
uint32_t OrigAutoReload = 0xFFFFFFFF;
int32_t  extAutoReload  = 0;

/* Local buffer for temporary operations */
uint8_t* TmpBuff = 0;

/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_FunctionPrototypes
  * @{
  */
static void STM32_AudioRec_GPIO_Init(void);
static void STM32_AudioRec_ADC_Init(void);
static void STM32_AudioRec_TIM_Init(uint32_t AudioFreq);
static void STM32_AudioRec_NVIC_Init(void);


/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Functions
  * @{
  */

/**
  * @brief  Initialize wave recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  /* Configure the GPIOs */
  STM32_AudioRec_GPIO_Init();
  
  /* Configure the interrupts (for timer) */
  STM32_AudioRec_NVIC_Init();
  
  /* Configure the ADC */
  STM32_AudioRec_ADC_Init();
  
  /* Configure the Timer which triggers the ADC */
  STM32_AudioRec_TIM_Init(AudioFreq);
  
  /* Set the local parameters */
  AudioRecBitRes = BitRes;
  AudioRecChnlNbr = ChnlNbr;
  

  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Start audio recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Start(uint8_t* pbuf, uint32_t size)
{
  /* Store the location and size of the audio buffer */
  pAudioRecBuf = pbuf;
  AudioRecCurrSize = size;
  
  /* AUDIO_REC_ADC regular channel configuration */
  ADC_RegularChannelConfig(AUDIO_REC_ADC, AUDIO_REC_ADC_CH, 1, ADC_SampleTime_480Cycles);
  
  /* Enable AUDIO_REC_ADC */
  ADC_Cmd(AUDIO_REC_ADC, ENABLE);
  
  /* Start AUDIO_REC_ADC Software Conversion */ 
  ADC_SoftwareStartConv(AUDIO_REC_ADC);
  //  while (ADC_GetFlagStatus(AUDIO_REC_ADC, ADC_FLAG_ADONS) == RESET)
  //  {}
  
  /* Enable AUDIO_REC_TIM interrupts */
  TIM_ITConfig(AUDIO_REC_TIM, TIM_IT_Update, ENABLE);
  
  /* Start AUDIO_REC_TIM */
  TIM_Cmd(AUDIO_REC_TIM, ENABLE);
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Stop audio recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Stop(void)
{
  /* Disable AUDIO_REC_TIM interrupts */
  TIM_ITConfig(AUDIO_REC_TIM, TIM_IT_Update, DISABLE);
  
  /* Disable AUDIO_REC_TIM */
  TIM_Cmd(AUDIO_REC_TIM, DISABLE);
  
  /* Stop conversion */
  ADC_Cmd(AUDIO_REC_ADC, DISABLE);
  
  /* Reset number of samples */
  AudioRecCounter = 0;
  AudioRecCurrSize = 0;
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Update the current value of the timer auto reload
  * @param  Step: decrement/increment step
  * @param  Direction: Direction of increment/decrement. Can be 
  *         AUDIO_REC_DIRECTION_DOWN or AUDIO_REC_DIRECTION_UP.
  * @retval None
  */
uint32_t STM32_AudioRec_ClkUpdate(uint32_t Step, uint32_t Direction)
{    
  
  /* Check on the direction of the clock update */
  if (Direction == AUDIO_REC_DIRECTION_UP)
  {
    /* Speed up the sampling rate */
    AutoReload  -= Step;
  }
  else
  {
    /* Slow down the sampling rate */
    AutoReload  += Step;
    
  }
  
  /* If operation is OK, return 0 */
  return 0;
}



/**
  * @brief  This function handles AUDIO_REC_TIM global interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_REC_TIM_IRQHANDLER(void)
{
  uint16_t SampleN = 0;
  uint32_t idx = 0;
  static uint32_t modcount = 0;
  uint32_t timeout = ADC_TIMEOUT_MAX;
  
  CRITICAL_SECTION_ON();
  
  /* Clear AUDIO_REC_TIM interrupt */
  TIM_ClearITPendingBit(AUDIO_REC_TIM, TIM_IT_Update);

  /* Update auto reload value */
  TIM_SetAutoreload(AUDIO_REC_TIM, AutoReload);
  
  /* Restore original autorelaod value (reduce dynamic of clock update) */
  if (AutoReload != OrigAutoReload)
  {   
    /* Keep modified value only for 10ms then return to original vlaue */
    if (modcount++ >= 160)
    {
      AutoReload = OrigAutoReload;
      modcount = 0;
    }
  }
  
  timeout = ADC_TIMEOUT_MAX;
  while (ADC_GetFlagStatus(AUDIO_REC_ADC, ADC_FLAG_EOC) == RESET)
  {
    if (timeout-- == 0) 
      break;
  }
  /* Get the ADC conversion value for sample N */
  SampleN = (uint16_t)(ADC_GetConversionValue(AUDIO_REC_ADC));
   
  /* Clear End Of Conversion flag */
  ADC_ClearFlag(AUDIO_REC_ADC, ADC_FLAG_EOC | ADC_FLAG_OVR);
  
#ifdef AUDIO_REC_USE_DATA_PROC_SINGLE
  /* Process the aquired data (Digital filter, Encoding ...) using single data */  
  STM32_AudioRec_DataProcessSingle((uint8_t*)(&SampleN), AudioRecBitRes, AudioRecChnlNbr);
#endif /* AUDIO_REC_USE_DATA_PROC_SINGLE */
  
  /* Store the converted sample N value (same value repeated if Stereo mode enabled) */
  while (idx++ <  AudioRecChnlNbr)
  {
    /* Check the data format type */
    if (AudioRecBitRes <= 8)
    {
      /* 1-byte data format */
      ((uint8_t*)(pAudioRecBuf))[AudioRecCounter] = (uint8_t)((SampleN >> 4) & 0x00FF);
    }
    else
    {      
      /* 2-bytes data format */
      //((uint16_t*)(pAudioRecBuf))[AudioRecCounter] = (uint16_t)((SampleN));
      ((uint16_t*)(pAudioRecBuf))[AudioRecCounter] = (uint16_t)((SampleN & 0x0FFF) - 0x30);
    }
    
    /* Increment the audio counter */
    AudioRecCounter++;
  }

  CRITICAL_SECTION_OFF();
  
  /* If the current buffer is full, prepare the next buffer */
  if (AudioRecCounter >= (AudioRecCurrSize / (AudioRecBitRes/8)))
  {    
    /* Reset the audio counter */
    AudioRecCounter = 0;
    
#ifdef AUDIO_REC_USE_DATA_PROC_BUFFER     
    /* Save the current buffer location */
    TmpBuff = (uint8_t*)pAudioRecBuf;
#endif /* AUDIO_REC_USE_DATA_PROC_BUFFER  */
    
    /* Call the record update function */
    STM32_AudioRec_Update(&pAudioRecBuf, &AudioRecCurrSize);
    
#ifdef AUDIO_REC_USE_DATA_PROC_BUFFER
    /* Process the aquired data (Digital filter, Encoding ...) using entire buffer */  
    STM32_AudioRec_DataProcessBuffer(TmpBuff, AudioRecCurrSize, AudioRecBitRes, AudioRecChnlNbr);
#endif /* AUDIO_REC_USE_DATA_PROC_BUFFER */
  }
}




/**
  * @brief  STM32_AudioRec_GPIO_Init.
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable ADC GPIO clock */
  RCC_AHB1PeriphClockCmd(AUDIO_REC_ADC_PORT_CLK, ENABLE);
    
  /* Configure PF.10 (ADC Channel8) as analog input */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = AUDIO_REC_ADC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(AUDIO_REC_ADC_PORT, &GPIO_InitStructure);
}

/**
  * @brief  STM32_AudioRec_ADC_Init.
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_ADC_Init(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  /* AUDIO_REC_ADC Periph clock enable */
  RCC_APB2PeriphClockCmd(AUDIO_REC_ADC_CLK, ENABLE);  
  
  /* Initialize and configure ADC */
  ADC_DeInit();
  ADC_CommonStructInit(&ADC_CommonInitStructure);
  //ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(AUDIO_REC_ADC, &ADC_InitStructure);  
}

/**
  * @brief  STM32_AudioRec_TIM_Init.
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_TIM_Init(uint32_t AudioFreq)
{
  RCC_ClocksTypeDef RCCClocks;
 
  RCC_GetClocksFreq(&RCCClocks);
  
  /* AUDIO_REC_TIM clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

  /* AUDIO_REC_TIM Peripheral Configuration */
  TIM_DeInit(AUDIO_REC_TIM);

  /* Set AUDIO_REC_TIM period to the audio frequency using system clock */
  AutoReload = (uint32_t)(RCCClocks.PCLK2_Frequency/AudioFreq);
  TIM_SetAutoreload(AUDIO_REC_TIM, AutoReload);

  /* AUDIO_REC_TIM TRGO selection */
  TIM_SelectOutputTrigger(AUDIO_REC_TIM, TIM_TRGOSource_Update);

  /* Enable AUDIO_REC_TIM interrupts */
  TIM_ITConfig(AUDIO_REC_TIM, TIM_IT_Update, ENABLE);
  
  /* Update the extended reload value */
  extAutoReload = 0;
  
  /* Save the original autoreload value */
  OrigAutoReload = AutoReload;
}

/**
  * @brief  STM32_AudioRec_NVIC_Init.
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_NVIC_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the Timer Interrupt to trigger the ADC */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_REC_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AUDIO_REC_TIM_PRE_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = AUDIO_REC_TIM_SUB_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

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
