/**
  ******************************************************************************
  * @file    stm324xg_clk_synthesizer_cs2200.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file contains all the functions prototypes for the external
  *          clock synthesizer controller.  
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

#ifndef __CLK_SYNTHESIZER_CS2200_H
#define __CLK_SYNTHESIZER_CS2200_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32_Audio_Utilities
  * @{
  */
  
/** @defgroup STM32_AUDIO_PLAYER
  * @brief This file is the header file for the stm32 audio player module 
  * @{
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Exported_Defines
  * @{
  */ 
/* Use this define to inject an error on the reference clock output to see how 
   synchronization mechanims adjust its value accordingly. 
   To remove injected error set this define to 0.
   Maximum allowed error is 500ppm ==> 0x284. 
   NOTE: don't forget that even when this parameter is set to 0, there shall be 
   an intrinsic error on the output frequency of the clock synthesize due to:
     - Reference clock accuracy (in this case the MCO is used as reference clock).
     - Prescalers calculation floating inaccuracy (in ratio caculation).
     - Variations due to temperature. 
     - Intrinsic clock error on the host clock. */
#define DEFAULT_INJECTED_ERROR                 0 /* 0x284 */

/* Value of the default ratio when reference clock is 40MHz and output 
   clock is 12.288MHz 

   You can add an error to this value to verify how correction 
   mechanism adjust it to the correct value. Use DEFAULT_INJECTED_ERROR define.
   Maximum supported error is 500 ppm. 

   RATIO = (OUTPUT_CLK / INPUT_CLK) * 2^20 
*/
#define DEFAULT_RATIO_12_288           (uint32_t)(0x0013A92A + DEFAULT_INJECTED_ERROR) /* Input= 40MHz, output = 12.288MHz * 4 */
#define DEFAULT_RATIO_11_2896          (uint32_t)(0x00121038 + DEFAULT_INJECTED_ERROR) /* Input= 40MHz, output = 11.2896MHz * 4 */ 
#define DEFAULT_RATIO_11_264           (uint32_t)(0x001205BC + DEFAULT_INJECTED_ERROR) /* Input= 40MHz, output = 11.264MHz * 4 */ 


/* Direction of the frequency update */
#define EXTCLK_DIR_UP                   0
#define EXTCLK_DIR_DOWN                 1

/* Registers mapping description */
#define CS_DEVICE_ID                    0x01
#define CS_DEVICE_CTRL                  0x02
#define CS_DEVICE_CFG1                  0x03
#define CS_GLOBAL_CFG                   0x05
#define CS_RATIO_3                      0x06 /* MSB byte of Ratio */
#define CS_RATIO_2                      0x07 /* MSB - 1 byte of Ratio */
#define CS_RATIO_1                      0x08 /* MSB - 2 byte of Ratio */
#define CS_RATIO_0                      0x09 /* LSB byte of Ratio */
#define CS_FUNCT_CFG1                   0x16
#define CS_FUNCT_CFG2                   0x17

/* Chip address 0x9C (when AD0 pin is connected to Gnd) */
#define CLK_SYNTH_ADDRESS               0x9C

/* I2C peripheral configuration defines (control interface) */
#define CLK_SYNTH_I2C                   I2C1
#define CLK_SYNTH_I2C_CLK               RCC_APB1Periph_I2C1
#define CLK_SYNTH_I2C_GPIO_CLOCK        RCC_AHB1Periph_GPIOB
#define CLK_SYNTH_I2C_GPIO_AF           GPIO_AF_I2C1
#define CLK_SYNTH_I2C_GPIO              GPIOB
#define CLK_SYNTH_I2C_SCL_PIN           GPIO_Pin_6
#define CLK_SYNTH_I2C_SDA_PIN           GPIO_Pin_9
#define CLK_SYNTH_I2C_SCL_PINSRC        GPIO_PinSource6
#define CLK_SYNTH_I2C_SDA_PINSRC        GPIO_PinSource9

