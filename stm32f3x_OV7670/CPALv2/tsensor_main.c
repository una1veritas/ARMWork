/**
  ******************************************************************************
  * @file    I2C/I2C_TSENSOR/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
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

/** @addtogroup STM32F37x_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_TSENSOR
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TEMPERATURE_THYS     31
#define TEMPERATURE_TOS      32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t SMbusAlertOccurred = 0;

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Config(void);

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
  file (startup_stm32f37x.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f37x.c file
  */
  uint8_t LCDStr[20]= {0};
  float temperaturecelsius = 0;
  int16_t tempvalue = 0;
  
  /* NVIC Configuration */
  NVIC_Config();
  
  /* Initialize the LCD */
  STM32373C_LCD_Init();
  
  /* Initialize the Temperature Sensor */
  LM75_Init();
  
  if (LM75_GetStatus() == SUCCESS)
  {
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);
    
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_GREEN);
    
    LCD_DisplayStringLine(LCD_LINE_0, (uint8_t*)"     Temperature    ");
    
    /* Configure the Temperature sensor device STLM75:
    - Thermostat mode Interrupt
    - Fault tolerance: 00
    */
    LM75_WriteConfReg(0x02);
    
    /* Configure the THYS and TOS in order to use the SMbus alert interrupt */
    LM75_WriteReg(LM75_REG_THYS, TEMPERATURE_THYS << 8);  /*31°C*/
    LM75_WriteReg(LM75_REG_TOS, TEMPERATURE_TOS << 8);   /*32°C*/
    
    /* Enables the I2C SMBus Alert feature */
    I2C_SMBusAlertCmd(LM75_I2C, ENABLE);    
    I2C_ClearFlag(LM75_I2C, I2C_FLAG_ALERT);
    
    SMbusAlertOccurred = 0;
    
    /* Enable SMBus Alert interrupt */
    I2C_ITConfig(LM75_I2C, I2C_IT_ERRI, ENABLE);
    
    /* Infinite Loop */
    while (1)
    {
       if (LM75_GetStatus() == SUCCESS)
       {
         /* Get temperature value */     
         tempvalue = LM75_ReadTemp()<<7;
         temperaturecelsius = tempvalue/256.0f;
         
         /* Set the Back Color */
         LCD_SetBackColor(LCD_COLOR_BLUE);
         /* Set the Text Color */
         LCD_SetTextColor(LCD_COLOR_GREEN);
         /* Display temperature value in celsius */
         sprintf((char*)LCDStr, "       %2.1f C       ", temperaturecelsius);
         LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)LCDStr);
         /* Display temperature value in fahrenheit */
         sprintf((char*)LCDStr, "       %2.1f F       ", 1.8f * temperaturecelsius + 32);
         LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)LCDStr);
          
          if (SMbusAlertOccurred == 1)
          {
            /* Set the Back Color */
            LCD_SetBackColor(LCD_COLOR_BLUE);
            /* Set the Text Color */
            LCD_SetTextColor(LCD_COLOR_RED);
            LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)"Warning: Temp exceed");
            LCD_DisplayStringLine(LCD_LINE_3, (uint8_t*)"        32 C        ");
          }
          if (SMbusAlertOccurred == 2)
          {
            /* Set the Back Color */
            LCD_SetBackColor(LCD_COLOR_WHITE);
            /* Set the Text Color */
            LCD_SetTextColor(LCD_COLOR_WHITE);
            LCD_ClearLine(LCD_LINE_2);
            LCD_ClearLine(LCD_LINE_3);
            SMbusAlertOccurred = 0;
            /* Set the Back Color */
            LCD_SetBackColor(LCD_COLOR_BLUE);
            /* Set the Text Color */
            LCD_SetTextColor(LCD_COLOR_GREEN);
          }
        }
        else
        {
          /* Set the Back Color */
          LCD_SetBackColor(LCD_COLOR_WHITE);
          /* Set the Text Color */
          LCD_SetTextColor(LCD_COLOR_RED);
          LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)"Communication fails ");
          LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"Check JP4 and JP5   "); 
       }
    }
  }
  else
  {
    LCD_Clear(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)" LM75 not correctly ");
    LCD_DisplayStringLine(LCD_LINE_3, (uint8_t*)" initialized...     ");
    LCD_DisplayStringLine(LCD_LINE_4, (uint8_t*)" Please restart the ");
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t*)" example.           ");   
    /* Infinite Loop */
    while(1)
    {
    }
  }
}

/**
  * @brief  Configures the different interrupt.
  * @param  None
  * @retval None
  */
static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Reconfigure and enable I2C2 error interrupt to have the higher priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
