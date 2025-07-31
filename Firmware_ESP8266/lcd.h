#pragma once

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include "ESP8266_Utils.h"
#include "ShutterManager.h"
#include "basic_defines.h"
#include "persistentVars.h"
#include "rtcTime.h"

extern struct WeatherData MyWeather;

LiquidCrystal_PCF8574 _lcd(LCD_I2C_ADDRESS);
static time_t adjustedTime                     = 0;
static long adjustedTimeZone                   = 0;
static uint32_t previousBuzzerVolume           = 0;
static unsigned long currentAdjustedLevelIndex = 0;
static bool previousBuzzerEnabled              = false;
static bool buttonIsReleased                   = false;

#define LCD_SPECIAL_CHAR_BASE                                         (char)(10)
#define LCD_SPECIAL_CHAR_LEFT_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 0)
#define LCD_SPECIAL_CHAR_UP_ARROW              (char)(LCD_SPECIAL_CHAR_BASE + 1)
#define LCD_SPECIAL_CHAR_DOWN_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 2)
#define LCD_SPECIAL_CHAR_RIGHT_ARROW           (char)(LCD_SPECIAL_CHAR_BASE + 3)
#define LCD_SPECIAL_CHAR_STOP_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 4)
#define LCD_SPECIAL_CHAR_UP_ARROW_CAN          (char)(LCD_SPECIAL_CHAR_BASE + 5)

