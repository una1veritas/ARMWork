#include "stm32f4xx.h"
#include "SSD1289.h"
//#include "touch_7846.h"
#include "ads7846.h"

#include "cmcore.h"
#include "delay.h"

#define Delay(x)  delay_us((x)/10)


char stringas[8];
int xold,yold;

spi spi2bus; // = {SPI2, PB13, PB14, PB15, PB12};
TouchScreen tp(spi2bus, PB12, PD6);

int main(void)
{
	int i;
	char tmp[64];
	
	cmcore_init();
	
	spi_init(&spi2bus, SPI2,  PB13, PB14, PB15, PB12);
	spi_begin(&spi2bus);
	tp.begin(tp.ReverseY);

  Delay(0x3FFFFF);
  LCD_Init();
  Delay(0x3FFFFF);
  //touch_init();
  LCD_Clear(BLACK);

  LCD_SetTextColor(WHITE);
  LCD_SetBackColor(BLACK);
//	LCD_SetCursor(10, 10);
	LCD_StringLine(0, 10, (uint8 *) "Resistive touch screen driver.");
  

while(1)
 {
  tp.convertPos();
  Pixel(tp.X0, tp.Y0,WHITE); 
  Pixel(tp.X0, tp.Y0+1,WHITE);
  Pixel(tp.X0+1, tp.Y0,WHITE);
  Pixel(tp.X0+1, tp.Y0+1,WHITE);
	 //
	 sprintf(tmp, "x =% 5d, y =% 5d", tp.X0, tp.Y0);
	 LCD_StringLine(0, 24, (uint8 *) tmp);
 }

}


