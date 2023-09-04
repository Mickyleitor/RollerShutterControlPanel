#pragma once

#include <LiquidCrystal_PCF8574.h>

#define FATAL_ERROR_CODE_LCD_INIT_FAILED                                     (1)
#define FATAL_ERROR_CODE_WIFI_STA_FAILED                                     (2)
#define FATAL_ERROR_CODE_WIFI_AP_FAILED                                      (3)

static char * fatalErrorListStr [] = {
    "   LCD FAILED   ",
    " WIFI NOT FOUND ",
    " WIFI AP FAILED "
};

static inline void errorHandler(LiquidCrystal_PCF8574 * screen, uint8_t fatalErrorCode) {
    // When no lcd is connected, blink the led on pin 16
    if( fatalErrorCode > FATAL_ERROR_CODE_LCD_INIT_FAILED ){
        screen->clear();
        screen->home();
        screen->print("FATAL ERROR: ");
        screen->print(fatalErrorCode);
        screen->setCursor(0, 1);
        screen->print(fatalErrorListStr[fatalErrorCode - 1]);
        WiFi.mode(WIFI_OFF);
        WiFi.forceSleepBegin();
        while (true){
            yield();
        }
    }else{
        uint32_t timerMs = millis();
        pinMode(16, OUTPUT);
        while (true){
            if( millis() - timerMs > 250 ){
                timerMs = millis();
                digitalWrite(16, !digitalRead(16));
            }
            yield();
        }
    }
}