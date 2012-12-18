/**
  ******************************************************************************
  * @file    I2C/I2C_TSENSOR/main.c 
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

/** @addtogroup I2C_TSENSOR
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TEMPERATURE_THYS        0x14 /* 20°C */
#define TEMPERATURE_TOS         0x28 /* 40°C */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t TempCelsiusDisplay[] = "    +abc.dddd C     ";
uint8_t TempFahrenheitDisplay[] = "    +abc.dddd F     ";
int32_t TempValue = 0, TempValueCelsius = 0, TempValueFahrenheit = 0;
__IO uint32_t SMbusAlertOccurred = 0;
uint32_t TempCelsius = 0;
float TempValueCelsiusFloat = 0.0f;
float TempValueFahrenheitFloat = 0.0f;
float TempFahrenheit = 0.0f;
uint8_t Tmp=0, Var = 0;
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
  file (startup_stm32f30x.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f30x.c file
  */
  
  uint32_t i = 0;
  
  /* Initialize the LCD */
  STM32303C_LCD_Init();
  
  /* Initialize the Temperature Sensor */
  TS751_Init();
  
  if (TS751_GetStatus() == SUCCESS)
  {
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);
    
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_GREEN);
    
    LCD_DisplayStringLine(LCD_LINE_0, "     Temperature    ");
    LCD_DisplayStringLine(LCD_LINE_8, " Check JP1 closed   ");
    
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_WHITE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_BLACK);
    
    /* NVIC Configuration */
    NVIC_Config();
  
    /* Enables the I2C SMBus Alert feature */
    I2C_SMBusAlertCmd(I2C2, ENABLE);
    
    I2C_ClearFlag(I2C2, I2C_FLAG_ALERT);
    
    SMbusAlertOccurred = 0;
    
    /* Enable SMBus Alert interrupt */
    I2C_ITConfig(I2C2, I2C_IT_ERRI, ENABLE);
    
    /* Configure the Temperature sensor device STTS751 */
    TS751_WriteConfReg(0x0C);    
    
    /* Configure the Temperature Therm limit as 40°C */
    TS751_WriteReg(0x05, TEMPERATURE_TOS);   
    TS751_WriteReg(0x20, TEMPERATURE_TOS);   

    /* Configure the Temperature Thys limit as 20°C */
    TS751_WriteReg(0x07, TEMPERATURE_THYS);   
    TS751_WriteReg(0x21, TEMPERATURE_THYS);
    
    /* Infinite Loop */
    while (1)
    {
      /* Get double of Temperature value */
      TempValue = TS751_ReadTemp();
      
      if (TempValue <= 2048)
      {
        /* Positive temperature measured */
        TempCelsiusDisplay[4] = '+';
        /* Initialize the temperature sensor value */
        TempValueCelsius = TempValue;
      }
      else
      {
        /* Negative temperature measured */
        TempCelsiusDisplay[4] = '-';
        /* Remove temperature value sign */
        TempValueCelsius = 0x1000 - TempValue;
      }
      
      TempCelsius = 0;
      
      /* Calculate temperature digits in ÝC */
      if (TempValueCelsius & 0x01)
      {
        TempCelsius += 625;     
        
      }
      if (TempValueCelsius & 0x02)
      {
        TempCelsius += 1250;
        
      }
      if (TempValueCelsius & 0x04)
      {
        TempCelsius += 2500;
      }
      if (TempValueCelsius & 0x08)
      {
        TempCelsius += 5000;
      }
      
      TempCelsiusDisplay[9] = (TempCelsius / 1000) + 0x30;
      TempCelsiusDisplay[10] = ((TempCelsius % 1000) / 100) + 0x30;
      TempCelsiusDisplay[11] = (((TempCelsius % 1000) % 100) / 10)+ 0x30;
      TempCelsiusDisplay[12] = (((TempCelsius % 1000) % 100) % 10) + 0x30;
      
      TempValueCelsius >>= 4;

      TempCelsiusDisplay[5] = (TempValueCelsius / 100) + 0x30;
      TempCelsiusDisplay[6] = ((TempValueCelsius % 100) / 10) + 0x30;
      TempCelsiusDisplay[7] = ((TempValueCelsius % 100) % 10) + 0x30;
      
      TempValueCelsiusFloat = TempValueCelsius + (float) (TempCelsius/10000.0);
      
      if (TempValue > 2048)
      {
        if (((9 * TempValueCelsiusFloat) / 5) <= 32)
        {
          /* Convert temperature °C to Fahrenheit */
          TempValueFahrenheitFloat = abs ((int)(32 - ((9 * TempValueCelsiusFloat) / 5)));
          
          TempValueFahrenheit = (int) (TempValueFahrenheitFloat);
          
          /* Calculate temperature digits in °F */
          TempFahrenheitDisplay[5] = (TempValueFahrenheit / 100) + 0x30;
          TempFahrenheitDisplay[6] = ((TempValueFahrenheit % 100) / 10) + 0x30;
          TempFahrenheitDisplay[7] = ((TempValueFahrenheit % 100) % 10) + 0x30;
          /* Positive temperature measured */
          TempFahrenheitDisplay[4] = '+';
          
          TempFahrenheit = TempValueFahrenheitFloat - TempValueFahrenheit;
          
          TempFahrenheitDisplay[9] =  (int)(TempFahrenheit * 10) + 0x30;
          TempFahrenheitDisplay[10] = ((int)(TempFahrenheit * 100) % 10) + 0x30;
          TempFahrenheitDisplay[11] = ((int)(TempFahrenheit * 1000) % 10) + 0x30;
          TempFahrenheitDisplay[12] = ((int)(TempFahrenheit * 10000) % 10) + 0x30;       
        }
        else
        {
          /* Convert temperature °C to Fahrenheit */
          TempValueFahrenheitFloat = abs((int)(((9 * TempValueCelsiusFloat) / 5) - 32));
          
          TempValueFahrenheit = (int) (TempValueFahrenheitFloat);
          
          /* Calculate temperature digits in °F */
          TempFahrenheitDisplay[5] = (TempValueFahrenheit / 100) + 0x30;
          TempFahrenheitDisplay[6] = ((TempValueFahrenheit % 100) / 10) + 0x30;
          TempFahrenheitDisplay[7] = ((TempValueFahrenheit % 100) % 10) + 0x30;
          
          /* Negative temperature measured */
          TempFahrenheitDisplay[4] = '-';
          
          TempFahrenheit = TempValueFahrenheitFloat - TempValueFahrenheit;
          
          TempFahrenheitDisplay[9] =  (int)(TempFahrenheit * 10) + 0x30;
          TempFahrenheitDisplay[10] = ((int)(TempFahrenheit * 100) % 10) + 0x30;
          TempFahrenheitDisplay[11] = ((int)(TempFahrenheit * 1000) % 10) + 0x30;
          TempFahrenheitDisplay[12] = ((int)(TempFahrenheit * 10000) % 10) + 0x30;
        }
      }
      else
      {
        /* Convert temperature °C to Fahrenheit */
        TempValueFahrenheitFloat = ((9 * TempValueCelsiusFloat) / 5) + 32;
        
        TempValueFahrenheit = (int) (TempValueFahrenheitFloat);
        
        /* Calculate temperature digits in °F */
        TempFahrenheitDisplay[5] = (TempValueFahrenheit / 100) + 0x30;
        TempFahrenheitDisplay[6] = ((TempValueFahrenheit % 100) / 10) + 0x30;
        TempFahrenheitDisplay[7] = ((TempValueFahrenheit % 100) % 10) + 0x30;
        
        /* Positive temperature measured */
        TempFahrenheitDisplay[4] = '+';

        TempFahrenheit = TempValueFahrenheitFloat - TempValueFahrenheit;
        
        TempFahrenheitDisplay[9] =  (int)(TempFahrenheit * 10) + 0x30;
        TempFahrenheitDisplay[10] = ((int)(TempFahrenheit * 100) % 10) + 0x30;
        TempFahrenheitDisplay[11] = ((int)(TempFahrenheit * 1000) % 10) + 0x30;
        TempFahrenheitDisplay[12] = ((int)(TempFahrenheit * 10000) % 10) + 0x30;
      }
      /* Display Fahrenheit value on LCD */
      for (i = 0; i < 20; i++)
      {
        LCD_DisplayChar(LCD_LINE_6, (319 - (16 * i)), TempCelsiusDisplay[i]);
        LCD_DisplayChar(LCD_LINE_7, (319 - (16 * i)), TempFahrenheitDisplay[i]);
      }
      
      if ((SMbusAlertOccurred == 1) && ((TempValueCelsius > (TEMPERATURE_TOS-1)) && (TempValue < 2048)))
      {     
        Var = 1;
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_BLUE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_DisplayStringLine(LCD_LINE_1, "Temp higher than 40C");
      }
      
      if ((SMbusAlertOccurred == 1) && ((TempValueCelsius < TEMPERATURE_THYS) || (TempValue > 2048)))
      {
        Var = 2;
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_BLUE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_DisplayStringLine(LCD_LINE_1, "Temp lower than 20C ");  
      }
      
      if ((SMbusAlertOccurred == 1) && (TempValueCelsius < TEMPERATURE_TOS) && (Var == 1))
      {
        Var = 0;
        SMbusAlertOccurred = 0;
        Tmp = TS751_AlerteResponseAddressRead();
        
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_WHITE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_WHITE);
        LCD_ClearLine(LCD_LINE_1);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_BLACK);  
      }
      
      if (((SMbusAlertOccurred == 1) && (TempValueCelsius > (TEMPERATURE_THYS-1)) && (Var == 2)) && (TempValue < 2048))
      {
        Var = 0;
        SMbusAlertOccurred = 0;
        Tmp = TS751_AlerteResponseAddressRead();
        
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_WHITE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_WHITE);
        LCD_ClearLine(LCD_LINE_1);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_BLACK);       
      }     
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
