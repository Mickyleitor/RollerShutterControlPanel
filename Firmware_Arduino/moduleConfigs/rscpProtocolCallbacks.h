#ifndef __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
#define __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__

#include <stdint.h>
#include <Wire.h>

#include "../Utils.h"
#include "../rscpProtocol/rscpProtocol.h"

static inline int8_t rscpGetRxByteCallback(uint8_t * data)
{
    if ( popByteFromRxBuffer(data) < 0 ){
        return -1;
    }
    return 0;
}

static inline int8_t rscpSendSlotCallback(uint8_t * data, uint8_t length)
{
    return (Wire.write(data, length) == length) ? 0 : -1;
}

static inline void rscpRxWaitingCallback(void)
{
    yield();    // This also kicks the watchdog
}

// Modbus-16 CRC calculation
static inline uint16_t rscpGetCrcCallback(uint8_t *data, uint8_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++){
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++){
            if (crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc;
}

static inline int8_t rscpSetShutterActionCallback(RSCP_Arg_rollershutter * arg)
{
    if ( pushTaskToBuffer(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t *)&arg, sizeof(struct RSCP_Arg_rollershutter)) < 0){
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

static inline int8_t rscpSetShutterPositionCallback(RSCP_Arg_rollershutterposition * arg)
{
    // TODO: Implement
    (void)arg;
    return RSCP_ERR_NOT_SUPPORTED;
}

static inline void rscpGetShutterPositionCallback(RSCP_Reply_rollershutterposition * reply)
{
    (void)reply;
    // TODO: Implement
}

static inline int8_t rscpSetSwitchRelayCallback(RSCP_Arg_switchrelay * arg)
{
    if ( pushTaskToBuffer(RSCP_CMD_SET_SWITCH_RELAY, (uint8_t *)&arg, sizeof(struct RSCP_Arg_switchrelay)) < 0){
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

static inline void rscpGetSwitchRelayCallback(RSCP_Reply_switchrelay * reply)
{
    reply->status = (digitalRead(PIN_RELAY) == HIGH) ? RSCP_DEF_SWITCH_RELAY_ON : RSCP_DEF_SWITCH_RELAY_OFF;
}

static inline int8_t rscpSetBuzzerActionCallback(RSCP_Arg_buzzer_action * arg)
{
    if ( pushTaskToBuffer(RSCP_CMD_SET_BUZZER_ACTION, (uint8_t *)&arg, sizeof(struct RSCP_Arg_buzzer_action)) < 0){
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

#endif // __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
