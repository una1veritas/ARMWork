#ifndef _sccb_H
#define _sccb_H

#define sic_H()     digitalWrite(sic, HIGH)
#define sic_L()     digitalWrite(sic, LOW)

#define sid_H()     digitalWrite(sid, HIGH)
#define sid_L()     digitalWrite(sid, LOW)
/**/

#define sid_STATE	 digitalRead(sid)

class SCCBus {
  const GPIOPin sic, sid;
  
public:
  SCCBus(const GPIOPin clk, const GPIOPin data) : sic(clk), sid(data) {} 
  
  void GPIO_Config(void);
  void SID_OUTPUT(void);
  void SID_INPUT(void);
  void start(void);
  void stop(void);
  void noAck(void);
  unsigned char write(uint8_t data);
  unsigned char read(void);

  uint8_t init(void);
  unsigned char readRegister(unsigned char regID, unsigned char *regDat);
  unsigned char writeRegister(unsigned char regID, unsigned char regDat);
};

class Cam {
  const GPIOPin VSYNC, HREF, WEN, RRST, OE, RCLK;
  
public:
  Cam(const GPIOPin vsync, const GPIOPin href, const GPIOPin wen, const GPIOPin rrst, const GPIOPin oe, const GPIOPin rclk) 
    :  VSYNC(vsync), HREF(href), WEN(wen), RRST(rrst), OE(oe), RCLK(rclk) {}

  void outEnable() { digitalWrite(OE, LOW); }
  void outDisable() { digitalWrite(OE, HIGH); }
	void writeEnable() { digitalWrite(WEN, LOW); }
	void writeDisable() { digitalWrite(WEN, HIGH); }
	void readReset(uint8_t val) { digitalWrite(RRST,val); }
  void clock(uint8_t val) { digitalWrite(RCLK, val); }
};

#endif /* _sccb_H */


