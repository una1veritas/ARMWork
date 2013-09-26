// 
// STM32F4discoveryボードで基本的な設定を行いLEDをチカらせるサンプルプログラム
//  TIM4のPWMを使用してホンワリと明滅させる
// 参考資料：
//	RM0090 Reference manual - STM32F4xx advanced ARM-based 32-bit MCUs
//	PM0056 Programming manual - STM32Fxxx Cortex-M3 programming manual
//	UM1472 Users manual - STM32F4DISCOVERY discovery board
//	STM32F405xx 407xx datasheet - refer to see Alternate functions on GPIO Table.7
// 
#include "stm32f4xx.h"
#include "core_cm4.h"

#define loop	while(1)

//	TIM4を使ったPWMはSOFT_PWMを#undefすること。
// #define SOFT_PWM

//	STM32F4discoveryは，外部のクロックソースを使う場合に，そのままだと
//	ST-linkコントローラーのMCO出力がOSC_INに接続されている。
//	そのままの場合は，SysClockSource_HSE_EXTを定義する。
//	STM32F4側に実装されているXtalをクロックソースとして使用するならば
//	ハンダ面にあるR68(100ohm)抵抗を外して(できればSB15&16をオープンに)
//	Xtal-OSCを有効にしてからSysClockSource_HSE_Xtalを定義する。
//	SysClockSource_HSE_*が定義されてなければ，内蔵HSI(16MHz)を使う。
// #define SysClockSource_HSE_Xtal
// #define SysClockSource_HSE_EXT

#ifndef SysClock
#	if defined(SysClockSource_HSE_Xtal) || defined(SysClockSource_HSE_EXT)
#		define SysClock (8000000)	// assumed external clock source is 8MHz
#	else
#		define SysClock (16000000)	// HSI=16MHz
#	endif
#endif

//
// メモリユティリティサブルーチン
//
static void *memset(void *dst,int c,long cnt){
unsigned char *p;
	p= (unsigned char *)dst;
	for(;cnt;cnt--){
		*p= (unsigned char)c;
		p++;
	};
	return(dst);
}
static void *memmove(void *dst,void *src,long cnt){
unsigned char *p,*q;
	if(0==cnt || src==dst)return(dst);
	if(src>dst){
		p= (unsigned char *)dst;
		q= (unsigned char *)src;
		for(;cnt;cnt--){
			*p= *q;
			p++;q++;
		};
	}else{
		p= (unsigned char *)((long)dst+cnt);
		q= (unsigned char *)((long)src+cnt);
		for(;cnt;cnt--){
			--p;--q;
			*p= *q;
		};
	};
	return(dst);
}

//
// リンカスクリプトで定義されているデータセクションのラベル
//
extern unsigned char _sidata[];
extern unsigned char _sdata[];
extern unsigned char _edata[];
extern unsigned char _sbss[];
extern unsigned char _ebss[];
extern unsigned char _endof_sram[];

//
// Reset and Clock Controller initialize
// LEDチカチカさせるためにGPIODにクロックを供給する指定をしている
//
void init_RCC(void){
#define default_RCC_CR (RCC_CR_HSION | RCC_CR_HSITRIM_4)	// 0x0000xx83
#define default_RCC_CFGR (0)	// 0x00000000
#define default_RCC_CIR (0) 	// 0x00000000
#define default_RCC_AHB1ENR (RCC_AHB1ENR_CCMDATARAMEN)  	// 0x00100000
#define default_RCC_AHB2ENR (0)	// 0x00000000
#define default_RCC_AHB3ENR (0)	// 0x00000000
#define default_RCC_APB1ENR (0)	// 0x00000000
#define default_RCC_APB2ENR (0)	// 0x00000000

#ifdef SysClockSource_HSE_Xtal
	RCC->CR |= RCC_CR_HSEON | default_RCC_CR;	// crystal osc. and ceramic res.
#else
#ifdef SysClockSource_HSE_EXT
	RCC->CR |= (RCC_CR_HSEBYP | RCC_CR_HSEON) | default_RCC_CR;	// EXT_CLOCK in
#else
	RCC->CR = default_RCC_CR;	// only HSI
#endif
#endif

	RCC->CFGR= default_RCC_CFGR;	// reset CFGR
	RCC->CIR= default_RCC_CIR;  	// disable all interrupts

#if defined(SysClockSource_HSE_Xtal) || defined(SysClockSource_HSE_EXT)
	while(0==(RCC->CR & RCC_CR_HSERDY));	// waiting for HSE ready to stable
	while(RCC_CFGR_SWS_HSE!=(RCC->CFGR & RCC_CFGR_SWS))RCC->CFGR= RCC_CFGR_SW_HSE;
#endif

// supply clock for inter-connect peripheral buses
	RCC->AHB1ENR = default_RCC_AHB1ENR;
	RCC->AHB2ENR = default_RCC_AHB2ENR;
	RCC->AHB3ENR = default_RCC_AHB3ENR;
	RCC->APB1ENR = default_RCC_APB1ENR;
	RCC->APB2ENR = default_RCC_APB2ENR;

//
//	GPIODにクロックを注入する
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
//	TIM4にクロックを注入する
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}

