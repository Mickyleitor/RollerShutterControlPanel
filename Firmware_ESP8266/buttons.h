#pragma once
////////////////////////////////////////////////////////////////////////////////
//! \file button.h
///
///  \brief  API for functions/variables related to buttons interactions
///
///  \author MickySim: https://www.mickysim.com
///
///  Copyright (c) 2023 Micky Simulation All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//---[ Includes ]---------------------------------------------------------------

#include <Ticker.h>

#include "basic_defines.h"

//---[ Macros ]-----------------------------------------------------------------

#define BUTTONS_TIMEOUT_SECONDS                                             (10)
#define BUTTON_DEBOUNCE_TIME_MS                                            (100)
#define BUTTON_LEFT_PIN_NR                                                   (4)
#define BUTTON_DOWN_PIN_NR                                                   (5)
#define BUTTON_UP_PIN_NR                                                    (12)
#define BUTTON_RIGHT_PIN_NR                                                 (13)

//---[ Constants ]--------------------------------------------------------------

//---[ Types ]------------------------------------------------------------------

static Ticker ButtonsDebounceTask, ButtonsTimeOutTask;
extern void systemStateGoToIdle(void);

//---[ Public Variables ]-------------------------------------------------------

enum ButtonStatus _currentButton = BUTTON_STATUS_NONE;

//---[ Public Function Prototypes ]---------------------------------------------

//---[ Public static inline functions ]-----------------------------------------

static inline void onButtonTimeout(void) {
    ButtonsTimeOutTask.detach();
    systemStateGoToIdle();
}

static inline void ButtonsISRFunction(void) {
    _currentButton = BUTTON_STATUS_NONE;
    if (digitalRead(BUTTON_LEFT_PIN_NR) == LOW) {
        _currentButton = BUTTON_STATUS_LEFT;
    } else if (digitalRead(BUTTON_DOWN_PIN_NR) == LOW) {
        _currentButton = BUTTON_STATUS_DOWN;
    } else if (digitalRead(BUTTON_UP_PIN_NR) == LOW) {
        _currentButton = BUTTON_STATUS_UP;
    } else if (digitalRead(BUTTON_RIGHT_PIN_NR) == LOW) {
        _currentButton = BUTTON_STATUS_RIGHT;
    }
    ButtonsDebounceTask.detach();
}

void IRAM_ATTR isrButtons(void) {
    ButtonsDebounceTask.detach();
    ButtonsDebounceTask.attach_ms(BUTTON_DEBOUNCE_TIME_MS, ButtonsISRFunction);
}

static inline void initButtonsFunction(void) {
    pinMode(BUTTON_LEFT_PIN_NR, INPUT);
    pinMode(BUTTON_DOWN_PIN_NR, INPUT);
    pinMode(BUTTON_UP_PIN_NR, INPUT);
    pinMode(BUTTON_RIGHT_PIN_NR, INPUT);
    attachInterrupt(BUTTON_LEFT_PIN_NR, isrButtons, FALLING);
    attachInterrupt(BUTTON_DOWN_PIN_NR, isrButtons, FALLING);
    attachInterrupt(BUTTON_UP_PIN_NR, isrButtons, FALLING);
    attachInterrupt(BUTTON_RIGHT_PIN_NR, isrButtons, FALLING);
}

static inline int buttonPressed(void) {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '4' or c == 'g') {
            _currentButton = BUTTON_STATUS_LEFT;
            break;
        }
        if (c == '2' or c == 'h') {
            _currentButton = BUTTON_STATUS_DOWN;
            break;
        }
        if (c == '8' or c == 'j') {
            _currentButton = BUTTON_STATUS_UP;
            break;
        }
        if (c == '6' or c == 'k') {
            _currentButton = BUTTON_STATUS_RIGHT;
            break;
        }
    }
    if (_currentButton != BUTTON_STATUS_NONE) {
        ButtonsTimeOutTask.detach();
        ButtonsTimeOutTask.attach(BUTTONS_TIMEOUT_SECONDS, onButtonTimeout);
    }
    int buttonPress = _currentButton;
    _currentButton  = BUTTON_STATUS_NONE;
    return buttonPress;
}
