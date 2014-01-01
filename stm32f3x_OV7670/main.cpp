/**
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "armcmx.h"
#include "USARTSerial.h"
#include "sccb.h"
#include "TFTLCD.h"


USARTSerial Serial(&stdserial, USART3, PC11, PC10);

TFTLCD tft(PB3, PB4, PB5, PB8, PB9);

SCCBus sccb(PC6, PC7);
const GPIOPin VSYNC = PB10;
const GPIOPin HREF  = PB11;
const GPIOPin WEN   = PB12;
const GPIOPin RRST  = PB13;
const GPIOPin OE    = PB14;
const GPIOPin RCLK  = PB15;
Cam cam(VSYNC, HREF, WEN, RRST, OE, RCLK);

#define __DELAY { __nop(); __nop(); __nop(); __nop(); }

void setup();
void loop();

	
int main(void)
{
  armcmx_init();
  
  setup();
  
  while (1) {
    loop();
  }
}

void setup(void) {
	uint8_t regval;
  char tmpstr[32];

  Serial.begin(57600);
  Serial.println("Hello! Hi, hi!\n");
  Serial.flush();
  
	tft.begin();
  tft.setRotation(3);
  tft.fillScreen(C565_BLACK);
	
  Serial.println("Starting OV7670 Test.");
  //delay(1000);

	if ( sccb.init() == 0 ) {
    Serial.println("Initializing sensor by sccb failed.\nHalt.");
    while (1);
  }
	sccb.readRegister(0x12, &regval);
	
	sprintf(tmpstr, "register 0x%02x = 0x%02x", 0x12, regval);
	Serial.println(tmpstr);

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
  
}

void loop() {
	uint16_t color;
	
	if ( digitalRead(VSYNC) == LOW ) {
    cam.waitVSYNCHigh();
    cam.writeDisable();
    cam.waitVSYNCFalling();
    cam.writeEnable();

		cam.outEnable();
		cam.readReset(LOW);
		cam.clock(LOW);
		cam.waitHREFPulse();
//		__nop();
		
		cam.clock(HIGH);
		cam.readReset(HIGH);
		__nop();
		cam.clock(LOW);
		__nop();
		cam.clock(HIGH);
		__nop();
		for(int row = 0; row <240; row++) {
      tft.drawPixel(0,row,0);
      for(int col = 0; col < 320 ; col++) {
        cam.clock(LOW);
        color = GPIORead(PD_ALL) & 0xff00;
        cam.clock(HIGH);
        __nop();
        cam.clock(LOW);
        color |= GPIORead(PD_ALL)>>8;
        cam.clock(HIGH);

        tft.drawPixel(color);
      }
    }
		cam.outDisable();
		cam.writeDisable();
		digitalWrite(PE8, LOW);
		delay(5);
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
