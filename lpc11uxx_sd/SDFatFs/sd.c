#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
//#include "gpio.h"
//#include "xprintf.h"
//#include "systick.h"
//#include "uart.h"
//#include "i2c.h"
//#include "i2clcd.h"
#include "ff.h"
//#include "cappuccino.h"

#include "armcmx.h"
#include "usart.h"

DWORD get_fattime()
{
  return 0;
}

FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
static uint8_t buff[32];


/*
 * SDカードからMESSAGE.TXTのファイルを読み込んでI2C液晶に表示します。
 * (先頭32文字だけ)
 * その後、SD0001.TXTというファイルを作成して、LPCcappuccino!+CR+LFという文字を永遠に書き込みます
 */

void die(FRESULT rc)
{

}


/*
 * SDカードのサンプル
 */
void sd_test()
{
	FRESULT rc;

//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT bw, br, i;
  char tmp[64];

	f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
	if (!rc){
		 USART_puts(&usart, "Type the file content."); //i2c_cmd(0x80);
//	xprintf("\nType the file content.\n");
    for (;;) {
      rc = f_read(&Fil, buff, sizeof(buff), &br);	/* Read a chunk of file */
      if (rc || !br) break;			/* Error or end of file */
/*
      for (i = 0; i < br; i++){
        if(i==0x10) Serial.println(); //i2c_cmd(0xC0);
        Serial.print(buff[i]); //i2c_data(buff[i]);
      }
      */
      USART_puts(&usart, (char*) buff); //xprintf("%s\n", buff);
    }
    if (rc) die(rc);
    rc = f_close(&Fil);
  }
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

    rc = f_open(&Fil, "SD0001.TXT", FA_WRITE | FA_OPEN_ALWAYS);
    f_lseek(&Fil, f_size(&Fil));
    if (rc) die(rc);
    i = 0;
    // 無限ループでこの関数からは抜けない
      while(i < 100){
        sprintf(tmp, "Strawberry Linux %d\r\n", i);
        rc = f_write(&Fil, tmp, strlen(tmp), &bw);
        if (rc) die(rc);
        USART_puts(&usart, (char*)tmp); //xprintf("%s\n", tmp);
        // SDカードに書き出します。
        f_sync(&Fil);
        i++;
      }
//	return;
    f_mount(0, NULL);
}


