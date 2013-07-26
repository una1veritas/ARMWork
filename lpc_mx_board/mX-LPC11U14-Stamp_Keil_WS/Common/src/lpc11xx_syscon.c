/***********************************************************************//**
 * @file	: lpc11xx_syscon.c
 * @brief	: Contains all functions support for SYSCON firmware library on LPC11xx
 * @version	: 1.0
 * @date	: 20. Jan. 2010
 * @author	: Coocox
 **********************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup SYSCON
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc11xx_syscon.h"
#include "lpc11xx_libcfg.h"
extern uint32_t SystemAHBFrequency;

						 
#if _SYSCON


/* Public Functions ---------------------------------------------------------- */
/** @defgroup SYSCON_Public_Functions
 * @{
 */

/*********************************************************************//**
 * @brief 		Sets remap mode, selects whether the ARM interrupt vectors 
 *              are read from the boot ROM, the flash, or the SRAM
 * @param[in]	MapMode	remap mode, should be one of the following:
 *              - MAP_BootLoader_MODE : Interrupt vectors are re-mapped 
 *                                      to Boot ROM
 *              - MAP_RAM_MODE        : Interrupt vectors are re-mapped 
 *                                      to Static RAM
 *              - MAP_FLASH_MODE      : Interrupt vectors are not re-mapped 
 *                                      and reside in Flash.          
 *
 * @return none
 **********************************************************************/
void SYSCON_MemRemap(uint32_t MapMode)
{
    CHECK_PARAM(PARAM_MapMode(MapMode)); 

    LPC_SYSCON->SYSMEMREMAP = MapMode;
}


/*********************************************************************//**
 * @brief 		Get PLL Lock status
 * @param[in]	None
 * @return  The new state of Lock (SET or RESET)
 **********************************************************************/
FlagStatus SYSCON_GetLockStatus(void)
{
    FlagStatus lockstatus = RESET;

    if (LPC_SYSCON->SYSPLLSTAT & SYSCONF_PLL_LOCK) {
        lockstatus = SET;
    } else {
        lockstatus = RESET;
    }  

    return lockstatus;
}


/*********************************************************************//**
 * @brief 		Select PLL clock source
 * @param[in]	ClkType clock source, it can be:
 *               - SYSCON_PLL_SOURCE_IRC    : IRC oscillator
 *               - SYSCON_PLL_SOURCE_SYSOSC : System oscillator
 *               - SYSCON_PLL_SOURCE_RES    : Reserved
 * @return      None
 **********************************************************************/
void SYSCON_PLLClkSel(uint32_t ClkType)
{
    CHECK_PARAM(PARAM_ClkType(ClkType));    

    LPC_SYSCON->SYSPLLCLKSEL = ClkType;

    //SYSCON_PLLUpdate();
}


/*********************************************************************//**
 * @brief 		Updates the clock source of the system PLL with the new 
 *              input clock after the SYSPLLCLKSEL register has been 
 *              written to. In order for the update to take effect, 
 *              first write a 0, then write 1
 * @param[in]	None
 * @return      None
 **********************************************************************/
void SYSCON_PLLUpdate(void)
{
    LPC_SYSCON->SYSPLLCLKUEN = 1; 
    LPC_SYSCON->SYSPLLCLKUEN = 0; 
    LPC_SYSCON->SYSPLLCLKUEN = 1; 

    while (!(LPC_SYSCON->SYSPLLCLKUEN & 1));
}

/*********************************************************************//**
 * @brief 		Configure PLL multiplier and divider values
 * @param[in]   msel Feedback divider value (msel + 1, 1 - 32)
 *              psel Post divider ratio P   (P * 2), it can be
 *               - SYSCON_PLL_P_1 : P = 1
 *               - SYSCON_PLL_P_2 : P = 2
 *               - SYSCON_PLL_P_4 : P = 4
 *               - SYSCON_PLL_P_8 : P = 8
 * @return      None
 **********************************************************************/
void SYSCON_PLLConfig(uint32_t msel, uint32_t psel) 
{
    uint32_t reg = 0;

    CHECK_PARAM(PARAM_MSEL(msel));
    CHECK_PARAM(PARAM_PSEL(psel));    

    reg = msel - 1;
  
    switch (psel) {
        case SYSCON_PLL_P_1:
            break;
        case SYSCON_PLL_P_2:
            reg |= (uint32_t)1<<6;
            break;
        case SYSCON_PLL_P_4:
            reg |= (uint32_t)2<<6;
            break;
        case SYSCON_PLL_P_8:
            reg |= (uint32_t)3<<6;
            break;
        default: break;
    }
    
    LPC_SYSCON->SYSPLLCTRL = reg;
}


