#ifndef __W5100_PIN_CONFIG__
#define __W5100_PIN_CONFIG__

//#include "wirish.h"


//#define USE_NO_DELAYED_ACK	/* if you need, activate this line */


/* Define SPI port and chip select pin */
#define DEF_SPI_PORT		HardwareSPI SPI(2)
#define ETHERNET_SS_PIN		PIO0_11 
#ifdef BOARD_STM32VLD
#define ETHERNET_SPI_FREQ	SPI_6MHZ 
#else
#	define ETHERNET_SPI_FREQ	SPI_4_5MHZ 
#endif

/* w5200/w5100 "nSCS" chip select pin config */
#define _initSS()			pinMode(      ETHERNET_SS_PIN, OUTPUT)
#define _setSS()			digitalWrite( ETHERNET_SS_PIN, 0 )
#define _resetSS()			digitalWrite( ETHERNET_SS_PIN, 1 ) 

/* w5200/w5100 "nRST" RESET pin config, if you need. */
/*
#define W5100_RESET_PIN		PB(11)
#define _initReset()		pinMode(      W5100_RESET_PIN, OUTPUT)
#define _setReset(m)		digitalWrite( W5100_RESET_PIN, m )
*/


#endif /* __W5100_PIN_CONFIG__ */

