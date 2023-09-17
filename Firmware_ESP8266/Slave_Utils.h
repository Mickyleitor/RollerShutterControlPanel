#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "error.h"
#include "basic_defines.h"

#include "rscpProtocol/rscpProtocol.h"

void checkSlaveConnection(LiquidCrystal_PCF8574 * mylcd) {
    struct RSCP_Reply_cpuquery cpureply;
    int8_t err = rscpRequestData(RSCP_CMD_CPU_QUERY, (uint8_t *)&cpureply, sizeof(cpureply), RSCP_TIMEOUT_MS);
    Serial.println("rscpRequestCPUQuery: " + String(err));

    if ( err != RSCP_ERR_OK) {
        errorHandler(mylcd, FATAL_ERROR_CODE_NO_SLAVE_HARDWARE);
    }

    Serial.println("reply.flags: " + String(cpureply.flags));
    Serial.println("reply.crcType: " + String(cpureply.crcType));
    Serial.println("reply.protocolversion: " + String(cpureply.protocolversion));
    Serial.println("reply.cpuType: " + String(cpureply.cpuType));
    Serial.println("reply.swversion: " + String(cpureply.swversion));
    Serial.println("reply.packetMaxLen: " + String(cpureply.packetMaxLen));

    // Just check crcType and protocolversion
    if ((cpureply.crcType != RSCP_DEF_CRC_TYPE_MODBUS16) || 
        (cpureply.protocolversion != RSCP_DEF_PROTOCOL_VERSION)) {
        errorHandler(mylcd, FATAL_ERROR_CODE_INVALID_SLAVE_HARDWARE);
    }
    struct RSCP_Arg_buzzer_action buzzerAction;
    buzzerAction.action = RSCP_DEF_BUZZER_ACTION_ON;
    buzzerAction.volume = 500;
    buzzerAction.duration_ms = 100;
    for (int i = 0; i < 3; i++) {
      int8_t err = rscpSendAction(RSCP_CMD_SET_BUZZER_ACTION, (uint8_t *)&buzzerAction, sizeof(buzzerAction), RSCP_TIMEOUT_MS);
      Serial.println("rscpSendBuzzerAction: " + String(err));
      delay(200);
      yield();
    }

    for (int i = 0 ; i < 4 ; i++) {
      struct RSCP_Reply_switchrelay switchRelayReply;
      err = rscpRequestData(RSCP_CMD_GET_SWITCH_RELAY, (uint8_t *)&switchRelayReply, sizeof(switchRelayReply), RSCP_TIMEOUT_MS);
      Serial.println("rscpRequestSwitchRelay: " + String(err));
      Serial.println("switchRelay.status: " + String(switchRelayReply.status));

      struct RSCP_Arg_switchrelay switchRelayArg;
      switchRelayArg.status = (switchRelayReply.status == RSCP_DEF_SWITCH_RELAY_ON) ? RSCP_DEF_SWITCH_RELAY_OFF : RSCP_DEF_SWITCH_RELAY_ON;
      err = rscpSendAction(RSCP_CMD_SET_SWITCH_RELAY, (uint8_t *)&switchRelayArg, sizeof(switchRelayArg), RSCP_TIMEOUT_MS);
      Serial.println("rscpSendSwitchRelay: " + String(err));
      delay(1000);
      yield();
    }
}