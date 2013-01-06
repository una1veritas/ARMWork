/**
  ******************************************************************************
  * @file    usbd_audio_synchprocess.c
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
#include "stm32_audio_synchprocess.h"


#ifdef USE_STM3210C_EVAL
 #include "stm3210c_audio_codec.h"

#elif defined(USE_STM322xG_EVAL)
 #if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
  #include "stm322xg_clk_synthesizer_cs2200.h"
 #endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */
  #include "stm322xg_audio_recorder.h"
  #include "stm322xg_audio_codec.h"
 #ifdef AUDIO_SYNCHRO_SRC_ENABLED
  #include "stm322xg_eval_resampling.h"
 #endif /* AUDIO_SYNCHRO_SRC_ENABLED */

#elif defined(USE_STM324xG_EVAL)
 #if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
  #include "stm324xg_clk_synthesizer_cs2200.h"
 #endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */
  #include "stm324xg_audio_recorder.h"
  #include "stm324xg_audio_codec.h"
 #ifdef AUDIO_SYNCHRO_SRC_ENABLED
  #include "stm324xg_eval_resampling.h"
 #endif /* AUDIO_SYNCHRO_SRC_ENABLED */

#elif defined(USE_STM32F4XX_DISCOVERY)
  #include "stm32f4_discovery_audio_recorder.h"
  #include "stm32f4_discovery_audio_codec.h"
 #ifdef AUDIO_SYNCHRO_SRC_ENABLED
  #include "stm32f4_discovery_resampling.h"
 #endif /* AUDIO_SYNCHRO_SRC_ENABLED */
#endif /* USE_STM3210C_EVAL */



							


/** @addtogroup STM32_Audio_Utilities
* @{
*/

/** @defgroup USBD_AUDIO_SYNCHPROCESS
* @brief    This file includes the audio correction functional layer
* @{
*/ 

/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_Variables
* @{
*/ 

 /*------------------------------- NO SYNCHRO -----------------------------------------*/
SynchStruct_TypeDef SYNCH_NONE_SynchStruct = {
  SYNCH_NONE,                  /* Method Name */
  SYNCH_NONE_PERIOD,           /* Synchro Period */
  SYNCH_NONE_STEP,             /* Synchro step */
  0,                           /* Synchro Threshold */
  NULL,                        /* Synchro option table pointer */
  NULL,                        /* Synchro Init callback */
  NULL,                        /* Synchro Process Callback */
  NULL,                        /* Synchro Update Callback */
  NULL,                        /* Synchro DeInit Callback */
};

#if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
 /*-------------------------- Extern Clock SYNCHRO -----------------------------------*/
uint8_t tOptTable[5] = {
  DEFAULT_INJECTED_ERROR,        /* Enable or disable the injected error on the output clock */
  SYNCH_EXTCLK_PERIOD_ALT,       /* Synhcronization period when injected error is enabled */
  SYNCH_EXTCLK_STEP_ALT,         /* Synhcronization step when injected error is enabled */
  USE_DYNAMIC_ADJUSTMENT_FACTOR, /* 0: use the dynamic adjustment factor, !0: don't use */ 
  DYNAMIC_ADJUSTMENT_FACTOR_MAX, /* Maximum adjustement factor value */
};

static uint32_t CLK_SYNTH_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf);

/* Main exported structure */
SynchStruct_TypeDef SYNCH_EXTCLK_SynchStruct = {
  SYNCH_EXTCLK,                /* Method Name */
  SYNCH_EXTCLK_PERIOD,         /* Synchro Period */
  SYNCH_EXTCLK_STEP,           /* Synchro step */
  SYNCH_EXTCLK_THRESHOLD,      /* Synchro Threshold */
  tOptTable,                   /* Synchro option table pointer */
  CLK_SYNTH_Init,              /* Synchro Init callback */
  CLK_SYNTH_Process,           /* Synchro Process Callback */
  CLK_SYNTH_UpdateFreq,        /* Synchro Update Callback */
  CLK_SYNTH_DeInit,            /* Synchro DeInit Callback */
};
#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */


 /*-------------------------- FEEDBACK SYNCHRO -------------------------------------*/
#if (AUDIO_SYNCHRO_FEEDBACK_ENABLED == 1)
uint8_t tFeedbackOptTable[1] = {
  FEEDBACK_REFRESH,              /* Enable or disable the injected error on the output clock */
};

static uint32_t SYNCH_FEEDBACK_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf);

/* Main exported structure */
SynchStruct_TypeDef SYNCH_FEEDBACK_SynchStruct = {
  SYNCH_FEEDBACK,              /* Method Name */
  SYNCH_FEEDBACK_PERIOD,       /* Synchro Period */
  SYNCH_FEEDBACK_STEP,         /* Synchro step */
  SYNCH_FEEDBACK_THRESHOLD,    /* Synchro Threshold */
  tFeedbackOptTable,           /* Synchro option table pointer */
  NULL,                        /* Synchro Init callback */
  SYNCH_FEEDBACK_Process,      /* Synchro Process Callback */
  NULL,                        /* Synchro Update Callback */
  NULL,                        /* Synchro DeInit Callback */
};
#endif /* AUDIO_SYNCHRO_FEEDBACK_ENABLED */

 /*-------------------------- ADD-REM SYNCHRO -----------------------------------------*/
