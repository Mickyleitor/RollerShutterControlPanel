#pragma once

#include <Ticker.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"
#include "rscpProtocol/rscpProtocol.h"

extern Ticker SystemFunctionTask;
extern enum SystemState _SystemState;
extern uint8_t _seleccionMenu;
extern struct ShutterParameters ShutterData[];

void SystemFunctionManagerISR() {
    SystemFunctionTask.detach();
    SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS, SystemFunctionManagerISR);

    // Check Roller Shutter status
    for (int index = 0; index < 3; index++) {
        // Is this roller(supposedly) not stopped?
        if (ShutterData[index].status != SHUTTER_STATUS_STOPPED) {
            // Is the total duration of the movement time out for this roller?
            int ShutterDuration = millis() - ShutterData[index].lastUpdate;
            if (abs(ShutterDuration) > SHUTTER_DURATION_SECONDS) {
                // This roller is for sure reached the limit.
                ShutterData[index].status = SHUTTER_STATUS_STOPPED;
            } else {
                // Check again next time
                SystemFunctionTask.detach();
                SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManagerISR);
            }
        }
    }
    Serial.println("System Manager updated");
}

void subirPersiana(int persiana) {
    ShutterData[persiana].status     = SHUTTER_STATUS_MOVING_UP;
    ShutterData[persiana].lastUpdate = millis();
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_UP;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);

    SystemFunctionTask.detach();
    SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManagerISR);
}

void bajarPersiana(int persiana) {
    ShutterData[persiana].status     = SHUTTER_STATUS_MOVING_DOWN;
    ShutterData[persiana].lastUpdate = millis();
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_DOWN;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);

    SystemFunctionTask.detach();
    SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManagerISR);
}

void PararPersiana(int persiana) {
    ShutterData[persiana].status = SHUTTER_STATUS_STOPPED;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_STOP;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}
