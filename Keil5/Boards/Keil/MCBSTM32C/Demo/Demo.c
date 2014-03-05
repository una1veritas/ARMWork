/*----------------------------------------------------------------------------
 * Name:    Demo.c
 * Purpose: Demo example for MCBSTM32C
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "Driver_I2C.h"
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Touch.h"

/* Bargraph position definitions */
#define BAR_ST      (8*6+3)
#define BAR_LEN     (217)
#define BAR_HGT     (20)
#define BAR_LN(ln)  ((ln)*24)

/* Joystick position definitions */
#define JOY_X       (13*16)
#define JOY_Y       ( 5*24 + 6)

/* Button position definitions */
#define BTN_X       (9)
#define BTN_Y       (190)
#define BTN_X_EVAL  (20)
#define BTN_Y_EVAL  (182)

/* Bulb position definition */
#define BULB_X       (209)
#define BULB_Y       (190)
#define BULB_X_EVAL  (9 * 16)
#define BULB_Y_EVAL  (8 * 24)

/* Keys bit masks */
#define KEY_WAKEUP  1
#define KEY_TAMPER  2
#define KEY_USER    4

/* I2C1 Driver */
extern ARM_DRIVER_I2C Driver_I2C1;

/* Mode switch timeout variable */
uint32_t KeyTick;

/* Extern variables */
extern unsigned char Arrows_16bpp_red[];
extern unsigned char Button_16bpp[];
extern unsigned char Bulb_16bpp[];


/*-----------------------------------------------------------------------------
  Draw rectangle at specified coordinates (x,y) with specified side lengths
  (h_len, v_len) and border.
 *----------------------------------------------------------------------------*/
void DrawRectangle (uint32_t x, uint32_t y, uint32_t h_len, uint32_t v_len, uint32_t border) {
  uint32_t i, b;

  for (b = 0; b < border; b++) {
    for (i = 0; i < h_len; i++) {
      GLCD_PutPixel (x + i, y + b);
    }
    for (i = 0; i < v_len; i++) {
      GLCD_PutPixel (x + b, y + i);
    }
    for (i = 0; i < h_len; i++) {
      GLCD_PutPixel (x + i, y + v_len - b);
    }
    for (i = 0; i < v_len; i++) {
      GLCD_PutPixel (x + h_len - b, y + i);
    }
  }
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  TSC_STATE  tsc_state;
  uint32_t ofs;
  uint32_t keyMsk, joyMsk, adcVal;
  int32_t key  = -1;
  int32_t joy  = -1;
  int32_t adc  = -1;

  LED_Initialize();                     /* LED Init                           */
  ADC_Initialize();                     /* A/D Converter Init                 */
  Keyboard_Initialize();                /* Keyboard Init                      */
  Joystick_Initialize();                /* Joystick Init                      */
  Touch_Initialize();                   /* Touchscreen Controller Init        */
  GLCD_Initialize();                    /* Graphical Display Init             */

  GLCD_Clear (White);
  GLCD_SetBackColor  (Blue);
  GLCD_SetTextColor  (White);
  GLCD_DisplayString (0, 0, 1, "     MCBSTM32C      ");
  GLCD_DisplayString (1, 0, 1, "    Demo Example    ");
  GLCD_SetBackColor  (White);
  GLCD_SetTextColor  (Blue);

#ifdef _EVAL_VERSION_
  GLCD_SetTextColor (Red);
  GLCD_DisplayString (8, 2, 1, "Touch");

  GLCD_WindowMax ();
  GLCD_SetTextColor (Green);
  DrawRectangle (BTN_X_EVAL, BTN_Y_EVAL, 100, 40, 5);
#endif

  while (1) {
    /* Show potentiometer, keys, joystick */
    keyMsk = Keyboard_GetKeys();    /* Show buttons state                 */
    if (key ^ keyMsk) {
    GLCD_SetTextColor(Black);
    if (keyMsk & KEY_USER  )    { GLCD_DisplayString (4, 1, 1, "USER");   }
    if (keyMsk & KEY_TAMPER)    { GLCD_DisplayString (5, 1, 1, "TAMPER"); }
    if (keyMsk & KEY_WAKEUP  )  { GLCD_DisplayString (6, 1, 1, "WAKEUP"); }
    
    GLCD_SetTextColor(LightGrey);
    if (!(keyMsk & KEY_USER  )) { GLCD_DisplayString (4, 1, 1, "USER");   }
    if (!(keyMsk & KEY_TAMPER)) { GLCD_DisplayString (5, 1, 1, "TAMPER"); }
    if (!(keyMsk & KEY_WAKEUP)) { GLCD_DisplayString (6, 1, 1, "WAKEUP"); }

    joyMsk = Joystick_GetButtons(); /* Show joystick arrows               */
    if (joy ^ joyMsk) {
      joy = joyMsk;

      /* Left arrow white/red? */
      ofs = (joy & JOY_LEFT)   ? (2416) : (0);
      GLCD_Bitmap(JOY_X+ 0, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 0*532]);

      /* Right arrow white/red? */
      ofs = (joy & JOY_RIGHT)     ? (2416) : (0);
      GLCD_Bitmap(JOY_X+35, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 1*532]);

      /* Center dot white/red */
      ofs = (joy & JOY_CENTER) ? (2416) : (0);
      GLCD_Bitmap(JOY_X+21, JOY_Y+21, 12, 12, &Arrows_16bpp_red[ofs + 4*532]);

      /* Up arrow white/red? */
      ofs = (joy & JOY_UP)   ? (2416) : (0);
      GLCD_Bitmap(JOY_X+20, JOY_Y+ 0, 14, 19, &Arrows_16bpp_red[ofs + 2*532]);

      /* Down arrow white/red? */
      ofs = (joy & JOY_DOWN)  ? (2416) : (0);
      GLCD_Bitmap(JOY_X+20, JOY_Y+35, 14, 19, &Arrows_16bpp_red[ofs + 3*532]);
    }

    ADC_StartConversion();          /* Show A/D conversion bargraph       */
    adcVal = ADC_GetValue();
    if (adc ^ adcVal) {
      GLCD_SetTextColor(Green);
      GLCD_Bargraph (144, 3*24, 160, 20, adcVal >> 2);
    }

    Touch_GetState (&tsc_state);    /* Get touch screen state             */

    if (!tsc_state.pressed) {
      tsc_state.x = 0;
      tsc_state.y = 0;
    }

    if (tsc_state.x >= 200 && tsc_state.x <= 1000 &&
        tsc_state.y >= 260 && tsc_state.y <= 1650) {
      GLCD_WindowMax ();
#ifdef _EVAL_VERSION_
      /* Draw a square dot instead of bitmap */
      GLCD_SetTextColor(Red);
      DrawRectangle (BULB_X_EVAL, BULB_Y_EVAL, 20, 20, 15);
#else
      GLCD_Bitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[1*11520]);
      GLCD_Bitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[1*2080]);
#endif
    }
    else {
      GLCD_WindowMax ();
#ifdef _EVAL_VERSION_
      /* Draw a square dot instead of bitmap */
      GLCD_SetTextColor(Yellow);
      DrawRectangle (BULB_X_EVAL, BULB_Y_EVAL, 20, 20, 15);
#else
      GLCD_Bitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[0*11520]);
      GLCD_Bitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[0*2080]);
#endif
   }
   }
 }
}
