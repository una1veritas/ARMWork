/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include "RCS620S.h"

#define DEBUG

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_DEFAULT_TIMEOUT  2000

/* --------------------------------
 * Variable
 * -------------------------------- */

/* --------------------------------
 * Prototype Declaration
 * -------------------------------- */

/* --------------------------------
 * Macro
 * -------------------------------- */

/* --------------------------------
 * Function
 * -------------------------------- */

/* ------------------------
 * public
 * ------------------------ */

RCS620S::RCS620S(Stream & ser) : port(ser), stat(0) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

int RCS620S::start(void) {
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	/* RFConfiguration (various timings) */
	memcpy(response, "\xd4\x32\x02\x00\x00\x00", 6);
	responseLen = rwCommand(response, 6);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
#ifdef DEBUG
		Serial.println("RF Config. failed.");
#endif
		return 0;
	}

	/* RFConfiguration (max retries) */
	memcpy(response, "\xd4\x32\x05\x00\x00\x00",6);
	responseLen = rwCommand(response, 6);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
#ifdef DEBUG
		Serial.println("RF Config. failed 2.");
#endif
		return 0;
	}

	/* RFConfiguration (additional wait time = 24ms) */
	memcpy(response, "\xd4\x32\x81\xb7", 4);
	responseLen = rwCommand(response, 4);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
#ifdef DEBUG
		Serial.println("RF Config. failed 3.");
#endif
		return 0;
	}

	return 1;
}

//int RCS620S::polling(const byte brty, uint16_t systemCode) {
byte RCS620S::listPassiveTarget(byte * resp, const byte brty, const word syscode) {
	int ret;
//	uint8_t buf[9];
	uint16_t responseLen;
	uint16_t cmdlen;
	/* InListPassiveTarget */
	memcpy(resp, "\xd4\x4a\x01\x01\x00\xff\xff\x00\x00", 9);
	resp[3] = brty;
	switch (brty) {
	case 0x00:
		cmdlen = 4;
		break;
	case 0x01:
		resp[5] = lowByte(syscode);
		resp[6] = highByte(syscode);
		cmdlen = 9;
		break;
	}
	ret = rwCommand(resp, cmdlen);
#ifdef DEBUG
	Serial.print("rwCommand ret = ");
	Serial.println(ret, HEX);
	for(int i = 0; i < ret; i++) {
		Serial.print(resp[i]>>4, HEX);
		Serial.print(resp[i]&0x0f, HEX);
		Serial.print(' ');
	}
	Serial.println();
#endif

	if (!ret //|| (responseLen != 22)
			|| ( memcmp(resp, "\xd5\x4b\x01", 3) != 0) ) {
		return 0;
	}

	switch(brty) {
	case 0x00:
		// d5 4b NumTag [Tg SENS_RES[2] SEL_RES NFCIDLength NFCID1[] [ATS[]]]
		memcpy(idm, resp+7, resp[7]+1);
		break;
	case 0x01:
		memcpy(idm, resp + 6, 8);
		memcpy(pmm, resp + 14, 8);
		break;
	}
	return ret;
}

int RCS620S::CommunicateThruEx(uint8_t* commresp, uint8_t commandLen) {
	int ret;
	uint16_t commandTimeout;
	uint8_t buf[RCS620S_MAX_RW_RESPONSE_LEN];
//	uint16_t len;

	if (this->timeout >= (0x10000 / 2)) {
		commandTimeout = 0xffff;
	} else {
		commandTimeout = (uint16_t) (this->timeout * 2);
	}

	/* CommunicateThruEX */
	buf[0] = RCS956_COMMAND;
	buf[1] = RCS956_COMMAND_CommunicateThruEx;
	buf[2] = lowByte(commandTimeout);
	buf[3] = highByte(commandTimeout);
	buf[4] = (uint8_t) (commandLen + 1);
	memcpy(buf + 5, commresp, commandLen);

#ifdef DEBUG
	Serial.print("rwCommand commandLen = ");
	Serial.println(commandLen, HEX);
	Serial.print("<< ");
	for(int i = 0; i < commandLen+5; i++) {
		Serial.print(buf[i]>>4, HEX);
		Serial.print(buf[i]&0x0f, HEX);
		Serial.print(' ');
	}
	Serial.println();
#endif

	ret = rwCommand(buf, 5 + commandLen);
#ifdef DEBUG
	Serial.print("rwCommand ret = ");
	Serial.println(ret, HEX);
	Serial.print(">> ");
	for(int i = 0; i < ret; i++) {
		Serial.print(buf[i]>>4, HEX);
		Serial.print(buf[i]&0x0f, HEX);
		Serial.print(' ');
	}
	Serial.println();
#endif
	if (!ret || (buf[0] != 0xd5) || (buf[1] != 0xa1)
			|| (buf[2] != 0x00) ) {
		return 0;
	}

	ret = (uint8_t) (buf[3] - 1);
	memcpy(commresp, buf + 4, ret);

	return ret;
}

