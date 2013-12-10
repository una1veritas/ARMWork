/**
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "armcmx.h"

#include "i2c_cpal.h"

#define LM73_1		0x98
//#define LM75_I2C_CLK                     RCC_APB1Periph_I2C1

static void NVIC_Config(void);

int main(void)
{
  float temperaturecelsius = 0;
  int16_t tempvalue = 0;
  char tmpstr[32];

    /* NVIC Configuration */
  NVIC_Config();

  armcmx_init();
  usart_begin(&stdserial, 57600);
    
  usart_print(&stdserial, "Starting OV7670 Test.\n");
  delay(1000);
  
    /* Initialize the Temperature Sensor */
  i2c_begin();
  
  if (i2c_getstatus(&i2c1, LM73_1) != SUCCESS) {
    usart_print(&stdserial, "Sensor get status failed.\n");
    while (1) {}
  } else {
    usart_print(&stdserial, "Succeeded to get the sensor status.\n");

    /* Configure the Temperature sensor device STLM75:
    - Thermostat mode Interrupt
    - Fault tolerance: 00
    */
    i2c_write8(&i2c1, LM73_1, 0x04, 0x60);
		
		sprintf((char*)tmpstr, "Config register: %02x\n", i2c_read8(&i2c1, LM73_1, 0x04));
		usart_print(&stdserial, tmpstr);

    /* Configure the THYS and TOS in order to use the SMbus alert interrupt */
//    LM75_WriteReg(LM75_REG_THYS, TEMPERATURE_THYS << 8);  /*31у*/
//    LM75_WriteReg(LM75_REG_TOS, TEMPERATURE_TOS << 8);   /*32у*/
    
    /* Enables the I2C SMBus Alert feature */
   // I2C_SMBusAlertCmd(LM75_I2C, ENABLE);    
    I2C_ClearFlag(i2c1.I2Cx, I2C_FLAG_ALERT);
    
//    SMbusAlertOccurred = 0;
    
    /* Enable SMBus Alert interrupt */
    I2C_ITConfig(i2c1.I2Cx, I2C_IT_ERRI, ENABLE);
  
		while (1)
		{
			 
			 /* Get temperature value */
			 tempvalue = i2c_read16(&i2c1, LM73_1, 0);
			 tempvalue &= 0xfffc;
			 temperaturecelsius = tempvalue/128.0f;
			 
			 /* Display temperature value in celsius & fahrenheit  */
			 sprintf((char*)tmpstr, "%2.2f C, %2.2f F\n", temperaturecelsius, 1.8f * temperaturecelsius + 32);
			 usart_print(&stdserial, tmpstr);
	 
			 delay(5000);
		}
	}
}


/**
  * @brief  Configures the different interrupt.
  * @param  None
  * @retval None
  */
void NVIC_Config(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Reconfigure and enable I2C1 error interrupt to have the higher priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
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
