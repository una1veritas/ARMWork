
#include "LPC11uxx.h"			/* LPC11uxx Peripheral Registers */
#include "type.h"
#include "delay.h"

volatile uint32 __counter_micros;
volatile uint32 __counter_millis;

// private
// volatile uint32 SysTickPeriod;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
	__counter_micros += 1000;
  __counter_millis++;
}

void init_systick_delay() {  
  SystemCoreClockUpdate();
  // systick initialize
  SysTick_Config(SystemCoreClock/1000L);
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
 }

/*
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
*/

void start_systick_delay(void)
{
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void stop_systick_delay(void) {
  // Disable SysTick Counter
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  // Clear SysTick Counter 
  // SysTick->VAL = 0;
}


uint32_t micros(void) {
// 	return __counter_micros + (1000 - SysTick->VAL/48);
  return __counter_micros + (1000 - ((SysTick->VAL*5)>> 8));
  //__counter_micros + ( 3000 - (SysTick->VAL >> 4) ) / 3;
}

uint32_t millis(void) {
	return __counter_millis;
}

void delay_ms(uint32_t w) {
	uint32_t wtill = millis() + w;
	if ( wtill < millis() ) {
		while ( millis() >= wtill );
	}
	while (millis() < wtill) ;
}

void delay_us(uint32_t w) {
	uint32_t tim2 = SysTick->VAL;
	for ( ; w > 0 ; ) {
		if (tim2 == SysTick->VAL) 
			continue;
		tim2 = SysTick->VAL;
		w--;
	}
}