/*********************************************************************//**
 * @brief 	   Reset the SPI or I2C peripherals	
 * @param[in]  Periph it can be	
 *              - SYSCON_RSTPeriph_SSP0 : Reset SSP0
 *              - SYSCON_RSTPeriph_I2C  : Reset I2C                           
 *              - SYSCON_RSTPeriph_SSP1 : Reset SSP1
 *              NewState new state of the specified peripheral, it can be:
 *              - ENABLE  : Resets the peripheral
 *              - DISABLE : Peripheral reset de-asserted
 *
 * @return none
 **********************************************************************/
void SYSCON_PeriphResetCmd(uint32_t Periph, FunctionalState NewState)
{
    if(NewState == ENABLE) {
        LPC_SYSCON->PRESETCTRL &= (~Periph) & 0x7;  
    } else if (NewState == DISABLE) {
        LPC_SYSCON->PRESETCTRL |= Periph & 0x7;          
    }       
}


/*********************************************************************//**
 * @brief 		Configure power supply for each peripheral according to NewState
 * @param[in]	Periph	Type of peripheral used to enable power,
 *     					should be one of the following:
 *              -  SYSCON_AHBPeriph_SYS        : AHB to APB bridge
 *              -  SYSCON_AHBPeriph_ROM	       : ROM
 *              -  SYSCON_AHBPeriph_RAM	       : RAM
 *              -  SYSCON_AHBPeriph_FLASHREG   : Flash register interface
 *              -  SYSCON_AHBPeriph_FLASHARRAY : Flash array access
 *              -  SYSCON_AHBPeriph_I2C        : I2C
 *              -  SYSCON_AHBPeriph_GPIO	   : GPIO
 *              -  SYSCON_AHBPeriph_CT16B0     : 16-bit counter/timer 0
 *              -  SYSCON_AHBPeriph_CT16B1     : 16-bit counter/timer 1
 *              -  SYSCON_AHBPeriph_CT32B0     : 32-bit counter/timer 0
 *              -  SYSCON_AHBPeriph_CT32B1     : 32-bit counter/timer 1
 *              -  SYSCON_AHBPeriph_SSP0       : SSP0
 *              -  SYSCON_AHBPeriph_UART       : UART
 *              -  SYSCON_AHBPeriph_ADC        : ADC
 *              -  SYSCON_AHBPeriph_WDT        : WDT
 *              -  SYSCON_AHBPeriph_IOCON      : IOCON
 *              -  SYSCON_AHBPeriph_SSP1       : SSP1             
 * @param[in]	NewState	New state of Peripheral Power, should be:
 * 				- ENABLE	: Enable power for this peripheral
 * 				- DISABLE	: Disable power for this peripheral
 *
 * @return none
 **********************************************************************/
void SYSCON_AHBPeriphClockCmd(uint32_t Periph, FunctionalState NewState)
{
    if(NewState == DISABLE) {
        LPC_SYSCON->SYSAHBCLKCTRL &= (~Periph) & SYSCON_AHBPeriph_BITMASK;
    } else if (NewState == ENABLE) {
        LPC_SYSCON->SYSAHBCLKCTRL |= Periph & SYSCON_AHBPeriph_BITMASK;    
    }  	
}


/*********************************************************************//**
 * @brief 		 Selects the main system clock source
 * @param[in]	 SYSCON_MCK_source Clock source, it can be:
 *                - SYSCON_MCK_IRC      : IRC oscillator
 *                - SYSCON_MCK_PLLINPUT : Input clock to system PLL
 *                - SYSCON_MCK_WDTOSC   : WDT oscillator
 *                - SYSCON_MCK_PLL      : System PLL clock out 
 * @return none
 **********************************************************************/
void SYSCON_MCKSel(uint32_t SYSCON_MCK_source) 
{
    CHECK_PARAM(PARAM_MCKSEL(SYSCON_MCK_source));    
    
    LPC_SYSCON->MAINCLKSEL = SYSCON_MCK_source;
    
    //SYSCON_MCKUpdate();       
}

