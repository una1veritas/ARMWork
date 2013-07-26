/******************************************************************************
 * @file:    system_LPC11Uxx.c
 * @purpose: CMSIS Cortex-M0 Device Peripheral Access Layer Source File
 *           for the NXP LPC11xx Device Series 
 * @version: V1.0
 * @date:    21. March. 2011
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2008 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M3 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/
#include <stdint.h>
#include "LPC11Uxx.h"
#include "power_api.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
// <e> Clock Configuration
//   <h> System Controls and Status Register (SCS)
//     <o1.4>    OSCRANGE: Main Oscillator Range Select
//                     <0=>  1 MHz to 20 MHz
//                     <1=> 15 MHz to 24 MHz
//     <e1.5>       OSCEN: Main Oscillator Enable
//     </e>
//   </h>
//
//   <h> Clock Source Select Register (CLKSRCSEL)
//     <o2.0..1>   CLKSRC: PLL Clock Source Selection
//                     <0=> Internal RC oscillator
//                     <1=> Main oscillator
//                     <2=> RTC oscillator
//   </h>
//
//   <e3> PLL0 Configuration (Main PLL)
//     <h> PLL0 Configuration Register (PLL0CFG)
//                     <i> F_cco0 = (2 * M * F_in) / N
//                     <i> F_in must be in the range of 32 kHz to 50 MHz
//                     <i> F_cco0 must be in the range of 275 MHz to 550 MHz
//       <o4.0..14>  MSEL: PLL Multiplier Selection
//                     <6-32768><#-1>
//                     <i> M Value
//       <o4.16..23> NSEL: PLL Divider Selection
//                     <1-256><#-1>
//                     <i> N Value
//     </h>
//   </e>
//
//   <e5> PLL1 Configuration (USB PLL)
//     <h> PLL1 Configuration Register (PLL1CFG)
//                     <i> F_usb = M * F_osc or F_usb = F_cco1 / (2 * P)
//                     <i> F_cco1 = F_osc * M * 2 * P
//                     <i> F_cco1 must be in the range of 156 MHz to 320 MHz
//       <o6.0..4>   MSEL: PLL Multiplier Selection
//                     <1-32><#-1>
//                     <i> M Value (for USB maximum value is 4)
//       <o6.5..6>   PSEL: PLL Divider Selection
//                     <0=> 2
//                     <1=> 4
//                     <2=> 8
//                     <3=> 16
//                     <i> P Value
//     </h>
//   </e>
//
//   <h> CPU Clock Configuration Register (CCLKCFG)
//     <o7.0..7>  CCLKSEL: Divide Value for CPU Clock from PLL0
//                     <0-255>
//                     <i> Divide is CCLKSEL + 1. Only 0 and odd values are valid.
//   </h>
//
//   <h> USB Clock Configuration Register (USBCLKCFG)
//     <o8.0..3>   USBSEL: Divide Value for USB Clock from PLL1
//                     <0-15>
//                     <i> Divide is USBSEL + 1
//   </h>
//
// </e>
*/
#define CLOCK_SETUP           1

#define SYS_PLL_SETUP         1
#define SYS_OSC_USED          1
#define SYS_PLLSRCSEL_Val     0x00000001
#define SYS_PLL_M_Val         0x00000003
#define SYS_PLL_P_Val         0x00000001
#define MAIN_CLKSRCSEL_Val    0x00000003
#define SYS_AHB_DIV_Val       1			/* 1 through 255, typical is 1 or 2 or 4 */
#define USB_CLK_SETUP         1			/* If the USB is used, this bit needs to be set */
#define USE_USB_PLL           0			/* If USB_PLL_SETUP is 0, USE_USB_PLL can't be 1,
                                        USB clock can be either from main PLL or USB PLL. */
#define USB_CLKSRCSEL_Val     0x00000001
#define USB_PLL_M_Val         0x00000003
#define USB_PLL_P_Val         0x00000001

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
    
