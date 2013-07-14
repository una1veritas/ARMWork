/**********************************************************************
* $Id$		Uart_Polling.c			2011-06-02
*//**
* @file		Uart_Polling.c
* @brief	This example describes how to using UART in polling mode
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "lpc18xx_uart.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_libcfg.h"
#include "lpc18xx_scu.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup Uart_Polling	Uart_Polling
 * @ingroup UART_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define USED_UART_DEBUG_PORT 0

#if (USED_UART_DEBUG_PORT==0)
#define DEBUG_UART_PORT	LPC_USART0
#elif (USED_UART_DEBUG_PORT==1)
#define DEBUG_UART_PORT	LPC_UART1
#endif



/************************** PRIVATE VARIABLES *************************/
uint8_t menu2[] = 
"\n\r NGX Technologies \n\r"
"\t -	MCU LPC18xx - ARM Cortex-M3 \n\r"
#if (USED_UART_DEBUG_PORT ==0)
"\t -	UART0 - 9600 bps \n\r"
#else
"\t - 	UART1 - 9600 bps \n\r"
#endif
"\t -	Press ESC to exit \n\r"
"\t -	Press 'r' to print welcome screen menu again \n\r"
"\t - 	UART Polling mode demo using ring buffer \n\r";
uint8_t menu3[] = "\n\rUART demo terminated!\n\r";

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	UART_Send((LPC_USARTn_Type*)DEBUG_UART_PORT, menu2, sizeof(menu2), BLOCKING);
}



/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		Main UART program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	uint32_t idx, len;
	__IO FlagStatus exitflag;
	uint8_t buffer[10];

	SystemInit();
	CGU_Init();

	/*
	 * Initialize UART1 pin connect
	 */
#if (USED_UART_DEBUG_PORT==0)

	/*
	 * Initialize UART0 pin connect	NGX board
	 */
	scu_pinmux(0x6 ,4 , MD_PDN|MD_EZI, FUNC2); 	// UART0_TXD
	scu_pinmux(0x6 ,5 , MD_PDN|MD_EZI, FUNC2); 	// UART0_RXD

#elif (USED_UART_DEBUG_PORT==1)
	/*
	 * Initialize UART1 pin connect
	 */
	scu_pinmux(0x1 ,13 , MD_PDN, FUNC1); 	// PC.13 : UART1_TXD
	scu_pinmux(0x1 ,14 , MD_PLN|MD_EZI|MD_ZI, FUNC1); 	// PC.14 : UART1_RXD
#endif

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	// Initialize UART1 peripheral with given to corresponding parameter
	UART_Init((LPC_USARTn_Type*)DEBUG_UART_PORT, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART1 peripheral
	UART_FIFOConfig((LPC_USARTn_Type*)DEBUG_UART_PORT, &UARTFIFOConfigStruct);


	// Enable UART Transmit
	UART_TxCmd((LPC_USARTn_Type*)DEBUG_UART_PORT, ENABLE);

	// print welcome screen
	print_menu();

	// Reset exit flag
	exitflag = RESET;

    /* Read some data from the buffer */
    while (exitflag == RESET)
    {
       len = 0;
        while (len == 0)
        {
            len = UART_Receive((LPC_USARTn_Type*)DEBUG_UART_PORT, buffer, sizeof(buffer), NONE_BLOCKING);
        }

        /* Got some data */
        idx = 0;
        while (idx < len)
        {
            if (buffer[idx] == 27)
            {
                /* ESC key, set exit flag */
            	UART_Send((LPC_USARTn_Type*)DEBUG_UART_PORT, menu3, sizeof(menu3), BLOCKING);
                exitflag = SET;
            }
            else if (buffer[idx] == 'r')
            {
                print_menu();
            }
            else
            {
                /* Echo it back */
            	UART_Send((LPC_USARTn_Type*)DEBUG_UART_PORT, &buffer[idx], 1, BLOCKING);
            }
            idx++;
        }
    }

    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy((LPC_USARTn_Type*)DEBUG_UART_PORT) == SET);

    // DeInitialize UART1 peripheral
    UART_DeInit((LPC_USARTn_Type*)DEBUG_UART_PORT);

    /* Loop forever */
    while(1);
}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/**
 * @}
 */