/*********************************************************************//**
 * @brief 		 Updates the clock source of the main clock with the new 
 *               input clock
 * @param[in]	 None
 * @return       None
 **********************************************************************/
void SYSCON_MCKUpdate (void)
{
    LPC_SYSCON->MAINCLKUEN = 1;                 
    LPC_SYSCON->MAINCLKUEN = 0;
    LPC_SYSCON->MAINCLKUEN = 1;

    /* Wait until updated */
    while (!(LPC_SYSCON->MAINCLKUEN & 1));
}


/*********************************************************************//**
 * @brief 		Divides the main clock to provide the system clock to 
 *              the core, memories, and the peripherals. The system clock 
 *              can be shut down completely by setting the DivVal to 0x0.
 * @param[in]	DivVal	Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetAHBClockDiv(uint32_t DivVal) 
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));  
    
    LPC_SYSCON->SYSAHBCLKDIV = DivVal;
}

/*********************************************************************//**
 * @brief 		Configures the SPI0 peripheral clock SPI0_PCLK. The 
 *              SPI0_PCLK can be shut down by setting the DIV bits to 0x0.
 * @param[in]	DivVal	Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetSPI0ClockDiv(uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));
    LPC_SYSCON->SSP0CLKDIV = DivVal;
}


/*********************************************************************//**
 * @brief 		Configures the SPI1 peripheral clock SPI10_PCLK. The 
 *              SPI10_PCLK can be shut down by setting the DivVal to 0x0.
 * @param[in]	DivVal	Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetSPI1ClockDiv(uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));
    LPC_SYSCON->SSP1CLKDIV = DivVal;
}

/*********************************************************************//**
 * @brief 		Configures the UART peripheral clock UART_PCLK. The 
 *              UART_PCLK can be shut down by setting the DivVal to 0x0.
 * @param[in]	DivVal	Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetUARTClockDiv(uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));
    LPC_SYSCON->UARTCLKDIV = DivVal;
}


/*********************************************************************//**
 * @brief 		Determines the divider values for the watchdog clock wdt_clk
 * @param[in]	DivVal Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetWDTClockDiv(uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));
  //  LPC_SYSCON->WDTCLKDIV = DivVal;
}


/*********************************************************************//**
 * @brief 		Determines the divider value for the clkout_clk signal 
 *              on the CLKOUT pin
 * @param[in]	DivVal	Value of divider           
 *
 * @return none
 **********************************************************************/
void SYSCON_SetCLKOUTClockDiv(uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));
    LPC_SYSCON->CLKOUTDIV = DivVal;
}


/*********************************************************************//**
 * @brief 		Configures the frequency range for the system oscillator.
 * @param[in]	bypass  Enable or Disable system oscillator Bypass                
 *              SYSCON_MOSC_FREQRANG_x Determines frequency range for 
 *              Low-power oscillator, it can be:
 *               - SYSCON_MOSC_FREQRANG_1_20  : 1 - 20 MHz frequency range
 *               - SYSCON_MOSC_FREQRANG_15_25 : 15 - 25 MHz frequency range
 * @return none
 **********************************************************************/
void SysOscConfig (FunctionalState bypassState, uint32_t SYSCON_MOSC_FREQRANG_x)
{
    uint32_t reg = 0;
    if (bypassState == ENABLE) {
        reg |= 1;
    }  
    
    if (SYSCON_MOSC_FREQRANG_x == SYSCON_MOSC_FREQRANG_15_25) {
        reg |= (uint32_t)1 << 1;
    } 

    LPC_SYSCON->SYSOSCCTRL = reg;
}


