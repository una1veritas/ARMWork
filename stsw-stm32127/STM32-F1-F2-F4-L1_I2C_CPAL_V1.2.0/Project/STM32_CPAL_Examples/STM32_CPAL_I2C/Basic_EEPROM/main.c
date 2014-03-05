/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Basic_EEPROM/main.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
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

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/

#define BUFFER1_SIZE             (countof(Tx1Buffer)-1)
#define BUFFER2_SIZE             (countof(Tx2Buffer)-1)

#define EEPROM_READ_ADDR1               ((uint16_t)0x0010)
#define EEPROM_WRITE_ADDR1              ((uint16_t)0x0010)
#define EEPROM_READ_ADDR2               ((uint16_t)0x00FF)
#define EEPROM_WRITE_ADDR2              ((uint16_t)0x00FF)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
const uint8_t Tx1Buffer[] = "A";
const uint8_t Tx2Buffer[] = "B";

uint8_t Rx1Buffer[BUFFER1_SIZE];
uint8_t Rx2Buffer[BUFFER2_SIZE];
/* Private function prototypes -----------------------------------------------*/
		
/* Private functions ---------------------------------------------------------*/
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength); 
static void TIM7_Config(void); 


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{    
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32xxx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32xxx.c file
  */
  
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /*----------------- TIMER Configuration -----------------*/
  TIM7_Config();  
  
  /*------------- Initialize sEE_DevStructure -------------*/
  sEE_DevStructure.sEEAddress = EEPROM_ADDRESS;
  sEE_DevStructure.sEEPageSize = EEPROM_PAGE_SIZE;
  sEE_DevStructure.sEEMemoryAddrMode = sEE_OPT_16BIT_REG;
  
  /*-------------- Initialize sEE peripheral -------------*/
  sEE_StructInit(&sEE_DevStructure);
  sEE_Init(&sEE_DevStructure); 
  
  /*---------------------  Phase1 : Write data from Tx1_Buffer
  then Read data in Rx1_Buffer -----------------------------*/
  
  /* Write Data in EEPROM */
  sEE_WriteBuffer(&sEE_DevStructure, (uint8_t*)Tx1Buffer, EEPROM_WRITE_ADDR1, BUFFER1_SIZE);
  
  /* Wail until communication is complete */
  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
  { 
    /* Application may perform other tasks while CPAL write operation is ongoing */
  }	 
  
  /* Read Data from EEPROM */
  sEE_ReadBuffer(&sEE_DevStructure, Rx1Buffer, EEPROM_READ_ADDR1, BUFFER1_SIZE);
  
  /* Wail until communication is complete */
  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
  {
    /* Application may perform other tasks while CPAL read operation is ongoing */
  }
  
  /* if Error Occurred reset sEE state */
  if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
  {
    sEE_DevStructure.sEEState = sEE_STATE_IDLE;
  }
  
  /*---------------------  Phase2 : Write data from Tx2_Buffer
  then Read data in Rx2_Buffer -----------------------------*/
  
  /* Write Data in EEPROM */
  sEE_WriteBuffer(&sEE_DevStructure, (uint8_t*)Tx2Buffer, EEPROM_WRITE_ADDR2, BUFFER2_SIZE);
  
  /* Wail until communication is complete */
  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
  { 
    /* Application may perform other tasks while CPAL write operation is ongoing */	   
  }
  
  /* Read Data from EEPROM */
  sEE_ReadBuffer(&sEE_DevStructure, Rx2Buffer, EEPROM_READ_ADDR2, BUFFER2_SIZE);
  
  /* Wail until communication is complete */
  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
  {
    /* Application may perform other tasks while CPAL read operation is ongoing */      
  }
  
  /* if Error Occurred reset sEE state */
  if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
  {
    sEE_DevStructure.sEEState = sEE_STATE_IDLE;
  }
  
  /* Turn all leds off */
  STM_EVAL_LEDOff(LED1);
  STM_EVAL_LEDOff(LED2);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED4);
  
  /*----------------- Test transfer phase 1 -----------------*/
  
  if (Buffercmp ((uint8_t*)Tx1Buffer,Rx1Buffer,BUFFER1_SIZE) == PASSED )
  {
    /* Turn on LED1 if Transfer1 Passed */
    STM_EVAL_LEDOn(LED1);
  }
  else
  {
    /* Turn on LED3 if Transfer1 Failed */
    STM_EVAL_LEDOn(LED3);
  }
  
  /*----------------- Test transfer phase 2 -----------------*/
  
  if (Buffercmp ((uint8_t*)Tx2Buffer,Rx2Buffer,BUFFER2_SIZE) == PASSED )
  {
    /* Turn on LED2 if Transfer2 Passed */
    STM_EVAL_LEDOn(LED2);	    
  }
  else
  { 
    /* Turn on LED3 if Transfer2 Failed */
    STM_EVAL_LEDOn(LED3);
  } 
  
  /* Deinitialize peripheral */
  sEE_DeInit(&sEE_DevStructure);
  
  while(1)
  { 
    
  }  
}		  

/**
  * @brief  Configures TIM7 and associated resources to generate an update 
  *         interrupt each 50 ms.
  * @param  None
  * @retval None
  */
static void TIM7_Config(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* TIM7 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);
  
  /* Enable the TIM7 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure TIM7 to generate interrupt each 50ms */
  TIM_TimeBaseStructure.TIM_Period  = 50000;
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/1000000)-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  /* TIM7 IT enable */
  TIM_ITConfig(TIM7, TIM_IT_Update , ENABLE);
  
  /* TIM7 enable counter */
  TIM_Cmd(TIM7, ENABLE);  
}
	

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}
		
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