void init(void){
// reset & clock configure
	init_RCC();

// set vector table
#ifdef VECTOR_ON_SRAM
	SCB->VTOR = SRAM_BASE;	// vector table from top of SRAM
#else
	SCB->VTOR = FLASH_BASE;	// vector table from top of FLASH
#endif

// .data and .bss,.COMM area initializing
	memmove(_sdata, _sidata, _edata - _sdata);	// set initial values onto .data 
	memset(_sbss, 0, _ebss - _sbss);	// clear .bss, .COMM


//
// GPIOD configure
	GPIOD->PUPDR = 0x00aaa2aa; // GPIOD_11..0:input_PullDown(except PD5)
#ifdef SOFT_PWM
// GPIOD_15=BL,14=RE,13=OR,12=GR is output mode
// default I/O_LEVEL=L
	GPIOD->MODER = (GPIO_MODER_MODER15_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER12_0);
	GPIOD->ODR = 0;
#else
// GPIOD_15..12 set alternate function mode (TIM4 = AF2)
	GPIOD->MODER = (GPIO_MODER_MODER15_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER12_1);
	GPIOD->AFR[1]= 0x22220000; // DPIOD_15..12 as TIM4_CH4..CH1
#endif /* SOFT_PWM */

//
// TIM4 configure
// TIM4_CH4=BL,3=RE,2=OR,1=GR is output PWM-edgeAlign mode
//
#define	default_TIMx_CR1	(0)
#define	default_TIMx_CR2	(0)
#define	CCER_SET(x) 1<<(4*(x))
TIM4->CR1= default_TIMx_CR1; // reset value
TIM4->CR2= default_TIMx_CR2; // reset value

TIM4->ARR= 255; // auto reload 255 as like a 8bit-timer :)
TIM4->CCR1= TIM4->CCR2= TIM4->CCR3= TIM4->CCR4= 0; // clear compair value(PWM OFF)
TIM4->CCER=  TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E; // enable output all channels
TIM4->CCMR1= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
TIM4->CCMR2= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
TIM4->CNT= 0;
TIM4->SR=  0;
TIM4->CR1= TIM_CR1_CEN; // f/1,PWM-edgeAligned,upcount,countEnable
TIM4->BDTR = TIM_BDTR_MOE;
//
// set up sysTick facility
// 10ms ごとにsysTickイベントを発生させる
//
#define user_SysTick_CTRL (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk)
	SysTick->LOAD= (10 * (SysClock / 1000)) - 1; // SysTick event every a ten milli-seconds
	SysTick->CTRL= user_SysTick_CTRL;
}


//
// mainルーチン
// tickカウンタの値を元にボード上のLEDを明滅させる
//
static volatile long CNT_tick;
#define LED_base	(12)	// PD12-15
#define LED_GR	(1 << (LED_base + 0))
#define LED_OR	(1 << (LED_base + 1))
#define LED_RE	(1 << (LED_base + 2))
#define LED_BL	(1 << (LED_base + 3))

#define BLIGHT(x) (((x)&0x80)?(0x80-((x)&0x7f)):((x)&0x7f))

int main (void){
#ifdef SOFT_PWM
uint16_t	t4;
#endif /* SOFT_PWM */
	CNT_tick= 0;
	loop{
		switch( (CNT_tick >> 8) & 0x03 ){
		  case(0):
			TIM4->CCR1= BLIGHT(CNT_tick);
			TIM4->CCR2= TIM4->CCR3= TIM4->CCR4= 0;
			break;;
		  case(1):
			TIM4->CCR2= BLIGHT(CNT_tick);
			TIM4->CCR1= TIM4->CCR3= TIM4->CCR4= 0;
			break;;
		  case(2):
			TIM4->CCR3= BLIGHT(CNT_tick);
			TIM4->CCR1= TIM4->CCR2= TIM4->CCR4= 0;
			break;;
		  case(3):
			TIM4->CCR4= BLIGHT(CNT_tick);
			TIM4->CCR1= TIM4->CCR2= TIM4->CCR3= 0;
			break;;
		  default:
			CNT_tick= 0;
			break;;
		};
#ifdef SOFT_PWM
		GPIOD->ODR = 0;
		if(t4 < TIM4->CCR1)	GPIOD->ODR = LED_GR;
		if(t4 < TIM4->CCR2)	GPIOD->ODR = LED_OR;
		if(t4 < TIM4->CCR3)	GPIOD->ODR = LED_RE;
		if(t4 < TIM4->CCR4)	GPIOD->ODR = LED_BL;
		t4++;
		t4&=0x0ff;
#endif /* SOFT_PWM */
	};
}

//
// リセット後に実行されるルーチン(vector.cで指定している)
void start(void){
// configure cortex-M4 core facility
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL<<(10*2))|(3UL<<(11*2))); // set enable CP10&CP11 access
#endif /* FPU */
	loop{
		init();
		main();
	};
}
//
// sysTickイベントごとにtickカウンタをインクリメントする(vector.cで指定している)
void hndl_SysTick(void)	{	// 100Hz system tick count and go around
	SysTick->CTRL= user_SysTick_CTRL; // clear all status flag quickly
	CNT_tick++;
}

