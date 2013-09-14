#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
#include "ff.h"

#include "armcmx.h"
#include "usart.h"
#include "sdfatfs.h"


DWORD get_fattime()
{
  uint32_t cal = SD.cal;
  uint32_t time = SD.time;
  
  uint8_t y,m,d, hh, mm, ss;
  y = 20 + (cal>>16&0x0f) + (cal>>20&0x0f)*10;
  m = (cal>>8&0x0f) + (cal>>12&0x0f)*10;
  d = (cal&0x0f) + (cal>>4&0x0f)*10;
  hh = (time>>16&0x0f) + (time>>20&0x0f)*10;
  mm = (time>>8&0x0f) + (time>>12&0x0f)*10;
  ss = (time&0x0f) + (time>>4&0x0f)*10;
  
  return ((uint32_t)y<<25) | m<<21 | d<<16 | hh << 11 | mm<<5 | ss>>1;
}

void set_fattime(uint32_t date, uint32_t time) {
  SD.time = time;
  SD.cal = date;
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

	f_mount(0, &SD.fatfs);		/* Register volume work area (never fails) */

	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	rc = f_open(&SD.file, "MESSAGE.TXT", FA_READ);
	if (!rc){
		 USART_puts(&usart, "Type the file content."); //i2c_cmd(0x80);
//	xprintf("\nType the file content.\n");
    for (;;) {
      rc = f_read(&SD.file, SD.buff, sizeof(SD.buff), &br);	/* Read a chunk of file */
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
    rc = f_close(&SD.file);
    USART_puts(&usart, "\n\nFile read test finished.\n\n");
  } else {
		 USART_puts(&usart, "Read file open failed.\n"); 
  }
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

  rc = f_open(&SD.file, "LOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  if ( !rc ) {
    f_lseek(&SD.file, f_size(&SD.file));
    if (rc) die(rc);
    i = 0;
    // 無限ループでこの関数からは抜けない
    while(i < 50){
      sprintf(tmp, "%06u %06u Strawberry Linux %d\r\n", SD.cal, SD.time, i);
      rc = f_write(&SD.file, tmp, strlen(tmp), &bw);
      if (rc) die(rc);
      USART_puts(&usart, (char*)tmp); //xprintf("%s\n", tmp);
      // SDカードに書き出します。
      f_sync(&SD.file);
      i++;
    }
    USART_puts(&usart, "\n\nWrite file test finished.\n\n");
  } else {
    USART_puts(&usart, "Write file open failed.\n"); //i2c_cmd(0x80);
  }
//	return;
    f_mount(0, NULL);
}


