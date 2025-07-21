// Domotic Shutter sketch master by Mickyleitor
// This sketch is in development and uses ESP13-WROOM-02.
//
// Libraries used and need to be installed from Arduino IDE
// - Shield manager ESP8266 by ESP8266 Community version 3.1.2 :
// http://arduino.esp8266.com/stable/package_esp8266com_index.json
// - NTPClient by Fabrice Weinberg 3.2.0 : https://github.com/arduino-libraries/NTPClient
// - LiquidCrystal_PCF8574 by mathertel 2.2.0  : https://github.com/mathertel/LiquidCrystal_PCF8574

#include <Ticker.h>

#include "EEPROM_Utils.h"
#include "ESP8266_Utils.h"
#include "ShutterManager.h"
#include "Slave_Utils.h"
#include "basic_defines.h"
#include "buttons.h"
#include "error.h"
#include "lcd.h"
#include "rscpProtocol/rscpProtocol.h"

enum SystemState _SystemState = SYSTEM_STATE_ENTERING_IDLE;
uint8_t _seleccionMenu        = DEFAULT_SELECTION_MENU;
struct ShutterParameters ShutterData[NUMBER_OF_SHUTTERS];

void systemStateGoToIdle(void) { _SystemState = SYSTEM_STATE_ENTERING_IDLE; }

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Master inicializado");
    EEPROM_Begin(&settings);

    if (initLCDFunction(10000) < 0) {
        errorHandler(FATAL_ERROR_CODE_LCD_INIT_FAILED);
    }

    sendLcdBuffer("    INICIANDO   PANEL DE CONTROL");
    initButtonsFunction();
    checkSlaveConnection();
    if (!ESP8266Utils_Connect_STA(
                settings.wifiSettings.ssid_sta,
                settings.wifiSettings.password_sta,
                settings.wifiSettings.hostname,
                WIFI_CONNECTION_TIMEOUT)) {
        Serial.print("Error connecting to SSID: ");
        Serial.println(settings.wifiSettings.ssid_sta);
    } else {
        Serial.println("STA connected");
        ESP8266Utils_update_WeatherData(&settings);
    }
}

void loop() {
    static unsigned long lastScreenUpdate = 0;
    unsigned long currentMillis           = millis();
    switch (_SystemState) {
        case SYSTEM_STATE_ENTERING_IDLE: {
            apagarBrilloPantalla();
            // If the menu is not a shutter and we are entering idle state,
            // return to the default shutter menu when we wake up again.
            if (_seleccionMenu >= SELECCION_MENU_INFO) {
                _seleccionMenu = DEFAULT_SELECTION_MENU;
            }
            if (ntp_time_is_initialized()) {
                actualizarHoraPantalla();
            } else {
                actualizarMenuPantalla(_seleccionMenu);
            }
            _SystemState = SYSTEM_STATE_IDLING;
            break;
        }
        case SYSTEM_STATE_IDLING: {
            if (buttonPressed() != BUTTON_STATUS_NONE) {
                _SystemState = SYSTEM_STATE_WAKEUP;
            } else {
                unsigned long elapsedTime = (currentMillis > lastScreenUpdate)
                                                  ? (currentMillis - lastScreenUpdate)
                                                  : (lastScreenUpdate - currentMillis);
                if (elapsedTime >= (UPDATE_SCREEN_ON_IDLE_INTERVAL_SECONDS * 1000)) {
                    lastScreenUpdate = currentMillis;
                    if (ntp_time_is_initialized()) {
                        actualizarHoraPantalla();
                    } else {
                        actualizarMenuPantalla(_seleccionMenu);
                    }
                }
            }
            break;
        }
        case SYSTEM_STATE_WAKEUP: {
            encenderBrilloPantalla();
            _SystemState = SYSTEM_STATE_MENU;
            break;
        }
        case SYSTEM_STATE_MENU: {
            uint8_t button = buttonPressed();
            handleButtonFromMenu(&_seleccionMenu, button);
            actualizarMenuPantalla(_seleccionMenu);
            break;
        }
    }
}