int RCS620S::requestService(uint16_t serviceCode) {
	int ret;
	uint8_t buf[RCS620S_MAX_CARD_RESPONSE_LEN];

	buf[0] = FELICA_CMD_REQUESTSERVICE;
	memcpy(buf + 1, idm, 8);
	buf[9] = 0x01;
	buf[10] = lowByte(serviceCode);
	buf[11] = highByte(serviceCode);

	ret = CommunicateThruEx(buf, 12);

	if (!ret || (ret != 12) || (buf[0] != 0x03)
			|| (memcmp(buf + 1, idm, 8) != 0)
			|| ((buf[10] == 0xff) && (buf[11] == 0xff))) {
		return 0;
	}

	return ret;
}

int RCS620S::readWithoutEncryption(uint16_t serviceCode, word blknum,
		byte* responce) {
	int ret;
	uint8_t buf[32];

	buf[0] = FELICA_CMD_READWITHOUTENCRYPTION;
	memcpy(buf + 1, idm, 8);
	buf[9] = 0x01;      // �T�[�r�X��
	buf[10] = lowByte(serviceCode);
	buf[11] = highByte(serviceCode);
	buf[12] = 0x01;     // �u���b�N��
	buf[13] = 0x80 | highByte(blknum);
	buf[14] = lowByte(blknum);

	ret = CommunicateThruEx(buf, 15);
	if (!ret || (ret != 28) || (buf[0] != 0x07)
			|| (memcmp(buf + 1, idm, 8) != 0)) {
		return 0;
	}
	memcpy(responce, buf + 12, 16);
	return 1;
}

int RCS620S::RFOff(void) {
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	/* RFConfiguration (RF field) */
	memcpy(response, "\xd4\x32\x01\x00", 4);
	responseLen = rwCommand(response, 4);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}

	return 1;
}

int RCS620S::push(const uint8_t* data, uint8_t dataLen) {
	int ret;
	uint8_t buf[RCS620S_MAX_CARD_RESPONSE_LEN];
	uint8_t responseLen;

	if (dataLen > 224) {
		return 0;
	}

	/* Push */
	buf[0] = 0xb0;
	memcpy(buf + 1, this->idm, 8);
	buf[9] = dataLen;
	memcpy(buf + 10, data, dataLen);

	ret = CommunicateThruEx(buf, 10 + dataLen);
	if (!ret || (ret != 10) || (buf[0] != 0xb1)
			|| (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != dataLen)) {
		return 0;
	}

	buf[0] = 0xa4;
	memcpy(buf + 1, this->idm, 8);
	buf[9] = 0x00;

	ret = CommunicateThruEx(buf, 10);
	if (!ret || (responseLen != 10) || (buf[0] != 0xa5)
			|| (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != 0x00)) {
		return 0;
	}

	delay(1000);

	return 1;
}


/* ------------------------
 * private
 * ------------------------ */