/*********************************************************************//**
 * @brief 		Configures the watchdog oscillator
 * @param[in]	div  divider for Fclkana, wdt_osc_clk = Fclkana/(2 ¡Á (1 + DIVSEL))
 *              FreqSel Select watchdog oscillator analog output frequency (Fclkana)
 *              it can be:
 *               - SYSCON_WDT_FREQ_0_5 : 0.5 MHz
 *               - SYSCON_WDT_FREQ_0_8 : 0.8 MHz
 *               - SYSCON_WDT_FREQ_1_1 : 1.1 MHz
 *               - SYSCON_WDT_FREQ_1_4 : 1.4 MHz
 *               - SYSCON_WDT_FREQ_1_6 : 1.6 MHz (Reset)
 *               - SYSCON_WDT_FREQ_1_8 : 1.8 MHz
 *               - SYSCON_WDT_FREQ_2_0 : 2.0 MHz
 *               - SYSCON_WDT_FREQ_2_2 : 2.2 MHz
 *               - SYSCON_WDT_FREQ_2_4 : 2.4 MHz
 *               - SYSCON_WDT_FREQ_2_6 : 2.6 MHz
 *               - SYSCON_WDT_FREQ_2_7 : 2.7 MHz
 *               - SYSCON_WDT_FREQ_2_9 : 2.9 MHz
 *               - SYSCON_WDT_FREQ_3_1 : 3.1 MHz
 *               - SYSCON_WDT_FREQ_3_2 : 3.2 MHz
 *               - SYSCON_WDT_FREQ_3_4 : 3.4 MHz
 * @return none
 **********************************************************************/
void SYSCON_WDTOscConfig (uint32_t div, uint32_t FreqSel)
{
    CHECK_PARAM(PARAM_WDTDIV(div));
    CHECK_PARAM(PARAM_FREQSEL(FreqSel));
    LPC_SYSCON->WDTOSCCTRL = div | (FreqSel << 5);   
}


/*********************************************************************//**
 * @brief 		Configures the watchdog oscillator
 * @param[in]	ClkSel WDT Clock source, it can be:
 *               - SYSCON_WDT_CLK_IRC    : IRC oscillator
 *               - SYSCON_WDT_CLK_MCK    : Main clock
 *               - SYSCON_WDT_CLK_WDTOSC : Watchdog oscillator
 * @return none
 **********************************************************************/
void SYSCON_WDTClkSel (uint32_t ClkSel)
{
    CHECK_PARAM(PARAM_WDTCLKSEL(ClkSel)); 
        
  //  LPC_SYSCON->WDTCLKSEL = ClkSel;

    //SYSCON_WDTClkUpdate();
    
}


/*********************************************************************//**
 * @brief 		Updates the clock source of the watchdog timer
 * @param[in]	None
 * @return      None
 **********************************************************************/
void SYSCON_WDTClkUpdate (void)
{
   // LPC_SYSCON->WDTCLKUEN = 1;
  //  LPC_SYSCON->WDTCLKUEN = 0;
  //  LPC_SYSCON->WDTCLKUEN = 1;

   // while(!(LPC_SYSCON->WDTCLKUEN & 1));
}


/*********************************************************************//**
 * @brief      Configures the clkout_clk signal to be output on the CLKOUT pin
 * @param[in]  ClkSel CLKOUT clock source, it can be:
 *              - SYSCON_CLKOUT_CLK_IRC    : IRC oscillator
 *              - SYSCON_CLKOUT_CLK_SYSOSC : System oscillator
 *              - SYSCON_CLKOUT_CLK_WDTOSC : Watchdog oscillator
 *              - SYSCON_CLKOUT_CLK_MCK    : Main clock
 * @return      None
 **********************************************************************/
void SYSCON_CLKOUTClkSel (uint32_t ClkSel)
{
    if (ClkSel <=  3) {
      //  LPC_SYSCON->CLKOUTCLKSEL = ClkSel;

        //SYSCON_CLKOUTClkUpdate();
    }
}


/*********************************************************************//**
 * @brief      Determines the divider value for the clkout_clk signal on 
 *             the CLKOUT pin
 * @param[in]  DivVal Clock divider values (0 - 255)
 *             The clkout_clk can be shut down by setting the DivVal to 0x0.
 * @return     None
 **********************************************************************/
void SYSCON_CLKOUTClkDiv (uint32_t DivVal)
{
    CHECK_PARAM(PARAM_DIVVAL(DivVal));    
    LPC_SYSCON->CLKOUTDIV = DivVal;
}


/*********************************************************************//**
 * @brief      Updates the clock source of the CLKOUT pin               
 * @param[in]  None
 *             
 * @return     None
 **********************************************************************/
void SYSCON_CLKOUTClkUpdate (void)
{
    LPC_SYSCON->CLKOUTUEN = 1;
    LPC_SYSCON->CLKOUTUEN = 0;
    LPC_SYSCON->CLKOUTUEN = 1;

    while(!(LPC_SYSCON->CLKOUTUEN & 1));
}


