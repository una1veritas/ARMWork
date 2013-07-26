#include "LPC17xx.H"                        /* LPC17xx definitions */
#include "lcd.h"
#include "type.h"
#include "uart.h"
#include "adc.h"
#include "hiduser.h"
#include "dac.h"
#include "extint.h"
#include "i2c.h"
#include "ps2kbd.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "demo.h"

#include <stdio.h>

#define TEST_LED (1 << 2)

extern volatile uint32_t UART3Count;
extern volatile uint8_t UART3Buffer[BUFSIZE];
extern volatile uint32_t UART1Count;
extern volatile uint8_t UART1Buffer[BUFSIZE];



#if POWERDOWN_MODE_USB_WAKEUP
#include "timer.h"
extern volatile uint32_t SuspendFlag;
extern volatile uint32_t timer0_counter;
extern volatile uint32_t USBActivityInterruptFlag;
volatile uint32_t WakeupFlag = 0;
#endif





/**
********************************************************************************************
	Function Name :	DAC_test()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void DAC_test(void)
{
	uint32_t i=0,j=0;
	uint32_t k=0;

  for(k=0;k<10000;k++)
  {
	LPC_DAC->DACR = (i << 6) | DAC_BIAS;
	i++;
	for(j = 1000; j > 1; j--);
	if ( i == 1024 )
	{
	  i = 0;
	}
  }
}

/**
********************************************************************************************
	Function Name :	test_i2c_at24c256_flash()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void test_i2c_at24c256_flash(void)
{
  char  i=0;
  
  char status=1;
  unsigned char buf[128];

  for(i=0;i<sizeof(buf);i++)
  
    buf[i] = i;
  
  m24xx_write(EEPROM_24XX256, 0 , 0 , (char *)buf , sizeof(buf));

  for(i=0;i<sizeof(buf);i++)
    buf[i] = 0;

  m24xx_read(EEPROM_24XX256, 0 , 0 , (char *)buf , sizeof(buf));

  for(i=0;i<sizeof(buf);i++)
  {
    if(buf[i] != i)
    {
      status = 0;
      break;
    }    
  }

  if(status == 1)
  {
    LPC_GPIO0->FIOSET = TEST_LED;
	lcd_putstring(1,"I2C EEPROM PASS");
	
  }
  else
  {
    LPC_GPIO0->FIOCLR = TEST_LED;
	lcd_putstring(1,"I2C EEPROM FAIL");
  }
}
/**
********************************************************************************************
	Function Name :	test_led()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void test_led (void)
{
	int i=0,j=0,k=0;

	LPC_GPIO0->FIODIR |= TEST_LED;
	for(i=0; i < 5 ;i++ )
		{
			LPC_GPIO0->FIOSET = TEST_LED;
			for(j=0;j<1000000;j++);
			LPC_GPIO0->FIOCLR  = TEST_LED;
			for(k=0;k<1000000;k++);
		}
}


/**
********************************************************************************************
	Function Name :	is_valid_ascii()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
U8 is_valid_ascii(S8 key)
{
  U8 retval = FALSE;
  
  if(key >= ' ' && key <= '~')
    retval = TRUE;

  return retval;
}

/**
********************************************************************************************
	Function Name :	process_lcd()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void process_kbd(void)
{
  S8 key;
  static U8 position = 0;

	if(keyhit())
	{	
  	key = xgetkey();
  	if(is_valid_ascii(key))
  	{
    lcd_gotoxy(LINE2, position);
    lcd_putchar(key);
    if(position == MAX_CHAR_IN_ONE_LINE)
    {
      lcd_clear();
      position = 0;
    }		
    position++;
  }
}
}

/**
********************************************************************************************
	Function Name :	process_adc()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/

void process_adc(void)
{
	unsigned char buf[16] = {0};
	unsigned int adc_value = 0;
	adc_value = ADC0Read( 5 );
  	sprintf((char *)buf, "ADC:%d ", adc_value);
  	lcd_putstring(LINE1, (char *)buf);
	
}
/**
********************************************************************************************
	Function Name :	hardware_init()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
void hardware_init(void)
{
	init_lcd();
	lcd_backlight_on();
	UARTInit(3, 38400);	/* baud rate setting */
  	UARTInit(1, 38400);	/* baud rate setting */
	i2c_lpc_init(I2C_SPEED_100);
	DACInit();
	EINTInit();
	init_keyboard();
	ADCInit( ADC_CLK );
	USB_Init();                           /* USB Initialization */	
}

/**
********************************************************************************************
	Function Name :	main()

	Description :
	
	Input :	Void

	Output : Void

	Note :
**********************************************************************************************
*/
int main (void) 
{
	SystemInit();                         /* initialize clocks */
	hardware_init();
  	lcd_clear();
	test_led();
	test_i2c_at24c256_flash();	
	DAC_test();


#if POWERDOWN_MODE_USB_WAKEUP
	uint32_t i, j;
#endif


#if POWERDOWN_MODE_USB_WAKEUP
	init_timer(0, TIME_INTERVAL);		/* the timer is for USB suspend and resume */
#endif

  
	USB_Connect(TRUE);                    /* USB Connect */

	
	while (1) 
	{				/* Loop forever */
		if ( UART3Count != 0 )
		{
	  		LPC_UART3->IER = IER_THRE | IER_RLS;			/* Disable RBR */
	  		UARTSend( 3, (uint8_t *)UART3Buffer, UART3Count );
	  		UART3Count = 0;
	  		LPC_UART3->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
		}
		if ( UART1Count != 0 )
		{
	  		LPC_UART1->IER = IER_THRE | IER_RLS;			/* Disable RBR */
	  		UARTSend( 1, (uint8_t *)UART1Buffer, UART1Count );
	  		UART1Count = 0;
	  		LPC_UART1->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
		}
		process_adc();
		process_kbd();

#if POWERDOWN_MODE_USB_WAKEUP
		if ( USBActivityInterruptFlag )
		{
	  /* If the MCU is waked up from power down mode, the PLL needs to
	  be reconfigured, and USB block needs tn be reset and reconnect. */
	  		USBActivityInterruptFlag = 0;
	  		SystemInit();
	  		USB_Init();                           /* USB Initialization */
	  		USB_Connect(TRUE);                    /* USB Connect */
	
		}
	/* bit 8 on USN_INT_STAT is USB NeedClk bit. */
		if ( SuspendFlag == 1 )
		{
	  		if ( timer0_counter > 200 )
	  		{
		/* If it's a true suspend, turn off USB device bit the 
		USBNeed_clk will go away once USB device is disabled. */
				LPC_USB->USBClkCtrl = 0x00;
				while ( LPC_SC->USBIntSt & (1 << 8) );
		/* Regular USB interrupt is disabled to test USBActivity interrupt. 
		It will be enabled once it's waken up. */
					NVIC_DisableIRQ(USB_IRQn);
					WakeupFlag = 1;
		/* USB Activity interrupt won't occur until Deepsleep bit is set in SCR.
		UM needs to be updated regarding this. */
					SCB->SCR |= 0x04;	/* Set SLEEPDEEP bit in SCR in Cortex M3 core. */	
					LPC_SC->PCON = 0x1;
					__WFI();
	  		}
		}
#endif
	
	}
}