/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define IRC_OSC     (12000000UL)        /* Internal RC oscillator frequency   */
#define WDT_OSC     (  250000UL)        /* WDT oscillator frequency           */

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
    
/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __XTAL            (12000000UL)    /* Oscillator frequency             */
#define __SYS_OSC_CLK     (    __XTAL)    /* Main oscillator frequency        */
#define __IRC_OSC_CLK     (12000000UL)    /* Internal RC oscillator frequency */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t ClockSource = IRC_OSC;
uint32_t SystemFrequency = IRC_OSC; /*!< System Clock Frequency (Core Clock)  */
uint32_t SystemAHBFrequency = IRC_OSC;
uint32_t SystemCoreClock = __IRC_OSC_CLK;/*!< System Clock Frequency (Core Clock)*/


void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  uint32_t wdt_osc = 0;

  /* Determine clock frequency according to clock register values             */
  switch ((LPC_SYSCON->WDTOSCCTRL >> 5) & 0x0F) {
    case 0:  wdt_osc =       0; break;
    case 1:  wdt_osc =  500000; break;
    case 2:  wdt_osc =  800000; break;
    case 3:  wdt_osc = 1100000; break;
    case 4:  wdt_osc = 1400000; break;
    case 5:  wdt_osc = 1600000; break;
    case 6:  wdt_osc = 1800000; break;
    case 7:  wdt_osc = 2000000; break;
    case 8:  wdt_osc = 2200000; break;
    case 9:  wdt_osc = 2400000; break;
    case 10: wdt_osc = 2600000; break;
    case 11: wdt_osc = 2700000; break;
    case 12: wdt_osc = 2900000; break;
    case 13: wdt_osc = 3100000; break;
    case 14: wdt_osc = 3200000; break;
    case 15: wdt_osc = 3400000; break;
  }
  wdt_osc /= ((LPC_SYSCON->WDTOSCCTRL & 0x1F) << 1) + 2;
 
  switch (LPC_SYSCON->MAINCLKSEL & 0x03) {
    case 0:                             /* Internal RC oscillator             */
      SystemCoreClock = __IRC_OSC_CLK;
      break;
    case 1:                             /* Input Clock to System PLL          */
      switch (LPC_SYSCON->SYSPLLCLKSEL & 0x03) {
          case 0:                       /* Internal RC oscillator             */
            SystemCoreClock = __IRC_OSC_CLK;
            break;
          case 1:                       /* System oscillator                  */
            SystemCoreClock = __SYS_OSC_CLK;
            break;
          case 2:                       /* Reserved                           */
          case 3:                       /* Reserved                           */
            SystemCoreClock = 0;
            break;
      }
      break;
    case 2:                             /* WDT Oscillator                     */
      SystemCoreClock = wdt_osc;
      break;
    case 3:                             /* System PLL Clock Out               */
      switch (LPC_SYSCON->SYSPLLCLKSEL & 0x03) {
          case 0:                       /* Internal RC oscillator             */
            if (LPC_SYSCON->SYSPLLCTRL & 0x180) {
              SystemCoreClock = __IRC_OSC_CLK;
            } else {
              SystemCoreClock = __IRC_OSC_CLK * ((LPC_SYSCON->SYSPLLCTRL & 0x01F) + 1);
            }
            break;
          case 1:                       /* System oscillator                  */
            if (LPC_SYSCON->SYSPLLCTRL & 0x180) {
              SystemCoreClock = __SYS_OSC_CLK;
            } else {
              SystemCoreClock = __SYS_OSC_CLK * ((LPC_SYSCON->SYSPLLCTRL & 0x01F) + 1);
            }
            break;
          case 2:                       /* Reserved                           */
          case 3:                       /* Reserved                           */
            SystemCoreClock = 0;
            break;
      }
      break;
  }

  SystemCoreClock /= LPC_SYSCON->SYSAHBCLKDIV;  

}

