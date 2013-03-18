#include "armcmx.h"
#include <stdlib.h>
#include "KS0108/glcd.h"

#include <math.h>

#include "fonts/allFonts.h"         // system and arial14 fonts are used
#include "bitmaps/allBitmaps.h"       // all images in the bitmap dir 

void setup(void);
void loop(void);

int main(void) {
  
  armcmx_init();
  setup();
  while (1)
    loop();
}

