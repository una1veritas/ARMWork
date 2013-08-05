
#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "systick.h"


volatile static uint32_t TimeTick = 0;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
  TimeTick++;
}


void Systick_Start()
{
	  TimeTick=0;

	/* Clear SysTick Counter */
	  SysTick->VAL = 0;
	  /* Enable the SysTick Counter */
	  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

}

uint32_t Systick_Gettime()
{
	return TimeTick;

}

void Systick_Stop()
{
	  /* Disable SysTick Counter */
	  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	  /* Clear SysTick Counter */
	  SysTick->VAL = 0;

}



void delaySysTick(uint32_t tick)
{
  uint32_t timetick;

  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  /* Enable the SysTick Counter */
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

  timetick = TimeTick;
  while ((TimeTick - timetick) < tick);

  /* Disable SysTick Counter */
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  /* Clear SysTick Counter */
  SysTick->VAL = 0;
  return;
}


void wait_ms(uint32_t time)
{
  delaySysTick(time);
}
  
  
  
void wait_sec(uint32_t sec)
{
  while(sec--){
    wait_ms(1000);
  }
}
  










