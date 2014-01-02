/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    17-June-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "demo.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__IO Operation_Typedef ReqOperation = NO_OP;

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
       file (startup_stm32xxx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32xxx.c file
  */

  /* Initialize all demo components */
  Demo_Init();
  
  /* Start menu display */
  Display_Menu();
  
  while(1)
  {
    /* Check which is the requested operation */
    switch (ReqOperation)
    {
      /* EEPROM Erase Operation */
    case ERASE_EEPROM_OP:
      Erase_EEPROM();     
      ReqOperation = NO_OP;
      break;
      
      /* Get temperature report from EEPROM */
    case GET_REPORT_OP:
      Get_Report();
      ReqOperation = NO_OP;
      break;    
      
      /* Display the current temperature value */
    case SHOW_TEMPERATURE_OP:
      Show_Temperature();
      ReqOperation = NO_OP;
      break;     
      
      /* Display the current time value */
    case SHOW_TIME_OP:
      Show_Time();
      ReqOperation = NO_OP;
      break;    
      
      /* Save the current report (periodic operation) */
    case SAVE_REPORT_OP:
      Save_Report();
      ReqOperation = NO_OP;
      break;
      
    default:
      break;           
    }
  }
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

   LCD_DisplayStringLine(Line0, "assert_param error!!");

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
