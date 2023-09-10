#ifndef _RSCP_PROTOCOL_H_
#define _RSCP_PROTOCOL_H_

/*! \file **********************************************************************
 *
 *  \brief  Roller Shutter Control Panel Protocol (RSCP) for i2c communication
 *  This protocol is used to communicate between the main CPU and radio module
 *
 *  \author MickySim: https://www.mickysim.com
 *
 *  Copyright (c) 2023 MickySim All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#define RSCP_MAX_TX_BUFFER_SIZE                                             (64)

#define RSCP_PREAMBLE_BYTE                                                (0xAA)

#define RSCP_CMD_FAIL                                                   (0x0001) // CMD failed. This is a lesser failure compared to NOK.
#define RSCP_CMD_NOK                                                    (0x0002) // CMD not handled or parameter error. This is a fatal error.
#define RSCP_CMD_CPU_QUERY                                              (0x0003) // Query CPU type and protocol version
#define RSCP_CMD_SET_SHUTTER_ACTION                                     (0x0004) // Set shutter action
#define RSCP_CMD_SET_SHUTTER_POSITION                                   (0x0005) // Set shutter position
#define RSCP_CMD_GET_SHUTTER_POSITION                                   (0x0006) // Get shutter position
#define RSCP_CMD_SET_SWITCH_RELAY                                       (0x0007) // Set switch relay
#define RSCP_CMD_GET_SWITCH_RELAY                                       (0x0008) // Get switch relay
#define RSCP_CMD_SET_BUZZER_VOLUME                                      (0x0009) // Increase or decrease buzzer volume
#define RSCP_CMD_SET_BUZZER_ACTION                                      (0x000A) // Set buzzer action

// RSCP_CMD_CPU_QUERY
#define RSCP_DEF_PROTOCOL_VERSION                                         (0x01)
#define RSCP_DEF_SWVERSION_VERSION                                        (0x01)
#define RSCP_DEF_CRC_TYPE_MODBUS16                                        (0x01)
#define RSCP_DEF_CPU_TYPE_ATMEGA328P_8MHZ                                 (0x01)
#define RSCP_DEF_CPU_TYPE_ESP32_WROOM_02D                                 (0x02)

// RSCP_CMD_SET_SHUTTER_ACTION
#define RSCP_DEF_SHUTTER_ACTION_STOP                                      (0x01)
#define RSCP_DEF_SHUTTER_ACTION_UP                                        (0x02)
#define RSCP_DEF_SHUTTER_ACTION_DOWN                                      (0x03)
#define RSCP_DEF_SHUTTER_ACTION_OPEN                                      (0x04)
#define RSCP_DEF_SHUTTER_ACTION_CLOSE                                     (0x05)

// RSCP_CMD_SWITCH_RELAY
#define RSCP_DEF_SWITCH_RELAY_OFF                                         (0x01)
#define RSCP_DEF_SWITCH_RELAY_ON                                          (0x02)

// RSCP_CMD_BUZZER_ACTION
#define RSCP_DEF_BUZZER_ACTION_BUTTON_BEEP                                (0x01)

typedef enum {
    RSCP_ERR_OK = 0,
    RSCP_ERR_TIMEOUT = -1,
    RSCP_ERR_OVERFLOW = -2,
    RSCP_ERR_MALFORMED = -3,
    RSCP_ERR_NOT_SUPPORTED = -4,
    RSCP_ERR_TX_BUFFER_FULL = -5,
} RSCP_ErrorType;

struct RSCP_frame
{
    uint8_t length; // Length without crc field
    uint8_t command;
    // The reason for this is that Wire library only supports 32 bytes of data 
    // and therefore we need to limit the data to 26 bytes 
    // (32 - Wire overhead (2 bytes) - length - command - crc (2 bytes)) = 26 bytes)
    uint8_t data[26];
    uint16_t crc;
};

struct __attribute__ ((__packed__)) RSCP_Arg_rollershutter
{
    uint8_t shutter;
    uint8_t action;
    uint8_t retries;
};

struct __attribute__ ((__packed__)) RSCP_Arg_rollershutterposition
{
    uint8_t shutter;
    uint8_t position;
};

struct __attribute__ ((__packed__)) RSCP_Arg_switchrelay
{
    uint8_t status;
};

struct __attribute__ ((__packed__)) RSCP_Arg_buzzer_volume
{
    uint8_t volume;
};

struct __attribute__ ((__packed__)) RSCP_Arg_buzzer_action
{
    uint8_t action;
};

struct __attribute__ ((__packed__)) RSCP_Reply_cpuquery
{
    uint16_t flags;
    uint8_t crcType;
    uint8_t protocolversion;
    uint8_t cpuType;
    uint8_t swversion;
    uint16_t packetMaxLen;
};

struct __attribute__ ((__packed__)) RSCP_Reply_rollershutterposition
{
    uint8_t shutter;
    uint8_t position;
};

struct __attribute__ ((__packed__)) RSCP_Reply_switchrelay
{
    uint8_t status;
};

RSCP_ErrorType rscpHandleCPUQuery(void);
RSCP_ErrorType rscpHandle(uint32_t timeout_ticks);

#include "rscpProtocol.c"

#endif // _RSCP_PROTOCOL_H_