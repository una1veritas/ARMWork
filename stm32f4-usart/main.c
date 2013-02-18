#include <ctype.h>

#include "stm32f4xx.h"
#include <stm32f4xx_rtc.h>

#include "cmcore.h"
#include "delay.h"
#include "usart.h"

// rtc timer
static __IO uint32_t TimingDelay;
__IO uint32_t Secondfraction = 0;
__IO uint8_t Startevent = 0;
RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStruct; 

typedef struct {
  uint8_t tab[12];
} Table_TypeDef;

void RTC_Config(void);
void RTC_AlarmConfig(void);
Table_TypeDef RTC_Get_Time(uint32_t Secondfraction , RTC_TimeTypeDef* RTC_TimeStructure );

usart Serial6;
 
int main(void) {
	char printbuf[64];
//	uint32_t tmp32;

	RTC_Config();	

	cmcore_init();

	usart_init(&Serial6, USART6, PC7, PC6); // 7 6
	usart_begin(&Serial6, 57600); // 7 6
	usart_print(&Serial6, "\r\nWelcome to USART.\r\n\r\n");

	delay_ms(500);

	while (1) {

		    /* Check on the event 'start'*/
    if(Startevent != 0x0) {
       /* Get the RTC sub second fraction */
       Secondfraction = 1000 - ((uint32_t)((uint32_t)RTC_GetSubSecond() * 1000) / (uint32_t)0x3FF);
    } else {
      /* Idle */
      Secondfraction =0x0;
    }
    
    /* Get the Curent time */
    RTC_GetTime(RTC_Format_BCD, &RTC_TimeStruct);
    
    /* Display the curent time and the sub second on the LCD */
    RTC_Get_Time(Secondfraction , &RTC_TimeStruct);
		sprintf(printbuf, "second fraction = %d ", Secondfraction),
		usart_print(&Serial6, printbuf);
		
		usart_print(&Serial6, ".");
		sprintf(printbuf, "millis = %dl\r\n", millis()),
		usart_print(&Serial6, printbuf);

		delay_ms(250);
	}
}

/**
  * @brief  Configures the RTC Alarm.
  * @param  None
  * @retval None
  */
void RTC_AlarmConfig(void)
{
//  EXTI_InitTypeDef EXTI_InitStructure;
  RTC_AlarmTypeDef RTC_AlarmStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* EXTI configuration */
//  EXTI_ClearITPendingBit(EXTI_Line17);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Set the alarmA Masks */
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interripts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);

  /* Enable AlarmA interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

}


/**
  * @brief  Returns the current time and sub second.
  * @param  Secondfraction: the sub second fraction.
  * @param  RTC_TimeStructure : pointer to a RTC_TimeTypeDef structure that 
  *         contains the current time values. 
  * @retval table : return current time and sub second in a table form
  */
Table_TypeDef RTC_Get_Time(uint32_t Secondfraction , RTC_TimeTypeDef* RTC_TimeStructure )
{
  Table_TypeDef table2;

  /* Fill the table2 fields with the current Time*/
  table2.tab[0] = (((uint8_t)(RTC_TimeStructure->RTC_Hours & 0xF0) >> 0x04) + 0x30);
  table2.tab[1]  = (((uint8_t)(RTC_TimeStructure->RTC_Hours & 0x0F))+ 0x30);
  table2.tab[2]  = 0x3A;
  
  table2.tab[3]  = (((uint8_t)(RTC_TimeStructure->RTC_Minutes & 0xF0) >> 0x04) + 0x30);
  table2.tab[4]  =(((uint8_t)(RTC_TimeStructure->RTC_Minutes & 0x0F))+ (uint8_t)0x30);
  table2.tab[5]  = 0x3A;

  table2.tab[6]   = (((uint8_t)(RTC_TimeStructure->RTC_Seconds & 0xF0) >> 0x04)+ 0x30);
  table2.tab[7]   = (((uint8_t)(RTC_TimeStructure->RTC_Seconds & 0x0F)) + 0x30);
  table2.tab[8]   = 0x2E;
  
  table2.tab[9]   = (uint8_t)((Secondfraction / 100) + 0x30);
  table2.tab[10]  = (uint8_t)(((Secondfraction % 100 ) / 10) +0x30);
  table2.tab[11]  =  (uint8_t)((Secondfraction % 10) +0x30);
  
  /* return table2 */
  return table2;
}
