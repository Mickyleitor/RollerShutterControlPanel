#pragma once

#include <Ticker.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"
#include "rscpProtocol/rscpProtocol.h"

#define SHUTTER_MAX_DURATION_SECONDS                                        (27)
#define SHUTTER_CHECK_LIMITS_INTERVAL_SECONDS                                (5)

Ticker ShutterFunctionTask;
extern struct ShutterParameters ShutterData[];

void ShutterFunctionManagerISR() {
    ShutterFunctionTask.detach();

    unsigned long currentMillis = millis();

    // Check Roller Shutter status
    for (int index = 0; index < NUMBER_OF_SHUTTERS; index++) {
        // Is this roller(supposedly) not stopped?
        if (ShutterData[index].status != SHUTTER_STATUS_STOPPED) {
            // Is the total duration of the movement time out for this roller?
            int ShutterDuration = (currentMillis > ShutterData[index].lastUpdate)
                                        ? (currentMillis - ShutterData[index].lastUpdate)
                                        : (ShutterData[index].lastUpdate - currentMillis);
            if (abs(ShutterDuration) > SHUTTER_MAX_DURATION_SECONDS) {
                // This roller is for sure reached the limit.
                ShutterData[index].status = SHUTTER_STATUS_STOPPED;
            } else {
                // Check again in a few seconds
                ShutterFunctionTask.attach(
                        SHUTTER_CHECK_LIMITS_INTERVAL_SECONDS,
                        ShutterFunctionManagerISR);
            }
        }
    }
}

void subirPersiana(int persiana) {
    if (persiana < 0 || persiana >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(persiana));
        return;
    }
    ShutterData[persiana].status     = SHUTTER_STATUS_MOVING_UP;
    ShutterData[persiana].lastUpdate = millis();
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_UP;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);

    ShutterFunctionTask.detach();
    ShutterFunctionTask.attach(SHUTTER_MAX_DURATION_SECONDS, ShutterFunctionManagerISR);
}

void bajarPersiana(int persiana) {
    if (persiana < 0 || persiana >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(persiana));
        return;
    }
    ShutterData[persiana].status     = SHUTTER_STATUS_MOVING_DOWN;
    ShutterData[persiana].lastUpdate = millis();
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_DOWN;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);

    ShutterFunctionTask.detach();
    ShutterFunctionTask.attach(SHUTTER_MAX_DURATION_SECONDS, ShutterFunctionManagerISR);
}

void PararPersiana(int persiana) {
    if (persiana < 0 || persiana >= NUMBER_OF_SHUTTERS) {
        Serial.println("Invalid shutter index: " + String(persiana));
        return;
    }
    ShutterData[persiana].status = SHUTTER_STATUS_STOPPED;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_STOP;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}
