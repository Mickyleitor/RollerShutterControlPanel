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
#include "rtcTime.h"

static SystemState_t _SystemState     = SYSTEM_STATE_ENTERING_IDLE;
static seleccionMenu_t _seleccionMenu = DEFAULT_SELECTION_MENU;
struct ShutterParameters ShutterData[NUMBER_OF_SHUTTERS];

void systemStateGoToIdle(void) {
    if (_seleccionMenu <= SELECCION_MENU_CONFIG) {
        _SystemState = SYSTEM_STATE_ENTERING_IDLE;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Master inicializado");

    EEPROM_Begin(&settings);
    rtc_init();

    if (!pantalla_iniciar(10000)) {
        errorHandler(FATAL_ERROR_CODE_LCD_INIT_FAILED);
    }
    pantalla_encenderBrillo();
    pantalla_sendLcdBuffer("    INICIANDO   PANEL DE CONTROL");
    // We wait a bit just in case we just powered on the whole
    // device, sometimes the power is still not enough at this point.
    delay(2000);
    initButtonsFunction();
    checkSlaveConnection();
    /*
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
    */
}

void loop() {
    switch (_SystemState) {
        case SYSTEM_STATE_ENTERING_IDLE: {
            pantalla_apagarBrillo();
            // On entering idle, if menu is beyond info,
            // reset to default when we wake up again.
            if (_seleccionMenu >= SELECCION_MENU_CONFIG) {
                _seleccionMenu = DEFAULT_SELECTION_MENU;
            }
            pantalla_actualizar(true, SELECCION_MENU_RELOJ);
            _SystemState = SYSTEM_STATE_IDLING;
            break;
        }
        case SYSTEM_STATE_IDLING: {
            if (buttonPressed() != BUTTON_STATUS_NONE) {
                _SystemState = SYSTEM_STATE_WAKEUP;
            } else {
                pantalla_actualizar(true, _seleccionMenu);
            }
            break;
        }
        case SYSTEM_STATE_WAKEUP:
            pantalla_encenderBrillo();
            _SystemState = SYSTEM_STATE_MENU;
            break;
        case SYSTEM_STATE_MENU: {
            pantalla_handleButtonInMenu(&_seleccionMenu, buttonPressed(), buttonHolding());
            pantalla_actualizar(false, _seleccionMenu);
            break;
        }
    }
    rtc_handler();
}
