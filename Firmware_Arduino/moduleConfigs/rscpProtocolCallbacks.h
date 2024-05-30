#ifndef __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
#define __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__

#include <Wire.h>
#include <stdint.h>

#include "../TaskBuffer.h"
#include "../rscpProtocol/rscpProtocol.h"


/**
 * @brief Callback function to get a received byte from the I2C bus.
 *
 * @param data Pointer to store the received byte.
 * @return 0 if a byte is available and received, -1 otherwise.
 */
static inline int8_t rscpGetRxByteCallback(uint8_t* data) {
    if (Wire.available()) {
        *data = Wire.read();
        return 0;
    }
    return -1;
}

/**
 * @brief Callback function to send a data slot over the I2C bus.
 *
 * @param data Pointer to the data to be sent.
 * @param length Length of the data to be sent.
 * @return 0 if the data is sent successfully, -1 otherwise.
 */
static inline int8_t rscpSendSlotCallback(uint8_t* data, uint8_t length) {
    return (Wire.write(data, length) == length) ? 0 : -1;
}

/**
 * @brief Callback function to handle waiting for incoming data on the I2C bus.
 */
static inline void rscpRxWaitingCallback(void) {
    yield(); // This also kicks the watchdog
}

/**
 * @brief Callback function to calculate the CRC algorithm for data.
 *
 * @param data Pointer to the data for CRC calculation.
 * @param length Length of the data.
 * @return Calculated CRC value.
 */
static inline uint16_t rscpGetCrcCallback(uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc  ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Callback function to set the shutter action.
 *
 * @param arg Pointer to the shutter action argument.
 * @return RSCP error code.
 */
static inline int8_t rscpSetShutterActionCallback(RSCP_Arg_rollershutter* arg) {
    if (pushTaskToBuffer(
                RSCP_CMD_SET_SHUTTER_ACTION,
                (uint8_t*)&arg[0],
                sizeof(struct RSCP_Arg_rollershutter))
        < 0) {
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

/**
 * @brief Callback function to set the shutter position
 *
 * @param arg Pointer to the shutter position argument.
 * @return RSCP error code.
 */
static inline int8_t rscpSetShutterPositionCallback(RSCP_Arg_rollershutterposition* arg) {
    (void)arg;
    return RSCP_ERR_NOT_SUPPORTED;
}

/**
 * @brief Callback function to get the shutter position
 *
 * @param reply Pointer to the shutter position reply.
 */
static inline void rscpGetShutterPositionCallback(RSCP_Reply_rollershutterposition* reply) {
    (void)reply;
    // TODO: Implement
}

/**
 * @brief Callback function to set the switch relay.
 *
 * @param arg Pointer to the switch relay argument.
 * @return RSCP error code.
 */
static inline int8_t rscpSetSwitchRelayCallback(RSCP_Arg_switchrelay* arg) {
    if (pushTaskToBuffer(
                RSCP_CMD_SET_SWITCH_RELAY,
                (uint8_t*)&arg[0],
                sizeof(struct RSCP_Arg_switchrelay))
        < 0) {
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

/**
 * @brief Callback function to get the switch relay status.
 *
 * @param reply Pointer to the switch relay reply.
 */
static inline void rscpGetSwitchRelayCallback(RSCP_Reply_switchrelay* reply) {
    reply->status = (digitalRead(PIN_RELAY) == HIGH) ? RSCP_DEF_SWITCH_RELAY_ON
                                                     : RSCP_DEF_SWITCH_RELAY_OFF;
}

/**
 * @brief Callback function to get the switch relay status.
 *
 * @param reply Pointer to the switch relay reply.
 */
static inline void rscpGetSwitchButtonCallback(RSCP_Reply_switchbutton* reply) {
    reply->status = (digitalRead(PIN_BUTTON_USER) == HIGH) ? RSCP_DEF_SWITCH_BUTTON_ON
                                                           : RSCP_DEF_SWITCH_BUTTON_OFF;
}

/**
 * @brief Callback function to set the buzzer action.
 *
 * @param arg Pointer to the buzzer action argument.
 * @return RSCP error code.
 */
static inline int8_t rscpSetBuzzerActionCallback(RSCP_Arg_buzzer_action* arg) {
    if (pushTaskToBuffer(
                RSCP_CMD_SET_BUZZER_ACTION,
                (uint8_t*)&arg[0],
                sizeof(struct RSCP_Arg_buzzer_action))
        < 0) {
        return RSCP_ERR_TASK_BUFFER_FULL;
    }
    return RSCP_ERR_OK;
}

#endif // __RSCP_PROTOCOL_CALLBACKS_H_INCLUDED__
