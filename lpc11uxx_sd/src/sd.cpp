#include <stdio.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
//#include "ff.h"
#include "mmc_ssp.h"

#include "armcmx.h"
#include "usart.h"

#include "SDFatFs.h"

/*
 * SDカードからMESSAGE.TXTのファイルを読み込んでI2C液晶に表示します。
 * (先頭32文字だけ)
 * その後、SD0001.TXTというファイルを作成して、LPCcappuccino!+CR+LFという文字を永遠に書き込みます
 */

static SDFile file(SD);

/*
 * SDカードのサンプル
 */
void sd_test()
{
	FRESULT rc;
//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT bw, br, i;
  char tmp[80];
  size_t n;
  
	SD.mount(); //f_mount(0, &fatfs);		/* Register volume work area (never fails) */

	rc = file.open("MESSAGE.TXT", FA_READ);
	if (!rc){
		USART_puts(&usart, "Type the file content.\n\n"); //i2c_cmd(0x80);
//	xprintf("\nType the file content.\n");
    for (; ;) {
      rc = f_read(file(), tmp, 64, &br);	/* Read a chunk of file */
      if ( rc ) break;			/* Error or end of file */
      tmp[br] = 0;
/*
      for (i = 0; i < br; i++){
        if(i==0x10) Serial.println(); //i2c_cmd(0xC0);
        Serial.print(buff[i]); //i2c_data(buff[i]);
      }
      */
      USART_puts(&usart, (char*)tmp); //xprintf("%s\n", buff);
      if ( br < 64 ) 
        break;
    }
    rc = f_close(file());
    USART_puts(&usart, "\n\nFile read test finished.\n\n");
  } else {
		 USART_puts(&usart, "Read file open failed.\n"); 
  }
  /*
   *	ファイル書き込みテスト
   *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
   */

  rc = f_open(file(), "LOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  if ( !rc ) {
    file.seek(file.size());
    i = 0;
    // 無限ループでこの関数からは抜けない
    while(i < 50){
      sprintf(tmp, "%08u Strawberry Linux %d\r\n", (uint32_t)get_fattime(), i);
      bw = file.write((uint8_t*)tmp, strlen(tmp));
      if (file.error()) 
        break;
      USART_puts(&usart, (char*)tmp); //xprintf("%s\n", tmp);
      // SDカードに書き出します。
      file.flush();
      i++;
    }
    rc = f_close(file());
    USART_puts(&usart, "\n\nWrite file test finished.\n\n");
  } else {
    USART_puts(&usart, "Write file open failed.\n"); //i2c_cmd(0x80);
  }
//	return;
  SD.unmount(); //f_mount(0, NULL);
}


void sd_load()
{
	FRESULT rc;
  char tmp[80];
  uint32 count = 0;
//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	size_t n;

	SD.mount(); //f_mount(0, &fatfs);		/* Register volume work area (never fails) */

	rc = f_open(file(), "KEYID.TXT", FA_READ);
	if (!rc){
		for (; !f_eof(file()) ; ) {
#ifdef ORIGINAL
      p = f_gets(tmp, 64, file());	/* Read a chunk of file */
      count++;
      if ( count % 100 == 0 ) 
        USART_puts(&usart, (char*)tmp); //xprintf("%s\n", buff);
      if ( p == NULL ) break;			/* Error or end of file */
#endif
      n = file.getLine(tmp, 64, SDFile::EOL_CRNL);
      if ( file.error() )
        break;
      if ( tmp[0] == '#' )
        continue;
      if ( count % 100 == 0 ) {
        USART_puts(&usart, (char*)tmp);
        sprintf(tmp, " %04d\n", count);
        USART_puts(&usart, (char*)tmp);
      }
      count++;
    }
    rc = f_close(file());
    USART_puts(&usart, "\n\nFile read test finished.\n\n");
  } else {
		 USART_puts(&usart, "Read file open failed.\n"); 
  }
  SD.unmount(); //  f_mount(0, NULL);
}
