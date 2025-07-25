#pragma once

#include <stdint.h>

#include "rscpProtocol/rscpProtocol.h"

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
    .general_volume = 500, // Default volume in millivolts
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
