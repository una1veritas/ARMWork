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
SSD1289 lcd;

int main(void)
{
//	int i;
	char tmp[64];
	
	cmcore_init();
	
	spi_init(&spi2bus, SPI2,  PB13, PB14, PB15, PB12);
	spi_begin(&spi2bus);
	tp.begin(tp.Portrait | tp.ReverseY);

  Delay(0x3FFFFF);
  lcd.init();
	lcd.start();
  Delay(0x3FFFFF);
  //touch_init();
  lcd.clear(BLACK);

	lcd.textSize(16);
  lcd.SetTextColor(WHITE);
  lcd.SetBackColor(BLACK);
	lcd.print("Hi.\n");
	lcd.print("Resistive touch screen driver.");
  

while(1)
 {
  tp.convertPos();
  lcd.Pixel(tp.x, tp.y,WHITE); 
  lcd.Pixel(tp.x, tp.y+1,WHITE);
  lcd.Pixel(tp.x+1, tp.y,WHITE);
  lcd.Pixel(tp.x+1, tp.y+1,WHITE);
	 //
	 sprintf(tmp, "x =% 5d, y =% 5d", tp.x, tp.y);
	 lcd.textCursor(0, 4);
	 lcd.print(tmp);
 }
}


/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*
void TouchPanel_Calibrate(void) {
	static Point sample[3];
  Point * pt;
	uint8_t i;

  for(i=0; i < 3 ; i++) {
   LCD_Clear(CYAN);
   GUI_Text(44,10,"Touch crosshair to calibrate", RED,WHITE);
   delay_ms(500);
   DrawCross(DisplaySample[i].x,DisplaySample[i].y);
   do {
     pt = readAds7846();
   }
   while( ptr == (void*)0 );
   sample[i].X= pt->X; 
	 sample[i].Y= pt->Y;
  }
  setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
  LCD_Clear(RGB565CONVERT(200,200,120));
} 
*/

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
