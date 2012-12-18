#include "stm32f4xx.h"
#include "core_cm4.h"

#define loop	while(1)

void hndl_TRAP(void){
	loop;
}

extern unsigned char _endof_stack[];
void start(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_NMI(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_HardFault(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_MemManage(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_BusFault(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_UsageFault(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SVCall(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_Debug_Monitor(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_PendSV(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SysTick(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_WWDG(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_PVD(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TAMP_STAMP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_RTC_WKUP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_FLASH(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_RCC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI0(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI4(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream0(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream4(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream5(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream6(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_ADC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN1_TX(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN1_RX0(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN1_RX1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN1_SCE(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI9_5(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM1_BRK_TIM9(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM1_UP_TIM10(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM1_TRG_COM_TIM11(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM1_CC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM4(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C1_EV(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C1_ER(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C2_EV(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C2_ER(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SPI1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SPI2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_EXTI15_10(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_RTC_Alarm(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_FS_WKUP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM8_BRK_TIM12(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM8_UP_TIM13(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM8_TRG_COM_TIM14(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM8_CC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA1_Stream7(void) __attribute ((weak, alias ("hndl_TRAP")));

void hndl_FSMC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SDIO(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM5(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_SPI3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART4(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART5(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM6_DAC(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_TIM7(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream0(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream2(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream3(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream4(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_ETH(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_ETH_WKUP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN2_TX(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN2_RX0(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN2_RX1(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CAN2_SCE(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_FS(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream5(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream6(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DMA2_Stream7(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_USART6(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C3_EV(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_I2C3_ER(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_HS_EP1_OUT(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_HS_EP1_IN(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_HS_WKUP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_OTG_HS(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_DCMI(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_CRYP(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_HASH_RNG(void) __attribute ((weak, alias ("hndl_TRAP")));
void hndl_FPU(void) __attribute ((weak, alias ("hndl_TRAP")));


// see RM0090 Reference Manual
//	9.1.3 Inerrupt and exception vectors(Table30. Vector table)
void* const vector[] __attribute__ ((section(".VECTOR"))) = {	/* Vector table to be allocated to address 0x00000000 */
	_endof_stack,	// initial SP(MSP) (address 0x00)
	start,      	//	Reset
	hndl_NMI,   	//	NMI
	hndl_HardFault,	//	HardFault

	hndl_MemManage,	//	MemManage (0x10)
	hndl_BusFault,	//	BusFault
	hndl_UsageFault,	//	UsageFault

	hndl_TRAP, hndl_TRAP, hndl_TRAP, hndl_TRAP,	//	Reserved(address 0x1c,0x20,0x24,0x28)

	hndl_SVCall,	//	SVCall
	hndl_Debug_Monitor,	//	Debug_Monitor (0x30)

	hndl_TRAP,  	// Reserved (0x34)

	hndl_PendSV,	//	PendSV
	hndl_SysTick,	//	SysTick

	hndl_WWDG,  	//	WWDG (0x40)
	hndl_PVD,   	//	PVD
	hndl_TAMP_STAMP,	//	TAMP_STAMP
	hndl_RTC_WKUP,	//	RTC_WKUP
	hndl_FLASH, 	//	FLASH (0x50)
	hndl_RCC,   	//	RCC
	hndl_EXTI0, 	//	EXTI0
	hndl_EXTI1, 	//	EXTI1

	hndl_EXTI2, 	//	EXTI2 (0x60)
	hndl_EXTI3, 	//	EXTI3
	hndl_EXTI4, 	//	EXTI4
	hndl_DMA1_Stream0,	//	DMA1_Stream0
	hndl_DMA1_Stream1,	//	DMA1_Stream1 (0x70)
	hndl_DMA1_Stream2,	//	DMA1_Stream2
	hndl_DMA1_Stream3,	//	DMA1_Stream3
	hndl_DMA1_Stream4,	//	DMA1_Stream4

	hndl_DMA1_Stream5,	//	DMA1_Stream5 (0x80)
	hndl_DMA1_Stream6,	//	DMA1_Stream6
	hndl_ADC,	//	ADC
	hndl_CAN1_TX,	//	CAN1_TX
	hndl_CAN1_RX0,	//	CAN1_RX0 (0x90)
	hndl_CAN1_RX1,	//	CAN1_RX1
	hndl_CAN1_SCE,	//	CAN1_SCE
	hndl_EXTI9_5,	//	EXTI9_5

	hndl_TIM1_BRK_TIM9,	//	TIM1_BRK_TIM9 (0xa0)
	hndl_TIM1_UP_TIM10,	//	TIM1_UP_TIM10
	hndl_TIM1_TRG_COM_TIM11,	//	TIM1_TRG_COM_TIM11
	hndl_TIM1_CC,	//	TIM1_CC
	hndl_TIM2,  	//	TIM2 (0xb0)
	hndl_TIM3,  	//	TIM3
	hndl_TIM4,  	//	TIM4
	hndl_I2C1_EV,	//	I2C1_EV

	hndl_I2C1_ER,	//	I2C1_ER (0xc0)
	hndl_I2C2_EV,	//	I2C2_EV
	hndl_I2C2_ER,	//	I2C2_ER
	hndl_SPI1,  	//	SPI1
	hndl_SPI2,  	//	SPI2 (0xd0)
	hndl_USART1,	//	USART1
	hndl_USART2,	//	USART2
	hndl_USART3,	//	USART3

	hndl_EXTI15_10,	//	EXTI15_10 (0xe0)
	hndl_RTC_Alarm,	//	RTC_Alarm
	hndl_OTG_FS_WKUP,	//	OTG_FS_WKUP
	hndl_TIM8_BRK_TIM12,	//	TIM8_BRK_TIM12
	hndl_TIM8_UP_TIM13,	//	TIM8_UP_TIM13 (0xf0)
	hndl_TIM8_TRG_COM_TIM14,	//	TIM8_TRG_COM_TIM14
	hndl_TIM8_CC,	//	TIM8_CC
	hndl_DMA1_Stream7,	//	DMA1_Stream7

	hndl_FSMC,  	//	FSMC (0x100)
	hndl_SDIO,  	//	SDIO
	hndl_TIM5,  	//	TIM5
	hndl_SPI3,  	//	SPI3
	hndl_USART4,	//	USART4 (0x110)
	hndl_USART5,	//	USART5
	hndl_TIM6_DAC,	//	TIM6_DAC
	hndl_TIM7,  	//	TIM7

	hndl_DMA2_Stream0,	//	DMA2_Stream0 (0x120)
	hndl_DMA2_Stream1,	//	DMA2_Stream1
	hndl_DMA2_Stream2,	//	DMA2_Stream2
	hndl_DMA2_Stream3,	//	DMA2_Stream3
	hndl_DMA2_Stream4,	//	DMA2_Stream4 (0x130)
	hndl_ETH,   	//	ETH
	hndl_ETH_WKUP,	//	ETH_WKUP
	hndl_CAN2_TX,	//	CAN2_TX

	hndl_CAN2_RX0,	//	CAN2_RX0 (0x140)
	hndl_CAN2_RX1,	//	CAN2_RX1
	hndl_CAN2_SCE,	//	CAN2_SCE
	hndl_OTG_FS,	//	OTG_FS
	hndl_DMA2_Stream5,	//	DMA2_Stream5 (0x150)
	hndl_DMA2_Stream6,	//	DMA2_Stream6
	hndl_DMA2_Stream7,	//	DMA2_Stream7
	hndl_USART6,	//	USART6

	hndl_I2C3_EV,	//	I2C3_EV (0x160)
	hndl_I2C3_ER,	//	I2C3_ER
	hndl_OTG_HS_EP1_OUT,	//	OTG_HS_EP1_OUT
	hndl_OTG_HS_EP1_IN,	//	OTG_HS_EP1_IN
	hndl_OTG_HS_WKUP,	//	OTG_HS_WKUP (0x170)
	hndl_OTG_HS,	//	OTG_HS
	hndl_DCMI,  	//	DCMI
	hndl_CRYP,  	//	CRYP

	hndl_HASH_RNG,	//	HASH_RNG (0x180)
	hndl_FPU,   	//	FPU
};