/**
 * Enable PLL via power API
 *
 * @param  none
 * @return none
 *
 * @brief  By default, PLL configuration is disabled by bootrom to preserve
 *         the power. SystemPLL_setup() can not be called if PLL configuration is
 *         disabled. At validation stage, all the options and parameters are enabled
 *         for now. This module may be removed once the chip validation is done.
 */
void EnablePLLViaPowerAPI( void )
{
  unsigned int command[5], result[5];             //command and result arrays
  ROM **rom;

  rom = (ROM **) 0x1FFF1FF8;                  // pointer for power API calls
  /* user must select the correct operating frequency before setting up the PLL   */
  /* or the voltage regulator and the flash interface setup might not be able     */
  /* to support application running at higher frequencies                         */
  command[0] = 48;                            //system freq 48 MHz
  command[1] = PARAM_DEFAULT;                 //specify system power to default
//  command[1] = PARAM_CPU_EXEC;                //specify system power for cpu performance run
//  command[1] = PARAM_EFFICIENCY;              //specify system power for efficiency
//  command[1] = PARAM_LOW_CURRENT;             //specify system power for low active current
  (*rom)->pPWRD->set_power(command,result);   //set system power
  if (result[0] != PARAM_CMD_SUCCESS){        //if a failure is reported...
    while(1);                               //... stay in the loop
  }
  return;
}

/**
 * Misc. clock generation modules
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemPLL_Setup ( void )
{
  uint32_t regVal;

  ClockSource = OSC_CLK;
  LPC_SYSCON->SYSPLLCLKSEL = SYS_PLLSRCSEL_Val;    /* Select system OSC */
  LPC_SYSCON->SYSPLLCLKUEN = 0x01;                 /* Update clock source */
  LPC_SYSCON->SYSPLLCLKUEN = 0x00;                 /* toggle Update register once */
  LPC_SYSCON->SYSPLLCLKUEN = 0x01;
  while ( !(LPC_SYSCON->SYSPLLCLKUEN & 0x01) ); /* Wait until updated */

  regVal = LPC_SYSCON->SYSPLLCTRL;
  regVal &= ~0x1FF;
  LPC_SYSCON->SYSPLLCTRL = (regVal | (SYS_PLL_P_Val<<5) | SYS_PLL_M_Val);
  
  /* Enable main system clock, main system clock bit 7 in PDRUNCFG. */
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<7);
  while ( !(LPC_SYSCON->SYSPLLSTAT & 0x01) );	/* Wait until it's locked */

  LPC_SYSCON->MAINCLKSEL = MAIN_CLKSRCSEL_Val;	/* Select PLL clock output */
  LPC_SYSCON->MAINCLKUEN = 0x01;		/* Update MCLK clock source */
  LPC_SYSCON->MAINCLKUEN = 0x00;		/* Toggle update register once */
  LPC_SYSCON->MAINCLKUEN = 0x01;
  while ( !(LPC_SYSCON->MAINCLKUEN & 0x01) );	/* Wait until updated */

  LPC_SYSCON->SYSAHBCLKDIV = SYS_AHB_DIV_Val;	/* SYS AHB clock, typical is 1 or 2 or 4 */

#if SYS_PLL_SETUP
  /* If the SYS PLL output is selected as the main clock. Even if SYS PLL is
  configured and enabled, it doesn't mean it will be selected as the MAIN clock
  source. Be careful with MAINCLKSEL value. If SYS PLL is not selected, System
  Frequence should be the same as either IRC, external OSC(SYS), or WDT OSC clock. */
  SystemFrequency = ClockSource * (SYS_PLL_M_Val+1);
#else
  SystemFrequency = ClockSource;
#endif
  SystemAHBFrequency = (uint32_t)(SystemFrequency/SYS_AHB_DIV_Val);
  return;
}

/**
 * USB clock generation modules
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the USB clock.
 *         USB clock can be obtained from either System clock if it's set to 48Mhz
 *         or from USB PLL (48Mhz). If USB PLL is not used, it should be tuned
 *         off in PDRUNCFG register.
 */
