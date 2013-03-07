#include "armcmx.h"
#include <stdlib.h>
#include "GLCD/KS0108.h"
#include "GLCD/glcd.h"

#include <math.h>

#include "GLCD/fonts/allFonts.h"         // system and arial14 fonts are used
#include "GLCD/bitmaps/allBitmaps.h"       // all images in the bitmap dir 

KS0108 lcd;
glcd GLCD(lcd);  // w/ init()

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	//setup
  GLCD.init();   // initialise the library, non inverted writes pixels onto a clear screen
  GLCD.ClearScreen(BLACK);
	delay(500);
  GLCD.ClearScreen(WHITE); 

  GLCD.SelectFont(System5x7, BLACK); // font for the default text area
	GLCD.println();
	GLCD.println();
  GLCD.println("  GLCD version ");
  GLCD.println("    armcmx ");		
  GLCD.DrawRect(8,8,110,32);  // rounded rectangle around text area   

	delay(500);
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
	uint8 line = 0;
	uint8 d;
	while (1) {
		GLCD.println(millis());
		delay(200);
	}
}

