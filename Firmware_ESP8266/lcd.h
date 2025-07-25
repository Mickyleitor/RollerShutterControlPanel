#pragma once

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include "ESP8266_Utils.h"
#include "ShutterManager.h"
#include "basic_defines.h"
#include "rtcTime.h"

extern struct ShutterParameters ShutterData[];
extern struct Settings settings;
extern struct WeatherData MyWeather;

LiquidCrystal_PCF8574 _lcd(LCD_I2C_ADDRESS);
static time_t adjustedTime = 0;

#define LCD_TOTAL_WIDTH                                                     (16)
#define LCD_TOTAL_HEIGHT                                                     (2)

#define LCD_SPECIAL_CHAR_BASE                                         (char)(10)
#define LCD_SPECIAL_CHAR_LEFT_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 0)
#define LCD_SPECIAL_CHAR_UP_ARROW              (char)(LCD_SPECIAL_CHAR_BASE + 1)
#define LCD_SPECIAL_CHAR_DOWN_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 2)
#define LCD_SPECIAL_CHAR_RIGHT_ARROW           (char)(LCD_SPECIAL_CHAR_BASE + 3)
#define LCD_SPECIAL_CHAR_STOP_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 4)
#define LCD_SPECIAL_CHAR_UP_ARROW_CAN          (char)(LCD_SPECIAL_CHAR_BASE + 5)

#define LCD_TRANSITION_SPEED_MS                                           (5000)
#define LCD_SLIDE_OR_FLASH_SPEED_MS                                        (500)
#define LCD_CONFIG_FECHA_HORA_UPDATE_INTERVAL_MS                           (200)
#define LCD_CONFIG_BUTTON_HOLDING_UPDATE_INTERVAL_MS                      (1500)

#define LCD_CLOCK_UPDATE_INTERVAL_MS                                     (60000)

void pantalla_apagarBrillo(void) { _lcd.setBacklight(0); }

void pantalla_encenderBrillo(void) { _lcd.setBacklight(255); }

void _sendLcdBuffer(String line1, String line2) {
    _lcd.home();
    _lcd.clear();
    for (int i = 0; i < 16; i++) {
        _lcd.setCursor(i, 0);
        if ((line1[i] >= LCD_SPECIAL_CHAR_BASE) && (line1[i] < LCD_SPECIAL_CHAR_BASE + 6)) {
            _lcd.write(line1[i] - LCD_SPECIAL_CHAR_BASE);
        } else {
            _lcd.print(line1[i]);
        }
        _lcd.setCursor(i, 1);
        if ((line2[i] >= LCD_SPECIAL_CHAR_BASE) && (line2[i] < LCD_SPECIAL_CHAR_BASE + 6)) {
            _lcd.write(line2[i] - LCD_SPECIAL_CHAR_BASE);
        } else {
            _lcd.print(line2[i]);
        }
    }
}

bool pantalla_sendLcdBuffer(String newBuffer) {
    static String bufferAnterior;
    if ((bufferAnterior != newBuffer) && (newBuffer != "")) {
        bufferAnterior = newBuffer;
        String line1   = newBuffer.substring(0, 16);
        String line2   = newBuffer.substring(16, 32);
        _sendLcdBuffer(line1, line2);
        return true;
    }
    return false;
}

bool pantalla_iniciar(int32_t timeout_ms) {
    // Search LCD into I2C line:
    while (timeout_ms > 0) {
        Wire.begin(2, 14);
        Wire.beginTransmission(LCD_I2C_ADDRESS);
        if (Wire.endTransmission() != 0) {
            // wait 2 seconds for reconnection:
            delay(2000);
            timeout_ms -= 2000;
        } else {
            // LCD found, we can continue
            break;
        }
    }

    if (timeout_ms <= 0) {
        return false;
    }

    _lcd.begin(16, 2);
    uint8_t customArrayChar[6][8] = {
        /* Flecha izquierda */ { 0x00, 0x07, 0x0E, 0x1C, 0x1C, 0x0E, 0x07, 0x00 },
        /* Flecha arriba    */
        { 0x00, 0x04, 0x0E, 0x1F, 0x1B, 0x11, 0x00, 0x00 },
        /* Flecha abajo     */
        { 0x00, 0x00, 0x11, 0x1B, 0x1F, 0x0E, 0x04, 0x00 },
        /* Flecha derecha   */
        { 0x00, 0x1C, 0x0E, 0x07, 0x07, 0x0E, 0x1C, 0x00 },
        /* Flecha STOP      */
        { 0x00, 0x0E, 0x1B, 0x11, 0x11, 0x1B, 0x0E, 0x00 },
        /* Flecha arribacan */
        { 0x04, 0x0E, 0x1F, 0x15, 0x04, 0x04, 0x07, 0x00 }
    };
    for (int i = 0; i < 6; i++) {
        _lcd.createChar(i, customArrayChar[i]);
    }
    return true;
}

