/**
  ******************************************************************************
  * @file    stm32_audio_synchprocess.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for the audio correction.
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

#ifndef __STM32_AUDIO_SYNCHPROCESS_H
#define __STM32_AUDIO_SYNCHPROCESS_H

/* Includes ------------------------------------------------------------------*/
#ifdef STM32F2XX
 #include "stm32f2xx.h"
#elif defined(STM32F4XX)
 #include "stm32f4xx.h"
#elif defined(STM32F10X_CL)
 #include "stm32f10x.h"
#endif /* STM32F2XX */

#include "stm32_audio_correction.h"



/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup AUDIO_SYNCHPROCESS
  * @brief This file is the Header file for stm32_audio_synchprocess.c
  * @{
  */ 

/** @defgroup AUDIO_SYNCHPROCESS_Exported_Defines
  * @{
  */ 
/* ----------------------------------------------------------------------------

   All the commented configuration parameters are gathered into audio_app_conf.h 
   file. If you already set them in audio_app_conf.h file, there is no need 
   to reconfigure them here.

-----------------------------------------------------------------------------*/

/* Set the relative define to 0 to disable a synchronization method or to 1 to 
   enable that method.
   You can disable all methods: In this case no correction method is selected
   and the streaming will not be synchronized. 

   Add-Remove samples synchronization method. This method doesn't require 
   additional hardware. */
/* #define AUDIO_SYNCHRO_ADDREM_ENABLED         1 */

/* External Dynamic PLL with clock synthesizer. This method uses the clock 
   synthesizer component implemented in STM322xG-EVAL revB board. */
/* #define AUDIO_SYNCHRO_CLKSYNTH_ENABLED       1 */

/* Feedback pipe synchronization method. This method doesn't require 
   additional hardware. */
/* #define AUDIO_SYNCHRO_FEEDBACK_ENABLED       1 */

/* Dynamic Audio recording clock adjustment using Timer auto reload register udpate. */
/* #define USB_AUDIO_IN_SYNCHRO_CLKUPDT_ENABLED     1 */

/* Correction timing parameters: Make sure these parameters are set to suitable 
  values depending on the allowed error and the application requirements. */
/*-------------------- SYNCH_NONE -------------------------------------------*/
/* #define SYNCH_NONE_PERIOD                        300 *//* Used as indicative way only */
/* #define SYNCH_NONE_STEP                          0   *//* Not used in this case */

/*-------------------- SYNCH_EXTCLK -----------------------------------------*/
/* Period of time between corrections in ms (This value must be multiple of 10 ms).
    This constraint (multiple of 10 ms) is only to avoid intensive entrance into 
    interrupt routine (save CPU load). If higher granularity is required you can 
    disable the option AUDIO_CORR_INERR_MULT_10 by commenting the define. */
/* This is the case when clock shift is less than 100ppm */
/* #define SYNCH_EXTCLK_PERIOD                      200 */
/* Step for ppm adjustment */
/* #define SYNCH_EXTCLK_STEP                        1 */
/* This is the case when clock shift is higher than 100ppm and less than 500ppm */
/* #define SYNCH_EXTCLK_PERIOD_ALT                  200 */
/* Step for ppm adjustment */
/* #define SYNCH_EXTCLK_STEP_ALT                    5 */
/* External Clock synchro threshold not used here, keep it at 0 */
/* #define SYNCH_EXTCLK_THRESHOLD                   0 */

/*-------------------- SYNCH_FEEDBACK ---------------------------------------*/
 /* Period of time between corrections in ms (This value must be multiple of 10 ms).
    This constraint (multiple of 10 ms) is only to avoid intensive entrance into 
    interrupt routine (save CPU load). If higher granularity is required you can 
    disable the option AUDIO_CORR_INERR_MULT_10 by commenting the define. */
/* #define SYNCH_FEEDBACK_PERIOD                    500 */
 /* Fractional part to be added/removed to/from original frame size (fractional
    format is 10.10 (or 10.14), refer to USB 2.0 specification for more details).
    when the define FEEDBACK_EXTENDED_PRECISION_10_14 is enabled, the used format
    is 10.14. Else it is 10.10 */
/* #define SYNCH_FEEDBACK_STEP                      1 */
 /* Feedback pipe refresh value. This value indicates the rate at which the host
    should request for the updated feedback value. Min = 1, Max = 9.
    The refresh period = 2^FEEDBACK_REFRESH. 
    e.g. FEEDBACK_REFRESH = 5: bRefresh = 2^5 = 32 */
/*  #define FEEDBACK_REFRESH                        8 */
 /* Feedback threshold is the maximum number of corrections in one direction.
    This parameter defines the dynamic of the feedback value update.
    e.g if FEEDBACK_THRESHOLD = 30: if the feedback value is decremented 30 times
    then it is reset to the orginal value. 
    You can set this value to 0xFFFFFFF to see the effect of not limiting the 
    dynamics of feedback by a threshold value: correction will go too far in 
    one direction before going in the opposite one. */
/* #define SYNCH_FEEDBACK_THRESHOLD                30 */

/*-------------------- SYNCH_ADDREM -----------------------------------------*/
 /* Period of time between corrections in ms (This value must be multiple of 10 ms).
    This constraint (multiple of 10 ms) is only to avoid intensive entrance into 
    interrupt routine (save CPU load). If higher granularity is required you can 
    disable the option AUDIO_CORR_INERR_MULT_10 by commenting the define. */
/* #define SYNCH_ADDREM_PERIOD                      50*/
 /* Number of samples to be removed or added each time an overrun/underrun 
    condition is detected. */
/* #define SYNCH_ADDREM_STEP                        4 */

/*-------------------- SYNCH_CLKUPDT ----------------------------------------*/
 /* Period of time between corrections in ms (This value must be multiple of 10 ms).
    This constraint (multiple of 10 ms) is only to avoid intensive entrance into 
    interrupt routine (save CPU load). If higher granularity is required you can 
    disable the option AUDIO_CORR_INERR_MULT_10 by commenting the define. */
/* #define SYNCH_CLKUPDT_PERIOD                    50 *//* 50 ms */ 
 /* Step value for the input clock update. */
/* #define SYNCH_CLKUPDT_STEP                      2 */

/*-------------------- Optional adjustment Parameters -----------------------*/
/* If higher granularity is required for CORRECTION_PERIOD you can disable the option
    AUDIO_CORR_INERR_MULT_10 by commenting this define.
    In this case the CORRECTION_PERIOD value can be non multiple of 10.
    WARNING: Uncommenting this define will lead to increasing CPU load since the periodic
    correction interrupt will be exected at higher frequency. */
/* #define AUDIO_CORR_INERR_MULT_10 */

/* Uncomment this define to extend the precision of the feedback value from 10.10 to 10.14
   format. This means that the lower 4 bits of the fractional part will be used.*/
/* #define FEEDBACK_EXTENDED_PRECISION_10_14 */

/* Uncomment these defines to enable the Dynamic Adjustment Factor: that means
   the correction step will be increased/decreased depending on the error value.
   This method is available only with external clock synthesizer correction enabled. */
/* #define USE_DYNAMIC_ADJUSTMENT_FACTOR            0 */
/* #define DYNAMIC_ADJUSTMENT_FACTOR_MAX            4 */


/*---------------------- 
                             DO NOT MODIFY THESE DEFINES
                                              ------------------------------------*/

/* Security check: bRefresh value must be in range [1,9] */
#ifdef AUDIO_SYNCHRO_FEEDBACK_ENABLED
 #if (FEEDBACK_REFRESH > 9)
  #error "ERROR: Feedabck refresh value out of spec ! (FEEDBACK_REFRESH in usbd_audio_synchprocess.h)"
 #endif
#endif /* AUDIO_SYNCHRO_FEEDBACK_ENABLED */

/* Do not modify this define */
#ifdef FEEDBACK_EXTENDED_PRECISION_10_14
 #define MAX_FRACTION                            0x3FFF
#else
 #define MAX_FRACTION                            0x3FF
#endif /* FEEDBACK_EXTENDED_PRECISION_10_14 */
/*----------------------------------------------------------------------------*/
/**
  * @}
  */ 


/** @defgroup AUDIO_SYNCHPROCESS_Exported_TypesDefinitions
  * @{
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
extern SynchStruct_TypeDef*  OutMethTab[];
extern SynchStruct_TypeDef*  InMethTab[];
/**
  * @}
  */ 
#endif /* __STM32_AUDIO_SYNCHPROCESS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
