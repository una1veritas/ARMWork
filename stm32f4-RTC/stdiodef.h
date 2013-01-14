#ifndef __STDIODEFC_H
#define __STDIODEF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

#include "armcore.h"
#include "usart.h"

extern USART usart6;
#define STDSERIAL 	usart6

#ifdef __cplusplus
}
#endif

#endif
