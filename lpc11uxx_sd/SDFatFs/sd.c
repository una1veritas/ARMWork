#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
#include "ff.h"

#include "armcmx.h"
#include "usart.h"
#include "sdfatfs.h"


static SDFatFs SD;

DWORD get_fattime()
{
  return 0;
}

#ifdef ORGINAL
FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
static uint8_t buff[32];
#endif

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

	f_mount(0, &SD.Fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&SD.Fil, "MESSAGE.TXT", FA_READ);
	if (!rc){
		 USART_puts(&usart, "Type the file content."); //i2c_cmd(0x80);
//	xprintf("\nType the file content.\n");
    for (;;) {
      rc = f_read(&SD.Fil, SD.buff, sizeof(SD.buff), &br);	/* Read a chunk of file */
      if (rc ) break;			/* Error or end of file */
      if ( br < sizeof(SD.buff) ) {
        SD.buff[br] = 0;
      }
/*
      for (i = 0; i < br; i++){
        if(i==0x10) Serial.println(); //i2c_cmd(0xC0);
        Serial.print(buff[i]); //i2c_data(buff[i]);
      }
      */
      USART_puts(&usart, (char*)SD.buff); //xprintf("%s\n", buff);
      if ( br < sizeof(SD.buff) ) 
        break;
    }
    if (rc) die(rc);
    rc = f_close(&SD.Fil);
  }

  USART_puts(&usart, "\n\nFile read test finished.\n\n");
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

    rc = f_open(&SD.Fil, "LOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
    f_lseek(&SD.Fil, f_size(&SD.Fil));
    if (rc) die(rc);
    i = 0;
    // 無限ループでこの関数からは抜けない
      while(i < 50){
        sprintf(tmp, "Strawberry Linux %d\r\n", i);
        rc = f_write(&SD.Fil, tmp, strlen(tmp), &bw);
        if (rc) die(rc);
        USART_puts(&usart, (char*)tmp); //xprintf("%s\n", tmp);
        // SDカードに書き出します。
        f_sync(&SD.Fil);
        i++;
      }
//	return;
    f_mount(0, NULL);
}


