/***********************************************************************//**
 * @file	: lpc11xx_libcfg_default.h
 * @brief	: Default Library configuration header file
 * @version	: 1.0
 * @date	: 20. Jan. 2010
 * @author	: Coocox
 **************************************************************************/

/* Library Configuration group ----------------------------------------------------------- */
/** @defgroup LIBCFG_DEFAULT
 * @ingroup LPC1100CMSIS_FwLib_Drivers
 * @{
 */

#ifndef __LPC11XX_LIBCFG_DEFAULT_H_
#define __LPC11XX_LIBCFG_DEFAULT_H_

/* Includes ------------------------------------------------------------------- */
#include "lpc_types.h"


/* Public Macros -------------------------------------------------------------- */
/** @defgroup LIBCFG_DEFAULT_Public_Macros
 * @{
 */

/************************** DEBUG MODE DEFINITIONS *********************************/
/* Un-comment the line below to compile the library in DEBUG mode, this will expanse
   the "CHECK_PARAM" macro in the FW library code */

//#define DEBUG    1


/******************* PERIPHERAL FW LIBRARY CONFIGURATION DEFINITIONS ***********************/

/* Comment the line below to disable the specific peripheral inclusion */

/* SYSCON ------------------------------- */
#define _SYSCON	   1

/* PMU ---------------------------------- */
#define _PMU       1

/* GPIO --------------------------------- */
#define _GPIO      1

/* IOCON--------------------------------- */
#define _IOCON     1

/* ADC ---------------------------------- */
#define _ADC   	   1

/* I2C ---------------------------------- */
#define _UART      1

/* I2C ---------------------------------- */
#define _I2C       1

/* I2C ---------------------------------- */
#define _SSP       1

/* TMR16B or TMR32B---------------------- */
#define _TMR       1

/* I2C ---------------------------------- */
#define _WDT       1

/* SYSTICK------------------------------- */
#define _SYSTICK   1


/************************** GLOBAL/PUBLIC MACRO DEFINITIONS *********************************/

#ifdef  DEBUG      
/*******************************************************************************
* @brief		The CHECK_PARAM macro is used for function's parameters check.
* 				It is used only if the library is compiled in DEBUG mode.
* @param[in]	expr - If expr is false, it calls check_failed() function
*                    	which reports the name of the source file and the source
*                    	line number of the call that failed.
*                    - If expr is true, it returns no value.
* @return		None
*******************************************************************************/
#define CHECK_PARAM(expr) ((expr) ? (void)0 : check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define CHECK_PARAM(expr)
#endif /* DEBUG */

/**
 * @}
 */


/* Public Functions ----------------------------------------------------------- */
/** @defgroup LIBCFG_DEFAULT_Public_Functions
 * @{
 */

#ifdef  DEBUG
void check_failed(uint8_t *file, uint32_t line);
#endif

/**
 * @}
 */

#endif /* __LPC11XX_LIBCFG_DEFAULT_H_ */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
