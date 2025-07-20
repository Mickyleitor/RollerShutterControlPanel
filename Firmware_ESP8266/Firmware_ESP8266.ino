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

enum SystemState _SystemState  = SYSTEM_STATE_ENTERING_IDLE;
uint8_t _seleccionMenu         = SELECCION_MENU_PERSIANA_TO_INDEX(SELECCION_MENU_PERSIANA_CENTRAL);
uint8_t _seleccionMenuAnterior = SELECCION_MENU_PERSIANA_TO_INDEX(SELECCION_MENU_PERSIANA_CENTRAL);
struct ShutterParameters ShutterData[3];
Ticker TimeOutTask, SystemFunctionTask;

void systemStateGoToIdle(void) { _SystemState = SYSTEM_STATE_ENTERING_IDLE; }

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Master inicializado");
    EEPROM_Begin(&settings);

    if (initLCDFunction(10000) < 0) {
        errorHandler(FATAL_ERROR_CODE_LCD_INIT_FAILED);
    }

    // sendLcdBuffer("                ", "                ");
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
    SystemFunctionTask.detach();
    SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS, SystemFunctionManagerISR);
}

void loop() {
    switch (_SystemState) {
        case SYSTEM_STATE_ENTERING_IDLE: {
            apagarBrilloPantalla();
            if (ntp_time_is_initialized()) {
                actualizarHoraPantallaISR();
            } else {
                actualizarMenuPantalla(_seleccionMenu);
            }
            TimeOutTask.detach();
            TimeOutTask.attach(UPDATE_SCREEN_SECONDS, actualizarHoraPantallaISR);
            if (_seleccionMenu > SELECCION_MENU_NONE) {
                _seleccionMenuAnterior = _seleccionMenu;
            }
            _seleccionMenu = SELECCION_MENU_NONE;
            _SystemState   = SYSTEM_STATE_IDLING;
            break;
        }
        case SYSTEM_STATE_IDLING: {
            if (buttonPressed() != BUTTON_STATUS_NONE) {
                _SystemState = SYSTEM_STATE_WAKEUP;
            }
            break;
        }
        case SYSTEM_STATE_WAKEUP: {
            encenderBrilloPantalla();
            _seleccionMenu = _seleccionMenuAnterior;
            _SystemState   = SYSTEM_STATE_MENU;
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
