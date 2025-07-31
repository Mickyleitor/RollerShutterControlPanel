#pragma once

#include "basic_defines.h"
#include "buzzer.h"
#include "rscpProtocol/rscpProtocol.h"

#define SHUTTER_MAX_DURATION_SECONDS                                        (27)

struct ShutterParameters ShutterData[NUMBER_OF_SHUTTERS];

void shutterHandler() {
    // This function is called periodically to check the status of the shutters
    unsigned long now = millis();

    // Check Roller Shutter status
    for (int index = 0; index < NUMBER_OF_SHUTTERS; index++) {
        // Is this roller(supposedly) not stopped?
        if (ShutterData[index].status != SHUTTER_STATUS_STOPPED) {
            // Is the total duration of the movement time out for this roller?
            unsigned long currentShutterDuration = now - ShutterData[index].lastUpdate;
            if (currentShutterDuration > (SHUTTER_MAX_DURATION_SECONDS * 1000UL)) {
                // This roller is for sure reached the limit.
                ShutterData[index].status = SHUTTER_STATUS_STOPPED;
                buzzer_sound_warning();
            }
        }
    }
}

ShutterStatus_t shutterGetStatus(int shutterIndex) {
    if (shutterIndex < 0 || shutterIndex >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(shutterIndex));
        return SHUTTER_STATUS_STOPPED;
    }
    return ShutterData[shutterIndex].status;
}

void shutterSubirPersiana(int shutterIndex) {
    if (shutterIndex < 0 || shutterIndex >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(shutterIndex));
        return;
    }
    ShutterData[shutterIndex].lastUpdate = millis();
    ShutterData[shutterIndex].status     = SHUTTER_STATUS_MOVING_UP;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_UP;
    arg.shutter = shutterIndex;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}

void shutterBajarPersiana(int shutterIndex) {
    if (shutterIndex < 0 || shutterIndex >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(shutterIndex));
        return;
    }
    ShutterData[shutterIndex].lastUpdate = millis();
    ShutterData[shutterIndex].status     = SHUTTER_STATUS_MOVING_DOWN;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_DOWN;
    arg.shutter = shutterIndex;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}

void shutterPararPersiana(int shutterIndex) {
    if (shutterIndex < 0 || shutterIndex >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(shutterIndex));
        return;
    }
    ShutterData[shutterIndex].status = SHUTTER_STATUS_STOPPED;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_STOP;
    arg.shutter = shutterIndex;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}
