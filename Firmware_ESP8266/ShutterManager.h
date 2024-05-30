#pragma once

#include <Ticker.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"
#include "rscpProtocol/rscpProtocol.h"


#define SCHEDULED_DATA_FLAG_PENDING                                       (0x01)
#define SCHEDULED_DATA_FLAG_DONE                                          (0x02)

extern Ticker SystemFunctionTask;
extern enum SystemState _SystemState;
extern uint8_t _seleccionMenu;
extern struct ShutterParameters ShutterData[];

// Replace clearAllDoneFlagNotNeeded by a meaningful name
static bool isDoneFlagClearedOnNewYear = false;

void setScheduledDataFlag(int dia, int mes, int mask, int value) {
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12) {
        return;
    }
    settings.ScheduledData[mes - 1][dia - 1] &= ~mask;
    settings.ScheduledData[mes - 1][dia - 1] |= value;
    EEPROM_Write(&settings);
}

void setAllScheduledDataFlag(int mask, int value) {
    for (int index_mes = 0; index_mes < 12; index_mes++) {
        for (int index_dia = 0; index_dia < 31; index_dia++) {
            settings.ScheduledData[index_mes][index_dia] &= ~mask;
            settings.ScheduledData[index_mes][index_dia] |= value;
        }
    }
    EEPROM_Write(&settings);
}

void clearAllDoneFlagAtNewYear(bool isNewYear) {
    if (isNewYear && !isDoneFlagClearedOnNewYear) {
        setAllScheduledDataFlag(SCHEDULED_DATA_FLAG_DONE, 0);
        isDoneFlagClearedOnNewYear = true;
    } else if (!isNewYear && isDoneFlagClearedOnNewYear) {
        isDoneFlagClearedOnNewYear = false;
    }
}

void SystemFunctionManager() {
    SystemFunctionTask.detach();
    SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS, SystemFunctionManager);

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
                SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManager);
            }
        }
    }
    // Check if time is ahead from sunrise time.
    time_t nowSecondsUTC = time(NULL) + MyWeather.timezoneshift;
    if ((nowSecondsUTC % (60 * 60 * 24))
        >= (MyWeather.sunriseSecondsUTC + MyWeather.timezoneshift)) {
        struct tm* timenow;
        timenow = gmtime(&nowSecondsUTC);
        // Check if a reset of the data is needed at new year
        clearAllDoneFlagAtNewYear((timenow->tm_year) == 0);
        // Check if today there is a scheduled task and isn't done yet.
        if (settings.ScheduledData[timenow->tm_mon][timenow->tm_mday - 1]
            == SCHEDULED_DATA_FLAG_PENDING) {
            // Switch to true the notification flag
            settings.ScheduledData[timenow->tm_mon][timenow->tm_mday - 1]
                    |= SCHEDULED_DATA_FLAG_DONE;
            _SystemState = SYSTEM_STATE_ACTION_SLEEP;
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
    SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManager);
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
    SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS, SystemFunctionManager);
}

void PararPersiana(int persiana) {
    ShutterData[persiana].status = SHUTTER_STATUS_STOPPED;
    struct RSCP_Arg_rollershutter arg;
    arg.action  = RSCP_DEF_SHUTTER_ACTION_STOP;
    arg.shutter = persiana;
    arg.retries = 3;
    (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t*)&arg, sizeof(arg), 1000);
}
