#pragma once

/**
 * rtcTime — persistent time across soft resets using no‑init RAM only.
 *
 * We deliberately dropped any EEPROM/flash backup.
 * • ESP8266 RTC drifts ~3 min every month; flash‐stored timestamps go stale too fast.
 * • Frequent flash writes accelerate wear.
 *
 * On boot we restore from noninit RAM if valid.
 * A periodic handler clears that RAM if we go long enough without an RTC sync.
 * Otherwise, we store the current time into noninit RAM periodically.
 */

#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <user_interface.h> // for rst_info

#include "ESP8266_Utils.h"

#define RTC_USER_MEMORY_MAGIC_WORD 0xCAFEDEAD

// Maximum tolerated drift (in seconds) when restoring time from no‑init RAM after a software
// reset. Prevents the system clock from jumping to an overly stale timestamp if no RTC sync has
// occurred after a software reset for such a long time.
#define RTC_HANDLER_MAX_DRIFT_SECONDS (60 * 60 * 24 * 30) // 1 month

// Interval to store the current time into no-init RAM. This is used to ensure that the time is
// stored periodically, so that if the system is reset, we have a recent timestamp to restore.
#define RTC_HANDLER_STORE_INTERVAL_SECONDS (60 * 60) // 1 hour

struct rtcTime_t {
    time_t myTime;
    uint32_t magicWord;
};
static_assert(sizeof(rtcTime_t) % 4 == 0, "rtcTime_t must be aligned to 4 bytes");

static struct rtcTime_t rtcTime_data;
static unsigned long lastRtcSyncMillis  = 0;
static unsigned long lastRtcStoreMillis = 0;
static bool rtc_time_initialized        = false;

bool rtc_is_initialized() { return rtc_time_initialized; }

bool rtc_drift_exceeded() {
    return ((millis() - lastRtcSyncMillis) >= (RTC_HANDLER_MAX_DRIFT_SECONDS * 1000UL));
}

bool rtc_on_sync() { return (rtc_is_initialized() && !rtc_drift_exceeded()); }

// This callback is called when the time is set by any source (e.g. NTP sync).
void rtc_is_set_cb() {
    rtc_time_initialized = true;
    lastRtcSyncMillis    = millis();
}

bool rtc_is_stored() {
    return (ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcTime_data, sizeof(rtcTime_data))
            && rtcTime_data.magicWord == RTC_USER_MEMORY_MAGIC_WORD);
}

bool rtc_should_store() {
    return ((millis() - lastRtcStoreMillis) >= (RTC_HANDLER_STORE_INTERVAL_SECONDS * 1000UL));
}

/// Called regularly; clears non-init RAM if no RTC sync occurred within threshold.
void rtc_handler(void) {
    if (rtc_drift_exceeded() && rtc_is_stored()) {
        // If the system clock has not been set by RTC for too long, we clear
        // the no-init RAM timestamp to prevent stale data from being used if
        // the system is reset again before RTC sync.
        memset(&rtcTime_data, 0, sizeof(rtcTime_data));
        ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtcTime_data, sizeof(rtcTime_data));

        // To avoid reading continuously the no-init RAM, we tick lastRtcSyncMillis
        // so we execute this code only once per RTC_HANDLER_MAX_DRIFT_SECONDS.
        lastRtcSyncMillis = millis();

        // To prevent showing the stale time on the LCD;
        rtc_time_initialized = false;
    } else if (rtc_is_stored() && rtc_should_store()) {
        // If the system clock is set, we store the current time into no-init RAM.
        // This is done periodically to ensure that we have a recent timestamp.
        rtcTime_data.myTime    = time(NULL);
        rtcTime_data.magicWord = RTC_USER_MEMORY_MAGIC_WORD;
        ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtcTime_data, sizeof(rtcTime_data));

        lastRtcStoreMillis = millis();
    }
}

/// Set system time and persist into non-init RAM.
void rtc_set(time_t newTime) {
    struct timeval tv;
    tv.tv_sec  = newTime;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

    rtcTime_data.myTime    = newTime;
    rtcTime_data.magicWord = RTC_USER_MEMORY_MAGIC_WORD;
    ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtcTime_data, sizeof(rtcTime_data));
}

/// On boot/soft reset; if valid data in no-init RAM, restore system clock.
void rtc_init() {
    // Initialize the rtc time system so it periodically tries to sync with NTP.
    // We want UTC time.
    int dst = 0;
    settimeofday_cb(rtc_is_set_cb);
    configTime(0, dst * 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");

    if (rtc_is_stored()) {
        struct timeval tv = { .tv_sec = rtcTime_data.myTime, .tv_usec = 0 };
        settimeofday(&tv, NULL);
    }
    // Note: EEPROM/flash fallback intentionally removed (see module header).
}
