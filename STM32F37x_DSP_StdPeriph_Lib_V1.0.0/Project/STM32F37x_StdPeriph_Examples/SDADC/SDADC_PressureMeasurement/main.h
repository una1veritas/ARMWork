/**
  ******************************************************************************
  * @file    SDADC/SDADC_PressureMeasurement/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f37x.h"
#include "stm32373c_eval.h"
#include <stdio.h>
#include "stm32373c_eval/stm32373c_eval_lcd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Messages to be displayed on LCD */
#define MESSAGE1   "     Pressure       "
#define MESSAGE2   "Measurement MPX2102A"

/* Pressure sensor MPX2102 is connected to PE8 and PE9 which corresponds to SDADC1 channel 8:
   Refer to STM32373C-EVAL user manual for more details about how the MPX2102 is
   connected to PE8 and PE9 */
#define MPX2102_SDADC             SDADC1
#define MPX2102_SDADC_GPIO_PORT   GPIOE
#define MPX2102_GPIO_PIN_P        GPIO_Pin_8
#define MPX2102_GPIO_PIN_N        GPIO_Pin_9
#define MPX2102_GPIO_CLK          RCC_AHBPeriph_GPIOE
#define MPX2102_SDADC_CLK         RCC_APB2Periph_SDADC1
#define MPX2102_SDADC_PWR         PWR_SDADCAnalog_1
#define MPX2102_SDADC_VREF        SDADC_VREF_Ext /* Select External reference */
#define MPX2102_SDADC_GAIN        SDADC_Gain_4   /* Select Internal gain = 4 */
#define SDADC_GAIN                (uint32_t) 4   /* SDADC internal gain is set to 4: update this define
                                                    according to MPX2102_SDADC_GAIN */
#define MPX2102_SDADC_CHANNEL     SDADC_Channel_8
#define SDADC_RESOL               (uint32_t) 65535 /* 2e16 - 1 */
#define SDADC_INIT_TIMEOUT        30 /* about two SDADC clock cycles after INIT is set */
#define SDADC_CAL_TIMEOUT         4*30720 /*  ~5.12 ms at 6 MHz  in a single calibration sequence */
#define SDADC_VREF                (float) 3.3f /* SDADC external reference is set to 3.3V */
#define MPX2102_SENSITIVITY       (float) 13.2 /* MPX2102 sensitivity when powered by 3.3V
                                                  is 3.3V * 40mV / 10V = 13.2mV/1000mB = 13.2 microV/mB */
#define EXTERNGAIN                (float) 45.1  /* External gain equal to 2*(22K/1K)+1
                                                   is set using TVS632 mounted on STM32373C-EVAL */

/* An offset equal to VDD * R200 / R205 = VDD * 22/220 = VDD/10 */
#define OFFSET_VOLTAGE            (float)(SDADC_VREF/10.0f)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
