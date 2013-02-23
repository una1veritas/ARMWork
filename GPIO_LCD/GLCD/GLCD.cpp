#include "GLCD.h"
#include "KS0108_compat.h"

	void GLCD::start(void) {
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		for(int i = 0; i < buswidth; i++) {
			pinMode(D0+i, OUTPUT);
		}
	}

