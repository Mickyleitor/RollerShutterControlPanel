#pragma once

#include <stdint.h>

#include "basic_defines.h"
#include "rscpProtocol/rscpProtocol.h"

#define BUZZER_VOLUME_MIN  (2600)
#define BUZZER_VOLUME_MAX  (5500)

enum BuzzerPending {
    BUZZER_PENDING_NONE  = 0,
    BUZZER_PENDING_BEEP  = 1,
    BUZZER_PENDING_BURST = 2,
};

typedef uint8_t BuzzerPending_t;

struct buzzerStatus_t {
    BuzzerPending_t pending;
    struct RSCP_Arg_buzzer_action rscpAction;
    uint32_t general_volume; // Volume in millivolts
    bool isEnabled;          // True if buzzer is enabled
    union {
        uint32_t beepDurationMs; // Duration in milliseconds for a single beep
        struct {
            uint32_t durationMs; // Duration in milliseconds for a burst of beeps
            uint32_t intervalMs; // Interval in milliseconds between beeps in a burst
            uint32_t count;      // Number of beeps in a burst
        } burst;
    };
} __attribute__((packed));

static buzzerStatus_t buzzerStatus = {
    .general_volume = DEFAULT_BUZZER_VOLUME,
    .isEnabled      = true,
};

void buzzer_schedule_beep_once(uint32_t durationMs) {
    // Schedule a beep
    buzzerStatus.beepDurationMs = durationMs;
    buzzerStatus.pending        = BUZZER_PENDING_BEEP;
}

void buzzer_schedule_burst(uint32_t durationMs, uint32_t intervalMs, uint32_t count) {
    // Schedule a burst of beeps
    buzzerStatus.burst.durationMs = durationMs;
    buzzerStatus.burst.intervalMs = intervalMs;
    buzzerStatus.burst.count      = count;
    buzzerStatus.pending          = BUZZER_PENDING_BURST;
}

void buzzer_beep_once(buzzerStatus_t* status, uint32_t durationMs) {
    status->rscpAction.action      = RSCP_DEF_BUZZER_ACTION_ON;
    status->rscpAction.volume      = status->general_volume;
    status->rscpAction.duration_ms = durationMs;
    rscpSendAction(
            RSCP_CMD_SET_BUZZER_ACTION,
            (uint8_t*)&status->rscpAction,
            sizeof(status->rscpAction),
            RSCP_TIMEOUT_MS);
}

void buzzer_handler() {
    if (!buzzerStatus.isEnabled) {
        return; // Buzzer is disabled, do nothing
    }
    switch (buzzerStatus.pending) {
        case BUZZER_PENDING_BEEP:
            buzzerStatus.pending = BUZZER_PENDING_NONE;
            buzzer_beep_once(&buzzerStatus, buzzerStatus.beepDurationMs);
            break;
        case BUZZER_PENDING_BURST:
            if (buzzerStatus.burst.count > 0) {
                static unsigned long lastBurstMillis = 0;
                unsigned long now                    = millis();
                unsigned long nextBurstMillis
                        = (buzzerStatus.burst.intervalMs + buzzerStatus.burst.durationMs);

                if (now - lastBurstMillis >= nextBurstMillis) {
                    lastBurstMillis = now;
                    buzzer_beep_once(&buzzerStatus, buzzerStatus.burst.durationMs);
                    buzzerStatus.burst.count--;
                }
            } else {
                buzzerStatus.pending = BUZZER_PENDING_NONE;
            }
            break;
    }
}

// Sound when a button is pressed
void buzzer_sound_button() { buzzer_schedule_beep_once(50); }

// Sound when an action is accepted
void buzzer_sound_accept() { buzzer_schedule_beep_once(400); }

// Sound when an action is rejected
void buzzer_sound_error() { buzzer_schedule_burst(50, 30, 2); }

// Warning sound for events like shutter limits reached
void buzzer_sound_warning() { buzzer_schedule_burst(50, 100, 3); }

void buzzer_set_volume(uint32_t volume) { buzzerStatus.general_volume = volume; }

uint32_t buzzer_get_volume() { return buzzerStatus.general_volume; }

uint32_t buzzer_get_volume_min() { return BUZZER_VOLUME_MIN; }

uint32_t buzzer_get_volume_max() { return BUZZER_VOLUME_MAX; }

void buzzer_enable() { buzzerStatus.isEnabled = true; }

void buzzer_disable() { buzzerStatus.isEnabled = false; }

bool buzzer_is_enabled() { return buzzerStatus.isEnabled; }

void buzzer_init() {
    buzzerStatus.isEnabled      = settings.buzzerSettings.isEnabled;
    buzzerStatus.general_volume = settings.buzzerSettings.general_volume;
}

void buzzer_store_settings() {
    settings.buzzerSettings.isEnabled      = buzzerStatus.isEnabled;
    settings.buzzerSettings.general_volume = buzzerStatus.general_volume;
    EEPROM_Write(&settings);
}
