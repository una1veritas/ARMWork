#ifndef _MMC_SSP_H_
#define _MMC_SSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "integer.h"
  
DWORD get_fattime(void);

extern uint32_t fatfs_time;
extern uint32_t fatfs_date;

#ifdef __cplusplus
}
#endif

#endif /* _MMC_SSP_H_ */