/*********************************************************************//**
 * @brief      Captures the state (HIGH or LOW) of the PIO pins of ports 0,1, 
 *             2 and 3(pins PIO3_0 to PIO3_5) at power-on-reset.
 * @param[in]  portNum Port number, can be 0, 1, 2, 3
 * @param[in]  pinNum  Pins number, can be 0-11 (if Port is 0,1,2) or 
 *             0-5 (if Port is 3)
 *             
 * @return     None
 **********************************************************************/
FlagStatus SYSCON_GetPIORstState(uint32_t portNum, uint32_t pinNum)
{
    FlagStatus pinSta = RESET;
    uint32_t regTmp;

    if((portNum<=2 && pinNum<=11) || (portNum==3 && pinNum<=5)) {
        switch (portNum) {
            case 0:         // PIO0
                regTmp = LPC_SYSCON->PIOPORCAP0;
                if (regTmp & ((uint32_t)1<<pinNum)) {
                    pinSta = SET;
                }                
                break;
                   
            case 1:         // PIO1
                regTmp = LPC_SYSCON->PIOPORCAP0 >> 12;
                if (regTmp & ((uint32_t)1<<pinNum)) {
                    pinSta = SET;
                }
                break; 
                               
            case 2:         // PIO2
                if (pinNum <= 7) {
                    regTmp = LPC_SYSCON->PIOPORCAP0 >> 24;
                    if (regTmp & ((uint32_t)1<<pinNum)) {
                        pinSta = SET;
                    }
                } else {
                    regTmp = LPC_SYSCON->PIOPORCAP1;
                    if (regTmp & ((uint32_t)1<<(pinNum-8))) {
                        pinSta = SET;
                    }
                }
                break;

            case 3:         // PIO3
                regTmp =  LPC_SYSCON->PIOPORCAP1 >> 4;
                if (regTmp & ((uint32_t)1<<pinNum)) {
                    pinSta = SET;
                }
        }
    }    

    return pinSta;
}


/*********************************************************************//**
 * @brief      Control the power to the analog blocks
 * @param[in]  block   Analog block, it can be
 *              - SYSCON_ABLOCK_IRCOUT : IRC oscillator output wake-up configuration
 *              - SYSCON_ABLOCK_IRC    : IRC oscillator power-down wake-up configuration 
 *              - SYSCON_ABLOCK_FLASH  : Flash wake-up configuration
 *              - SYSCON_ABLOCK_BOD    : BOD wake-up configuration
 *              - SYSCON_ABLOCK_ADC    : ADC wake-up configuration
 *              - SYSCON_ABLOCK_SYSOSC : System oscillator wake-up configuration
 *              - SYSCON_ABLOCK_WDTOSC : Watchdog oscillator wake-up configuration
 *              - SYSCON_ABLOCK_SYSPLL : System PLL wake-up configuration
 * @param[in]  NewState  power state, it can be:
 *              - ENABLE: Power
 *              - DISABLE: Powered down
 *             
 * @return     None
 **********************************************************************/
void SYSCON_PowerCon (uint32_t block, FunctionalState NewState)
{
    uint32_t reg;

    reg = LPC_SYSCON->PDRUNCFG & SYSCON_ABLOCK_BITMASK;
    reg |= 0x800;

    if (NewState == ENABLE) {
        LPC_SYSCON->PDRUNCFG = reg & ((~block) & SYSCON_ABLOCK_BITMASK);
    } else if (NewState == DISABLE) {
        LPC_SYSCON->PDRUNCFG = reg | (block & SYSCON_ABLOCK_BITMASK);
    }       
}


/*********************************************************************//**
 * @brief      Get Power state of analog blocks
 * @param[in]  block   Analog block, it can be
 *              - SYSCON_ABLOCK_IRCOUT : IRC oscillator output wake-up configuration
 *              - SYSCON_ABLOCK_IRC    : IRC oscillator power-down wake-up configuration 
 *              - SYSCON_ABLOCK_FLASH  : Flash wake-up configuration
 *              - SYSCON_ABLOCK_BOD    : BOD wake-up configuration
 *              - SYSCON_ABLOCK_ADC    : ADC wake-up configuration
 *              - SYSCON_ABLOCK_SYSOSC : System oscillator wake-up configuration
 *              - SYSCON_ABLOCK_WDTOSC : Watchdog oscillator wake-up configuration
 *              - SYSCON_ABLOCK_SYSPLL : System PLL wake-up configuration
 * @param[in]  None
 *             
 * @return     NewState  power state, it can be:
 *              - ENABLE: Power
 *              - DISABLE: Powered down
 **********************************************************************/
