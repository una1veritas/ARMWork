/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/SPI_DataExchangeInterrupt/main.h
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
#include "stm32f37x_it.h"

/* Exported typedef ----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Exported define -----------------------------------------------------------*/

/* Uncomment the line below if you will use the SPI peripheral as a Master */
//#define SPI_MASTER 
/* Uncomment the line below if you will use the SPI peripheral as a Slave */
#define SPI_SLAVE

/* Uncomment the size of data to be transmetted (only one data size must be selected) */
//#define SPI_DATASIZE_8
//#define SPI_DATASIZE_7
//#define SPI_DATASIZE_6
#define SPI_DATASIZE_5


/* USER_TIMEOUT value for waiting loops. This timeout is just guarantee that the
   application will not remain stuck if the USART communication is corrupted. 
   You may modify this timeout value depending on CPU frequency and application
   conditions (interrupts routines, number of data to transfer, baudrate, CPU
   frequency...). */ 
#define USER_TIMEOUT                    ((uint32_t)0x64) /* Waiting 1s */

/* Communication boards SPIx Interface */
#define SPIx                             SPI2
#define SPIx_CLK                         RCC_APB1Periph_SPI2
#define SPIx_IRQn                        SPI2_IRQn

#define SPIx_SCK_PIN                     GPIO_Pin_8
#define SPIx_SCK_GPIO_PORT               GPIOD
#define SPIx_SCK_GPIO_CLK                RCC_AHBPeriph_GPIOD
#define SPIx_SCK_SOURCE                  GPIO_PinSource8
#define SPIx_SCK_AF                      GPIO_AF_5

#define SPIx_MISO_PIN                    GPIO_Pin_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_GPIO_CLK               RCC_AHBPeriph_GPIOB
#define SPIx_MISO_SOURCE                 GPIO_PinSource14
#define SPIx_MISO_AF                     GPIO_AF_5

#define SPIx_MOSI_PIN                    GPIO_Pin_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_GPIO_CLK               RCC_AHBPeriph_GPIOB
#define SPIx_MOSI_SOURCE                 GPIO_PinSource15
#define SPIx_MOSI_AF                     GPIO_AF_5

#define TXBUFFERSIZE                     (countof(TxBuffer) - 1)
#define RXBUFFERSIZE                     TXBUFFERSIZE

/* Joystick Commands */
#define CMD_RIGHT                        0x15
#define CMD_LEFT                         0x1A
#define CMD_UP                           0x13
#define CMD_DOWN                         0x1C
#define CMD_SEL                          0x1F 

#define CMD_ACK                          0x16 

/* Define numbers of bytes to transmit from TxBuffer */
#define DATA_SIZE                        TXBUFFERSIZE

/* Define data size and data masks */
#ifdef SPI_DATASIZE_8
 #define SPI_DATASIZE                     SPI_DataSize_8b
 #define SPI_DATAMASK                     (uint8_t)0xFF
#elif defined (SPI_DATASIZE_7)
 #define SPI_DATASIZE                     SPI_DataSize_7b
 #define SPI_DATAMASK                     (uint8_t)0x7F
#elif defined (SPI_DATASIZE_6)
 #define SPI_DATASIZE                     SPI_DataSize_6b
 #define SPI_DATAMASK                     (uint8_t)0x3F
#elif defined (SPI_DATASIZE_5)
 #define SPI_DATASIZE                     SPI_DataSize_5b
 #define SPI_DATAMASK                     (uint8_t)0x1F
#else
 #error " Select at least one data size in main.h file " 
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t GetVar_NbrOfData(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
