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
<<<<<<< HEAD
  int i = 1;
=======
	int i = 1;
>>>>>>> origin/win@home

  Delay(0x3FFFFF);
  LCD_Init();
  Delay(0x3FFFFF);
  touch_init();
<<<<<<< HEAD
  LCD_Clear(BLACK);
  LCD_SetTextColor(BLUE);
  
=======
  LCD_Clear(DGRAY);

  LCD_SetTextColor(WHITE);
  LCD_SetBackColor(BLACK);
//	LCD_SetCursor(10, 10);
	LCD_StringLine(10, 10, "This is a pen.");
  Delay(10000);
	
>>>>>>> origin/win@home
  while(1)
  {
    Julia3(240,320,120,160,i);
    i+=10;
    if(i>1000)
      i=0;
  }
<<<<<<< HEAD
/*	while(1)
 {
   Convert_Pos();
  Pixel(Pen_Point.X0,Pen_Point.Y0,WHITE); 
  Pixel(Pen_Point.X0,Pen_Point.Y0+1,WHITE);
  Pixel(Pen_Point.X0+1,Pen_Point.Y0,WHITE);
  Pixel(Pen_Point.X0+1,Pen_Point.Y0+1,WHITE);
 }
	*/
=======
	/*
	while(1) {
		Convert_Pos();
		Pixel(Pen_Point.X0,Pen_Point.Y0,WHITE); 
		Pixel(Pen_Point.X0,Pen_Point.Y0+1,WHITE);
		Pixel(Pen_Point.X0+1,Pen_Point.Y0,WHITE);
		Pixel(Pen_Point.X0+1,Pen_Point.Y0+1,WHITE);
	}
*/
>>>>>>> origin/win@home
}