FunctionalState SYSCON_GetPowerSta (uint32_t block)
{
    FunctionalState powerSta = DISABLE; 
    if(LPC_SYSCON->PDRUNCFG & (block & SYSCON_ABLOCK_BITMASK)) {
        powerSta = DISABLE;
    } else {
        powerSta = ENABLE;
    }

    return powerSta;
}

/*********************************************************************//**
 * @brief      Indicate the state the chip must enter when the Deep-sleep 
 *             mode is asserted by the ARM.
 * @param[in]  block   Analog block, it can be
 *              - SYSCON_ABLOCK_IRCOUT : IRC oscillator output wake-up configuration
 *              - SYSCON_ABLOCK_IRC    : IRC oscillator power-down wake-up configuration 
 *              - SYSCON_ABLOCK_FLASH  : Flash wake-up configuration
 *              - SYSCON_ABLOCK_BOD    : BOD wake-up configuration
 *              - SYSCON_ABLOCK_ADC    : ADC wake-up configuration
 *              - SYSCON_ABLOCK_SYSOSC : System oscillator wake-up configuration
 *              - SYSCON_ABLOCK_WDTOSC : Watchdog oscillator wake-up configuration
 *              - SYSCON_ABLOCK_SYSPLL : System PLL wake-up configuration
 * @param[in]  NewState  power state, it can be:
 *              - ENABLE: Powered in Deep-sleep mode
 *              - DISABLE: Powered down in Deep-sleep mode
 *             
 * @return     None
 **********************************************************************/
void SYSCON_DeepSleepPowerCon (uint32_t block, FunctionalState NewState)
{    
    uint32_t reg;

    reg = LPC_SYSCON->PDSLEEPCFG & SYSCON_ABLOCK_BITMASK;
    reg |= 0x800;

    if (NewState == ENABLE) {
        LPC_SYSCON->PDSLEEPCFG = reg & ((~block) & SYSCON_ABLOCK_BITMASK);
    } else if (NewState == DISABLE) {
        LPC_SYSCON->PDSLEEPCFG = reg | (block & SYSCON_ABLOCK_BITMASK);
    }   
}

/*********************************************************************//**
 * @brief      Indicate the state the chip must enter when 
 *             it is waking up from Deep-sleep mode.
 * @param[in]  block   Analog block, it can be
 *              - SYSCON_ABLOCK_IRCOUT : IRC oscillator output wake-up configuration
 *              - SYSCON_ABLOCK_IRC    : IRC oscillator power-down wake-up configuration 
 *              - SYSCON_ABLOCK_FLASH  : Flash wake-up configuration
 *              - SYSCON_ABLOCK_BOD    : BOD wake-up configuration
 *              - SYSCON_ABLOCK_ADC    : ADC wake-up configuration
 *              - SYSCON_ABLOCK_SYSOSC : System oscillator wake-up configuration
 *              - SYSCON_ABLOCK_WDTOSC : Watchdog oscillator wake-up configuration
 *              - SYSCON_ABLOCK_SYSPLL : System PLL wake-up configuration
 * @param[in]  NewState  power state, it can be:
 *              - ENABLE: Powered
 *              - DISABLE: Powered down
 *             
 * @return     None
 **********************************************************************/
void SYSCON_WakeupPowerCon (uint32_t block, FunctionalState NewState)
{
    uint32_t reg;

    reg = LPC_SYSCON->PDAWAKECFG & SYSCON_ABLOCK_BITMASK;
    reg |= 0x800;

    if (NewState == ENABLE) {
        LPC_SYSCON->PDAWAKECFG = reg & ((~block) & SYSCON_ABLOCK_BITMASK);
    } else if (NewState == DISABLE) {
        LPC_SYSCON->PDAWAKECFG = reg | (block & SYSCON_ABLOCK_BITMASK);
    }
}


/*********************************************************************//**
 * @brief      Get WDT oscillator freq
 * @param[in]  None
 *             
 * @return     WDT oscillator freq
 **********************************************************************/
