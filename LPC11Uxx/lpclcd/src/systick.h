
#ifdef __cplusplus
extern "C" {
#endif

volatile static uint32_t TimeTick;
//volatile static uint32_t TimeTick ;

void SysTick_Handler(void);

void delaySysTick(uint32_t tick);
void wait_ms(unsigned long time);
void wait_sec(unsigned long sec);

#ifdef __cplusplus
}
#endif
