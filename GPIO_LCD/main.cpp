
#include "armcmx.h"

#include "GLCD/GLCD.h"

GLCD lcd(GLCD::KS0108);  // w/ init()

int main(void) {
	armcmx_init();
	
	printf("Hello!\n");
	
}


