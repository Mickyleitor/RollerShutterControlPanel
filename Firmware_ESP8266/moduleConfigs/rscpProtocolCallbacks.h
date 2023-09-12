#ifndef __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
#define __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__

#include <stdint.h>
#include <Wire.h>

#include "../rscpProtocol/rscpProtocol.h"

#define SLAVE_I2C_ADDRESS                                                   (0x08)

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
    Wire.beginTransmission(SLAVE_I2C_ADDRESS); // transmit to device #8
    Wire.write(data, length);
    return (Wire.endTransmission() == 0) ? 0 : -1;
}

static inline int8_t rscpRequestSlotCallback(uint8_t length)
{
    return (Wire.requestFrom(SLAVE_I2C_ADDRESS, (int)length) == length) ? 0 : -1;
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

static inline void rscpSetShutterActionCallback(RSCP_Arg_rollershutter * arg)
{
    (void)arg;
}

static inline void rscpSetShutterPositionCallback(RSCP_Arg_rollershutterposition * arg)
{
    (void)arg;
}

static inline void rscpGetShutterPositionCallback(RSCP_Reply_rollershutterposition * reply)
{
    (void)reply;
}

static inline void rscpSetSwitchRelayCallback(RSCP_Arg_switchrelay * arg)
{
    (void)arg;
}

static inline void rscpGetSwitchRelayCallback(RSCP_Reply_switchrelay * reply)
{
    (void)reply;
}

static inline void rscpSetBuzzerVolumeCallback(RSCP_Arg_buzzer_volume * arg)
{
    (void)arg;
}

static inline void rscpSetBuzzerActionCallback(RSCP_Arg_buzzer_action * arg)
{
    (void)arg;
}

#endif // __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
