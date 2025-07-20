#pragma once

#include <Wire.h>

#include "basic_defines.h"
#include "error.h"
#include "rscpProtocol/rscpProtocol.h"

void checkSlaveConnection() {
    struct RSCP_Reply_cpuquery cpureply;
    int8_t err = rscpRequestData(
            RSCP_CMD_CPU_QUERY,
            (uint8_t*)&cpureply,
            sizeof(cpureply),
            RSCP_TIMEOUT_MS);
    Serial.println("rscpRequestCPUQuery: " + String(err));

    if (err != RSCP_ERR_OK) {
        errorHandler(FATAL_ERROR_CODE_NO_SLAVE_HARDWARE);
    }

    Serial.println("reply.flags: " + String(cpureply.flags));
    Serial.println("reply.crcType: " + String(cpureply.crcType));
    Serial.println("reply.protocolversion: " + String(cpureply.protocolversion));
    Serial.println("reply.cpuType: " + String(cpureply.cpuType));
    Serial.println("reply.swversion: " + String(cpureply.swversion));
    Serial.println("reply.packetMaxLen: " + String(cpureply.packetMaxLen));

    // Just check crcType and protocolversion
    if ((cpureply.crcType != RSCP_DEF_CRC_TYPE_MODBUS16)
        || (cpureply.protocolversion != RSCP_DEF_PROTOCOL_VERSION)) {
        errorHandler(FATAL_ERROR_CODE_INVALID_SLAVE_HARDWARE);
    }
}
