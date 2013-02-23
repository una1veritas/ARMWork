#include "GLCD_KS0108.h"

	void KS0108::start(void) {
		digitalWrite(RS, OUT);
		digitalWrite(RW, OUT);
		digitalWrite(EN, OUT);
		digitalWrite(CS1, OUT);
		digitalWrite(CS2, OUT);		
	}

