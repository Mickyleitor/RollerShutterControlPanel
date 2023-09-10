#pragma once

#include <LiquidCrystal_PCF8574.h>

#define LED_GREEN_PIN_NR                                                    (16)

#define FATAL_ERROR_CODE_LCD_INIT_FAILED                                     (1)
#define FATAL_ERROR_CODE_WIFI_AP_FAILED                                      (2)
#define FATAL_ERROR_CODE_SLAVE_HARDWARE                                      (3)

static String fatalErrorListStr [] = {
    "   LCD FAILED   ",
    " WIFI AP FAILED ",
    " NO SLAVE HARDW "
};

static inline void errorHandler(LiquidCrystal_PCF8574 * screen, uint8_t fatalErrorCode) {
    Serial.println("FATAL ERROR. code : " + String(fatalErrorCode));
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
        pinMode(LED_GREEN_PIN_NR, OUTPUT);
        while (true){
            if( millis() - timerMs > 250 ){
                timerMs = millis();
                digitalWrite(LED_GREEN_PIN_NR, !digitalRead(LED_GREEN_PIN_NR));
            }
            yield();
        }
    }
}