word RCS620S::rwCommand(uint8_t* commresp, const word commandLen, const word maxresplen)
//,	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN], uint16_t* responseLen)
{
	int ret;
	uint8_t buf[9];
	word resplen;

	flush();
	uint8_t dcs = calcDCS(commresp, commandLen);
	/* transmit the command */
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0xff;
	if (commandLen <= 255) {
		/* normal frame */
		buf[3] = commandLen;
		buf[4] = (uint8_t) - buf[3];
		write(buf, 5);
	} else {
		/* extended frame */
		buf[3] = 0xff;
		buf[4] = 0xff;
		buf[5] = (uint8_t) ((commandLen >> 8) & 0xff);
		buf[6] = (uint8_t) ((commandLen >> 0) & 0xff);
		buf[7] = (uint8_t) -(buf[5] + buf[6]);
		write(buf, 8);
	}
	write(commresp, commandLen);
	buf[0] = dcs;
	buf[1] = 0x00;
	write(buf, 2);

	/* receive an ACK */
	ret = read(buf, 6);
#ifdef DEBUG
	Serial.print("Listening ACK [");
	Serial.print(ret);
	Serial.print("] ");
	for(int i = 0; i < ret; i++) {
		Serial.print(buf[i], HEX);
		Serial.print(' ');
	}
	Serial.println();
#endif
	if (ret == 0 || (memcmp(buf, "\x00\x00\xff\x00\xff\x00", 6) != 0)) {
		cancel();
#ifdef DEBUG
		Serial.println("canceled.");
#endif
		return 0;
	}
#ifdef DEBUG
	Serial.println("ACK received.");
#endif
	/* receive a response */
	ret = read(buf, 5);
#ifdef DEBUG
	Serial.print("received resp, ");
	Serial.println(ret);
	for(int i = 0; i < ret; i++) {
		Serial.print(buf[i]>>4, HEX);
		Serial.print(buf[i]&0x0f, HEX);
		Serial.print(' ');
	}
	Serial.println();
#endif
	if (!ret) {
		cancel();
		return 0;
	} else if (memcmp(buf, "\x00\x00\xff", 3) != 0) {
		return 0;
	}
	if ((buf[3] == 0xff) && (buf[4] == 0xff)) {
		ret = read(buf + 5, 3);
		if (!ret || (((buf[5] + buf[6] + buf[7]) & 0xff) != 0)) {
			return 0;
		}
		resplen = (((uint16_t) buf[5] << 8) | ((uint16_t) buf[6] << 0));
	} else {
		if (((buf[3] + buf[4]) & 0xff) != 0) {
			return 0;
		}
		resplen = buf[3];
	}
	if (resplen > RCS620S_MAX_RW_RESPONSE_LEN) {
		return 0;
	}

	ret = read(commresp, resplen);
	if (!ret) {
		cancel();
		return 0;
	}

	dcs = calcDCS(commresp, resplen);

	ret = read(buf, 2);
	if (!ret || (buf[0] != dcs) || (buf[1] != 0x00)) {
		cancel();
		return 0;
	}

	return resplen;
}

void RCS620S::cancel(void) {
	/* transmit an ACK */
	write((const uint8_t*) "\x00\x00\xff\x00\xff\x00", 6);
	delay(1);
	flush();
}

uint8_t RCS620S::calcDCS(const uint8_t* data, uint16_t len) {
	uint8_t sum = 0;

	for (uint16_t i = 0; i < len; i++) {
		sum += data[i];
	}

	return (uint8_t) -(sum & 0xff);
}

void RCS620S::write(const uint8_t* data, uint16_t len) {
	port.write(data, len);
}

int RCS620S::read(uint8_t* data, uint16_t len) {
	uint16_t nread = 0;
	unsigned long t0 = millis();

	while (nread < len) {
		if (checkTimeout(t0)) {
			return 0;
		}
		if (port.available() > 0) {
			data[nread] = port.read();
			nread++;
		}
	}
	return nread;
}

void RCS620S::flush(void) {
	port.flush();
}

int RCS620S::checkTimeout(unsigned long t0) {
	unsigned long t = millis();

	if ((t - t0) >= this->timeout) {
		return 1;
	}
	return 0;
}


/************************************************************************************/
#ifdef OHNO
/*
下薗先生


　大野です。

以下がRCS620S.cppに相当する部分です。（実際はmain部に一緒にコーディングしています）

static const byte KYUTECH_KEY_A[] = { (byte)'K', (byte)'a', (byte)'Z', (byte)'u', (byte)'I', (byte)'i' }; static const byte KYUTECH_KEY_B[] = { (byte)'c', (byte)'E', (byte)'t', (byte)'U', (byte)'y', (byte)'K' };

unsigned long timeout = RCS620S_DEFAULT_TIMEOUT; uint8_t IDm[8]; uint8_t cardtype;
*/

int waitCardReleased = 0;
/* Here are RCS620/S commands */
/* writeSerial command */
void writeSerial(const uint8_t* data, uint16_t len){
//	int i;
	Serial1.write(data, len);
/*	Serial2.print("SEND:");			//デバッグ用RCS620＜＞Host通信確認
	for (i=0;i<len;i++) {
		Serial2.print(data[i]>>4,HEX);
		Serial2.print(data[i]&0x0f,HEX);
		Serial2.print(' ');
	}
	Serial2.println();
*/
}

