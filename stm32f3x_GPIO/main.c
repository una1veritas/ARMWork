/**
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "armcmx.h"


int main(void)
{
  int32 interval = 500;
  int i;
  
  armcmx_init();
  usart_begin(&stdserial, 57600);
  
  pinMode(PE14, OUTPUT);
  pinMode(PE15, OUTPUT);
  
  usart_print(&stdserial, "Hello!\n");
  
  while (1)
  {
    for(i = 0; i < 1000/abs(interval) * 4; i++) {
      if ( digitalRead(PE14) == LOW ) {
        digitalWrite(PE14, HIGH);
        digitalWrite(PE15, LOW);
      } else {
        digitalWrite(PE14, LOW);
        digitalWrite(PE15, HIGH);
      }
      delay(abs(interval));
    }
    
    if ( interval > 0 ) {
      interval *= 2;
      if ( abs(interval) >= 1000 ) 
        interval = -interval;
    } else if ( interval < 0 ) {
      interval /= 2;
      if ( abs(interval) <= 250 ) 
        interval = -interval;
    }
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
