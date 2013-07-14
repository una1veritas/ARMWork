#include "lpc17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "timer_utilities.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup TIMER_Polling_Match	Polling_Match
 * @ingroup TIMER_Examples
 * @{
 */
 

//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct;


int Timer_delay_ms(int count)
{
	 // Initialize timer 0, prescale count time of 100uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1000;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Disable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	// Set Match value, count value of 1000 (1000 * 100uS = 100mS --> 10Hz)
	TIM_MatchConfigStruct.MatchValue   = count;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	TIM_Cmd(LPC_TIM0,ENABLE);
	// Wait for 1000 millisecond
		while(!(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)));
		TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);

  return 0;
}