/* checkTimeout command */
int checkTimeout(unsigned long t0){
	unsigned long t = millis();

	if ((t - t0) >= timeout) {
		return 1;
	}
	return 0;
}

/* readSerial command */
int readSerial(uint8_t* data, uint16_t len){
	uint16_t nread = 0;
	unsigned long t0 = millis();
//	int i;
	while (nread < len) {
		if (checkTimeout(t0)) {
			return 0;
		}
		if (Serial1.available() > 0) {
			data[nread] = Serial1.read();
			nread++;
		}
		Serial1.flush();
	}
/*	Serial2.print("RECV:");			//デバッグ用RCS620＜＞Host通信確認
	for (i=0;i<len;i++){
		Serial2.print(data[i]>>4, HEX);
		Serial2.print(data[i]&0x0f, HEX);
		Serial2.print(" ");
	}
	Serial2.println();
*/
	return 1;
}

/* calcDCS command */
uint8_t calcDCS(const uint8_t* data, uint16_t len){
	uint8_t sum = 0;

	for (uint16_t i = 0; i < len; i++) {
		sum += data[i];
	}
	return (uint8_t)-(sum & 0xff);
}

/* cancel command */
void cancel(void) {
	writeSerial((const uint8_t*)"\x00\x00\xff\x00\xff\x00", 6);
	delay(1);
	Serial1.flush();
}


/* rwCommand command */
int rwCommand(const uint8_t* command, uint16_t commandLen, uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN], uint16_t* responseLen){
	int ret;
	uint8_t buf[9];
	Serial1.flush();
	uint8_t dcs = calcDCS(command, commandLen);
  /* transmit the command */
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0xff;
	if (commandLen <= 255) {
    /* normal frame */
		buf[3] = commandLen;
		buf[4] = (uint8_t)-buf[3];
		writeSerial(buf, 5);
	} else {
    /* extended frame */
		buf[3] = 0xff;
		buf[4] = 0xff;
		buf[5] = (uint8_t)((commandLen >> 8) & 0xff);
		buf[6] = (uint8_t)((commandLen >> 0) & 0xff);
		buf[7] = (uint8_t)-(buf[5] + buf[6]);
		writeSerial(buf, 8);
	}
	writeSerial(command, commandLen);
	buf[0] = dcs;
	buf[1] = 0x00;
	writeSerial(buf, 2);
  /* receive an ACK */
	ret = readSerial(buf, 6);
	if (!ret || (memcmp(buf, "\x00\x00\xff\x00\xff\x00", 6) != 0)) {
		cancel();
		return 0;
	}
	ret = readSerial(buf, 5);
	if (!ret) {
		cancel();
		return 0;
	} else if  (memcmp(buf, "\x00\x00\xff", 3) != 0) {
		return 0;
	}
	if ((buf[3] == 0xff) && (buf[4] == 0xff)) {
		ret = readSerial(buf + 5, 3);
		if (!ret || (((buf[5] + buf[6] + buf[7]) & 0xff) != 0)) {
			return 0;
		}
		*responseLen = (((uint16_t)buf[5] << 8) | ((uint16_t)buf[6] << 0));
	}  else {
		if (((buf[3] + buf[4]) & 0xff) != 0) {
		return 0;
		}
		*responseLen = buf[3];
	}
	if (*responseLen > RCS620S_MAX_RW_RESPONSE_LEN) {
		return 0;
	}
	ret = readSerial(response, *responseLen);
	if (!ret) {
		cancel();
		return 0;
	}
	dcs = calcDCS(response, *responseLen);
	ret = readSerial(buf, 2);

  if ( !ret || (buf[0] != dcs) || (buf[1] != 0x00) ) { 
    cancel();
    return 0;
  }

	return 1;
}

/* rfOff command */
int rfOff(void){
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;
	/* RFConfiguration (RF field) */
	ret = rwCommand((const uint8_t*)"\xd4\x32\x01\x00", 4,response, &responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}
	return 1;
}

/* initDevice command */
int initDevice(void){
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;
	/* RFConfiguration (various timings) */
	ret = rwCommand((const uint8_t*)"\xd4\x32\x02\x00\x00\x00", 6, response, &responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}
  /* RFConfiguration (max retries) */
	ret = rwCommand((const uint8_t*)"\xd4\x32\x05\x00\x00\x00", 6, response, &responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}
  /* RFConfiguration (additional wait time = 24ms) */
	ret = rwCommand((const uint8_t*)"\xd4\x32\x81\xb7", 4, response, &responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}
	return 1;
}



