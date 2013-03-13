#include "armcmx.h"
#include <stdlib.h>
#include "glcd/glcd.h"
//#include <glcd.h>

#include <math.h>

#include "glcd/fonts/allFonts.h"         // system and arial14 fonts are used
#include "glcd/bitmaps/allBitmaps.h"       // all images in the bitmap dir 

void setup(void);
void loop(void);

int main(void) {
  
  setup();
  while (1)
    loop();
}

