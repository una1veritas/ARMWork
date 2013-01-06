/**
  ******************************************************************************
  * @file    stm32_audio_correction.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes the audio correction functional layer
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
#include "stm32_audio_correction.h"

#ifdef AUDIO_CORR_USE_LEDS_3_4

 #ifdef USE_STM322xG_EVAL
  #include "stm322xg_eval.h"
 #elif defined(USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"
 #elif defined(USE_STM32F4XX_DISCOVERY)
  #include "stm32f4_discovery.h"
 #endif /* USE_STM322xG_EVAL */
#endif /* AUDIO_CORR_USE_LEDS_3_4 */



#include "stm32_audio_synchprocess.h"

#ifdef USE_STM3210C_EVAL
 #include "stm3210c_audio_codec.h" ///Should be removed for more abstraction
#elif defined (USE_STM322xG_EVAL)
 #include "stm322xg_audio_codec.h" ///Should be removed for more abstraction
#elif defined (USE_STM324xG_EVAL)
 #include "stm324xg_audio_codec.h" ///Should be removed for more abstraction
#elif defined(USE_STM32F4XX_DISCOVERY)
 #include "stm32f4_discovery_audio_codec.h" ///Should be removed for more abstraction
#endif /* USE_STM3210C_EVAL */


/** @addtogroup STM32_Audio_Utilities
* @{
*/

/** @defgroup STM32_AUDIO_CORRECTION
* @brief    This file includes the audio correction functional layer
* @{
*/ 

/** @defgroup STM32_AUDIO_CORRECTION_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup STM32_AUDIO_CORRECTION_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup STM32_AUDIO_CORRECTION_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup STM32_AUDIO_CORRECTION_Private_Variables
* @{
*/ 
uint32_t shift = 0;
uint32_t dir = 0;
uint32_t* pbuf = 0;

static SynchMethod_TypeDef OutActiveMethod = SYNCH_NONE;
static SynchMethod_TypeDef InActiveMethod  = SYNCH_NONE;

/**
* @}
*/

/** @defgroup STM32_AUDIO_CORRECTION_Private_Constants
* @{
*/ 

/**
* @}
*/

/** @defgroup STM32_AUDIO_CORRECTION_Private_FunctionPrototypes
* @{
*/ 
static void AUDIO_CORR_TIMInit(void);

/**
* @}
*/ 


/** @defgroup STM32_AUDIO_CORRECTION_Private_Functions
* @{
*/ 

/**
  * @brief  Initialize the audio correction mechanism and all necessary low layer
  *         components.
  * @param  AudioFreq: Target audio sampling rate
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t AUDIO_CORR_Init(uint32_t AudioFreq)
{  
  uint32_t result = 0;  

#if defined(STM32F2XX) || defined(STM32F4XX)
  /* Enable CRC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
#endif /* STM32F2XX or STM32F4XX */
  
  /* Check if the current Out method init function exists */
  if (OutMethTab[OutActiveMethod]->Init != NULL)
  {
    OutMethTab[OutActiveMethod]->Init((uint32_t)(AudioFreq * 256 * 4), 0, 1, 0);
  }
  
  /* Check if the current In method init function exists */
  if (InMethTab[InActiveMethod & 0x7F]->Init != NULL)
  {
    InMethTab[InActiveMethod & 0x7F]->Init(AudioFreq, 0, 0, 0);
  }
  
  /* Initialize the Timer for periodic audio correction mechanism */
  AUDIO_CORR_TIMInit();  
  
  return result;
}

/**
  * @brief  Free all resources used by audio correction mechanism.
  * @param  None.
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t AUDIO_CORR_DeInit(void)
{
  uint32_t result = 0;

  /* Check if the current Out method init function exists */
  if (OutMethTab[OutActiveMethod]->DeInit != NULL)
  {
    result = OutMethTab[OutActiveMethod]->DeInit();
  }
  
  return result;
}

/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t AUDIO_CORR_Out_Process(uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{  
  uint32_t result = 0;

  /* Check if the current Out method init function exists */
  if (OutMethTab[OutActiveMethod]->Process != NULL)
  {
    result = OutMethTab[OutActiveMethod]->Process(pValue, pDirection, pBuf);
  }  
  
#ifdef AUDIO_CORR_USE_LEDS_3_4
  if (*pDirection)
  { /* Overrun condition */    
    STM_EVAL_LEDToggle(LED3); 
  }
  else
  {/* Underrun condition */    
    STM_EVAL_LEDToggle(LED4); 
  }
#endif    
  
  return result;
}

/**
  * @brief  Processes the audio correction for the IN stream.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t AUDIO_CORR_In_Process(uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{  
  uint32_t result = 0;

  /* Check if the current Out method init function exists */
  if (InMethTab[InActiveMethod & 0x7F]->Process != NULL)
  {
    result += InMethTab[InActiveMethod & 0x7F]->Process(pValue, pDirection, pBuf);
  }  

#ifdef AUDIO_CORR_USE_LEDS_3_4
  if (*pDirection)
  { /* Overrun condition */   
    /* STM_EVAL_LEDToggle(LED3); */ //This LED is already used for other purposes  
  }
  else
  {/* Underrun condition */    
    /* STM_EVAL_LEDToggle(LED4); */ //This LED is already used for other purposes
  }
#endif 
  
  return result;
}

/**
  * @brief  Calls lower layer update functions.
  * @param  Opt0: The value of this parameter depend on the lower layer implementation.
  * @param  Opt1: The value of this parameter depend on the lower layer implementation.
  * @param  Opt2: The value of this parameter depend on the lower layer implementation.
  * @param  Opt3: The value of this parameter depend on the lower layer implementation.
  * @retval None
  */
