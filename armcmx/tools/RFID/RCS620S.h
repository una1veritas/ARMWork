/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#ifndef RCS620S_H_
#define RCS620S_H_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if defined (ARDUINO)
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wprogram.h>
#include <Print.h>
#endif
#elif defined (ARMCMX)
#include "armcmx.h"
#include "USARTSerial.h"
#endif

#include "ISO14443.h"

/* --------------------------------
 * Constants
 * -------------------------------- */

#define RCS620S_MAX_DATA_LEN    255
#define RCS620S_MAX_EXTENDED_DATA_LEN      265

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class RCS620S {
  public:
    RCS620S(Stream & ser);
    uint16_t sendCommand(const uint8_t* com, const word len);
    uint16_t receiveResponse(uint8_t* resp, const uint16_t maxresplen = RCS620S_MAX_DATA_LEN);

    uint16_t start(void);
		uint16_t InListPassiveTarget(const uint8_t maxtg, const byte brty, const byte * payload, byte * data);
		uint16_t CommunicateThruEx(uint8_t* command, uint8_t commandLen);

		uint16_t FeliCa_RequestService(uint16_t);
    uint16_t FeliCa_ReadWithoutEncryption(uint16_t serviceCode, word blknum, byte* responce);
    uint16_t RFOff(void);

    uint16_t push(const uint8_t* data, uint8_t dataLen);

  private:
    const static uint8_t ACK_FRAME[6];
    const static uint8_t ACK_FRAME_LEN = 6;
    const static uint8_t FRAME_HEADER[5];
    const static uint8_t POSTAMBLE = 0x00;

    uint16_t LEN;
    uint8_t LCS;
    uint8_t DCS;
    uint8_t LastCommand;

    uint16_t waitACK(uint8_t n = ACK_FRAME_LEN);
    void cancel(void);
    void calcDCS(const uint8_t* data, uint16_t len);
    void write(const uint8_t* data, uint16_t len);
    int read(uint8_t* data, uint16_t len);
    void flush(void);

    int checkTimeout(unsigned long t0);

    Stream & port;
    int stat;
    
  public:
    unsigned long timeout;
    uint8_t idm[8];
    uint8_t pmm[8];

    static const uint8_t COMMAND = 0xD4;
    static const uint8_t RESPONSE = 0xD5;
    static const uint8_t RESPONSE_BIT = 0x01;
    static const uint8_t SUBCOMMAND_CommunicateThruEx = 0xA0;
    static const uint8_t SUBCOMMAND_InListPassiveTarget = 0x4A;
    static const uint8_t SUBCOMMAND_Reset = 0x18;
    static const uint8_t SUBCOMMAND_RFConfiguration = 0x32;
    static const uint8_t SUBCOMMAND_PowerDown = 0x16;
    static const uint8_t SUBCOMMAND_GetFirmwareVersion = 0x02;

		static const uint8_t ERROR_DATA = 0x7f;

    static const byte FELICA_CMD_REQUESTSERVICE = 0x02;
 //	static const byte FELICA_CMD_ERQUESTRESPONSE = 0x04;
    static const byte FELICA_CMD_READWITHOUTENCRYPTION = 0x06;
//	static const byte FELICA_CMD_WRITEWITHOUTENCRYPTION = 0x08;
//	static const byte FELICA_CMD_REQUESTSYSTEMCODE = 0x0c;


};

#endif /* !RCS620S_H_ */
