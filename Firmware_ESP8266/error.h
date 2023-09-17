#pragma once

#include "lcd.h"

#define LED_GREEN_PIN_NR                                                    (16)

#define FATAL_ERROR_CODE_LCD_INIT_FAILED                                     (1)
#define FATAL_ERROR_CODE_WIFI_AP_FAILED                                      (2)
#define FATAL_ERROR_CODE_NO_SLAVE_HARDWARE                                   (3)
#define FATAL_ERROR_CODE_INVALID_SLAVE_HARDWARE                              (4)

static String fatalErrorListStr [] = {
    "   LCD FAILED   ",
    " WIFI AP FAILED ",
    " NO SLAVE HARDW ",
    "INVALID SLAVE HW"
};

static inline void errorHandler(uint8_t fatalErrorCode) {
    Serial.println("FATAL ERROR. code : " + String(fatalErrorCode));
    // When no lcd is connected, blink the led on pin 16
    if( fatalErrorCode > FATAL_ERROR_CODE_LCD_INIT_FAILED ){
        String buffer = String("FATAL ERROR: ");
        if( fatalErrorCode < 10) {
            buffer += String("00");
        }else if( fatalErrorCode < 100) {
            buffer += String("0");
        }
        buffer += String(fatalErrorCode);
        buffer += String(fatalErrorListStr[fatalErrorCode - 1]);
        sendLcdBuffer(buffer);
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