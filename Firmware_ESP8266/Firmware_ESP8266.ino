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

enum SystemState _SystemState  = SYSTEM_STATE_WIFI_STATION_CONNECTED;
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
    sendLcdBuffer("....INICIANDO...CONECTANDO  WIFI");
    if (!ESP8266Utils_Connect_STA(
                settings.wifiSettings.ssid_sta,
                settings.wifiSettings.password_sta,
                settings.wifiSettings.hostname,
                WIFI_CONNECTION_TIMEOUT)) {
        Serial.print("Error connecting to SSID: ");
        Serial.println(settings.wifiSettings.ssid_sta);
        if (!ESP8266Utils_Connect_AP(
                    settings.wifiSettings.ssid_ap,
                    settings.wifiSettings.password_ap,
                    settings.wifiSettings.hostname,
                    WIFI_CONNECTION_TIMEOUT)) {
            Serial.print("Error creating AP with SSID: ");
            Serial.println(settings.wifiSettings.ssid_ap);
            errorHandler(FATAL_ERROR_CODE_WIFI_AP_FAILED);
        } else {
            Serial.print("AP created with SSID: ");
            Serial.println(settings.wifiSettings.ssid_ap);
            _SystemState = SYSTEM_STATE_WIFI_ACCESS_POINT_OPENED;
        }
    }
}

void loop() {
    ESP8266Utils_handleWifi();

    switch (_SystemState) {
        case SYSTEM_STATE_WIFI_ACCESS_POINT_OPENED: {
            static int lcd_transition_state;
            static uint32_t lcd_transition_timerMs;
            static int indexDisplay;
            static uint32_t slide_timerMs;

            String displayString1;
            String displayString2;

            if (lcd_transition_state == 1) {
                sendLcdBuffer(String("PORFAVOR CONECTE A LA RED WIFI  "));

                if ((millis() - lcd_transition_timerMs) > LCD_TRANSITION_SPEED_MS) {
                    lcd_transition_timerMs = millis();
                    lcd_transition_state++;
                }
            } else if (lcd_transition_state == 2) {
                displayString1 = "      SSID      ";
                displayString2 = "                " + String(settings.wifiSettings.ssid_ap)
                               + "                ";
            } else if (lcd_transition_state == 3) {
                displayString1 = "    PASSWORD    ";
                displayString2 = "                " + String(settings.wifiSettings.password_ap)
                               + "                ";
            } else if (lcd_transition_state == 4) {
                displayString1 = " LOCAL  ADDRESS ";
                displayString2
                        = "                " + ESP8266Utils_get_hostname() + "                ";
            } else {
                lcd_transition_state   = 1;
                lcd_transition_timerMs = millis();
            }

            if (lcd_transition_state > 1) {
                if ((millis() - slide_timerMs) > LCD_SLIDE_SPEED_MS) {
                    slide_timerMs = millis();

                    indexDisplay++;
                    if (indexDisplay >= (displayString2.length() - 16)) {
                        indexDisplay = 0;
                        lcd_transition_state++;
                    } else {
                        sendLcdBuffer(
                                displayString1
                                + displayString2.substring(indexDisplay, indexDisplay + 16));
                    }
                }
            }

            break;
        }
        case SYSTEM_STATE_WIFI_STATION_CONNECTED: {
            sendLcdBuffer("....INICIANDO...    PERIFERICOS  ");
            // Before ACCESS_POINT_OPENED it makes no sense to do all this bottom functions
            initButtonsFunction();
            checkSlaveConnection();
            sendLcdBuffer("....INICIANDO... OBTENIENDO DATOS");
            ESP8266Utils_update_WeatherData(&settings);
            SystemFunctionTask.detach();
            SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS, SystemFunctionManager);
            _SystemState = SYSTEM_STATE_ENTERING_IDLE;
            break;
        }
        case SYSTEM_STATE_ENTERING_IDLE: {
            apagarBrilloPantalla();
            actualizarHoraPantalla();
            TimeOutTask.detach();
            TimeOutTask.attach(UPDATE_SCREEN_SECONDS, actualizarHoraPantalla);
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
            // Update Weather condition every wake up?
            // getWeatherDataFunction();
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
        case SYSTEM_STATE_ACTION_SLEEP: {
            Serial.println("Modo dormir activado");
            EEPROM_Write(&settings);
            struct RSCP_Arg_buzzer_action buzzerAction;
            buzzerAction.action      = RSCP_DEF_BUZZER_ACTION_ON;
            buzzerAction.volume      = 300;
            buzzerAction.duration_ms = 500;
            rscpSendAction(
                    RSCP_CMD_SET_BUZZER_ACTION,
                    (uint8_t*)&buzzerAction,
                    sizeof(buzzerAction),
                    1000);
            delay(200);
            bajarPersiana(SELECCION_MENU_PERSIANA_TO_INDEX(SELECCION_MENU_PERSIANA_DERECHA));
            delay(200);
            bajarPersiana(SELECCION_MENU_PERSIANA_TO_INDEX(SELECCION_MENU_PERSIANA_CENTRAL));
            EEPROM_Read(&settings);
            _SystemState = SYSTEM_STATE_ENTERING_IDLE;
            if (_seleccionMenu > SELECCION_MENU_NONE) {
                _SystemState = SYSTEM_STATE_MENU;
            }
            break;
        }
    }
}
