/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	rgb : HSL<=>RGB conversion using LPCXpresso Baseboard
 * File:	main.c
 * Description:
 * 			This is the main body of the program.
 *
 * 			If a debugger is detected the program will operate in
 * 			interactive mode, where the user can request specific HSL
 * 			colors to be displayed, or initiate a cycle sequence
 *
 * 			If the program is started and no debugger is found it will
 * 			initiate the cycle sequence automatically.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

/**********************************************************************
 * NOTE! - 	Please be aware that this application requires a wire
 * 			jumper connecting PIO1_2 to PIO1_11 be installed on the
 * 			base board.
 **********************************************************************/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "debug.h"
#include "small_systick.h"
#include "small_utils.h"
#include "rgb.h"

#include <stdint.h>

volatile uint32_t SysTickCount;    			/* counts 10ms timeTicks */
uint32_t DelayStart_SysTickCount;

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    SysTickCount++;                        /* increment counter necessary in Delay() */
}

#define LED_DELAY 25
#define FADE_DELAY 1
#define FADE_STEPS 25
#define HUE_STEP 1

typedef struct {
    uint32_t hsl;
    char name[8];
} Color;

const Color Colors[] =
{
        { 0x00FF3F, "maroon" },
        { 0x00FF7F, "red" },
        { 0x2AFF7F, "yellow" },
        { 0x54FF7F, "green" },
        { 0xAAFF7F, "blue" },
        { 0xAAFF3F, "navy" },
        { 0x7FFF7F, "aqua" },
        { 0xD4FF7F, "purple" },
        { 0xD5FF7F, "fuchsia" },
        { 0x0000FF, "white" },
        { 0x00007F, "gray" },
        { 0x000000, "black" },
        { 0, "" }
};

int main(void)
{
    double red, green, blue;
#ifdef DEBUG
    int i;
#endif

    //SystemInit(); is called by startup code

    //GPIO on LPC17XX is always enabled
    InitLEDs();
    InitSysTick(100);

#ifdef DEBUG
    if(ISDEBUGACTIVE())
    {
    	debug_puts("Enter 6 digit hex HSL color code or color name. Examples:\n");
    	for(i=0;;i++)
    	{
    		if(!Colors[i].name[0])
    			break;
    		debug_printf("\t%7s %06X", Colors[i].name, Colors[i].hsl);
    		if((i%4)==3)
    			debug_puts("\n");
    	}
    	if(i%4)
    		debug_puts("\n");
    }
#endif

    while(1)
    {
#ifdef DEBUG
        uint32_t hsl;
        int n;
        char *p;

        debug_puts("Color name/code ([G]o to free run): ");
        p = debug_getstr(&n);
        small_strim(p); // remove whitespace from end of string
        if(n!=0 && !((p[0] == 'g' || p[0] == 'G') && p[1]==0))
        {
            int fade_steps = 0;

            for(i=0;;i++)
            {
                if(!Colors[i].name[0])
                {
                    hsl = gethex(p);
                    debug_printf("HSL parsed from hex. HSL=%06X\n", hsl);
                    break;
                }
                if(!small_stricmp(Colors[i].name, p))
                {
                    hsl = Colors[i].hsl;
                    debug_printf("Selected HSL=%06X by name \"%s\"\n", hsl, Colors[i].name);
                    break;
                }
            }

             HSLtoRGB((((double)((hsl>>16)&0xFF)) / 255)*360,
                        ((double)((hsl>> 8)&0xFF)) / 255,
                        ((double) (hsl&0xFF))      / 255,
                    &red, &green, &blue);

            debug_printf("DEBUG: Setting red=%2.1f%% green=%2.1f%% blue=%2.1f%%\n",
                    red*100, green*100, blue*100);

            StartFade(red, green, blue, FADE_STEPS);

            debug_printf("DEBUG: New PWM values red=%d, green=%d, blue=%d\n",
                    newred, newgreen, newblue);
            debug_printf_flush();
            while(!FadeStep())
            {
                DelayStart_SysTickCount = SysTickCount;
                while((SysTickCount - DelayStart_SysTickCount) < FADE_DELAY)
                    __WFI();
                fade_steps++;
            }
            debug_printf("DEBUG: Color fade done after %d steps.\n", fade_steps);
        } else
#endif
        {
            int w = 0;
            double hue, lasthue;

            hue = lasthue = 0;

            debug_puts("Freerunning hue changes.\n");
            debug_printf_flush();
            while(1)
            {
                hue = small_fmodf(hue + HUE_STEP, 360);
                if(hue < lasthue)
                {
                    w++;
                    debug_printf("Wrap %d at %2d minutes %02.1f seconds\n", w, (SysTickCount/100) / 60,
                            small_fmodf((((double)SysTickCount)/100),60));
                    debug_printf_flush();
                }
                lasthue = hue;

                HSLtoRGB(hue, 1.0, 0.5,
                        &red, &green, &blue);

                SetRGB(red, green, blue);

                DelayStart_SysTickCount = SysTickCount;

                while((SysTickCount >= DelayStart_SysTickCount)
                    && ((SysTickCount - DelayStart_SysTickCount) < FADE_DELAY))
                    __WFI();
            }
        }
    }
    return 0 ;
}

