#include "armcmx.h"
#include <stdlib.h>
#include "GLCD/glcd.h"
#include <glcd.h>

#include <math.h>

#include "fonts/allFonts.h"         // system and arial14 fonts are used
#include "bitmaps/allBitmaps.h"       // all images in the bitmap dir 

Image_t icon;
gText textArea;              // a text area to be defined later in the sketch
gText textAreaArray[3];      // an array of text areas  
gText countdownArea =  gText(GLCD.CenterX, GLCD.CenterY,1,1,Arial_14); // text area for countdown digits

unsigned long startMillis;
unsigned int  loops = 0;
unsigned int  iter = 0;
         int  theDelay = 20; 

//glcd GLCD;  // w/ init()

void drawSpinner(byte pos, byte x, byte y);

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	//setup
  GLCD.Init();   // initialise the library, non inverted writes pixels onto a clear screen
	GLCD.Clear(0xff);
	delay(500);
	printf("Initialized.\n");
	delay(500);
  icon = ArduinoIcon64x64;  // the 64 pixel high icon

	GLCD.ClearScreen();
  GLCD.SelectFont(System5x7, BLACK); // font for the default text area
  GLCD.println("GLCD version ");
  GLCD.println(" armcmx ");
		
	/*
	int x, y, x0 = 5, y0 = 23;
	int loops = 0;
	y = y0;
	while (y < 56) {
		GLCD.SetDot(x, y, 1);
		x = x0 + loops/3;
		y = y0 + loops/5;
		loops++;
	}
	while(1);
	*/
	GLCD.DrawLine(12, 28, 107, 47);
	GLCD.DrawLine(13, 26, 103, 49);
//  GLCD.DrawCircle(70, 36, 32);  // rounded rectangle around text area   
	GLCD.DrawRoundRect(7, 13, 111, 42, 12);
	GLCD.GotoXY(40, 20);
	GLCD.print("Finished.");
	printf("fin.\n");
	/*
	srand(23);
	int x = 0, y = 0;
//	uint8 d;
//	uint16 color = 0xff;
	while (1) {
		x = rand() % 127;
		y = rand() % 63;
		delay_ms(20);
		GLCD.SetDot(x, y, 1);
//		x = ++x % 128;
//		if (x == 0) 
//			y = ++y % 64;
	}
	*/
	while (1) {
    //drawSpinner(loops++,100,48);       // draw new spinner position
	}
}

void drawSpinner(byte pos, byte x, byte y) {   
  // this draws an object that appears to spin
  switch(pos % 8) {
    case 0 : GLCD.DrawLine( x, y-8, x, y+8);        break;
    case 1 : GLCD.DrawLine( x+3, y-7, x-3, y+7);    break;
    case 2 : GLCD.DrawLine( x+6, y-6, x-6, y+6);    break;
    case 3 : GLCD.DrawLine( x+7, y-3, x-7, y+3);    break;
    case 4 : GLCD.DrawLine( x+8, y, x-8, y);        break;
    case 5 : GLCD.DrawLine( x+7, y+3, x-7, y-3);    break;
    case 6 : GLCD.DrawLine( x+6, y+6, x-6, y-6);    break; 
    case 7 : GLCD.DrawLine( x+3, y+7, x-3, y-7);    break;
  } 
}

