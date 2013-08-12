#include "stm32f4xx.h"
#include "SSD1289.h"
//#include "touch_7846.h"
#include "ads7846.h"

#include "armcmx.h"
#include "stm32_open407vc.h"

#define Delay(x)  delay_us((x)/10)

//char stringas[8];
//int xold,yold;

spi spi2bus; // = {SPI2, PB13, PB14, PB15, PB12};
TouchScreen tp(spi2bus, PB12, PD6);
SSD1289 lcd;

int main(void)
{
//	int i;
	char tmp[64];
	RCC_ClocksTypeDef RCC_Clocks;
	
	armcmx_init();
	RCC_GetClocksFreq(&RCC_Clocks);
	printf( "SYSCLK = %dl\n", RCC_Clocks.SYSCLK_Frequency);

	spi_init(&spi2bus, SPI2,  PB13, PB14, PB15, PB12);
	spi_begin(&spi2bus);
	tp.begin(240, 320);

  //pinMode(JOY_SEL, INPUT);
  GPIOMode(PinPort(JOY_SEL), PinBit(JOY_SEL), INPUT, MEDSPEED, PUSHPULL, PULLUP);
//  Delay(0x3FFFFF);
  lcd.init();
	lcd.start();
  uint8 mode = lcd.PORTRAIT;
  while (1) {
    lcd.clear(BLACK);
    lcd.displayOrientation(mode);

    lcd.SetCursor(15, 15);
    for(int i = 0; i < 24; i++) {
      lcd.WriteRAM_Prepare();
      lcd.WriteRAM(0xffff);
    }

    lcd.textSize(16);
    lcd.SetTextColor(WHITE);
    lcd.SetBackColor(BLACK);
    lcd.textCursor(0, 2);
    lcd.print("Hello, friends!");

    tp.setOrientation(mode);
    uint8 trap = 1;
    while (trap) {
      tp.update();
      lcd.PutPixel(tp.x, tp.y-1,WHITE); 
      lcd.PutPixel(tp.x, tp.y,WHITE); 
      lcd.PutPixel(tp.x, tp.y+1,WHITE);
      lcd.PutPixel(tp.x-1, tp.y,WHITE);
      lcd.PutPixel(tp.x, tp.y,WHITE);
      lcd.PutPixel(tp.x+1, tp.y,WHITE);
      sprintf(tmp, "x =% 4d, y =% 4d", tp.x, tp.y);
      lcd.textCursor(2, 3);
      lcd.print(tmp);
      if ( digitalRead(JOY_SEL) == LOW) {
        while ( digitalRead(JOY_SEL) == LOW );
        if ( mode == lcd.LANDSCAPE_REV )
          mode = lcd.PORTRAIT;
        else
          mode++;
        break;
      }
    }
  }
    //  Delay(0x3FFFFF);
    //touch_init();
    delay_ms(5);
    lcd.clear(BLACK);

    lcd.textSize(16);
    lcd.SetTextColor(WHITE);
    lcd.SetBackColor(BLACK);
    lcd.print("Hi.\n");
    lcd.print("Resistive touch screen driver.");
    
    sprintf(tmp, "device code %04x", lcd.device());
    lcd.textCursor(0, 2);
    lcd.print(tmp);
    printf("%s\n", tmp);

while(1) {
  tp.update();
  lcd.PutPixel(tp.x, tp.y,WHITE); 
  lcd.PutPixel(tp.x, tp.y+1,WHITE);
  lcd.PutPixel(tp.x+1, tp.y,WHITE);
  lcd.PutPixel(tp.x+1, tp.y+1,WHITE);
 //
  sprintf(tmp, "x =% 5d, y =% 5d", tp.x, tp.y);
  lcd.textCursor(0, 3);
  lcd.print(tmp);
	 /*
	 if ( tp.x != 0 and tp.y != 0 ) {
	 printf(tmp);
	 printf("\n");
		 }
	 */
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
