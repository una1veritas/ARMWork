/**
  ******************************************************************************
  * @file    I2C/I2C_WakeUpFromStop/main.c 
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

/** @addtogroup I2C_WakeUpFromStop
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t BufferTX[]= "STM32 CPAL FIRMWARE DRIVER FOR I2Cx DEVICE: WAKEUP FROM STOP MODE EXAMPLE";
uint8_t BufferRX[BUFFSIZE];
CPAL_TransferTypeDef  sRxStructure, sTxStructure;
uint32_t BufferSize = BUFFSIZE;
#ifdef I2C_SLAVE  
 GPIO_InitTypeDef GPIO_InitStructure;
#endif /* I2C_SLAVE */
__IO TestStatus TransferStatus = PASSED;

/* Private function prototypes -----------------------------------------------*/
static void RCC_Config(void);
static __IO uint32_t TimingDelay;
#ifdef I2C_SLAVE  
 static TestStatus Compare_bBuffer(uint8_t* pBuffer, uint8_t* pBuffer1, uint32_t BufferLength);
 static void Reset_bBuffer(uint8_t *pBuffer, uint32_t BufferLenght);
 static void RestoreConfiguration(void);
#endif /* I2C_SLAVE */
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
 
  /* Configure Clocks */
  RCC_Config();

  /* Initialize LEDs, Key Button and LCD available on
  STM32303C-EVAL board *****************************************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Initialize the LCD */
  STM32303C_LCD_Init();

  /* Display message on  LCD ***********************************************/
  /* Clear the LCD */ 
  LCD_Clear(White);  
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  /* Set the LCD Text Color */
  LCD_SetTextColor(Yellow);
  LCD_DisplayStringLine(Line0, (uint8_t*)MESSAGE1);
  LCD_DisplayStringLine(Line1, (uint8_t*)MESSAGE2);
  /* Set the LCD Back Color */
  LCD_SetBackColor(White);
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue);

  /* Configure the Push buttons in Polling mode */
  STM_EVAL_PBInit(BUTTON_KEY, Mode_GPIO);

  /* if STM32 device is set as Master */
#ifdef I2C_MASTER

  /* Configure and enable the systick timer to generate an interrupt each 1 ms */
  SysTick_Config((SystemCoreClock / 1000));

  /* Deinitialize I2Cx Device */ 
  CPAL_I2C_DeInit(&MASTERSTRUCTURE); 

  /* Initialize CPAL I2C structure parameters values */
  CPAL_I2C_StructInit(&MASTERSTRUCTURE);
  MASTERSTRUCTURE.wCPAL_Options =  CPAL_OPT_NO_MEM_ADDR;

#ifdef CPAL_I2C_DMA_PROGMODEL
  MASTERSTRUCTURE.CPAL_ProgModel = CPAL_PROGMODEL_DMA;
#elif defined (CPAL_I2C_IT_PROGMODEL)
  MASTERSTRUCTURE.CPAL_ProgModel = CPAL_PROGMODEL_INTERRUPT;
#else
 #error "Please select one of the programming model (in main.h)"
#endif

  /* Set I2C Speed */
  MASTERSTRUCTURE.pCPAL_I2C_Struct->I2C_Timing = MASTER_I2C_TIMING;

  /* Select Master Mode */
  MASTERSTRUCTURE.CPAL_Mode = CPAL_MODE_MASTER; 

  /* Initialize I2Cx Device*/
  CPAL_I2C_Init(&MASTERSTRUCTURE); 

  /* Infinite loop */
  while(1)
  {
    /* Initialize Transfer parameters */
    MASTERSTRUCTURE.pCPAL_TransferTx = &sTxStructure;
    sTxStructure.wNumData = BufferSize;
    sTxStructure.pbBuffer = (uint8_t*)BufferTX;
    sTxStructure.wAddr1 = OWNADDRESS;

    /* Update LCD Display */
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue);    
    LCD_DisplayStringLine(Line8, (uint8_t*)MEASSAGE_EMPTY);
    LCD_DisplayStringLine(Line5, (uint8_t*)MESSAGE4);
    LCD_DisplayStringLine(Line6, (uint8_t*)MESSAGE5);

    /* wait until Key button is pushed */
    while(STM_EVAL_PBGetState(BUTTON_KEY));

    /* Update LCD Display */
    LCD_DisplayStringLine(Line5, (uint8_t*)MEASSAGE_EMPTY);
    LCD_DisplayStringLine(Line6, (uint8_t*)MEASSAGE_EMPTY);

    /* Write operation */
    CPAL_I2C_Write(&MASTERSTRUCTURE);

    /* Wait until communication finishes */
    while ((MASTERSTRUCTURE.CPAL_State != CPAL_STATE_READY) && (MASTERSTRUCTURE.CPAL_State != CPAL_STATE_ERROR));

    if (TransferStatus == PASSED)
    {
      /* Update LCD Display */
      LCD_SetBackColor(Red);
      LCD_SetTextColor(White);
      LCD_DisplayStringLine(Line8, (uint8_t*)MESSAGE6);
    }
    else
    {
      TransferStatus = PASSED;
    }

    Delay(1000);
  }
#endif /* I2C_MASTER */

  /* if STM32 device is set as Slave */  
