#ifndef __RSCP_PROTOCOL_CONFIG_H_INCLUDED__
#define __RSCP_PROTOCOL_CONFIG_H_INCLUDED__

#include <stdint.h>
#include <Wire.h>
#include "../rscpProtocol/rscpProtocol.h"

static inline int8_t rscpGetRxByteCallback(uint8_t * data)
{
    if( Wire.available() > 0){
        *data = Wire.read();
        return 0;
    }
    return -1;
}

static inline int8_t rscpSendSlotCallback(uint8_t * data, uint8_t length)
{
    return (Wire.write(data, length) == length) ? 0 : -1;
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

static inline void rscpSetShutterActionCallback(RSCP_Arg_rollershutter * arg)
{
    sendCommand(arg->shutter, arg->action, arg->retries);
}

static inline void rscpSetShutterPositionCallback(RSCP_Arg_rollershutterposition * arg)
{
    (void)arg;
    // TODO: Implement
}

static inline void rscpGetShutterPositionCallback(RSCP_Reply_rollershutterposition * reply)
{
    (void)reply;
    // TODO: Implement
}

static inline void rscpSetSwitchRelayCallback(RSCP_Arg_switchrelay * arg)
{
    digitalWrite(PIN_RELAY, (arg->status == RSCP_DEF_SWITCH_RELAY_ON) ? HIGH : LOW);
}

static inline void rscpGetSwitchRelayCallback(RSCP_Reply_switchrelay * reply)
{
    reply->status = (digitalRead(PIN_RELAY) == HIGH) ? RSCP_DEF_SWITCH_RELAY_ON : RSCP_DEF_SWITCH_RELAY_OFF;
}

static inline void rscpSetBuzzerVolumeCallback(RSCP_Arg_buzzer_volume * arg)
{
    (void)arg;
    // TODO: Implement
}

static inline void rscpSetBuzzerActionCallback(RSCP_Arg_buzzer_action * arg)
{
    switch (arg->action){
        case RSCP_DEF_BUZZER_ACTION_BUTTON_BEEP: {
            // TODO: Implement
            break;
        }

    }
}

#endif // __RSCP_PROTOCOL_CONFIG_H_INCLUDED__