#define CLK_SYNTH_I2C_DMA               DMA1
#define CLK_SYNTH_I2C_DMA_CHANNEL       DMA_Channel_1
#define CLK_SYNTH_I2C_DR_ADDRESS        ((uint32_t)0x40005410)
#define CLK_SYNTH_I2C_DMA_STREAM_TX     DMA1_Stream6
#define CLK_SYNTH_I2C_DMA_STREAM_RX     DMA1_Stream0
#define CLK_SYNTH_I2C_TX_DMA_TCFLAG     DMA_FLAG_TCIF6
#define CLK_SYNTH_I2C_TX_DMA_FEIFLAG    DMA_FLAG_FEIF6
#define CLK_SYNTH_I2C_TX_DMA_DMEIFLAG   DMA_FLAG_DMEIF6
#define CLK_SYNTH_I2C_TX_DMA_TEIFLAG    DMA_FLAG_TEIF6
#define CLK_SYNTH_I2C_TX_DMA_HTIFLAG    DMA_FLAG_HTIF6
#define CLK_SYNTH_I2C_RX_DMA_TCFLAG     DMA_FLAG_TCIF0
#define CLK_SYNTH_I2C_RX_DMA_FEIFLAG    DMA_FLAG_FEIF0
#define CLK_SYNTH_I2C_RX_DMA_DMEIFLAG   DMA_FLAG_DMEIF0
#define CLK_SYNTH_I2C_RX_DMA_TEIFLAG    DMA_FLAG_TEIF0
#define CLK_SYNTH_I2C_RX_DMA_HTIFLAG    DMA_FLAG_HTIF0
#define DMAx_CLK                        RCC_AHB1Periph_DMA1 
#define CLK_SYNTH_I2C_DMA_TX_IRQn       DMA1_Stream6_IRQn
#define CLK_SYNTH_I2C_DMA_TX_IRQHandler DMA1_Stream6_IRQHandler

#define CLK_SYNTH_I2C_MAX_SIZE          5

/* Uncomment this define to use the I2C DMA TC interrupt when sending a buffer of data */
#define USE_I2C_DMA_TC_IT

/* Interrupt priority for the I2C communication with the CS2200 device 
   These defines may be set in a generic configuration file (ie. audio_app_conf.h)*/
/* 
#define STM32_CLK_SYNTH_PRE_PRIO        3
#define STM32_CLK_SYNTH_SUB_PRIO        3
*/

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define CLK_SYNTH_FLAG_TIMEOUT          ((uint32_t)0x1000)
#define CLK_SYNTH_LONG_TIMEOUT          ((uint32_t)(300 * CLK_SYNTH_FLAG_TIMEOUT))

/* Uncomment this line to enable verifying data sent to codec after each write 
  operation */
/* #define VERIFY_WRITTENDATA */

/* Current state defines */
#define STATE_NOTINITED                 0
#define STATE_INITED                    1

/* This is the reference clock input for the external clock synthesizer
   It should be generated from the STM32 MCO IO.*/
#define I2S_EXT_REF_CLK                 40000000 

/* I2C clock speed configuration (in Hz) 
  WARNING: 
   Make sure that this define is not already declared in other files (ie. 
  stm324xg_eval.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
 #define I2C_SPEED                      100000
#endif /* I2C_SPEED */

#if (I2C_SPEED > 100000)
 #error "Error: Clock Synthesizer CS2200 does not support I2C freq higher than 100KHz !"
#endif
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 


/** @defgroup STM32_AUDIO_PLAYER_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_AUDIO_PLAYER_Exported_FunctionsPrototype
  * @{
  */
uint32_t CLK_SYNTH_Init       (uint32_t OutFreq, uint32_t InFreq, uint32_t InitCtrlPort, uint32_t Opt);
uint32_t CLK_SYNTH_DeInit     (void);
uint32_t CLK_SYNTH_AdjustPpm  (uint32_t Ppm, uint32_t Direction);
uint32_t CLK_SYNTH_UpdateFreq (uint32_t Freq, uint32_t opt1, uint32_t opt2, uint32_t opt3);

/* CLK_SYNTH_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occurs during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...) on the Codec control interface (I2C).
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t CLK_SYNTH_TIMEOUT_UserCallback(void);
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#endif /* __CLK_SYNTHESIZER_CS2200_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
