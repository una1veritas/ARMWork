#include "stm32f4xx.h"
#include "delay.h"
#include "SSD1289.h"
#include "touch_7846.h"
#include "Julija.h"

#include "gpio.h"

extern unsigned int xxx,yyy;
extern unsigned char flag;


int main(void)
{
  int i = 1;
	
	pinMode(PB0, OUTPUT);
	digitalWrite(PB0, HIGH);
	Delay(0x3FFFFF);		// 6242 usec
	digitalWrite(PB0, LOW);
  Delay(90);	//	9.125 usec
	digitalWrite(PB0, HIGH);
	Delay(3000);		// 286.375 usec
	digitalWrite(PB0, LOW);
  Delay(50);	//	5.375 usec
	digitalWrite(PB0, HIGH);
  LCD_Init();
  Delay(0x3FFFFF);
  LCD_Clear(WHITE);
  while(1)
  {
    Julia2(240,320,120,160,i);
    i+=10;
    if(i>1000)
      i=0;
  }
}