void pantalla_handleButtonInMenu(
        uint8_t* currentMenu,
        uint8_t currentButtonPressed,
        uint8_t currentButtonHolding) {
    uint8_t newMenu = *currentMenu;
    switch (newMenu) {
        case SELECCION_MENU_RELOJ:
            break;
        case SELECCION_MENU_PERSIANA_IZQUIERDA:
            // fall through
        case SELECCION_MENU_PERSIANA_CENTRAL:
            // fall through
        case SELECCION_MENU_PERSIANA_DERECHA: {
            uint8_t _localShutterIndex = (SELECCION_MENU_PERSIANA_TO_INDEX(newMenu));
            switch (currentButtonPressed) {
                case BUTTON_STATUS_LEFT:
                    newMenu--;
                    // Sanitize menu transition to left
                    if (newMenu < SELECCION_MENU_PERSIANA_IZQUIERDA) {
                        newMenu = SELECCION_MENU_CONFIG;
                    }
                    break;
                case BUTTON_STATUS_RIGHT:
                    newMenu++;
                    // Sanitize menu transition to right
                    if (newMenu > SELECCION_MENU_PERSIANA_DERECHA) {
                        newMenu = SELECCION_MENU_CONFIG;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    if (ShutterData[_localShutterIndex].status == SHUTTER_STATUS_MOVING_UP) {
                        PararPersiana(_localShutterIndex);
                    } else {
                        subirPersiana(_localShutterIndex);
                    }
                    break;
                case BUTTON_STATUS_DOWN:
                    if (ShutterData[_localShutterIndex].status == SHUTTER_STATUS_MOVING_DOWN) {
                        PararPersiana(_localShutterIndex);
                    } else {
                        bajarPersiana(_localShutterIndex);
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG: {
            switch (currentButtonPressed) {
                case BUTTON_STATUS_LEFT:
                    newMenu = SELECCION_MENU_PERSIANA_DERECHA;
                    break;
                case BUTTON_STATUS_RIGHT:
                    newMenu = SELECCION_MENU_PERSIANA_IZQUIERDA;
                    break;
                case BUTTON_STATUS_DOWN:
                    newMenu = SELECCION_MENU_CONFIG_FECHA_HORA;
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG_FECHA_HORA: {
            switch (currentButtonPressed) {
                case BUTTON_STATUS_UP:
                    newMenu = SELECCION_MENU_CONFIG;
                    break;
                case BUTTON_STATUS_RIGHT:
                    adjustedTime = time(NULL);
                    newMenu      = SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE: {
            switch (currentButtonPressed) {
                case BUTTON_STATUS_LEFT:
                    newMenu = SELECCION_MENU_CONFIG_FECHA_HORA;
                    break;
                case BUTTON_STATUS_RIGHT:
                    // Save the adjusted time
                    rtc_set(adjustedTime);
                    newMenu = SELECCION_MENU_CONFIG_FECHA_HORA;
                    break;
                default: {
                    static unsigned long lastClockUpdateMs   = 0;
                    static unsigned long lastButtonHoldingMs = 0;
                    static unsigned long currentAdjustedTime = 60; // start at 1 min

                    unsigned long now = millis();
                    if (now - lastClockUpdateMs > LCD_CONFIG_FECHA_HORA_UPDATE_INTERVAL_MS) {
                        lastClockUpdateMs = now;

                        if (currentButtonHolding == BUTTON_STATUS_UP
                            || currentButtonHolding == BUTTON_STATUS_DOWN) {
                            // if just transitioned to holding, reset timer
                            if (lastButtonHoldingMs == 0) {
                                lastButtonHoldingMs = now;
                            }

                            // decide step level
                            unsigned long holdMs = now - lastButtonHoldingMs;
                            unsigned level
                                    = min(4UL,
                                          holdMs / LCD_CONFIG_BUTTON_HOLDING_UPDATE_INTERVAL_MS);

                            // map to increments
                            static const unsigned long incs[5]
                                    = { 60UL, 3600UL, 86400UL, 2592000UL, 31536000UL };
                            currentAdjustedTime = incs[level];

                            // apply adjustment
                            if (currentButtonHolding == BUTTON_STATUS_UP) {
                                adjustedTime += currentAdjustedTime;
                            } else {
                                adjustedTime -= currentAdjustedTime;
                            }
                        } else {
                            // button released or other key pressed → reset
                            lastButtonHoldingMs = 0;
                            currentAdjustedTime = 60;
                        }
                    }
                    if (adjustedTime < 0) {
                        adjustedTime = 0;
                    }
                } break;
            }
            break;
        }
    }
    *currentMenu = newMenu;
}

void pantalla_actualizarReloj(String* lcdBuffer) {
    *lcdBuffer = "   ";
    time_t now;
    struct tm* timeinfo;
    now      = time(&now) + MyWeather.timezoneshift;
    timeinfo = localtime(&now);

    if ((timeinfo->tm_hour) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_hour);
    *lcdBuffer += String(':');

    if ((timeinfo->tm_min) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_min);
    *lcdBuffer += String("   ");

    uint8_t _localLength = String((int)MyWeather.TemperatureDegree).length();
    // Limit degree to 2 digits
    if (_localLength < 4) {
        for (int i = 0; i < (3 - _localLength); i++) {
            *lcdBuffer += String(" ");
        }
        *lcdBuffer += String((int)MyWeather.TemperatureDegree);
        *lcdBuffer += (char)223;
        *lcdBuffer += String("C");
    } else {
        *lcdBuffer += String("     ");
    }

    if ((timeinfo->tm_mday) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_mday);
    *lcdBuffer += String("/");

    if ((timeinfo->tm_mon + 1) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_mon + 1);
    *lcdBuffer += String("/");

    *lcdBuffer += String((timeinfo->tm_year) + 1900);
    *lcdBuffer += String("  ");

    int dayofweek = (timeinfo->tm_wday);
    switch (dayofweek) {
        case 0:
            *lcdBuffer += String("Dom.");
            break;
        case 1:
            *lcdBuffer += String("Lun.");
            break;
        case 2:
            *lcdBuffer += String("Mar.");
            break;
        case 3:
            *lcdBuffer += String("Mie.");
            break;
        case 4:
            *lcdBuffer += String("Jue.");
            break;
        case 5:
            *lcdBuffer += String("Vie.");
            break;
        case 6:
            *lcdBuffer += String("Sab.");
            break;
        default:
            *lcdBuffer += String("Err.");
            break;
    }
}

void pantalla_actualizarMenuShutter(String* lcdBuffer, uint8_t currentShutterIndex) {
    String namePersiana[] = { " PERSIANA IZQDA ", "PERSIANA CENTRAL", "PERSIANA DERECHA" };

    *lcdBuffer += namePersiana[currentShutterIndex];
    *lcdBuffer += String("<    ");

    if (ShutterData[currentShutterIndex].status == SHUTTER_STATUS_MOVING_UP) {
        *lcdBuffer += LCD_SPECIAL_CHAR_STOP_ARROW;
    } else {
        *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    }

    *lcdBuffer += String("    ");

    if (ShutterData[currentShutterIndex].status == SHUTTER_STATUS_MOVING_DOWN) {
        *lcdBuffer += LCD_SPECIAL_CHAR_STOP_ARROW;
    } else {
        *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    }

    *lcdBuffer += String("    >");
}

void pantalla_actualizarMenuConfig(String* lcdBuffer) {
    *lcdBuffer += String(" MENU DE CONFIG ");
    *lcdBuffer += String("<    ");
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String(" OK ");
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String("    >");
}

void pantalla_actualizarMenuConfigFechaHora(String* lcdBuffer) {
    *lcdBuffer += String("  CONFIG. HORA  ");
    *lcdBuffer += String("     ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += String("    ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += String("  OK>");
}

void pantalla_actualizarMenuConfigFechaHoraAjuste(String* lcdBuffer) {
    // Show the adjusted time
    // format: 00:00 16/10/2023
    struct tm* timeinfo;
    static unsigned long lastClockFlashMs = 0;
    static bool clockStatus               = false;

    timeinfo = localtime(&adjustedTime);

    if ((timeinfo->tm_hour) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_hour);
    if ((millis() - lastClockFlashMs) > LCD_SLIDE_OR_FLASH_SPEED_MS) {
        lastClockFlashMs = millis();
        clockStatus      = !clockStatus;
    }

    if (clockStatus) {
        *lcdBuffer += String(":");
    } else {
        *lcdBuffer += String(" ");
    }

    if ((timeinfo->tm_min) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_min);
    *lcdBuffer += String(" ");

    if ((timeinfo->tm_mday) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_mday);
    *lcdBuffer += String("/");

    if ((timeinfo->tm_mon + 1) < 10) {
        *lcdBuffer += String('0');
    }
    *lcdBuffer += String(timeinfo->tm_mon + 1);
    *lcdBuffer += String("/");

    *lcdBuffer += String((timeinfo->tm_year) + 1900);
    *lcdBuffer += String("<     ");
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String("  ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += String("   OK>");
}

void pantalla_actualizarMenu(uint8_t selectedMenu) {
    String lcdBuffer = "";
    switch (selectedMenu) {
        case SELECCION_MENU_RELOJ:
            pantalla_actualizarReloj(&lcdBuffer);
            break;
        case SELECCION_MENU_PERSIANA_IZQUIERDA:
            // fall through
        case SELECCION_MENU_PERSIANA_CENTRAL:
            // fall through
        case SELECCION_MENU_PERSIANA_DERECHA: {
            uint8_t currentShutterIndex = (SELECCION_MENU_PERSIANA_TO_INDEX(selectedMenu));
            pantalla_actualizarMenuShutter(&lcdBuffer, currentShutterIndex);
            break;
        }
        case SELECCION_MENU_CONFIG:
            pantalla_actualizarMenuConfig(&lcdBuffer);
            break;
        case SELECCION_MENU_CONFIG_FECHA_HORA:
            pantalla_actualizarMenuConfigFechaHora(&lcdBuffer);
            break;
        case SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE:
            pantalla_actualizarMenuConfigFechaHoraAjuste(&lcdBuffer);
            break;
    }
    pantalla_sendLcdBuffer(lcdBuffer);
}

void pantalla_actualizar(bool showClock, uint8_t wantedMenu) {
    bool isNtpReady = rtc_on_sync();

    // 1) explicit clock request: always show if NTP is ready
    if ((wantedMenu == SELECCION_MENU_RELOJ) && isNtpReady) {
        Serial.println("Forcing clock update on LCD");
        pantalla_actualizarMenu(SELECCION_MENU_RELOJ);
        return;
    }

    // 2) periodic clock refresh if allowed and NTP is ready
    if (showClock && isNtpReady) {
        static unsigned long lastClockUpdate = 0;
        unsigned long now                    = millis();
        if ((now - lastClockUpdate) >= LCD_CLOCK_UPDATE_INTERVAL_MS) {
            lastClockUpdate = now;
            pantalla_actualizarMenu(SELECCION_MENU_RELOJ);
        }
        return;
    }

    // 3) normal menus (valid and not clock)
    if (wantedMenu < SELECCION_MENU_MAX && (wantedMenu != SELECCION_MENU_RELOJ)) {
        pantalla_actualizarMenu(wantedMenu);
    }
}

