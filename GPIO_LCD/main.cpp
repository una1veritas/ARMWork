#include "armcmx.h"
#include <stdlib.h>
#include "GLCD/KS0108.h"
#include "GLCD/glcd.h"

#include <math.h>

#include "GLCD/fonts/allFonts.h"         // system and arial14 fonts are used
#include "GLCD/bitmaps/allBitmaps.h"       // all images in the bitmap dir 


	const char message[] = 
			"This royal throne of kings, this scepter'd isle, \n"
			"This earth of majesty, this seat of Mars, \n"
			"This other Eden, demi-paradise, \n"
			"This fortress built by Nature for herself\n"
			"Against infection and the hand of war, \n"
			"This happy breed of men, this little world,\n" 
			"This precious stone set in the silver sea, \n"
			"Which serves it in the office of a wall, \n"
			"Or as a moat defensive to a house, \n"
			"Against the envy of less happier lands, \n"
			"This blessed plot, this earth, this realm, this England,";
	const uint16 messlen = strlen(message);

KS0108 lcd;
glcd GLCD(lcd);  // w/ init()

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

int main(void) {
	armcmx_init();
	printf("Hello!\n");
	
	//setup
  GLCD.init();   // initialise the library, non inverted writes pixels onto a clear screen
  GLCD.ClearScreen(BLACK);
	delay(500);
  GLCD.ClearScreen(WHITE); 

  GLCD.DrawRect(8,8,110,32);  // rounded rectangle around text area   

  GLCD.SelectFont(System5x7, BLACK); // font for the default text area
	GLCD.CursorTo(2, 2);
  GLCD.println("  GLCD version ");
  GLCD.println("    armcmx ");

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
  long iter, startMillis, loops = 0;
  while (1) {
    iter = 0;
  startMillis = millis();
  while( millis() - startMillis < 1000){ // loop for one second
    GLCD.DrawRect(0, 0, 64, 61, BLACK); // rectangle in left side of screen
    GLCD.DrawRoundRect(68, 0, 58, 61, 5, BLACK);  // rounded rectangle around text area   
    for(int i=0; i < 62; i += 4)
      GLCD.DrawLine(1,1,63,i, BLACK);  // draw lines from upper left down right side of rectangle  
    GLCD.DrawCircle(32,31,30,BLACK);   // draw circle centered in the left side of screen  
    GLCD.FillRect(92,40,16,16, WHITE); // clear previous spinner position  
    drawSpinner(loops++,100,48);       // draw new spinner position
    //GLCD.FillRect(24,txtLINE3,14,14, WHITE);  // clear text area that will be drawn below 
    GLCD.CursorTo(5,5);               // locate curser for printing text
    GLCD.print(++iter);         // print current iteration at the current cursor position 
  } 
  // display number of iterations in one second
  GLCD.ClearScreen();               // clear the screen  
  GLCD.CursorTo(14,2);              // positon cursor  
  GLCD.print("FPS= ");               // print a text string
  GLCD.print(iter);           // print a number 
}  
//  char a[] = {'a', 'b', 'c', 'd', 'e', 'f'};
/*
	uint32 t = 0;
	while (1) {
    char c = message[t % messlen];
    if ( c == '\n' || c == '\r' ) {
      GLCD.print(' ');
      GLCD.print(millis());
      GLCD.print(' ');
    } else {
      GLCD.print(c);
    }
    delay(200);
    t++;
	}
*/
}