#if (AUDIO_SYNCHRO_ADDREM_ENABLED == 1)
static uint32_t SYNCH_ADDREM_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf);

/* Main exported structure */
SynchStruct_TypeDef SYNCH_ADDREM_SynchStruct = {
  SYNCH_ADDREM,                /* Method Name */
  SYNCH_ADDREM_PERIOD,         /* Synchro Period */
  SYNCH_ADDREM_STEP,           /* Synchro step */
  NULL,                        /* Synchro Threshold */
  NULL,                        /* Synchro option table pointer */
  NULL,                        /* Synchro Init callback */
  SYNCH_ADDREM_Process,        /* Synchro Process Callback */
  NULL,                        /* Synchro Update Callback */
  NULL,                        /* Synchro DeInit Callback */
};
#endif /* AUDIO_SYNCHRO_ADDREM_ENABLED */

 /*----------------------------- SRC SYNCHRO -----------------------------------------*/
#ifdef AUDIO_SYNCHRO_SRC_ENABLED
uint8_t tSrcOptTable[1] = {
  0,            /* For future use */ 
};

static uint32_t SYNCH_SRC_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf);

/* Main exported structure */
SynchStruct_TypeDef SYNCH_SRC_SynchStruct = {
  SYNCH_SRC,              /* Method Name */
  SYNCH_SRC_PERIOD,       /* Synchro Period */
  SYNCH_SRC_STEP,         /* Synchro step */
  NULL,                   /* Synchro Threshold */
  tSrcOptTable,           /* Synchro option table pointer */
  STM32_Resample_Init,    /* Synchro Init callback */
  SYNCH_SRC_Process,      /* Synchro Process Callback */
  NULL,                   /* Synchro Update Callback */
  NULL,                   /* Synchro DeInit Callback */
};
#endif /* AUDIO_SYNCHRO_SRC_ENABLED */

#if (USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED == 1)
 /*-------------------------- CLKUPDT SYNCHRO -----------------------------------------*/
static uint32_t SYNCH_CLKUPDT_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf);

/* Main exported structure */
SynchStruct_TypeDef SYNCH_CLKUPDT_SynchStruct = {
  SYNCH_CLKUPDT,               /* Method Name */
  SYNCH_CLKUPDT_PERIOD,        /* Synchro Period */
  SYNCH_CLKUPDT_STEP,          /* Synchro step */
  NULL,                        /* Synchro Threshold */
  NULL,                        /* Synchro option table pointer */
  NULL,                        /* Synchro Init callback */
  SYNCH_CLKUPDT_Process,       /* Synchro Process Callback */
  NULL,                        /* Synchro Update Callback */
  NULL,                        /* Synchro DeInit Callback */
};
 /*-----------------------------------------------------------------------------------*/
#endif /* USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED */

/* Table for Out stream synchronization methods list */
SynchStruct_TypeDef*  OutMethTab[] = {
  &SYNCH_NONE_SynchStruct,
 #if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
  &SYNCH_EXTCLK_SynchStruct,
 #endif
 #if (AUDIO_SYNCHRO_FEEDBACK_ENABLED == 1)
  &SYNCH_FEEDBACK_SynchStruct,
 #endif
 #if (AUDIO_SYNCHRO_ADDREM_ENABLED == 1)
  &SYNCH_ADDREM_SynchStruct,
 #endif
 #ifdef AUDIO_SYNCHRO_SRC_ENABLED
  &SYNCH_SRC_SynchStruct,
 #endif /* AUDIO_SYNCHRO_SRC_ENABLED */
  
};

/* Table for In stream synchronization methods list */
SynchStruct_TypeDef*  InMethTab[] = {
  &SYNCH_NONE_SynchStruct,
#if (USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED == 1)
  &SYNCH_CLKUPDT_SynchStruct,
#endif
  
};

/**
* @}
*/

