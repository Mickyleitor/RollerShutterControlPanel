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

//---[ Macros ]-----------------------------------------------------------------

#define BUTTONS_TIMEOUT_SECONDS                                             (10)
#define BUTTON_DEBOUNCE_TIME_MS                                            (100)
#define BUTTON_LEFT_PIN_NR                                                   (4)
#define BUTTON_DOWN_PIN_NR                                                   (5)
#define BUTTON_UP_PIN_NR                                                    (12)
#define BUTTON_RIGHT_PIN_NR                                                 (13)

//---[ Constants ]--------------------------------------------------------------

enum ButtonInput {
  NONE,
  LEFT,
  UP,
  DOWN,
  RIGHT
};

//---[ Types ]------------------------------------------------------------------

extern Ticker TimeOutTask;
static Ticker ButtonsISRTask;
extern void goIdleState(void);

//---[ Public Variables ]-------------------------------------------------------

ButtonInput _currentButton = NONE;

//---[ Public Function Prototypes ]---------------------------------------------

//---[ Public static inline functions ]-----------------------------------------

static inline void ButtonsISRFunction(void) {
  _currentButton = NONE;
  if (digitalRead(BUTTON_LEFT_PIN_NR) == LOW) {
    _currentButton = LEFT;
  } else if (digitalRead(BUTTON_DOWN_PIN_NR) == LOW) {
    _currentButton = DOWN;
  } else if (digitalRead(BUTTON_UP_PIN_NR) == LOW) {
    _currentButton = UP;
  } else if (digitalRead(BUTTON_RIGHT_PIN_NR) == LOW) {
    _currentButton = RIGHT;
  }
  ButtonsISRTask.detach();
}

void IRAM_ATTR isrButtons(void) {
  ButtonsISRTask.detach();
  ButtonsISRTask.attach_ms(BUTTON_DEBOUNCE_TIME_MS, ButtonsISRFunction);
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
    while ( Serial.available() ) {
        char c = (char)Serial.read();
        if ( c == '4' or c == 'g'){
            _currentButton = LEFT;
            break;
        }
        if ( c == '2' or c == 'h'){
            _currentButton = DOWN;
            break;
        }
        if ( c == '8' or c == 'j'){
            _currentButton = UP;
            break;
        }
        if ( c == '6' or c == 'k'){
            _currentButton = RIGHT;
            break;
        }
    }
    if(_currentButton != NONE){
        TimeOutTask.detach();
        TimeOutTask.attach(BUTTONS_TIMEOUT_SECONDS, goIdleState);
    }
    int buttonPress = _currentButton;
    _currentButton = NONE;
    return buttonPress;
}