/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include "RCS620S.h"

//#define DEBUG

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_DEFAULT_TIMEOUT  2000


const uint8_t RCS620S::ACK_FRAME[6] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
const uint8_t RCS620S::FRAME_HEADER[5] = { 0x00, 0x00, 0xff, 0xff, 0xff }; // normal frame 0--2, extended 0--4


RCS620S::RCS620S(Stream & ser) : port(ser), stat(0) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

uint16_t RCS620S::start(void) {
	uint8_t response[RCS620S_MAX_DATA_LEN];

	/* RFConfiguration (various timings) */
	if ( sendCommand((const uint8_t *) "\xd4\x32\x02\x00\x00\x00", 6) == 0  || receiveResponse(response) != 2 ) {
#ifdef DEBUG
		Serial.println("RF Config. failed.");
#endif
		return 0;
	}

	/* RFConfiguration (max retries) */
	if ( sendCommand((const uint8_t *) "\xd4\x32\x05\x00\x00\x00", 6) == 0 || receiveResponse(response) != 2 ) {
#ifdef DEBUG
		Serial.println("RF Config. failed 2.");
#endif
		return 0;
	}

	/* RFConfiguration (additional wait time = 24ms) */
	if ( sendCommand((const uint8_t *) "\xd4\x32\x81\xb7", 4) == 0 || receiveResponse(response) != 2 ) {
#ifdef DEBUG
		Serial.println("RF Config. failed 3.");
#endif
		return 0;
	}

	return 1;
}

//(const uint8_t maxtg, const byte brty, byte * data)
uint16_t RCS620S::InListPassiveTarget(const uint8_t maxtg, const byte brty, const uint8_t * payload, byte * resp) {
	int ret;
	uint16_t cmdlen;
	/* InListPassiveTarget */
	resp[0] = COMMAND;
	resp[1] = SUBCOMMAND_InListPassiveTarget;
	resp[2] = maxtg;
	resp[3] = brty;
	switch (brty) {
	case NFC::BAUDTYPE_106K_A:
		cmdlen = 4;
		break;
	case NFC::BAUDTYPE_212K_F:
	case NFC::BAUDTYPE_424K_F:
		memcpy(resp+4, payload, 5);
		cmdlen = 9;
		break;
	}
  
	if ( sendCommand(resp, cmdlen) == 0 ) 
		return 0;
  if ( (ret = receiveResponse(resp)) == 0 )
    return 0;
  if ( resp[2] == 0 ) // no cards.
    return 0;
  
	switch(brty) {
	case 0x00:
		// d5 4b NumTag [Tg SENS_RES[2](=atqa) SEL_RES NFCIDLength NFCID1[] [ATS[]]]
		memcpy(idm, resp+7, resp[7]+1);
		break;
	case 0x01:
		memcpy(idm, resp + 6, 8);
		memcpy(pmm, resp + 14, 8);
		break;
	}
	return ret;
}

uint16_t RCS620S::InDataExchange(uint8_t* cmdresp, uint16_t cmdlen) {
  uint8_t buf[32];
  uint16_t reslen = 0;
  buf[0] = COMMAND;
  buf[1] = SUBCOMMAND_InDataExchange;
  buf[2] = 0x01;  // target no.; we support only 1 card
  memcpy(buf+3, cmdresp, cmdlen);
  reslen = sendCommand(buf, cmdlen+3);
  reslen = receiveResponse(cmdresp);
  return reslen;
}