uint32_t AUDIO_CORR_Out_Update (uint32_t Opt0, uint32_t Opt1, uint32_t Opt2, uint32_t Opt3)
{ 
  uint32_t result = 0;
  
#if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
  if (OutActiveMethod == SYNCH_EXTCLK)
  {
    /* Update the hardware with the new configuration */
    Codec_AudioInterface_Init(Opt0, (uint32_t)(Opt0 * 256 * 4));
    
    result += OutMethTab[OutActiveMethod]->Update((uint32_t)(Opt0 * 256 * 4), Opt1, Opt2, Opt3);
  }
  else
  {
#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */
    /* Update the hardware with the new configuration */
    Codec_AudioInterface_Init(Opt0, 0);
#if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
  }
#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */
  
  return result;  
}

/**
  * @brief  Set the current active synchronization method.
  * @param  Dirction: Direction for which the synchronization method is selected.
  *         it can be DIRECTION_IN or DIRECTION_OUT.
  * @retval 0 if all operations are OK. !0 else.
  */
SynchStruct_TypeDef* AUDIO_CORR_SetActiveMethod(uint32_t Direction, SynchMethod_TypeDef Method)
{
  if (Direction)
  { /* DIRECTION_IN */
    InActiveMethod = Method;

    return InMethTab[InActiveMethod & 0x7F];
  }
  else 
  { /* DIRECTION_OUT */
    OutActiveMethod = Method;

    return OutMethTab[OutActiveMethod];
  }
}

/**
  * @brief  Returns the ID of the current active synchronization method.
  * @param  Dirction: Direction for which the synchronization method is selected.
  *         it can be DIRECTION_IN or DIRECTION_OUT.
  * @retval The synchronization method currently active.
  */
SynchMethod_TypeDef AUDIO_CORR_GetActiveMethod(uint32_t Direction)
{
  if (Direction)
  { /* DIRECTION_IN */
    return InActiveMethod;
  }
  else 
  { /* DIRECTION_OUT */
    return OutActiveMethod;
  }  
}

/**
  * @brief  Configure the Timer to generate an interrupt each 10ms.
  * @param  None
  * @retval None
  */
static void AUDIO_CORR_TIMInit(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_ClocksTypeDef RCC_Clocks;
  
  /* Get the current configuration of the RCC */
  RCC_GetClocksFreq(&RCC_Clocks);  
  
  /* Timer clock enable */
  RCC_APB1PeriphClockCmd(AUDIO_CORR_TIM_CLK, ENABLE);  
  
  /* timer Peripheral Configuration */
  TIM_DeInit(AUDIO_CORR_TIM);

#ifdef AUDIO_CORR_INERR_MULT_10
  /* Configure the Timer prescaler */
  TIM_PrescalerConfig(AUDIO_CORR_TIM, 100, TIM_PSCReloadMode_Immediate);
#else
  /* Configure the Timer prescaler */
  TIM_PrescalerConfig(AUDIO_CORR_TIM, 10, TIM_PSCReloadMode_Immediate);
#endif /* AUDIO_CORR_INERR_MULT_10 */  
  
  /* Set the timer period: Timer should generate 10ms period interrupt */
  TIM_SetAutoreload(AUDIO_CORR_TIM, (uint32_t)((RCC_Clocks.PCLK1_Frequency/5000)));

  /* Enable timer interrupts */
  TIM_ITConfig(AUDIO_CORR_TIM, TIM_IT_Update, ENABLE);
  
  /* Enable the timer Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_CORR_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AUDIO_CORR_TIM_PRE_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = AUDIO_CORR_TIM_SUB_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Start the timer */
  TIM_Cmd(AUDIO_CORR_TIM, ENABLE);
}

/**
  * @brief  This function handles timer global interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_CORR_TIM_IRQHANDLER(void)
{
  static uint32_t countout = 0;
  static uint32_t countin = 0;
  
  /* Clear timer interrupt */
  TIM_ClearITPendingBit(AUDIO_CORR_TIM, TIM_IT_Update);
  
  /* Get the current shift value only each CORRECTION_PERIOD period of time */
#ifdef AUDIO_CORR_INERR_MULT_10  
  if (++countout == (OutMethTab[OutActiveMethod]->SynchPeriod/10))
#else
  if (++countout == (OutMethTab[OutActiveMethod]->SynchPeriod))
#endif /* AUDIO_CORR_INERR_MULT_10 */
  {      
    /* Check if the shift value needs correction */
    if (AUDIO_CORR_Out_GetCurrent(&shift, &dir, &pbuf) != 0)
    {
      /* Process the necessary correction */
      AUDIO_CORR_Out_Process(&shift, &dir, pbuf);
    }
    
    /* Reinitialize counter to 0 */
    countout = 0;
  }  
  
  /* Get the current shift value only each CORRECTION_PERIOD period of time */
#ifdef AUDIO_CORR_INERR_MULT_10  
  if (++countin == (InMethTab[InActiveMethod & 0x7F]->SynchPeriod/10))
#else
  if (++countin == (InMethTab[InActiveMethod & 0x7F]->SynchPeriod))
#endif /* AUDIO_CORR_INERR_MULT_10 */
  {      
    /* Check if the shift value needs correction */
    if (AUDIO_CORR_In_GetCurrent(&shift, &dir, &pbuf) != 0)
    {
      /* Process the necessary correction */
      AUDIO_CORR_In_Process(&shift, &dir, pbuf);
    }
    
    /* Reinitialize counter to 0 */
    countin = 0;
  }  
}

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
