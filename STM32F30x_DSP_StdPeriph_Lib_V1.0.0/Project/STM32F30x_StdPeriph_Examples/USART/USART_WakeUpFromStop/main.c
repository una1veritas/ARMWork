/**
  ******************************************************************************
  * @file    USART/USART_WakeUpFromStop/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Main program body
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
#include "main.h"

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_WakeUpFromStop
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t DataReceived = 0;
extern __IO uint32_t InterruptCounter;

/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
static void RestoreConfiguration(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */
    
  /* Initialize LEDs available on STM32303C-EVAL board ************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Enable PWR APB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* USART configuration to Wake up from STOP mode by Start bit Method */
  USART_Config();

  /* Waiting Wake Up interrupt */
  while(InterruptCounter == 0x00)
  {}
  
  /* Disable USART peripheral in STOP mode */ 
  USART_STOPModeCmd(USART1, DISABLE);
  
  while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
  {}
  DataReceived = USART_ReceiveData(USART1);
  
  /* Clear the TE bit (if a transmission is on going or a data is in the TDR, it will be sent before
  efectivelly disabling the transmission) */
  USART_DirectionModeCmd(USART1, USART_Mode_Tx, DISABLE);
  
  /* Check the Transfer Complete Flag */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  
  /* USART Disable */
  USART_Cmd(USART1, DISABLE);
    
  /* Configure SystemClock*/
  RestoreConfiguration();
  
  /* Configure and enable the systick timer to generate an interrupt each 1 ms */
  SysTick_Config((SystemCoreClock / 1000));
  
  while(1)
  {}
}

/**
  * @brief Configure the USART to Wake up from STOP mode by Start bit Method
  * @param  None
  * @retval None
  */
static void USART_Config(void)
{ 
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable GPIOD clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOE , ENABLE);
  
  /* Enable USART1 APB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* Configure the HSI as USART clock */
  RCC_USARTCLKConfig(RCC_USART1CLK_HSI);
  
  /* USART1 Pins configuration ***********************************************/

  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);    
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_7); 
  
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
   GPIO_Init(GPIOE, &GPIO_InitStructure);
   
  /* USARTx configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  
  USART_DeInit(USART1);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
  /* USART2 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure the wake up Method = Start bit */ 
  USART_StopModeWakeUpSourceConfig(USART1, USART_WakeUpSource_StartBit);
  
  /* Enable USART1 */ 
  USART_Cmd(USART1, ENABLE);
  
  /* Before entering the USART in STOP mode the REACK flag must be checked to ensure the USART RX is ready */
  while(USART_GetFlagStatus(USART1, USART_FLAG_REACK) == RESET)
  {}
  
  /* Enable USART STOP mode by setting the UESM bit in the CR1 register.*/
  USART_STOPModeCmd(USART1, ENABLE);
  
  /* Enable the wake up from stop Interrupt */ 
  USART_ITConfig(USART1, USART_IT_WU, ENABLE);   
  
  /* Enter USART in STOP mode with regulator in low power mode */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
  
}

/**
  * @brief  Restore peripheral config before entering STOP mode.
  * @param  None
  * @retval None
  */
static void RestoreConfiguration(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/    
  /* Enable HSE */    
  RCC_HSEConfig(RCC_HSE_ON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  HSEStatus = RCC_WaitForHSEStartUp();

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH_SetLatency(FLASH_Latency_1);
    
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);
    
    /*  PLL configuration:  = HSE *  9 = 72 MHz */
    RCC_PREDIV1Config(RCC_PREDIV1_Div1);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
    
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    
    /* PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  } 
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

