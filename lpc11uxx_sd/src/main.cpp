/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#include "LPC11Uxx.h"
#include "type.h"
#include "armcmx.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "I2Cbus.h"
#include "ST7032i.h"
#include "spi.h"
#include "ff.h"

#include "PWM0Tone.h"

#include "cappuccino.h"

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

void sd_test(void);

ST7032i lcd(Wire, LED_LCDBKLT);

#define RTC_ADDR 0x68

int main(void) {
	int i;

	SystemInit();
	GPIOInit();
  start_delay();

	// GPIO, delay, PWM0Tone test.
	pinMode(LED_SDBUSY, OUTPUT);
  for (i = 0; i < 3; i++) {
    digitalWrite(LED_SDBUSY, HIGH);
    delay(100);
    digitalWrite(LED_SDBUSY, LOW);
    delay(100);
  }
  digitalWrite(LED_SDBUSY, HIGH);
  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);
  delay(500);
  digitalWrite(LED_SDBUSY, LOW);
  //
  
	USART_init(&usart, RXD_A, TXD_A);
//  USART_init(&usart, RXD_C, TXD_C);
	USART_begin(&usart, 115200);
  USART_puts(&usart, "\nHello, \nits nice to see you!\n");

  Wire.begin();
  lcd.begin();
  lcd.backlightHigh();
  lcd.print("Let's start!");

//	 下記は不要な部分はコメントアウトしてお試しください。

/*
 *	SDカードのデモ（エンドレス）
 */
  SPI_init(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, SSP_CS0);

	sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SDBUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SDBUSY, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

//	return 0 ;
}

DWORD get_fattime()
{
  return 0;
}

FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
static uint8_t buff[128];


/*
 * SDカードからMESSAGE.TXTのファイルを読み込んでI2C液晶に表示します。
 * (先頭32文字だけ)
 * その後、SD0001.TXTというファイルを作成して、LPCcappuccino!+CR+LFという文字を永遠に書き込みます
 */

/*
 * SDカードのサンプル
 */
void sd_test()
{
	FRESULT rc;
  long swatch;
  
//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT bw, br, i;

	f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
	if (!rc){
    USART_puts(&usart, "\nType the file content.\n");
    for (;;) {
      f_gets((TCHAR*)buff, sizeof(buff), &Fil);	/* Read a chunk of file */
      if (rc || !br) break;			/* Error or end of file */

      USART_write(&usart, (char*)buff);
    }
    if (rc) {
      USART_puts(&usart, "\nFailed while reading the file.\n");
      return;
    }
    rc = f_close(&Fil);
    /*
     *	ファイル書き込みテスト
     *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
     */
#ifdef TEST_F_WRITE
    rc = f_open(&Fil, "SD0001.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) {
      USART_puts(&usart, "\nCouldn't open SD0001.TXT.\n");
      return;
    }

    swatch = millis();
    // 無限ループでこの関数からは抜けない
    while(1){
      i = sprintf((char*)buff, "%08u ", millis());
      f_write(&Fil, buff, i, &bw);
      rc = f_write(&Fil, "Strawberry Linux\r\n", 18, &bw);
      if (rc) 
        break;
      // SDカードに書き出します。
      f_sync(&Fil);
      if ( swatch + 2000 < millis() ) 
        break;
    }
    f_close(&Fil);
    USART_puts(&usart, "\nSD File IO test finished.\n");
  	return;
#endif
  }
  if ( rc == FR_NOT_READY )
    USART_puts(&usart, "\nCouldn't open MESSAGE.TXT.\n");

}
