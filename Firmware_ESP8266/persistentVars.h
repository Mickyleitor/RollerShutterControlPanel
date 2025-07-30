#pragma once

/**
 * @file persistentVars.h
 * @brief API for managing persistent variables in no-init RAM.
 *        This file provides functions to store and retrieve data that needs to persist across
 *        resets (note that this is not the same as EEPROM or flash storage) but doesn't survive
 *        power loss.
 *
 * This implementation uses the RTC user memory area of the ESP8266 to store data that
 * persists across soft resets. The data is stored in a specific format to ensure that it can
 * be reliably retrieved after a reset.
 *
 * The data structure is defined as follows:
 * - `softResetCount`: A counter that increments each time the system is reset.
 * - `rtcTime`: A structure that holds the current time and timezone shift.
 * - `magicWord`: A magic word to verify that the data is valid.
 */

#include <Arduino.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define PERSISTENTVARS_USER_MEMORY_MAGIC_WORD 0xCAFEDEAD

struct rtcTime_t {
    time_t myTime;
    long timezoneShift;
};

static_assert(sizeof(rtcTime_t) % 4 == 0, "rtcTime_t must be aligned to 4 bytes");

struct persistentVars_t {
    uint32_t softResetCount;
    struct rtcTime_t rtcTime;
    uint32_t magicWord;
};

static_assert(sizeof(persistentVars_t) % 4 == 0, "persistentVars_t must be aligned to 4 bytes");

static struct persistentVars_t persistentVars_data;

bool persistentVars_is_stored(void) {
    return (ESP.rtcUserMemoryRead(0, (uint32_t*)&persistentVars_data, sizeof(persistentVars_data))
            && persistentVars_data.magicWord == PERSISTENTVARS_USER_MEMORY_MAGIC_WORD);
}

void persistentVars_store_softResetCount(uint32_t softResetCount) {
    persistentVars_data.magicWord      = PERSISTENTVARS_USER_MEMORY_MAGIC_WORD;
    persistentVars_data.softResetCount = softResetCount;

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&persistentVars_data, sizeof(persistentVars_data));
}

void persistentVars_clear_all(void) {
    memset(&persistentVars_data, 0, sizeof(persistentVars_data));

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&persistentVars_data, sizeof(persistentVars_data));
}

uint32_t persistentVars_get_softResetCount(void) { return persistentVars_data.softResetCount; }

void persistentVars_store_rtcTime(const struct rtcTime_t* rtcTime) {
    if (rtcTime == nullptr) {
        return;
    }

    memcpy(&persistentVars_data.rtcTime, rtcTime, sizeof(struct rtcTime_t));
    persistentVars_data.magicWord = PERSISTENTVARS_USER_MEMORY_MAGIC_WORD;

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&persistentVars_data, sizeof(persistentVars_data));
}

struct rtcTime_t persistentVars_get_rtcTime(void) { return persistentVars_data.rtcTime; }

// This function initializes the persistent variables by reading them from no-init RAM.
void persistentVars_init(void) {
    // This will just call persistentVars_is_stored() which will read the data from no-init RAM.
    if (!persistentVars_is_stored()) {
        // If no data is stored because magic word is invalid, we clear all persistent variables.
        // This is to avoid using uninitialized or random data. This can happen if the
        // device has been updated via OTA.
        persistentVars_clear_all();
    }
    // Increment the soft reset count and store the updated data back to no-init RAM.
    persistentVars_store_softResetCount(persistentVars_get_softResetCount() + 1);
}
