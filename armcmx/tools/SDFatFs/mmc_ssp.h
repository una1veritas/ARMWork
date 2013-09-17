#ifndef _MMC_SSP_H_
#define _MMC_SSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "integer.h"
  
DWORD get_fattime(void);

extern uint32_t time;
extern uint32_t date;

#ifdef __cplusplus
}
#endif

#endif /* _MMC_SSP_H_ */
