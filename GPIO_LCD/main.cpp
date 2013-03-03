#include "armcmx.h"
#include <stdlib.h>
#include "GLCD/glcd.h"
#include <glcd.h>

#include <math.h>

#include "fonts/allFonts.h"         // system and arial14 fonts are used
#include "bitmaps/allBitmaps.h"       // all images in the bitmap dir 

//glcd GLCD;  // w/ init()

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	//setup
  GLCD.Init();   // initialise the library, non inverted writes pixels onto a clear screen
  GLCD.Clear(BLACK); 
	delay(500);
  GLCD.ClearScreen(WHITE); 

  GLCD.SelectFont(System5x7, BLACK); // font for the default text area
  GLCD.println("GLCD version ");
  GLCD.println(" armcmx ");
		
  GLCD.DrawRect(8,16,110,32);  // rounded rectangle around text area   

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
		line++;
		if ( line >= 8 ) {
			for(int pg = 1; pg < 8; pg++) {
				for(int i = 0; i < 128; i++) {
					GLCD.SetAddress(pg, i);
					d = GLCD.ReadData();
					GLCD.SetAddress(pg-1, i);
					GLCD.WriteData(d);
				}
			}
		}
		delay(200);
	}
}

