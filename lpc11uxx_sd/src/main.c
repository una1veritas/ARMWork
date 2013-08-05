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
//#include "i2c.h"
//#include "i2clcd.h"
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


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
static uint8_t buff[32];
void sd_test(void);
void die(FRESULT rc);
DWORD get_fattime(void);

int main(void) {
	long cn;

	SystemInit();
	GPIOInit();
  start_delay();

  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);

	  // PIO1_6 USR LED
	pinMode(USERLED, OUTPUT);
	digitalWrite(USERLED, HIGH);
  
	USART_init(&usart, PIO1_26, PIO1_27);
//  USART_init(&usart, PIO0_18, PIO0_19);
	USART_begin(&usart, 115200);	
  USART_puts(&usart, "Hello, its nice to see you!");

/*
	  i2c_puts("LPCcappuccino!");
	  i2c_cmd(0xC0);	// move to 2nd line
	  i2c_puts("StrawberryLinux");
*/

//	 下記は不要な部分はコメントアウトしてお試しください。

/*
 *	SDカードのデモ（エンドレス）
 */
  SPI_init(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, SSP_CS0);

	sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  cn = 0;
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SD_BUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SD_BUSY, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

//	return 0 ;
}


void sd_test() {
	FRESULT rc;

//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT bw, br, i;
  uint16_t linenum;
  
	f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
	if (!rc){
		USART_puts(&usart, "\nType the file content.\n");
    for (;;) {
      rc = f_read(&Fil, buff, sizeof(buff), &br);	/* Read a chunk of file */
      if (rc || !br) break;			/* Error or end of file */

      for (i = 0; i < br; i++){
        USART_write(&usart, buff[0]);
      }
    }
    if (rc) die(rc);
    rc = f_close(&Fil);
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

    rc = f_open(&Fil, "SD0001.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) die(rc);

    // 無限ループでこの関数からは抜けない
      while(1){
        rc = f_write(&Fil, "Strawberry Linux\r\n", 18, &bw);
        if (rc) die(rc);

        // SDカードに書き出します。
        f_sync(&Fil);
      }
//	return;
}
}

DWORD get_fattime()
{
  return 0;
}

void die(FRESULT rc) {
  switch(rc) {
    case 0x03:
      USART_puts(&usart, "FR_NOT_READY.\n");
      break;
    default:
      sprintf((uint8_t*)buff, "error %d.\n", rc);
  }
  USART_puts(&usart, "\ndied.\n");
}