#ifdef I2C_SLAVE

  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Output System Clock on MCO pin (PA.08) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_MCOConfig(RCC_MCOSource_SYSCLK);

  /* Deinitialize I2Cx Device */ 
  CPAL_I2C_DeInit(&SLAVESTRUCTURE); 

  /* Initialize CPAL I2C structure parameters values */
  CPAL_I2C_StructInit(&SLAVESTRUCTURE);
  SLAVESTRUCTURE.wCPAL_Options = CPAL_OPT_I2C_NACK_ADD | CPAL_OPT_I2C_WAKEUP_STOP;
  
#ifdef CPAL_I2C_DMA_PROGMODEL
  SLAVESTRUCTURE.CPAL_ProgModel = CPAL_PROGMODEL_DMA;
#elif defined (CPAL_I2C_IT_PROGMODEL)
  SLAVESTRUCTURE.CPAL_ProgModel = CPAL_PROGMODEL_INTERRUPT;
#else
 #error "Please select one of the programming model (in main.h)"
#endif

  /* Configure Own address 1 */
  SLAVESTRUCTURE.pCPAL_I2C_Struct->I2C_OwnAddress1 = OWNADDRESS;

  /* Set I2C Speed */
  SLAVESTRUCTURE.pCPAL_I2C_Struct->I2C_Timing = SLAVE_I2C_TIMING;

  /* Select Slave Mode */ 
  SLAVESTRUCTURE.CPAL_Mode = CPAL_MODE_SLAVE; 

  /* Initialize I2Cx Device*/
  CPAL_I2C_Init(&SLAVESTRUCTURE);

  /* Infinite loop */
  while(1)
  {
    /* Reset BufferRX value */
    Reset_bBuffer(BufferRX, BufferSize);

    /* Initialize Transfer parameters */
    SLAVESTRUCTURE.pCPAL_TransferRx = &sRxStructure;
    sRxStructure.wNumData = BufferSize;
    sRxStructure.pbBuffer = (uint8_t*)BufferRX;

    /* Update LCD Display */
    LCD_SetBackColor(White);
    LCD_SetTextColor(Blue);
    LCD_DisplayStringLine(Line8, (uint8_t*)MEASSAGE_EMPTY);
    LCD_DisplayStringLine(Line9, (uint8_t*)MEASSAGE_EMPTY);
    LCD_DisplayStringLine(Line5, (uint8_t*)MESSAGE7);

    Delay(1000);

    /* Update LCD Display */
    LCD_DisplayStringLine(Line5, (uint8_t*)MEASSAGE_EMPTY);
    LCD_DisplayStringLine(Line6, (uint8_t*)MESSAGE8);
    
    /* Read operation */
    CPAL_I2C_Read(&SLAVESTRUCTURE);  

    /* Enter Stop Mode and wait for interrupt to wake up */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    /* Wait until communication finishes */
    while ((SLAVESTRUCTURE.CPAL_State != CPAL_STATE_READY) && (SLAVESTRUCTURE.CPAL_State != CPAL_STATE_ERROR));

    /* Configure SystemClock*/
    RestoreConfiguration();

    /* Configure and enable the systick timer to generate an interrupt each 1 ms */
    SysTick_Config((SystemCoreClock / 1000));

    /* Update LCD Display */
    LCD_DisplayStringLine(Line6, (uint8_t*)MEASSAGE_EMPTY);
    LCD_SetBackColor(Red);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(Line8, (uint8_t*)MESSAGE9);

    /* If are received correctly */
    if (Compare_bBuffer((uint8_t*)BufferTX, BufferRX, BufferSize) == PASSED )
    {
      /* Update LCD Display */
      LCD_DisplayStringLine(Line9, (uint8_t*)MESSAGE6);
    }
    else
    {
      /* Update LCD Display */
      LCD_DisplayStringLine(Line9, (uint8_t*)MESSAGE10);
    }

    Delay(1500);
  }
#endif /* I2C_SLAVE */
}


/**
  * @brief  Configure the I2C Clock source and Power clock
  * @param  None
  * @retval None
  */
static void RCC_Config(void)
{
  /* Configure the I2C2 Clock Source */
  RCC_I2CCLKConfig(RCC_I2C2CLK_HSI);
  
  /* Enable PWR APB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

#ifdef I2C_SLAVE  
/**
  * @brief  Restore peripheral config before entering STOP mode.
  * @param  None
  * @retval None
  */
static void RestoreConfiguration(void)
{
  __IO uint32_t HSEStatus = 0;
  
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

/**
  * @brief  Compare two 8-bit buffers and returns the comparison result
  * @param  pBuffer: pointer to the source buffer to be compared to.
  * @param  pBuffer1: pointer to the second source buffer to be compared to the first.
  * @param  BufferLength: size of buffer to compare.
  * @retval PASSED: Comparison is OK (the two Buffers are identical)
  *         FAILED: Comparison is NOK (Buffers are different)
  */
static TestStatus Compare_bBuffer(uint8_t* pBuffer, uint8_t* pBuffer1, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer != *pBuffer1)
    {
      return FAILED;
    }

    pBuffer++;
    pBuffer1++;
  }

  return PASSED;
}

/**
  * @brief  Fill Buffer with zero value
  * @param  pBuffer: pointer on the Buffer to fill.
  * @param  BufferLength: size of buffer to Fill.
  * @retval None
  */

static void Reset_bBuffer(uint8_t *pBuffer, uint32_t BufferLenght)
{
  uint32_t indextmp = 0;

  /* Put in global buffer same values */
  for ( indextmp = 0; indextmp < BufferLenght; indextmp++ )
  {
    pBuffer[indextmp] = 0;
  }
}
#endif /* I2C_SLAVE */

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
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