void USB_PLL_Setup ( void )
{
  uint32_t regVal;

  /* Enable USB PLL. USB clock and PHY bit 8 and 10 in PDRUNCFG. */
  LPC_SYSCON->PDRUNCFG &= ~((0x1<<8)|(0x1<<10));

  LPC_SYSCON->USBPLLCLKSEL = USB_CLKSRCSEL_Val;    /* Select system OSC */
  LPC_SYSCON->USBPLLCLKUEN = 0x01;                 /* Update clock source */
  LPC_SYSCON->USBPLLCLKUEN = 0x00;                 /* toggle Update register once */
  LPC_SYSCON->USBPLLCLKUEN = 0x01;
  while ( !(LPC_SYSCON->USBPLLCLKUEN & 0x01) );    /* Wait until updated */

  regVal = LPC_SYSCON->USBPLLCTRL;
  regVal &= ~0x1FF;
  LPC_SYSCON->USBPLLCTRL = (regVal | (USB_PLL_P_Val<<5) | USB_PLL_M_Val);
  
  while ( !(LPC_SYSCON->USBPLLSTAT & 0x01) );	/* Wait until it's locked */

#if USE_USB_PLL
  LPC_SYSCON->USBCLKSEL = 0x00;               /* Select USB PLL */
#else
  /* If it comes here, System PLL clock has to be multiple of 48Mhz */
  LPC_SYSCON->USBCLKSEL = 0x01;               /* Select System clock */
#endif
  
  LPC_SYSCON->USBCLKUEN = 0x01;               /* Update clock */
  LPC_SYSCON->USBCLKUEN = 0x00;               /* Toggle update register once. */
  LPC_SYSCON->USBCLKUEN = 0x01;
  while ( !(LPC_SYSCON->USBCLKUEN & 0x01) );  /* Wait until updated */
  LPC_SYSCON->USBCLKDIV = 1;                  /* USB clock must be 48Mhz. */
#if !USE_USB_PLL
  /* Once the USB PLL is not used as the USB clock, the PDRUN for the USB
  PLL can be turned off. */
  LPC_SYSCON->PDRUNCFG |= (0x1<<8);
#endif 
  return;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemInit (void)
{
  uint32_t i;

  EnablePLLViaPowerAPI();

#ifdef __DEBUG_RAM    
  LPC_SYSCON->SYSMEMREMAP = 0x1;		/* remap to internal RAM */ 
#else
#ifdef __DEBUG_FLASH    
  LPC_SYSCON->SYSMEMREMAP = 0x2;		/* remap to internal flash */
#endif
#endif

#if (CLOCK_SETUP)                       /* Clock Setup */
  /* bit 0 default is crystal bypass, 
  bit1 0=0~20Mhz crystal input, 1=15~50Mhz crystal input. */
#if (SYS_OSC_USED)
  /* System OSC is used, no bypass, enable main sys OSC in PDRUNCFG. */
  LPC_SYSCON->SYSOSCCTRL = 0x00;
  /* main system OSC run is cleared, bit 5 in PDRUNCFG register */
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<5);
#else
  /* bypass enabled, to save power, disable main sys OSC in PDRUNCFG */
  LPC_SYSCON->SYSOSCCTRL = 0x01;
  /* main system OSC run is NOT cleared, bit 5 in PDRUNCFG register */
  LPC_SYSCON->PDRUNCFG |= (0x1<<5);
#endif
  /* Wait 200us for OSC to be stablized, no status 
  indication, dummy wait. */
  for ( i = 0; i < 0x100; i++ );

#if (SYS_PLL_SETUP)
  SystemPLL_Setup();
#endif

#if (USB_CLK_SETUP)
  USB_PLL_Setup();
#else 
  /* Enable USB clock. USB clock bit 8 and 10 in PDRUNCFG. */
  LPC_SYSCON->PDRUNCFG &= ~((0x1<<8)|(0x1<<10));
#endif

#endif	/* endif CLOCK_SETUP */

  /* System clock to the IOCON needs to be enabled or
  most of the I/O related peripherals won't work. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);
  return;
}