uint16_t RCS620S::CommunicateThruEx(uint8_t* commresp, uint8_t commandLen) {
	int ret;
	uint16_t commandTimeout;
	uint8_t buf[RCS620S_MAX_DATA_LEN];
//	uint16_t len;

	if (this->timeout >= (0x10000 / 2)) {
		commandTimeout = 0xffff;
	} else {
		commandTimeout = (uint16_t) (this->timeout * 2);
	}

	/* CommunicateThruEX */
	buf[0] = COMMAND;
	buf[1] = SUBCOMMAND_CommunicateThruEx;
	buf[2] = lowByte(commandTimeout);
	buf[3] = highByte(commandTimeout);
	buf[4] = (uint8_t) (commandLen + 1);
	memcpy(buf + 5, commresp, commandLen);

	if ( sendCommand(buf, 5 + commandLen) == 0 || receiveResponse(buf) == 0 || (buf[2] != 0x00) ) {
		return 0;
	}

	ret = (uint8_t) (buf[3] - 1);
	memcpy(commresp, buf + 4, ret);

	return ret;
}

uint16_t RCS620S::FeliCa_RequestService(uint16_t serviceCode) {
	int ret;
	uint8_t buf[32];

	buf[0] = FELICA_CMD_REQUESTSERVICE;
	memcpy(buf + 1, idm, 8);
	buf[9] = 0x01;
	buf[10] = lowByte(serviceCode);
	buf[11] = highByte(serviceCode);

	ret = CommunicateThruEx(buf, 12);

	if ( ret != 12 || (buf[0] != 0x03)
			|| (memcmp(buf + 1, idm, 8) != 0)
			|| ((buf[10] == 0xff) && (buf[11] == 0xff))) {
		return 0;
	}

	return ret;
}

uint16_t RCS620S::FeliCa_ReadWithoutEncryption(uint16_t serviceCode, uint16_t blknum,
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
	if ( ret != 28 || (buf[0] != 0x07)
			|| (memcmp(buf + 1, idm, 8) != 0)) {
		return 0;
	}
	memcpy(responce, buf + 12, 16);
	return 1;
}


uint16_t RCS620S::Mifare_Authenticate(const uint8_t key[8]) {
  uint8_t buf[32];
  uint16_t reslen;
  
  buf[0] = MIFARE_CMD_AUTH_KEYA;
  buf[1] = 0x04;
  memcpy(buf+2, key, 6);
	memcpy(buf+8, idm + 1, idm[0]);
  //
  Serial.print("InDataExchange send: ");
  Serial.printBytes(buf, 8 + idm[0]);
  Serial.println(", ");
  //
	reslen = InDataExchange(buf, 8 + idm[0]);
  //
  Serial.print("receive: ");
  Serial.printBytes(buf, reslen);
  Serial.println(".\n");

  buf[0] = MIFARE_CMD_READ;
  buf[1] = 0x04;
  
  reslen = InDataExchange(buf, 2);
  Serial.print("receive: ");
  Serial.printBytes(buf, reslen);
  Serial.println(".\n");
  /*
		buf[0]='\0';
		memcpy(buf,"\x30\x04",2);
		responseLen = 2;
		if(InDataExchange(buf,&responseLen)) {
//			for (i=0; i<10; i++) {	//example S828234212
//			Outbuf[i] = buf[i];
//			}
//			memcpy(Outbuf + 10,"\x00\x00\x00\x00\x00\x00",6); 
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
*/
  return reslen;
}


uint16_t RCS620S::RFOff(void) {
	uint8_t response[RCS620S_MAX_DATA_LEN];

	/* RFConfiguration (RF field) */
	if ( sendCommand((const uint8_t *) "\xd4\x32\x01\x00", 4) == 0 || receiveResponse(response) != 2 ) {
		return 0;
	}

	return 1;
}

