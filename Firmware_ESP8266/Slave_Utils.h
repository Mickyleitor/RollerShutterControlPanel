#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "error.h"
#include "basic_defines.h"

#include "rscpProtocol/rscpProtocol.h"

#define SLAVE_I2C_ADDRESS                                                   (0x08)

void sendCommandToSlave(char type) {
  // Wire.beginTransmission(SLAVE_I2C_ADDRESS); // transmit to device #8
  // Wire.write(1);        // This should wake up the device
  // Wire.write(type);        // sends five bytes
  // Wire.endTransmission();    // stop transmitting
}

void sendRollerCommand(int persiana, int comando) {
  char type = 1 << (persiana + 2) | (0x3  & comando);
  sendCommandToSlave(type);
}

void checkSlaveConnection(LiquidCrystal_PCF8574 * mylcd) {
    struct RSCP_Reply_cpuquery reply;
    int8_t err = rscpRequestCPUQuery(&reply, 1000);
    Serial.println("rscpRequestCPUQuery: " + String(err));
    if ( err != RSCP_ERR_OK) {
        errorHandler(mylcd, FATAL_ERROR_CODE_NO_SLAVE_HARDWARE);
    }

    Serial.println("reply.flags: " + String(reply.flags));
    Serial.println("reply.crcType: " + String(reply.crcType));
    Serial.println("reply.protocolversion: " + String(reply.protocolversion));
    Serial.println("reply.cpuType: " + String(reply.cpuType));
    Serial.println("reply.swversion: " + String(reply.swversion));
    Serial.println("reply.packetMaxLen: " + String(reply.packetMaxLen));

    // Just check crcType and protocolversion
    if ((reply.crcType != RSCP_DEF_CRC_TYPE_MODBUS16) || 
        (reply.protocolversion != RSCP_DEF_PROTOCOL_VERSION)) {
        errorHandler(mylcd, FATAL_ERROR_CODE_INVALID_SLAVE_HARDWARE);
    }
    struct RSCP_Arg_buzzer_action buzzerAction;
    buzzerAction.action = RSCP_DEF_BUZZER_ACTION_ON;
    buzzerAction.volume = 5000;
    buzzerAction.duration_ms = 500;
    for (int i = 0; i < 3; i++) {
      rscpSendBuzzerAction(&buzzerAction, 1000);
      delay(1000);
      yield();
    }
}