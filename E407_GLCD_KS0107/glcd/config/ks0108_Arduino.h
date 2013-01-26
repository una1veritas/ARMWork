/*
 * ks0108_Arduino.h - User specific configuration for Arduino GLCD library
 *
 * Use this file to set io pins
 * This version is for a standard ks0108 display
 * connected using the default Arduino wiring
 *
*/

#ifndef GLCD_PIN_CONFIG_H
#define GLCD_PIN_CONFIG_H

/*
 * define name for pin configuration
 */
#define glcd_PinConfigName "ks0108-Arduino"

/*********************************************************/
/*  Configuration for assigning LCD bits to Arduino Pins */
/*********************************************************/
/* Data pin definitions
 */
#define glcdData0Pin        4
#define glcdData1Pin        5
#define glcdData2Pin        6
#define glcdData3Pin        7
#define glcdData4Pin        8
#define glcdData5Pin        9
#define glcdData6Pin        10
#define glcdData7Pin        11

/* Arduino pins used for Commands
 * default assignment uses the first five analog pins
 */

#define glcdCSEL1        12
#define glcdCSEL2        13

#if NBR_CHIP_SELECT_PINS > 2
#define glcdCSEL3         3   // third chip select if needed
#endif

#if NBR_CHIP_SELECT_PINS > 3
#define glcdCSEL4         2   // fourth chip select if needed
#endif

#define glcdRW           15
#define glcdDI           14
#define glcdEN           16
// Reset Bit  - uncomment the next line if reset is connected to an output pin
#define glcdRES          17    // Reset Bit

#endif //GLCD_PIN_CONFIG_H