#define LCD_SLIDE_OR_FLASH_SPEED_MS                                        (500)

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
            uint8_t _localShutterIndex     = (SELECCION_MENU_PERSIANA_TO_INDEX(newMenu));
            ShutterStatus_t _shutterStatus = shutterGetStatus(_localShutterIndex);
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
                    if ((_shutterStatus == SHUTTER_STATUS_MOVING_UP)
                        || _shutterStatus == SHUTTER_STATUS_MOVING_DOWN) {
                        shutterPararPersiana(_localShutterIndex);
                    } else {
                        shutterSubirPersiana(_localShutterIndex);
                    }
                    break;
                case BUTTON_STATUS_DOWN:
                    if ((_shutterStatus == SHUTTER_STATUS_MOVING_DOWN)
                        || _shutterStatus == SHUTTER_STATUS_MOVING_UP) {
                        shutterPararPersiana(_localShutterIndex);
                    } else {
                        shutterBajarPersiana(_localShutterIndex);
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
                case BUTTON_STATUS_UP:
                    buzzer_sound_error();
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
                    adjustedTime              = time(NULL);
                    currentAdjustedLevelIndex = 0;
                    newMenu                   = SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE;
                    break;
                case BUTTON_STATUS_DOWN:
                    newMenu = SELECCION_MENU_CONFIG_VOLUMEN;
                    break;
                case BUTTON_STATUS_LEFT:
                    buzzer_sound_error();
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE: {
            static const unsigned long incs[5] = {
                3600UL,    // hours
                60UL,      // minutes
                86400UL,   // days
                2592000UL, // months (~30d)
                31536000UL // years (~365d)
            };
            static const unsigned long incsCount = sizeof(incs) / sizeof(incs[0]);

            switch (currentButtonPressed) {
                case BUTTON_STATUS_LEFT:
                    if (currentAdjustedLevelIndex) {
                        currentAdjustedLevelIndex--;
                    } else {
                        newMenu = SELECCION_MENU_CONFIG_FECHA_HORA;
                    }
                    break;
                case BUTTON_STATUS_RIGHT:
                    if (currentAdjustedLevelIndex < (incsCount - 1)) {
                        currentAdjustedLevelIndex++;
                    } else {
                        adjustedTimeZone = persistentVars_get_rtcTime().timezoneShift;
                        newMenu          = SELECCION_MENU_CONFIG_ZONA_HORARIA_AJUSTE;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    adjustedTime += incs[currentAdjustedLevelIndex];
                    break;
                case BUTTON_STATUS_DOWN:
                    adjustedTime -= incs[currentAdjustedLevelIndex];
                    break;
            }

            if (adjustedTime < 0) {
                adjustedTime = 0;
            }

            break;
        }
        case SELECCION_MENU_CONFIG_ZONA_HORARIA_AJUSTE: {
            switch (currentButtonPressed) {
                case BUTTON_STATUS_LEFT:
                    currentAdjustedLevelIndex = 0;
                    newMenu                   = SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    buzzer_sound_accept();
                    rtc_set(adjustedTime, adjustedTimeZone);
                    newMenu = SELECCION_MENU_CONFIG;
                    break;
                case BUTTON_STATUS_UP:
                    adjustedTimeZone += 3600;       // Increase by 1 hour
                    if (adjustedTimeZone > 43200) { // 12 hours in seconds
                        adjustedTimeZone = 43200;
                    }
                    break;
                case BUTTON_STATUS_DOWN:
                    adjustedTimeZone -= 3600;        // Decrease by 1 hour
                    if (adjustedTimeZone < -43200) { // -12 hours in seconds
                        adjustedTimeZone = -43200;
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG_VOLUMEN: {
            buttonIsReleased = true;
            switch (currentButtonPressed) {
                case BUTTON_STATUS_UP:
                    newMenu = SELECCION_MENU_CONFIG_FECHA_HORA;
                    break;
                case BUTTON_STATUS_RIGHT:
                    previousBuzzerVolume  = buzzer_get_volume();
                    previousBuzzerEnabled = buzzer_is_enabled();
                    buttonIsReleased      = false;
                    newMenu               = SELECCION_MENU_CONFIG_VOLUMEN_AJUSTE;
                    break;
                case BUTTON_STATUS_DOWN:
                case BUTTON_STATUS_LEFT:
                    buzzer_sound_error();
                    break;
            }
            break;
        }
        case SELECCION_MENU_CONFIG_VOLUMEN_AJUSTE:
            if (!buttonIsReleased) {
                if (currentButtonHolding != BUTTON_STATUS_NONE) {
                    break; // We are waiting for the button to be released
                }
                buttonIsReleased = true;
            }
            switch (currentButtonHolding) { // Note that we use holding here
                case BUTTON_STATUS_UP:
                    if (buzzer_get_volume() < buzzer_get_volume_max()) {
                        buzzer_set_volume(buzzer_get_volume() + 100);
                        buzzer_enable();
                    } else {
                        buzzer_set_volume(buzzer_get_volume_max());
                        buzzer_enable();
                    }
                    break;
                case BUTTON_STATUS_DOWN:
                    if (buzzer_get_volume() > buzzer_get_volume_min()) {
                        buzzer_set_volume(buzzer_get_volume() - 100);
                    } else {
                        buzzer_set_volume(buzzer_get_volume_min());
                        buzzer_disable();
                    }
                    break;
                default:
                    switch (currentButtonPressed) {
                        case BUTTON_STATUS_RIGHT:
                            buzzer_sound_accept();
                            buzzer_store_settings();
                            newMenu = SELECCION_MENU_CONFIG;
                            break;
                        case BUTTON_STATUS_LEFT:
                            buzzer_set_volume(previousBuzzerVolume);
                            if (previousBuzzerEnabled) {
                                buzzer_enable();
                            } else {
                                buzzer_disable();
                            }
                            newMenu = SELECCION_MENU_CONFIG_VOLUMEN;
                            break;
                    }
                    break;
            }
            break;
    }
    *currentMenu = newMenu;
}

void pantalla_actualizarReloj(String* lcdBuffer) {
    *lcdBuffer = "   ";
    time_t now;
    struct tm* timeinfo;
    now      = time(&now) + persistentVars_get_rtcTime().timezoneShift;
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

    if (shutterGetStatus(currentShutterIndex) == SHUTTER_STATUS_MOVING_UP) {
        *lcdBuffer += LCD_SPECIAL_CHAR_STOP_ARROW;
    } else {
        *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    }

    *lcdBuffer += String("    ");

    if (shutterGetStatus(currentShutterIndex) == SHUTTER_STATUS_MOVING_DOWN) {
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
    *lcdBuffer += String("       ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String("      >");
}

void pantalla_actualizarMenuConfigFechaHoraAjuste(String* lcdBuffer) {
    struct tm* timeinfo = localtime(&adjustedTime);

    static unsigned long lastFlashMs = 0;
    static bool flashOn              = false;

    if ((millis() - lastFlashMs) > LCD_SLIDE_OR_FLASH_SPEED_MS) {
        lastFlashMs = millis();
        flashOn     = !flashOn;
    }

    auto printField = [&](int value, unsigned long index, int width) {
        bool visible = (currentAdjustedLevelIndex != index) || flashOn;
        if (visible) {
            if (value < 10 && width >= 2) {
                *lcdBuffer += '0';
            }
            *lcdBuffer += String(value);
        } else {
            for (int i = 0; i < width; ++i) {
                *lcdBuffer += ' ';
            }
        }
    };

    // Format: hh:mm dd/mm/yyyy
    printField(timeinfo->tm_hour, 0, 2);
    *lcdBuffer += ':';
    printField(timeinfo->tm_min, 1, 2);
    *lcdBuffer += ' ';
    printField(timeinfo->tm_mday, 2, 2);
    *lcdBuffer += '/';
    printField(timeinfo->tm_mon + 1, 3, 2);
    *lcdBuffer += '/';
    printField(timeinfo->tm_year + 1900, 4, 4);

    *lcdBuffer += "<      ";
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += "      >";
}

void pantalla_actualizarMenuConfigZonaHorariaAjuste(String* lcdBuffer) {
    *lcdBuffer += String("ZONA UTC: ");
    *lcdBuffer += String(adjustedTimeZone / 3600);
    *lcdBuffer += String(" h");
    // Add spaces to fill the line.
    while (lcdBuffer->length() < 16) {
        *lcdBuffer += String(" ");
    }
    *lcdBuffer += String("<      ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String("    OK>");
}

void pantalla_actualizarMenuConfigVolumen(String* lcdBuffer) {
    *lcdBuffer += String("CONFIG.  VOLUMEN");
    *lcdBuffer += String("       ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += String("      >");
}

void pantalla_actualizarMenuConfigVolumenAjuste(String* lcdBuffer) {
    *lcdBuffer += String("VOLUMEN:  ");
    // If disabled, show a message
    if (!buzzer_is_enabled()) {
        *lcdBuffer += String(" NO");
    } else {
        uint32_t volumePercentage = (buzzer_get_volume() - buzzer_get_volume_min()) * 100
                                  / (buzzer_get_volume_max() - buzzer_get_volume_min());
        *lcdBuffer += String(volumePercentage);
        *lcdBuffer += String(" %");
    }
    // Add spaces to fill the line.
    while (lcdBuffer->length() < 16) {
        *lcdBuffer += String(" ");
    }
    *lcdBuffer += String("       ");
    *lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
    *lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
    *lcdBuffer += String("    OK>");
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
        case SELECCION_MENU_CONFIG_ZONA_HORARIA_AJUSTE:
            pantalla_actualizarMenuConfigZonaHorariaAjuste(&lcdBuffer);
            break;
        case SELECCION_MENU_CONFIG_VOLUMEN:
            pantalla_actualizarMenuConfigVolumen(&lcdBuffer);
            break;
        case SELECCION_MENU_CONFIG_VOLUMEN_AJUSTE:
            pantalla_actualizarMenuConfigVolumenAjuste(&lcdBuffer);
            break;
    }
    pantalla_sendLcdBuffer(lcdBuffer);
}

void pantalla_actualizar(bool showClock, uint8_t wantedMenu) {
    bool isNtpReady = rtc_on_sync();

    // 1) explicit clock request: always show if NTP is ready
    if ((wantedMenu == SELECCION_MENU_RELOJ) && isNtpReady) {
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

