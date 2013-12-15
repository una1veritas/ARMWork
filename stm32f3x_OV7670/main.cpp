/**
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "armcmx.h"

#include "sccb.h"

SCCBus sccb(PC6, PC7);
const GPIOPin VSYNC = PB10;
const GPIOPin HREF  = PB11;
const GPIOPin WEN   = PB12;
const GPIOPin RRST  = PB13;
const GPIOPin OE    = PB14;
const GPIOPin RCLK  = PB15;
Cam cam(VSYNC, HREF, WEN, RRST, OE, RCLK);

#define __DELAY { __nop(); __nop(); __nop(); __nop(); }

int main(void) {
	uint8_t regval;
  char tmpstr[32];

  armcmx_init();
  usart_begin(&stdserial, 57600);
    
  usart_print(&stdserial, "Starting OV7670 Test.\n");
  //delay(1000);

	if ( sccb.init() == 0 ) {
    usart_print(&stdserial, "Initializing sensor by sccb failed.\nHalt.\n");
    while (1);
  }
	sccb.readRegister(0x12, &regval);
	
	sprintf(tmpstr, "register 0x%02x = 0x%02x\n", 0x12, regval);
	usart_print(&stdserial, tmpstr);

  pinMode(VSYNC, INPUT);
  pinMode(HREF, INPUT);
  pinMode(WEN, OUTPUT);
  digitalWrite(WEN, HIGH);
  pinMode(RRST, OUTPUT);
  digitalWrite(RRST, HIGH);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, HIGH);
  pinMode(RCLK, OUTPUT);
  digitalWrite(RCLK, HIGH);
  
  pinMode(PE8, OUTPUT);
  pinMode(PE9, OUTPUT);
	while (1) {
    if ( digitalRead(VSYNC) == LOW ) {
      digitalWrite(PE8, HIGH);
      while( digitalRead(VSYNC) == LOW );
      cam.writeEnable();
      cam.outEnable();
      cam.readReset(LOW);
      cam.clock(LOW);
      __nop();
      cam.clock(HIGH);
      cam.readReset(HIGH);
      __nop();
      cam.clock(LOW);
      __nop();
      cam.clock(HIGH);
      __nop();
      for(int n = 0; n < 320*240; n++) {
        cam.clock(LOW); __nop(); cam.clock(HIGH); __nop();
      }
      cam.outDisable();
      cam.writeDisable();
      digitalWrite(PE8, LOW);
      delay(5);
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
