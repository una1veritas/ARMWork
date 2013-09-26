/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#ifndef RCS620S_H_
#define RCS620S_H_

#include <inttypes.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wprogram.h>
#include <Print.h>
#include <HardwareSerial.h>
#endif

#include <SoftwareSerial.h>

#include "ISO14443.h"

/* --------------------------------
 * Constants
 * -------------------------------- */

#define RCS620S_MAX_CARD_RESPONSE_LEN    254
#define RCS620S_MAX_RW_RESPONSE_LEN      265

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class RCS620S {
public:
    RCS620S();
    RCS620S(Stream & ser);

    inline void begin() { init(); }
    int init(void);
//    int polling(const byte brtype = 0x01, uint16_t syscode = 0xffff);
	byte listPassiveTarget(byte * data, const byte brty =
			TypeA, const word syscode = 0xffff);

    int CommunicateThruEx(uint8_t* command, uint8_t commandLen);
    int requestService(uint16_t);
    int readWithoutEncryption(uint16_t serviceCode, word blknum, byte* responce);
    int rfOff(void);

    int push(const uint8_t* data, uint8_t dataLen);

private:
    word rwCommand(uint8_t* command, const word commandLen, const word maxresplen = RCS620S_MAX_RW_RESPONSE_LEN); //,
//        uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN],
 //       uint16_t* responseLen);
    void cancel(void);
    uint8_t calcDCS(
        const uint8_t* data,
        uint16_t len);

    void writeSerial(
        const uint8_t* data,
        uint16_t len);
    int readSerial(
        uint8_t* data,
        uint16_t len);
    void flushSerial(void);

    int checkTimeout(unsigned long t0);

    Stream & port;
    const enum {
    	HARDWARESERIAL,
    	SOFTWARESERIAL
    } portType;
    int stat;
public:
    unsigned long timeout;
    uint8_t idm[8];
    uint8_t pmm[8];

    static const int RCS956_COMMAND = 0xD4;
    static const int RCS956_COMMAND_CommunicateThruEx = 0xA0;
    static const int RCS956_COMMAND_InListPassiveTarget = 0x4A;
    static const int RCS956_COMMAND_Reset = 0x18;
    static const int RCS956_COMMAND_RFConfiguration = 0x32;
    static const int RCS956_COMMAND_PowerDown = 0x16;
    static const int RCS956_COMMAND_GetFirmwareVersion = 0x02;

	static const byte FELICA_CMD_REQUESTSERVICE = 0x02;
//	static const byte FELICA_CMD_ERQUESTRESPONSE = 0x04;
	static const byte FELICA_CMD_READWITHOUTENCRYPTION = 0x06;
//	static const byte FELICA_CMD_WRITEWITHOUTENCRYPTION = 0x08;
//	static const byte FELICA_CMD_REQUESTSYSTEMCODE = 0x0c;


};

#endif /* !RCS620S_H_ */