uint16_t RCS620S::push(const uint8_t* data, uint8_t dataLen) {
	int ret;
	uint8_t buf[RCS620S_MAX_DATA_LEN];
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


uint16_t RCS620S::waitACK(uint8_t n) {
	uint16_t nread = 0;
	
	/* receive an ACK */
	unsigned long t0 = millis();

	while (nread < n) {
		if (checkTimeout(t0)) 
			break;
		if ( !port.available() ) 
			continue;
		if ( ACK_FRAME[nread] == port.read() ) {
			nread++;
		} else {
			break;
		}
	}

#ifdef DEBUG
	Serial.print("ACK (");
	Serial.print(nread);
	Serial.print("); ");
#endif

	if ( nread != n ) {
		return nread;
	}
	return nread;
}


uint16_t RCS620S::sendCommand(const uint8_t* cmdresp, const uint16_t cmdlen) {
	int ret;
	uint8_t buf[5];

  if ( cmdresp[0] == COMMAND )
    LastCommand = cmdresp[1]; // store the subcommand code
  
#ifdef DEBUG
	Serial.print("send ");
	Serial.printBytes(cmdresp, cmdlen);
	Serial.print("; ");
#endif
	calcDCS(cmdresp, cmdlen);
	/* transmit the command */
	if (cmdlen <= 255) {
		/* normal frame */
		buf[0] = cmdlen;
		buf[1] = (uint8_t) - cmdlen;
		write(FRAME_HEADER, 3);
		write(buf, 2);
	} else {
		/* extended frame */
		buf[0] = (uint8_t) ((cmdlen >> 8) & 0xff);
		buf[1] = (uint8_t) ((cmdlen >> 0) & 0xff);
		buf[2] = (uint8_t) -(buf[0] + buf[1]);
		write(FRAME_HEADER, 5);
		write(buf, 3);
	}
	write(cmdresp, cmdlen);
	buf[0] = DCS;
	buf[1] = POSTAMBLE;
	write(buf, 2);
	
	/* receive an ACK */
	ret = waitACK();
	return ret;
}


uint16_t RCS620S::receiveResponse(uint8_t* resp, const uint16_t maxresplen) {
	int ret;
	uint8_t buf[5];
	uint16_t resplen;

	/* receive a frame header of response */
	ret = waitACK(3);
	if (!ret || (ret != 3) ) {
		// may be timed-out., or something going wrong.
		cancel();
		return 0;
	}
	
	ret = read(buf, 2);
	if ((buf[0] == 0xff) && (buf[1] == 0xff)) {
		// ext. frame
		ret = read(buf + 2, 3); // read three additional bytes for the length and LCS
		if (!ret || (((buf[2] + buf[3] + buf[4]) & 0xff) != 0)) 
			return 0;
		resplen = buf[2];
		resplen <<= 8;
		resplen |= buf[3]; // <= RCS620S_MAX_EXTENDED_DATA_LEN
	} else {
		if ( (uint8_t)(buf[0] + buf[1]) != 0) 
			return 0;
		resplen = buf[0];
	}
#ifdef DEBUG
	Serial.print("\nresponse (");
	Serial.print(resplen);
	Serial.print(") ");
#endif
	if (resplen > maxresplen) {
		// may be extended frame, but not supported yet...
		//return 0;
    resplen = maxresplen;
	}
	if ( read(resp, resplen) == 0 ) {
		cancel();
		return 0;
	}
	ret = read(buf, 2);
	calcDCS(resp, resplen);
	if (!ret || (buf[0] != DCS) || (buf[1] != 0x00)) {
		cancel();
#ifdef DEBUG
		Serial.println("DCS error.\n");
#endif
		return 0;
	}
#ifdef DEBUG
	Serial.printBytes(resp, resplen);
	Serial.println(".\n");
	Serial.flush();
#endif
  if ( (resp[0] == RESPONSE) && resp[1] == (LastCommand | RESPONSE_BIT) ) {
    return resplen;
  }
  return 0;
}

void RCS620S::cancel(void) {
	/* transmit an ACK */
	write(ACK_FRAME, 6);
	delay(1);
	flush();
}

void RCS620S::calcDCS(const uint8_t* data, uint16_t len) {
	uint8_t sum = 0;

	for (uint16_t i = 0; i < len; i++) {
		sum += data[i];
	}

	DCS = (uint8_t) -(sum & 0xff);
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