uint32_t SYSCON_GetWDTOsc(void)
{
    uint32_t sel;
    uint32_t freq;

    sel = LPC_SYSCON->WDTOSCCTRL;

    switch ((sel>>5) & 0xF) {
    case 0x0:
        freq = 0; break;
    case 0x1:
        freq = 500000; break;
    case 0x2:
        freq = 800000; break;
    case 0x3:
        freq = 1100000; break;
    case 0x4:
        freq = 1400000; break;
    case 0x5:
        freq = 1600000; break;  
    case 0x6:
        freq = 1800000; break;  
    case 0x7:
        freq = 2000000; break;  
    case 0x8:
        freq = 2200000; break;  
    case 0x9:
        freq = 2400000; break;  
    case 0xA:
        freq = 2600000; break;  
    case 0xB:
        freq = 2700000; break;  
    case 0xC:
        freq = 2900000; break;  
    case 0xD:
        freq = 3100000; break;  
    case 0xE:
        freq = 3200000; break;  
    case 0xF:
        freq = 3400000; break;  
    }

    freq = freq / (((sel&0x1F)+1) * 2);

    return freq;
}


/*********************************************************************//**
 * @brief      Edge select for start logic input
 * @param[in]  pins the selected start logic inputs, it can be:
 *                    - START_LOGIC_PIO0_0
 *                    - START_LOGIC_PIO0_1
 *                    - START_LOGIC_PIO0_2
 *                    - START_LOGIC_PIO0_3
 *                    - START_LOGIC_PIO0_4
 *                    - START_LOGIC_PIO0_5
 *                    - START_LOGIC_PIO0_6
 *                    - START_LOGIC_PIO0_7
 *                    - START_LOGIC_PIO0_8
 *                    - START_LOGIC_PIO0_9
 *                    - START_LOGIC_PIO0_10
 *                    - START_LOGIC_PIO0_11
 *                    - START_LOGIC_PIO1_0
 * @param[in]  edge Falling or rising edge on the corresponding PIO 
 *                  input, it can be:
 *                    - START_LOGIC_EDGE_Falling : Falling edge
 *                    _ START_LOGIC_EDGE_Rising  : Rising edge
 *             
 * @return  None
 **********************************************************************/
void SYSCON_StartLogicEdge(uint32_t pins, START_LOGIC_INPUT_EDGE edge)
{
#if 0
    if(edge == START_LOGIC_EDGE_Falling) {
        LPC_SYSCON->STARTAPRP0 &= (~pins) & PARAM_START_LOGIC_MASK;
    } else if(edge == START_LOGIC_EDGE_Rising){
        LPC_SYSCON->STARTAPRP0 |= pins & PARAM_START_LOGIC_MASK;
    }
#endif
}


/*********************************************************************//**
 * @brief      Enable or disable start signal for start logic input
 * @param[in]  pins the selected start logic inputs, it can be:
 *                    - START_LOGIC_PIO0_0
 *                    - START_LOGIC_PIO0_1
 *                    - START_LOGIC_PIO0_2
 *                    - START_LOGIC_PIO0_3
 *                    - START_LOGIC_PIO0_4
 *                    - START_LOGIC_PIO0_5
 *                    - START_LOGIC_PIO0_6
 *                    - START_LOGIC_PIO0_7
 *                    - START_LOGIC_PIO0_8
 *                    - START_LOGIC_PIO0_9
 *                    - START_LOGIC_PIO0_10
 *                    - START_LOGIC_PIO0_11
 *                    - START_LOGIC_PIO1_0
 * @param[in]  cmd  Enable or disable start signal for start logic input
 *                  it can be:
 *                    - ENABLE  : Enable start signal for corresponding 
 *                                start logic input
 *                    _ DISABLE : Disable start signal for corresponding 
 *                                start logic input
 *             
 * @return  None
 **********************************************************************/
