#ifndef _sccb_H
#define _sccb_H

#define SCCB_SIC     PB4
#define SCCB_SID     PB5

#define SCCB_SIC_H()     digitalWrite(SCCB_SIC, HIGH)
#define SCCB_SIC_L()     digitalWrite(SCCB_SIC, LOW)

#define SCCB_SID_H()     digitalWrite(SCCB_SID, HIGH)
#define SCCB_SID_L()     digitalWrite(SCCB_SID, LOW)
/**/
#define SCCB_SID_IN      portMode(SCCB_SID, PinBit(SCCB_SID), INPUT, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_UP)
#define SCCB_SID_OUT     pinMode(SCCB_SID, OUTPUT)

#define SCCB_SID_STATE	 digitalRead(SCCB_SID)

///////////////////////////////////////////
void SCCB_GPIO_Config(void);
void SCCB_SID_GPIO_OUTPUT(void);
void SCCB_SID_GPIO_INPUT(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned char SCCBwriteByte(unsigned char m_data);
unsigned char SCCBreadByte(void);

unsigned char Sensor_init(void);
unsigned char rd_Sensor_Reg(unsigned char regID, unsigned char *regDat);
unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat);

#endif /* _IIC_H */


