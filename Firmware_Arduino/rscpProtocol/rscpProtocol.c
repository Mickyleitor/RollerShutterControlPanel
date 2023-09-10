/*! \file **********************************************************************
 *
 *  \brief  Roller Shutter Control Panel Protocol (RSCP) for i2c communication
 *  This protocol is used to communicate between the main CPU and radio module
 *
 *  \author MickySim: https://www.mickysim.com
 *
 *  Copyright (c) 2023 MickySim All rights reserved.
 ******************************************************************************/

//---[ Includes ]---------------------------------------------------------------

#include "stdint.h"
#include "stdbool.h"

#include "rscpProtocol.h"

#include "../moduleConfigs/rscpProtocolCallbacks.h"

//---[ Macros ]-----------------------------------------------------------------

//---[ Constants ]--------------------------------------------------------------

//---[ Types ]------------------------------------------------------------------

//---[ Private Variables ]------------------------------------------------------

//---[ Public Variables ]-------------------------------------------------------

//---[ Private Functions ]------------------------------------------------------

//---[ Public Functions ]-------------------------------------------------------

RSCP_ErrorType rscpHandleCPUQuery(void){
    RSCP_ErrorType err = RSCP_ERR_OK;
    uint32_t txBufferIndex = 0;
    uint8_t txBuffer[RSCP_MAX_TX_BUFFER_SIZE];

    // Fill reply
    struct RSCP_Reply_cpuquery reply;
    reply.flags = 0;
    reply.crcType = RSCP_DEF_CRC_TYPE_MODBUS16;
    reply.protocolversion = RSCP_DEF_PROTOCOL_VERSION;
    reply.cpuType = RSCP_DEF_CPU_TYPE_ATMEGA328P_8MHZ;
    reply.swversion = RSCP_DEF_SWVERSION_VERSION;
    reply.packetMaxLen = sizeof(struct RSCP_frame);
    
    // Fill txBuffer
    txBuffer[txBufferIndex++] = RSCP_PREAMBLE_BYTE;
    txBuffer[txBufferIndex++] = 2 + sizeof(struct RSCP_Reply_cpuquery);
    txBuffer[txBufferIndex++] = RSCP_CMD_CPU_QUERY;

    // Use memcpy to copy struct data to txBuffer
    memcpy(&txBuffer[txBufferIndex], &reply, sizeof(struct RSCP_Reply_cpuquery));
    txBufferIndex += sizeof(struct RSCP_Reply_cpuquery);

    uint16_t crc = rscpGetCrcCallback(&txBuffer[1], txBuffer[1]);
    txBuffer[txBufferIndex++] = (crc >> 8);
    txBuffer[txBufferIndex++] = (crc & 0xFF);

    Serial.print("txBuffer : ");
    for (uint32_t i = 0; i < txBufferIndex; i++) {
        Serial.print(txBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    if ( (rscpSendSlotCallback(txBuffer, txBufferIndex)) < 0){
        err = RSCP_ERR_TX_BUFFER_FULL;
    }
    return err;
}

RSCP_ErrorType rscpSetShutterAction(struct RSCP_frame * frame){
    struct RSCP_Arg_rollershutter * data = (struct RSCP_Arg_rollershutter *)frame->data;
    (void)rscpSetShutterActionCallback(data);
    // TODO: Answer?
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpSetShutterPosition(struct RSCP_frame * frame){
    struct RSCP_Arg_rollershutterposition * data = (struct RSCP_Arg_rollershutterposition *)frame->data;
    (void)rscpSetShutterPositionCallback(data);
    // TODO: Answer?
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpGetShutterPosition(void){
    uint32_t txBufferIndex = 0;
    uint8_t txBuffer[RSCP_MAX_TX_BUFFER_SIZE];

    struct RSCP_Reply_rollershutterposition reply;
    (void)rscpGetShutterPositionCallback(&reply);
    txBuffer[txBufferIndex++] = RSCP_PREAMBLE_BYTE;
    txBuffer[txBufferIndex++] = sizeof(struct RSCP_Reply_rollershutterposition) + 2;
    txBuffer[txBufferIndex++] = RSCP_CMD_GET_SHUTTER_POSITION;

    // Use memcpy to copy struct data to txBuffer
    memcpy(&txBuffer[txBufferIndex], &reply, sizeof(struct RSCP_Reply_rollershutterposition));
    txBufferIndex += sizeof(struct RSCP_Reply_rollershutterposition);

    uint16_t crc = rscpGetCrcCallback(&txBuffer[1], txBuffer[1]);
    txBuffer[txBufferIndex++] = (crc >> 8);
    txBuffer[txBufferIndex++] = (crc & 0xFF);
    (void)txBuffer;
    // TODO: Send txBuffer
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpSetSwitchRelay(struct RSCP_frame * frame){
    struct RSCP_Arg_switchrelay * data = (struct RSCP_Arg_switchrelay *)frame->data;
    (void)rscpSetSwitchRelayCallback(data);
    // TODO: Answer?
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpGetSwitchRelay(void){
    uint32_t txBufferIndex = 0;
    uint8_t txBuffer[RSCP_MAX_TX_BUFFER_SIZE];

    struct RSCP_Reply_switchrelay reply;
    rscpGetSwitchRelayCallback(&reply);
    txBuffer[txBufferIndex++] = RSCP_PREAMBLE_BYTE;
    txBuffer[txBufferIndex++] = sizeof(struct RSCP_Reply_switchrelay) + 2;
    txBuffer[txBufferIndex++] = RSCP_CMD_GET_SWITCH_RELAY;

    // Use memcpy to copy struct data to txBuffer
    memcpy(&txBuffer[txBufferIndex], &reply, sizeof(struct RSCP_Reply_switchrelay));
    txBufferIndex += sizeof(struct RSCP_Reply_switchrelay);

    uint16_t crc = rscpGetCrcCallback(&txBuffer[1], txBuffer[1]);
    txBuffer[txBufferIndex++] = (crc >> 8);
    txBuffer[txBufferIndex++] = (crc & 0xFF);
    (void)txBuffer;
    // TODO: Send txBuffer
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpSetBuzzerVolume(struct RSCP_frame * frame){
    struct RSCP_Arg_buzzer_volume * data = (struct RSCP_Arg_buzzer_volume *)frame->data;
    (void)rscpSetBuzzerVolumeCallback(data);
    // TODO: Answer?
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpSetBuzzerAction(struct RSCP_frame * frame){
    struct RSCP_Arg_buzzer_action * data = (struct RSCP_Arg_buzzer_action *)frame->data;
    (void)rscpSetBuzzerActionCallback(data);
    return RSCP_ERR_OK;
}

int32_t rscpGetRxByteBlocking(uint8_t * readByte, uint32_t timeout_ticks){
    while(rscpGetRxByteCallback(readByte) < 0){
        if ( timeout_ticks-- == 0){
            return -1;
        }
        yield();
    }
    return 0;
}

RSCP_ErrorType rscpSendFail(void){
    return RSCP_ERR_NOT_SUPPORTED;
}

RSCP_ErrorType rscpGetMsg(struct RSCP_frame * frame, uint32_t timeout_ticks){
    uint32_t bufferIndex = 0;
    uint8_t rscpRxStatus = 0;
    uint8_t readByte;
    while(bufferIndex < sizeof(frame->data)){
        if(rscpGetRxByteBlocking(&readByte, timeout_ticks) < 0){
            return RSCP_ERR_TIMEOUT;
        }
        switch (rscpRxStatus){
            case 0 : // Waiting for length byte
                if(readByte != RSCP_PREAMBLE_BYTE){
                    frame->length = readByte;
                    rscpRxStatus = 1;
                }
                break;
            case 1 : // Waiting for command byte
                frame->command = readByte;
                rscpRxStatus = 2;
                break;
            case 2 : // Waiting for data bytes
                frame->data[bufferIndex++] = readByte;
                // Retrieve CRC by the current buffer index
                if(bufferIndex >= (uint32_t)(frame->length - sizeof(frame->crc))){
                    rscpRxStatus = 3;
                }
                break;
            case 3 : // Waiting for CRC high byte
                frame->crc = (readByte << 8);
                rscpRxStatus = 4;
                break;
            case 4 : // Waiting for CRC low byte
                frame->crc |= readByte;
                return RSCP_ERR_OK;
        }
    }
    return RSCP_ERR_OVERFLOW;
}

RSCP_ErrorType rscpHandle(uint32_t timeout_ticks){
    RSCP_ErrorType err = RSCP_ERR_OK;
    struct RSCP_frame frame;
    memset(&frame, 0, sizeof(frame));

    if( (err = rscpGetMsg(&frame, timeout_ticks)) != RSCP_ERR_OK){
        return err;
    }
    
    if(rscpGetCrcCallback(((uint8_t *)&frame), frame.length) != frame.crc){
        return RSCP_ERR_MALFORMED;
    }

    switch (frame.command){
        case RSCP_CMD_CPU_QUERY:
            return rscpHandleCPUQuery();
        case RSCP_CMD_SET_SHUTTER_ACTION:
            return rscpSetShutterAction(&frame);
        case RSCP_CMD_SET_SHUTTER_POSITION:
            return rscpSetShutterPosition(&frame);
        case RSCP_CMD_GET_SHUTTER_POSITION:
            return rscpGetShutterPosition();
        case RSCP_CMD_SET_SWITCH_RELAY:
            return rscpSetSwitchRelay(&frame);
        case RSCP_CMD_GET_SWITCH_RELAY:
            return rscpGetSwitchRelay();
        case RSCP_CMD_SET_BUZZER_VOLUME:
            return rscpSetBuzzerVolume(&frame);
        case RSCP_CMD_SET_BUZZER_ACTION:
            return rscpSetBuzzerAction(&frame);
        default:
            // Send failure
            (void)rscpSendFail();
            return RSCP_ERR_NOT_SUPPORTED;
    }
}