void SYSCON_StartLogicCmd(uint32_t pins, FunctionalState cmd)
{
    if(cmd == DISABLE) {
        LPC_SYSCON->STARTERP0 &= (~pins) & PARAM_START_LOGIC_MASK; 
    } else if(cmd == ENABLE) {
        LPC_SYSCON->STARTERP0 |= pins & PARAM_START_LOGIC_MASK;     
    }   
}
#if 0
/*********************************************************************//**
 * @brief      Start signal reset for start logic input
 * @param[in]  pins the selected start logic inputs, it can be:
 *                    - START_LOGIC_PIO0_0
 *                    - START_LOGIC_PIO0_1
 *                    - START_LOGIC_PIO0_2
 *                    - START_LOGIC_PIO0_3
 *                    - START_LOGIC_PIO0_4
 *                    - START_LOGIC_PIO0_5
 *                    - START_LOGIC_PIO0_6
 *                    - START_LOGIC_PIO0_7
 *                    - START_LOGIC_PIO0_8
 *                    - START_LOGIC_PIO0_9
 *                    - START_LOGIC_PIO0_10
 *                    - START_LOGIC_PIO0_11
 *                    - START_LOGIC_PIO1_0
 *             
 * @return  None
 **********************************************************************/
void SYSCON_StartLogicReset(uint32_t pins)
{
    //LPC_SYSCON->STARTRSRP0CLR = pins &  PARAM_START_LOGIC_MASK;
}


/*********************************************************************//**
 * @brief      Get start signal status for start logic input 
 * @param[in]  None
 *             
 * @return  the status of START_LOGIC_PIO0_0 - START_LOGIC_PIO1_0, it 
 *          reflects the status of the enabled start signal bits, The 
 *           bit assignment is
 *                    - Bit  0 : START_LOGIC_PIO0_0
 *                    - Bit  1 : START_LOGIC_PIO0_1
 *                    - Bit  2 : START_LOGIC_PIO0_2
 *                    - Bit  3 : START_LOGIC_PIO0_3
 *                    - Bit  4 : START_LOGIC_PIO0_4
 *                    - Bit  5 : START_LOGIC_PIO0_5
 *                    - Bit  6 : START_LOGIC_PIO0_6
 *                    - Bit  7 : START_LOGIC_PIO0_7
 *                    - Bit  8 : START_LOGIC_PIO0_8
 *                    - Bit  9 : START_LOGIC_PIO0_9
 *                    - Bit 10 : START_LOGIC_PIO0_10
 *                    - Bit 11 : START_LOGIC_PIO0_11
 *                    - Bit 12 : START_LOGIC_PIO1_0
 **********************************************************************/
uint32_t SYSCON_StartLogicStatus(void)
{
   // return (LPC_SYSCON->STARTSRP0 & PARAM_START_LOGIC_MASK);
}

/*********************************************************************//**
 * @brief      Get WDT Clock
 * @param[in]  None
 *             
 * @return     WDT Clock
 **********************************************************************/
uint32_t SYSCON_GetWDTClock(void)
{
    uint32_t sel;
    uint32_t freq;   

  //  sel = LPC_SYSCON->WDTCLKSEL & 0x3;
    switch (sel) {
        case 0:     // IRC oscillator
            freq = IRC_OSC_FREQ; break;     
        case 1:     // Main clock
            freq = SystemFrequency; break;            
        case 2:     // Watchdog oscillator
            freq = SYSCON_GetWDTOsc(); break;   
        case 3:     // Reserved
            freq = 0;
    } 
   
    //sel = SYSCON_GetWDTClockDiv();
    
    if(sel == 0) {
        freq = 0;
    } else {
        freq /= sel;
    }
    
    return freq;     
}
#endif

/*********************************************************************//**
 * @brief      Get SSP0 or SSP1 Clock
 * @param[in]  SSP number, it can be
 *               - 0 : SSP0
 *               - 1 : SSP1
 *             
 * @return     SSP Clock
 **********************************************************************/
uint32_t SYSCON_GetSSP0Clock(uint8_t sspNum)
{
    uint32_t div = 0;
	
    if(sspNum == 0) {
        div = SYSCON_GetSPI0ClockDiv();
    } else {
        div = SYSCON_GetSPI1ClockDiv();
    }

    if (div == 0) {
        return 0;
    } else {
        return (SystemFrequency/div);
    }
}

/*********************************************************************//**
 * @brief      Get UART Clock
 * @param[in]  None
 *             
 * @return     UART Clock
 **********************************************************************/
uint32_t SYSCON_GetUARTClock(void)
{
    uint32_t div = 0;

    div = SYSCON_GetUARTClockDiv();
   
    if (div == 0) {
        return 0;
    } else {
        return (SystemFrequency/div);
    }
}

/**
 * @}
 */

#endif

/**
 * @}
 */
/* --------------------------------- End Of File ------------------------------ */