/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_Constants
* @{
*/ 

/**
* @}
*/

/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_FunctionPrototypes
* @{
*/

/**
* @}
*/ 


/** @defgroup USBD_AUDIO_SYNCHPROCESS_Private_Functions
* @{
*/ 

#if (AUDIO_SYNCHRO_CLKSYNTH_ENABLED == 1)
/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (not used for this method).
  * @retval 0 if passed. !0 if error occured during operation.
  */
static uint32_t CLK_SYNTH_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{  
  uint32_t result = 0;
  uint32_t factor = 1;
  
  if (SYNCH_EXTCLK_SynchStruct.pSynchOpt[3])
  {
    /* Get the adjustment factor. A maximum factor value of 4 is accepted (4 x 5ppm = 20ppm) */
    factor = ((*pValue) > SYNCH_EXTCLK_SynchStruct.pSynchOpt[4])? SYNCH_EXTCLK_SynchStruct.pSynchOpt[4]:(*pValue);
    
    /* Make sure that the  adjustment factor is not 0 */
    if (factor == 0)
    {
      factor = 1;
    }
  }
  
  /* Adjustment mechanism to correct the overrun/underrun situation */
  if (*pDirection)
  { /* Overrun condition */    
    /* Adjust the clock frequency to speed up output frequency */
    result = CLK_SYNTH_AdjustPpm((SYNCH_EXTCLK_SynchStruct.SynchStep * factor), EXTCLK_DIR_UP);
  }
  else
  {/* Underrun condition */    
    /* Adjust the clock frequency to slow down output frequency */
    result = CLK_SYNTH_AdjustPpm((SYNCH_EXTCLK_SynchStruct.SynchStep * factor), EXTCLK_DIR_DOWN);
  }
  
  /* Return the result of operations */
  return result;
}
#endif /* AUDIO_SYNCHRO_CLKSYNTH_ENABLED */

#if (AUDIO_SYNCHRO_FEEDBACK_ENABLED == 1)
/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
static uint32_t SYNCH_FEEDBACK_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{
  static uint32_t count = 0;
  
  /* Adjustment mechanism to correct the overrun/underrun situation */
  if (*pDirection)
  { /* Overrun condition */    
    if (++count < SYNCH_FEEDBACK_SynchStruct.SynchThreshold)
    {
      /* Adjust the feedback value */
      if ((*(uint32_t *)(pBuf + 1)) < SYNCH_FEEDBACK_SynchStruct.SynchStep)
      {
        (*pBuf) --;
        (*(uint32_t *)(pBuf + 1)) = MAX_FRACTION;
      }   
      else
      {
        (*(uint32_t *)(pBuf + 1)) -= SYNCH_FEEDBACK_SynchStruct.SynchStep;
      }
    }
    else
    {
      (*pBuf) = (*(uint32_t *)(pBuf + 2));
      (*(uint32_t *)(pBuf + 1)) = (*(uint32_t *)(pBuf + 3));
      count = 0;
    } 
  }
  else
  {/* Underrun condition */
    if (++count < SYNCH_FEEDBACK_SynchStruct.SynchThreshold)
    {    
      /* Adjust the feedback value */
      if ((*(uint32_t *)(pBuf + 1)) > (MAX_FRACTION - SYNCH_FEEDBACK_SynchStruct.SynchStep))
      {
        (*pBuf) ++;
        (*(uint32_t *)(pBuf + 1)) = 0;
      }
      else
      {
        (*(uint32_t *)(pBuf + 1)) += SYNCH_FEEDBACK_SynchStruct.SynchStep;
      }
    }
    else
    {
      (*pBuf) = (*(uint32_t *)(pBuf + 2));
      (*(uint32_t *)(pBuf + 1)) = (*(uint32_t *)(pBuf + 3));
      count = 0;
    }   
  }
  
  return 0;
}
#endif /* AUDIO_SYNCHRO_FEEDBACK_ENABLED */

#if (AUDIO_SYNCHRO_ADDREM_ENABLED == 1)
/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
uint32_t SYNCH_ADDREM_Process(uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{   
  /* Adjustment mechanism to correct the overrun/underrun situation */
  if (*pDirection)
  { /* Overrun condition */
    (*pBuf) -= SYNCH_ADDREM_SynchStruct.SynchStep;
  }
  else
  {/* Underrun condition */
    (*pBuf) += SYNCH_ADDREM_SynchStruct.SynchStep;   
  }
  
  return 0;
}
#endif /* AUDIO_SYNCHRO_ADDREM_ENABLED */

#ifdef AUDIO_SYNCHRO_SRC_ENABLED
/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
static uint32_t SYNCH_SRC_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{   
  /* Adjustment mechanism to correct the overrun/underrun situation */
  if (*pDirection)
  { /* Overrun condition */
    (*pBuf) -= SYNCH_SRC_SynchStruct.SynchStep;;
  }
  else
  {/* Underrun condition */
    (*pBuf) += SYNCH_SRC_SynchStruct.SynchStep;;   
  }
  
  return 0;
}
#endif /* AUDIO_SYNCHRO_SRC_ENABLED */

#if (USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED == 1)
/**
  * @brief  Processes the audio correction.
  * @param  Value: value of the shift to be corrected.
  * @param  pDirection: direction of the shift (up or down).
  * @param  pBuf: optional parameter (for future use).
  * @retval 0 if passed. !0 if error occured during operation.
  */
static uint32_t SYNCH_CLKUPDT_Process (uint32_t* pValue, uint32_t* pDirection, uint32_t* pBuf)
{    
  /* Adjustment mechanism to correct the overrun/underrun situation */
  if (*pDirection)
  { /* Overrun condition */  
    /* Adjust the clock frequency to speed up output frequency */
    STM32_AudioRec_ClkUpdate(SYNCH_CLKUPDT_SynchStruct.SynchStep, AUDIO_REC_DIRECTION_DOWN);
  }
  else
  {/* Underrun condition */    
    /* Adjust the clock frequency to speed up output frequency */
    STM32_AudioRec_ClkUpdate(SYNCH_CLKUPDT_SynchStruct.SynchStep, AUDIO_REC_DIRECTION_UP);
  }
  
  return 0;
}
#endif /* USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED */

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
