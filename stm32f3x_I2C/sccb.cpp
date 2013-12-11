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
void SCCB_GPIO_Config(void)
{
	pinMode(SCCB_SIC, OUTPUT);  // clock pin is always output
	pinMode(SCCB_SID, OUTPUT);
  
}

/*
-----------------------------------------------
   機能: startコマンド、SCCBの開始信号
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //データラインハイレベル
    delay_us(50);

    SCCB_SIC_H();	   //クロックハイの時、データラインはハイ→ロー
    delay_us(50);
 
    SCCB_SID_L();
    delay_us(50);

    SCCB_SIC_L();	 //データラインローレベル、単動作関数
    delay_us(50);


}
/*
-----------------------------------------------
   機能: stopコマンド、SCCBの終了信号
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID_L();
    delay_us(50);
 
    SCCB_SIC_H();	
    delay_us(50);
  

    SCCB_SID_H();	
    delay_us(50);
   
}

/*
-----------------------------------------------
   機能: noAck、連続読み取り中の最後の終了周期
   パラメータ: 無し
 戻り値: 無し
-----------------------------------------------
*/
void noAck(void)
{
	
	SCCB_SID_H();	
	delay_us(50);
	
	SCCB_SIC_H();	
	delay_us(50);
	
	SCCB_SIC_L();	
	delay_us(50);
	
	SCCB_SID_L();	
	delay_us(50);

}

/*
-----------------------------------------------
   機能: 1バイトのデータをSCCB書き込み
   パラメータ: データ書き込み
 戻り値: 送信成功1戻り、送信失敗0戻り
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j, tem;

	for(j=0;j<8;j++) //データ8回ループ送信
	{
		if((m_data<<j)&0x80)
		{
			SCCB_SID_H();	
		}
		else
		{
			SCCB_SID_L();	
		}
		delay_us(50);
		SCCB_SIC_H();	
		delay_us(50);
		SCCB_SIC_L();	
		delay_us(2);

	}
	//delay_us(50);
	SCCB_SID_IN;/*SDAを入力に設定*/
	delay_us(10);
	SCCB_SIC_H();	
	delay_us(80);
	if(SCCB_SID_STATE){tem=0;}   //SDA=1送信失敗、戻り0}
	else {tem=1;}   //SDA=0送信成功，戻り1
	SCCB_SIC_L();	
	delay_us(50);	
        SCCB_SID_OUT;/*SDAを出力に設定*/

	return (tem);  
}

/*
-----------------------------------------------
   機能: 1バイトデータ読み取り、戻り
   パラメータ: 無し
 戻り値: 読み取りのデータ
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read, j;
	read=0x00;
	
	SCCB_SID_IN;/*SDAを入力に設定*/
	delay_us(50);
	for(j=8;j>0;j--) //データ8回ループ受信
	{		     
		//delay_us(100);
		SCCB_SIC_H();
		delay_us(50);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC_L();
		delay_us(50);
	}	
        SCCB_SID_OUT;/*SDAを出力に設定*/
	return(read);
}


///////////////////////////////////////////////////////////////////////////
//機能：OV7660レジスタ書き込み
//戻り：1-成功	0-失敗
//guanfu_wang
unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat)
{
		startSCCB();//SCCB送信 パスコマンド伝送開始
	if(0==SCCBwriteByte(0x42))//アドレス書き込み
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0==SCCBwriteByte(regID))//レジスタID
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0==SCCBwriteByte(regDat))//レジスタにデータ書き込み
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
  	stopSCCB();//SCCB送信 パスコマンド伝送終了
	
  	return(1);//成功戻り
}
////////////////////////////
//機能：OV7660レジスタ読み取り
//戻り：1-成功	0-失敗
//guanfu_wang
unsigned char rd_Sensor_Reg(unsigned char regID, unsigned char *regDat)
{
	//書き込み動作でレジスタアドレス設定
	startSCCB();
	if(0==SCCBwriteByte(0x42))//アドレス書き込み
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	if(0==SCCBwriteByte(regID))//レジスタID
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	stopSCCB();//SCCB送信 パスコマンド伝送終了
	
	delay_us(20);
	
	//レジスタアドレス設定の次は読み取り設定
	startSCCB();
	if(0==SCCBwriteByte(0x43))//アドレス読み取り
	{
		stopSCCB();//SCCB送信 パスコマンド伝送終了
		return(0);//エラー戻り
	}
	delay_us(20);
  	*regDat=SCCBreadByte();//戻り读到的值
  	noAck();//NACKコマンド送信
  	stopSCCB();//SCCB送信 パスコマンド伝送終了
  	return(1);//成功戻り
}




/* Sensor_init() */
//戻り1成功，戻り0失敗
//guanfu_wang
unsigned char Sensor_init(void)
{
	unsigned char temp;
	
	unsigned int i=0;
   //      XCLK_init_ON();//MCO機能ON、 CMOSセンサーにクロックを提供する
	//uchar ovidmsb=0,ovidlsb=0;
//	Sensor_GPIO_Init();
	SCCB_GPIO_Config();//io init..
        
	temp=0x80;
	if(0==wr_Sensor_Reg(0x12, temp)) //Reset SCCB
	{
          return 0 ;//エラー戻り
	}
        delay(1);
	if(0==rd_Sensor_Reg(0x0b, &temp))//读ID
	{
		return 0 ;//エラー戻り
	}
      
         if(temp==0x73)//OV7670
	 {
	   for(i=0;i<OV7670_REG_NUM;i++)
	   {
		if( 0==wr_Sensor_Reg(OV7670_reg[i][0],OV7670_reg[i][1]))
		{
			return 0;//エラー戻り
		}
	    }
           
//          Sensor_EXTI_Config();
//          Sensor_Interrupts_Config();
	}

	return 0x01; //ok

        
} 

///////////////////
