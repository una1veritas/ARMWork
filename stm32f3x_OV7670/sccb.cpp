/*

*/

#include "armcmx.h"
#include "sccb.h"
#include "ov7670_config.h"

/*
-----------------------------------------------
   機能: 初期化シミュレーション、SCCBインタフェース
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void SCCBus::GPIO_Config(void) {
	pinMode(sic, OUTPUT);  // clock pin is always output
  GPIOMode(sid, PinBit(sid), OUTPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP);
  sic_H();
  sid_H();
  delay(5);
}

/*
-----------------------------------------------
   機能: startコマンド、SCCBの開始信号
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void SCCBus::start(void) {
    sid_H();     //データラインハイレベル
    delay_us(50);

    sic_H();	   //クロックハイの時、データラインはハイ→ロー
    delay_us(50);
 
    sid_L();
    delay_us(50);

    sic_L();	 //データラインローレベル、単動作関数
    delay_us(50);
}
/*
-----------------------------------------------
   機能: stopコマンド、SCCBの終了信号
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void SCCBus::stop(void) {
    sid_L();
    delay_us(50);
 
    sic_H();	
    delay_us(50);

    sid_H();	
    delay_us(50);
}

/*
-----------------------------------------------
   機能: noAck、連続読み取り中の最後の終了周期
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void SCCBus::noAck(void)
{
	
	sid_H();	
	delay_us(50);
	
	sic_H();	
	delay_us(50);
	
	sic_L();	
	delay_us(50);
	
	sid_L();	
	delay_us(50);

}


/*
-----------------------------------------------
   機能: 1バイトのデータをSCCB書き込み
   パラメータ: データ書き込み
 戻り値: 送信成功1戻り、送信失敗0戻り
-----------------------------------------------
*/
unsigned char SCCBus::write(unsigned char m_data) {
	unsigned char j, tem;

	for( j = 0; j < 8; j++) {
    if( (m_data<<j)&0x80 ) {
      sid_H();
		} else {
			sid_L();	
		}
		delay_us(50);
		sic_H();	
		delay_us(50);
		sic_L();	
		delay_us(2);
	}

	delay_us(40);
  GPIOMode(sid, PinBit(sid), INPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP); 
  //	sid_IN;/*SDAを入力に設定*/
	delay_us(10);
	sic_H();	
	delay_us(80);
	if(sid_STATE) {
		tem=0;   //SDA=1送信失敗、戻り0}
	} else {
		tem=1;
	}   //SDA=0送信成功，戻り1
	sic_L();	
	delay_us(50);	
  
  //sid_OUT;/*SDAを出力に設定*/
  GPIOMode(sid, PinBit(sid), OUTPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP); //pinMode(sid, OUTPUT);

	return tem;
}

/*
-----------------------------------------------
   機能: 1バイトデータ読み取り、戻り
   パラメータ: 無し
 戻り値: 読み取りのデータ
-----------------------------------------------
*/
unsigned char SCCBus::read(void) {
	unsigned char read, j;
	read=0x00;
	
	//sid_IN;/*SDAを入力に設定*/
  GPIOMode(sid, PinBit(sid), INPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP); 
	delay_us(50);
	for(j=8; j>0; j--) {		     
		//delay_us(100);
		sic_H();
		delay_us(50);
		read <<= 1;
		if(sid_STATE) {
			read |= 1;
		}
		sic_L();
		delay_us(50);
	}	
  //sid_OUT;/*SDAを出力に設定*/
  GPIOMode(sid, PinBit(sid), OUTPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP); //pinMode(sid, OUTPUT);
  
	return(read);
}


///////////////////////////////////////////////////////////////////////////
//機能：OV7660レジスタ書き込み
//戻り：1-成功	0-失敗
//guanfu_wang
unsigned char SCCBus::writeRegister(unsigned char regID, unsigned char regDat)
{
		start();//SCCB送信 パスコマンド伝送開始
	if(0== write(0x42))//アドレス書き込み
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0== write(regID))//レジスタID
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0== write(regDat))//レジスタにデータ書き込み
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
  	stop();//SCCB送信 パスコマンド伝送終了
	
  	return(1);//成功戻り
}
////////////////////////////
//機能：OV7660レジスタ読み取り
//戻り：1-成功	0-失敗
//guanfu_wang
unsigned char SCCBus::readRegister(unsigned char regID, unsigned char *regDat)
{
	//書き込み動作でレジスタアドレス設定
	start();
	if(0== write(0x42))//アドレス書き込み
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0== write(regID))//レジスタID
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	stop();//SCCB送信 パスコマンド伝送終了
	
	delay_us(20);
	
	//レジスタアドレス設定の次は読み取り設定
	start();
	if(0== write(0x43))//アドレス読み取り
	{
		stop();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	*regDat= read();//戻り读到的值
  	noAck();//NACKコマンド送信
  	stop();//SCCB送信 パスコマンド伝送終了
  	return(1);//成功戻り
}




/* Sensor_init() */
//戻り1成功，戻り0失敗
//guanfu_wang
uint8_t SCCBus::init(void) {
	unsigned char temp;
	unsigned int i = 0;
  // XCLK_init_ON();//MCO機能ON、 CMOSセンサーにクロックを提供する
  //uchar ovidmsb=0,ovidlsb=0;
  
  GPIO_Config();//io init..
  
	temp=0x80;
  //Reset SCCB
	if(0 == writeRegister(0x12, temp)) {
    return 0 ;//エラー戻り
	}
  delay(10);
	if(0 == readRegister(0x0b, &temp)) {
		return 0 ;//エラー戻り
	}
  if(temp==0x73) {
    for(i=0;i<OV7670_REG_NUM;i++)	{
      if( 0 == writeRegister(OV7670_reg[i][0],OV7670_reg[i][1])) 
        return 0;//エラー戻り
	  }
    // Sensor_EXTI_Config();
    // Sensor_Interrupts_Config();
  }
	return 0x01; //ok
}

///////////////////
