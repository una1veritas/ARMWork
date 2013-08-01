
#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "systick.h"

volatile static uint32_t TimeTick;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
  TimeTick++;
}

void delaySysTick(uint32_t tick)
{
  uint32_t timetick;

  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

  timetick = TimeTick;
  while ((TimeTick - timetick) < tick);

  // Disable SysTick Counter
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  return;
}

void wait_ms(unsigned long time)
{
  delaySysTick(time);
}
  
  
  
void wait_sec(unsigned long sec)
{
  while(sec--){
    wait_ms(1000);
  }
}
  
