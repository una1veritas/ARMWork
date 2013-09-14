#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
#include "ff.h"
#include "mmc_ssp.h"

#include "armcmx.h"
#include "usart.h"


FATFS fatfs;		/* File system object */
FIL file;			/* File object */
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

	f_mount(0, &fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&file, "MESSAGE.TXT", FA_READ);
	if (!rc){
		USART_puts(&usart, "Type the file content."); //i2c_cmd(0x80);
//	xprintf("\nType the file content.\n");
    for (;;) {
      rc = f_read(&file, buff, sizeof(buff), &br);	/* Read a chunk of file */
      if ( rc ) break;			/* Error or end of file */
      buff[br] = 0;
/*
      for (i = 0; i < br; i++){
        if(i==0x10) Serial.println(); //i2c_cmd(0xC0);
        Serial.print(buff[i]); //i2c_data(buff[i]);
      }
      */
      USART_puts(&usart, (char*)buff); //xprintf("%s\n", buff);
      if ( br < sizeof(buff) ) 
        break;
    }
    if (rc) die(rc);
    rc = f_close(&file);
    USART_puts(&usart, "\n\nFile read test finished.\n\n");
  } else {
		 USART_puts(&usart, "Read file open failed.\n"); 
  }
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

  rc = f_open(&file, "LOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  if ( !rc ) {
    f_lseek(&file, f_size(&file));
    if (rc) die(rc);
    i = 0;
    // 無限ループでこの関数からは抜けない
    while(i < 50){
      sprintf(tmp, "%08u Strawberry Linux %d\r\n", (uint32_t)get_fattime(), i);
      rc = f_write(&file, tmp, strlen(tmp), &bw);
      if (rc) die(rc);
      USART_puts(&usart, (char*)tmp); //xprintf("%s\n", tmp);
      // SDカードに書き出します。
      f_sync(&file);
      i++;
    }
    USART_puts(&usart, "\n\nWrite file test finished.\n\n");
  } else {
    USART_puts(&usart, "Write file open failed.\n"); //i2c_cmd(0x80);
  }
//	return;
    f_mount(0, NULL);
}


void sd_load()
{
	FRESULT rc;

//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT br;

	f_mount(0, &fatfs);		/* Register volume work area (never fails) */

	rc = f_open(&file, "KEYID.TXT", FA_READ);
	if (!rc){
		for (;;) {
      rc = f_read(&file, buff, sizeof(buff), &br);	/* Read a chunk of file */
      if ( rc ) break;			/* Error or end of file */
      buff[br] = 0;
      USART_puts(&usart, (char*)buff); //xprintf("%s\n", buff);
      if ( br < sizeof(buff) ) 
        break;
    }
    rc = f_close(&file);
    USART_puts(&usart, "\n\nFile read test finished.\n\n");
  } else {
		 USART_puts(&usart, "Read file open failed.\n"); 
  }
    f_mount(0, NULL);
}
