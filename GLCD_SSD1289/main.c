#include "stm32f4xx.h"
#include "delay.h"
#include "SSD1289.h"
#include "touch_7846.h"
#include "Julija.h"

extern unsigned char flag;

char stringas[8];
int xold,yold;


int main(void)
{
//s  int i = 1;

  Delay(0x3FFFFF);
  LCD_Init();
  Delay(0x3FFFFF);
  touch_init();
  LCD_Clear(BLUE);
  LCD_SetTextColor(WHITE);
	
	LCD_StringLine(64, 64, "Hello there!");
  
	/*
  while(1)
  {
    Julia4(240,320,120,160,i);
    i+=10;
    if(i>1000)
      i=0;
  }
	*/
	while(1) {
		Convert_Pos();
		Pixel(Pen_Point.X0,Pen_Point.Y0,WHITE); 
		Pixel(Pen_Point.X0,Pen_Point.Y0+1,WHITE);
		Pixel(Pen_Point.X0+1,Pen_Point.Y0,WHITE);
		Pixel(Pen_Point.X0+1,Pen_Point.Y0+1,WHITE);
	}
	
}