/* polling command */ //カードがかざされたことを検出しタイプ判別
int polling(uint8_t* cardtype) {
	uint16_t systemCode = 0xfe00;
	int ret;
	uint8_t buf[9];
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

  /* InListPassiveTarget */
	memcpy(buf, "\xd4\x4a\x01\x01\x00\xff\xff\x00\x00", 9); 
	buf[5] = (uint8_t)((systemCode >> 8) & 0xff);
	buf[6] = (uint8_t)((systemCode >> 0) & 0xff);

	ret = rwCommand(buf, 9, response, &responseLen);

	if  (!ret || (responseLen != 22) || memcmp(response, "\xd5\x4b\x01\x01\x12\x01", 6) != 0) {
		memcpy(buf, "\xd4\x4a\x01\x00", 4);
		ret = rwCommand(buf, 4, response, &responseLen);

		if (!ret || (memcmp(response, "\xd5\x4b\x01", 3) != 0)) {
			return 0;
		}
	}
	*cardtype = buf[3];
	switch(*cardtype) {
		case 0x00: // MIFARE
			memcpy(IDm, response + 7, response[7] + 1);
			break;
		case 0x01: // FELICA
			memcpy(IDm, response + 6, 8);
			break;
	}
/*	printf("GET Card IDm :");
	for (i = 0;i < 8;i++){
		Serial.print((unsigned char)IDm[i],HEX);
		Serial.print(" ");
	}
	Serial.println();
 
*/
	return 1;
}

