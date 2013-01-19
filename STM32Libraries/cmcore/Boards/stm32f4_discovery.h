/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_H
#define __STM32F4_DISCOVERY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "armcore.h"
	 
#define LED1 	PD12
#define LED2 	PD13
#define LED3 	PD14
#define LED4 	PD15

#define USER_BUTTON 	PA0
	 
#define STM_EVAL_LEDInit(x) 	pinMode((x), OUTPUT)
#define STM_EVAL_LEDOn(x) 		digitalWrite((x), HIGH)
#define STM_EVAL_LEDToggle(x) 	digitalToggle(x)
	 
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_H */
