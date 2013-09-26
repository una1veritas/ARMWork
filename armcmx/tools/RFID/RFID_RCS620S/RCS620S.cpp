/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wprogram.h>
#include <Print.h>
#include <HardwareSerial.h>
#endif

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

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

RCS620S::RCS620S() :
		port(Serial), portType(HARDWARESERIAL), stat(0) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

RCS620S::RCS620S(Stream & ser) :
		port(ser), portType(SOFTWARESERIAL), stat(0) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

int RCS620S::init(void) {
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	static_cast<HardwareSerial &>(port).begin(115200);

	/* RFConfiguration (various timings) */
	memcpy(response, "\xd4\x32\x02\x00\x00\x00", 6);
	responseLen = rwCommand(response, 6);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed.");
		return 0;
	}

	/* RFConfiguration (max retries) */
	memcpy(response, "\xd4\x32\x05\x00\x00\x00",6);
	responseLen = rwCommand(response, 6);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed 2.");
		return 0;
	}

	/* RFConfiguration (additional wait time = 24ms) */
	memcpy(response, "\xd4\x32\x81\xb7", 4);
	responseLen = rwCommand(response, 4);
	if (!responseLen || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed 3.");
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
	buf[9] = 0x01;      // サービス数
	buf[10] = lowByte(serviceCode);
	buf[11] = highByte(serviceCode);
	buf[12] = 0x01;     // ブロック数
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

int RCS620S::rfOff(void) {
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

	flushSerial();
	uint8_t dcs = calcDCS(commresp, commandLen);
	/* transmit the command */
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0xff;
	if (commandLen <= 255) {
		/* normal frame */
		buf[3] = commandLen;
		buf[4] = (uint8_t) -buf[3];
		writeSerial(buf, 5);
	} else {
		/* extended frame */
		buf[3] = 0xff;
		buf[4] = 0xff;
		buf[5] = (uint8_t) ((commandLen >> 8) & 0xff);
		buf[6] = (uint8_t) ((commandLen >> 0) & 0xff);
		buf[7] = (uint8_t) -(buf[5] + buf[6]);
		writeSerial(buf, 8);
	}
	writeSerial(commresp, commandLen);
	buf[0] = dcs;
	buf[1] = 0x00;
	writeSerial(buf, 2);

	/* receive an ACK */
	ret = readSerial(buf, 6);
	if (ret == 0 || (memcmp(buf, "\x00\x00\xff\x00\xff\x00", 6) != 0)) {
		cancel();
		Serial.println("canceled.");
		return 0;
	}
#ifdef DEBUG
	Serial.println("received ACK.");
#endif
	/* receive a response */
	ret = readSerial(buf, 5);
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
		ret = readSerial(buf + 5, 3);
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

	ret = readSerial(commresp, resplen);
	if (!ret) {
		cancel();
		return 0;
	}

	dcs = calcDCS(commresp, resplen);

	ret = readSerial(buf, 2);
	if (!ret || (buf[0] != dcs) || (buf[1] != 0x00)) {
		cancel();
		return 0;
	}

	return resplen;
}

void RCS620S::cancel(void) {
	/* transmit an ACK */
	writeSerial((const uint8_t*) "\x00\x00\xff\x00\xff\x00", 6);
	delay(1);
	flushSerial();
}

uint8_t RCS620S::calcDCS(const uint8_t* data, uint16_t len) {
	uint8_t sum = 0;

	for (uint16_t i = 0; i < len; i++) {
		sum += data[i];
	}

	return (uint8_t) -(sum & 0xff);
}

void RCS620S::writeSerial(const uint8_t* data, uint16_t len) {
	port.write(data, len);
}

int RCS620S::readSerial(uint8_t* data, uint16_t len) {
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

void RCS620S::flushSerial(void) {
	port.flush();
}

int RCS620S::checkTimeout(unsigned long t0) {
	unsigned long t = millis();

	if ((t - t0) >= this->timeout) {
		return 1;
	}

	return 0;
}
