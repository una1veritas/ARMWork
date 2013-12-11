/**
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "armcmx.h"

#include "sccb.h"


int main(void)
{
	uint8_t regval;
  char tmpstr[32];

  armcmx_init();
  usart_begin(&stdserial, 57600);
    
  usart_print(&stdserial, "Starting OV7670 Test.\n");
  delay(1000);

	Sensor_init();
	rd_Sensor_Reg(0x12, &regval);
	
	sprintf(tmpstr, "val = 0x%02x\n", regval);
	usart_print(&stdserial, tmpstr);

	while (1);
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
