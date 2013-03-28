#include "stm32f4xx.h"
#include "SSD1289.h"
#include "touch_7846.h"

#include "cmcore.h"
#include "delay.h"

#define Delay(x)  delay_us((x)/10)

extern unsigned char flag;

char stringas[8];
int xold,yold;


int main(void)
{
	int i;
	cmcore_init();
	
  Delay(0x3FFFFF);
  LCD_Init();
  Delay(0x3FFFFF);
  touch_init();
  LCD_Clear(BLACK);

  LCD_SetTextColor(WHITE);
  LCD_SetBackColor(BLACK);
//	LCD_SetCursor(10, 10);
	LCD_StringLine(10, 10, "This is a pen.");
	LCD_StringLine(10, 24, "Gosh its freezin cold.");
  

while(1)
 {
   Convert_Pos();
  Pixel(Pen_Point.X0,Pen_Point.Y0,WHITE); 
  Pixel(Pen_Point.X0,Pen_Point.Y0+1,WHITE);
  Pixel(Pen_Point.X0+1,Pen_Point.Y0,WHITE);
  Pixel(Pen_Point.X0+1,Pen_Point.Y0+1,WHITE);
 }

}


