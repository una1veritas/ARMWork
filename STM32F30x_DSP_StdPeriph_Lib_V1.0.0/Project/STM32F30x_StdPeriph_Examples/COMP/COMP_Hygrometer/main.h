/**
  ******************************************************************************
  * @file    COMP/COMP_Hygrometer/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
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
#include "stm32f30x.h"
#include "stm32303c_eval_lcd.h"
#include "stm32303c_eval.h"
#include <math.h>
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Polynomial Expression Coefficients: Capacitance = f(Humidity) */
#define P3 0.00000003903f
#define P2 -0.000008294f
#define P1 0.002188f
#define P0 0.898f

/* Reversed Polynomial Expression Coefficients: Humidity = f(Capacitance)
   RH (%) = -3.4656*10^3 * X^3 + 1.0732*10^4 * X^2 - 1.0457*10^4*X + 3.2459*10^3
   with X = C (read) / C@55%RH */
#define RP3 -3465.6f
#define RP2 10732
#define RP1 -10457
#define RP0 3245.9f

/* Reference Capacitance Value is 180 pF 1%. This capacitance is used in calibration phase */
#define REFCAP 180e-12

/* Resistance Value. The reference capacitance and humidity sensor are 
   charged/discharged through the resistance R159 available on STM32303C-EVAL.
   It's value is 82.5K 1%
*/
#define RES 82500

/* TIM3 PWM frequency value in Hertz. TIM3 is usd to charge/discharge the humidity sensor.
   Min TIM3 PWM frequency is SystemCoreClock/65535 and max TIM3 PWM frequency  = 1/(RES * CAP)
   with RES is the resistance and CAP is the minimum sensor capacitance.
   For HS1101, CAP is equal to 160 pF.
   when SystemCoreClock is 72MHz, the min frequency is 1099Hz and the max frequency is 75.7 KHz
*/
#define FREQ 1100

#define USE_DAC 1                   /* DAC1_OUT1 = 2.085V */
//#define USE_VREFINT 1            /*  VrefInt = 1.22V Reference Voltage Used */

#ifdef USE_VREFINT
  #define VREFINT_FACTOR log(3.3f/(3.3f - 1.22f))      /* ln(Vdd/(Vdd-VrefInt)) */
#endif /* USE_VREFINT */

#define HUM_OUT_PIN             GPIO_Pin_8
#define HUM_OUT_GPIO            GPIOC
#define HUM_OUT_GPIO_CLK        RCC_AHBPeriph_GPIOC
#define HUM_OUT_PIN_SOURCE      GPIO_PinSource8
#define HUM_OUT_AF              GPIO_AF_2

#define HUM_IN_PIN              GPIO_Pin_0
#define HUM_IN_GPIO             GPIOB
#define HUM_IN_GPIO_CLK         RCC_AHBPeriph_GPIOB

/* Comp4 Inverting Input can be either DAC1_OUT1 or VREFINT */
#ifdef USE_DAC
  #define COMP_INV_INPUT        COMP_InvertingInput_DAC1
#else /* USE_VREFINT */
  #define COMP_INV_INPUT        COMP_InvertingInput_VREFINT
#endif /* USE_DAC */

/* Comp4 Non Inverting Input is IO1 (PB0) */
#define  COMP_NINV_INPUT        COMP_NonInvertingInput_IO1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