/* cardCommand command */
int cardCommand(const uint8_t* command, uint8_t commandLen, uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], uint8_t* responseLen){
	int ret;
	uint16_t commandTimeout;
	uint8_t buf[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t len;

	if (timeout >= (0x10000 / 2)) {
		commandTimeout = 0xfe00;
  	} else {
		commandTimeout = (uint16_t)(timeout * 2);
	}

  /* CommunicateThruEX */
	buf[0] = 0xd4;
	buf[1] = 0xa0; 
	buf[2] = (uint8_t)((commandTimeout >> 0) & 0xff);
	buf[3] = (uint8_t)((commandTimeout >> 8) & 0xff);
	buf[4] = (uint8_t)(commandLen + 1);
	memcpy(buf + 5, command, commandLen);

	ret = rwCommand(buf, 5 + commandLen, buf, &len);

	if (!ret || (len < 4) || (buf[0] != 0xd5) || (buf[1] != 0xa1) || (buf[2] != 0x00) ||
		(len != (3 + buf[3]))) {
		return 0;
	}

	*responseLen = (uint8_t)(buf[3] - 1);
	memcpy(response, buf + 4, *responseLen);

	return 1;
}

int InDataExchange(uint8_t* commresp, uint8_t* commandLen) {
	int ret;
	uint8_t buf[RCS620S_MAX_RW_RESPONSE_LEN];
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen = 0;
	buf[0] = 0xd4;
	buf[1] = 0x40;
	buf[2] = 0x01;
	memcpy(buf+3, commresp, *commandLen);
	ret = rwCommand(buf, 3+(*commandLen), response, &responseLen);
	ret = (response[3] - 1);
	memcpy(commresp, response + 4, ret);
	*commandLen = responseLen;
	return ret;
}
/* readNFC command */     
//　カードの中を読み込む
int readNFC(uint8_t* cardtype){
	int ret,i,j;
	uint8_t buf[RCS620S_MAX_CARD_RESPONSE_LEN];
	uint8_t Outbuf[17];
	uint8_t responseLen;
	if(*cardtype == 0x01) { //FeliCa
		buf[0]  = 0x06;
		memcpy(buf + 1, IDm, 8);
		buf[9]  = 0x01;  // service count
		buf[10] = 0x8b;  // sid
		buf[11] = 0x1a;  // sid
		buf[12] = 0x04;  // block count
		buf[13] = 0x80u;
		buf[14] = 0x00;
		buf[15] = 0x80u;
		buf[16] = 0x01; 
		buf[17] = 0x80u;
		buf[18] = 0x02; 
		buf[19] = 0x80u;
		buf[20] = 0x03;
  
		ret = cardCommand(buf, 21, buf, &responseLen);
		if (!ret) {
			return 0;
		}
//		Serial.print(responseLen);
		if (responseLen == 11) {		// FCFカードでなければIDmをそのまま返す
			Outbuf[0] = ((IDm[0]>>4) & 0x0F) + 0x30 ;
			Outbuf[1] = ((IDm[0] & 0x0F) + 0x30 );
		 	if (Outbuf[0] > 0x39 ) Outbuf[0] = Outbuf[0] + 0x07;
			if (Outbuf[1] > 0x39 ) Outbuf[1] = Outbuf[1] + 0x07;
			for(i=1;i<8;i++) {	
				j = i * 2;
				Outbuf[j] = (((IDm[i]>>4) & 0x0F) + 0x30 );
				Outbuf[j+1] =((IDm[i] & 0x0F) + 0x30 );
				if (Outbuf[j] > 0x39 ) Outbuf[j] = Outbuf[j] + 0x07;
				if (Outbuf[j+1] > 0x39 ) Outbuf[j+1] = Outbuf[j+1] + 0x07;
			}
			Outbuf[16] = 0x00;
		} else if (memcmp(buf + 44, "\x34\x30\x30\x31\x30\x30\x37\x31", 8) == 0) {						//　FeliCaFCFだった場合
			//九工大のカードだったら
/*			j = 0;
			for(i = 12;i < 22; i++) { 	//学籍 職員番号
				Outbuf[j] = buf[i];
				j++;
			}
			Outbuf[10] = buf[26];  // 再発行フラグ
			memcpy(Outbuf + 11,"\x00\x00\x00\x00\x00",5); */
			Outbuf[0] = buf[12];
			Outbuf[1] = buf[13];
			Outbuf[2] = ',';
			Outbuf[3] = buf[14];
			Outbuf[4] = buf[15];
			Outbuf[5] = buf[16];
			Outbuf[6] = buf[17];
			Outbuf[7] = buf[18];
			Outbuf[8] = buf[19];
			Outbuf[9] = buf[20];
			Outbuf[10] = buf[21];
			Outbuf[11] = ',';
			Outbuf[12] = buf[26];
			memcpy(Outbuf + 13, "\x00\x00\x00\x00",4);
			
		} else {
			// FCFでも九工大のカードでなければIDmを返す
			Outbuf[0] = (((IDm[0]>>4) & 0x0F) + 0x30 );
			Outbuf[1] = ((IDm[0] & 0x0F) + 0x30 );
		 	if (Outbuf[0] > 0x39 ) Outbuf[0] = Outbuf[0] + 0x07;
			if (Outbuf[1] > 0x39 ) Outbuf[1] = Outbuf[1] + 0x07;
			for(i=1;i<8;i++) {
				j = i * 2;
				Outbuf[j] = (((IDm[i]>>4) & 0x0F) + 0x30 );
				Outbuf[j+1] =((IDm[i] & 0x0F) + 0x30 );
				if (Outbuf[j-1] > 0x39 ) Outbuf[j] = Outbuf[j] + 0x07;
				if (Outbuf[j] > 0x39 ) Outbuf[j+1] = Outbuf[j+1] + 0x07;
			}
		}

	} else if(*cardtype == 0x00){ //Mifare
		memcpy(buf, "\x61\x04", 2);
		memcpy(buf + 2, KYUTECH_KEY_B, 6);
		memcpy(buf + 8, IDm + 1, IDm[0]);
		responseLen = 8 + IDm[0];
		ret = InDataExchange(buf, &responseLen);
		buf[0]='\0';
		memcpy(buf,"\x30\x04",2);
		responseLen = 2;
		if(InDataExchange(buf,&responseLen)) {
/*			for (i=0; i<10; i++) {	//example S828234212
			Outbuf[i] = buf[i];
			}
			memcpy(Outbuf + 10,"\x00\x00\x00\x00\x00\x00",6); */
			Outbuf[0] = buf[0];
			Outbuf[1] = ',';
			Outbuf[2] = buf[1];
			Outbuf[3] = buf[2];
			Outbuf[4] = buf[3];
			Outbuf[5] = buf[4];
			Outbuf[6] = buf[5];
			Outbuf[7] = buf[6];
			Outbuf[8] = buf[7];
			Outbuf[9] = buf[8];
			Outbuf[10] = ',';
			Outbuf[11] = buf[9];
		
			memcpy(Outbuf + 12, "\x00\x00\x00\x00\x00",5);

		}
	}
	xQueueSendToBack(keyQueue, Outbuf, 0); //TASK4へ
	return 1;
}

/* END RCS620S command */


--
大野芳久 <ohno@tech-i.kyutech.ac.jp>

#endif
