#pragma once

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include "ESP8266_Utils.h"
#include "ShutterManager.h"
#include "basic_defines.h"

extern enum SystemState _SystemState;
extern uint8_t _seleccionMenu;
extern struct ShutterParameters ShutterData[];
extern struct Settings settings;
extern struct WeatherData MyWeather;

int8_t sleepModeDaySelected   = 1;
int8_t sleepModeMonthSelected = 1;

LiquidCrystal_PCF8574 _lcd(LCD_I2C_ADDRESS);

#define LCD_SPECIAL_CHAR_BASE                                         (char)(10)
#define LCD_SPECIAL_CHAR_LEFT_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 0)
#define LCD_SPECIAL_CHAR_UP_ARROW              (char)(LCD_SPECIAL_CHAR_BASE + 1)
#define LCD_SPECIAL_CHAR_DOWN_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 2)
#define LCD_SPECIAL_CHAR_RIGHT_ARROW           (char)(LCD_SPECIAL_CHAR_BASE + 3)
#define LCD_SPECIAL_CHAR_STOP_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 4)
#define LCD_SPECIAL_CHAR_UP_ARROW_CAN          (char)(LCD_SPECIAL_CHAR_BASE + 5)

#define LCD_TRANSITION_SPEED_MS                                           (5000)
#define LCD_SLIDE_SPEED_MS                                                 (500)

void apagarBrilloPantalla() { _lcd.setBacklight(0); }

void encenderBrilloPantalla() { _lcd.setBacklight(255); }

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

bool sendLcdBuffer(String newBuffer) {
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

String makeLcdStringDate(int sday, int smonth) {
    String date = "";
    if (sday < 10) {
        date += "0";
    }
    date += sday;
    date += "/";
    if (smonth < 10) {
        date += "0";
    }
    date += smonth;
    return date;
}

int initLCDFunction(int32_t timeout_ms) {
    int status = -1;
    // Search LCD into I2C line:
    while (status != 0 && timeout_ms > 0) {
        Wire.begin(2, 14);
        Wire.beginTransmission(LCD_I2C_ADDRESS);
        status = Wire.endTransmission();
        if (status != 0) {
            // wait 2 seconds for reconnection:
            delay(2000);
            timeout_ms -= 2000;
        }
    }

    if (timeout_ms <= 0) {
        return -1;
    }

    _lcd.begin(16, 2);
    encenderBrilloPantalla();
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
    return 0;
}

void actualizarHoraPantalla() {
    String lcdBuffer = "   ";
    time_t now;
    struct tm* timeinfo;
    now      = time(&now) + MyWeather.timezoneshift;
    timeinfo = localtime(&now);

    if ((timeinfo->tm_hour) < 10) {
        lcdBuffer += String('0');
    }
    lcdBuffer += String(timeinfo->tm_hour);
    lcdBuffer += String(':');

    if ((timeinfo->tm_min) < 10) {
        lcdBuffer += String('0');
    }
    lcdBuffer += String(timeinfo->tm_min);
    lcdBuffer += String("   ");

    uint8_t _localLength = String((int)MyWeather.TemperatureDegree).length();
    // Limit degree to 2 digits
    if (_localLength < 4) {
        for (int i = 0; i < (3 - _localLength); i++) {
            lcdBuffer += String(" ");
        }
        lcdBuffer += String((int)MyWeather.TemperatureDegree);
        lcdBuffer += (char)223;
        lcdBuffer += String("C");
    } else {
        lcdBuffer += String("     ");
    }

    if ((timeinfo->tm_mday) < 10) {
        lcdBuffer += String('0');
    }
    lcdBuffer += String(timeinfo->tm_mday);
    lcdBuffer += String("/");

    if ((timeinfo->tm_mon + 1) < 10) {
        lcdBuffer += String('0');
    }
    lcdBuffer += String(timeinfo->tm_mon + 1);
    lcdBuffer += String("/");

    lcdBuffer += String((timeinfo->tm_year) + 1900);
    lcdBuffer += String("  ");

    int dayofweek = (timeinfo->tm_wday);
    switch (dayofweek) {
        case 0:
            lcdBuffer += String("Dom.");
            break;
        case 1:
            lcdBuffer += String("Lun.");
            break;
        case 2:
            lcdBuffer += String("Mar.");
            break;
        case 3:
            lcdBuffer += String("Mie.");
            break;
        case 4:
            lcdBuffer += String("Jue.");
            break;
        case 5:
            lcdBuffer += String("Vie.");
            break;
        case 6:
            lcdBuffer += String("Sab.");
            break;
        default:
            lcdBuffer += String("Err.");
            break;
    }
    sendLcdBuffer(lcdBuffer);
}

void handleButtonFromMenu(uint8_t* _localMenu, uint8_t _localButton) {
    uint8_t _nextMenu = *_localMenu;
    switch (_nextMenu) {
        case SELECCION_MENU_NONE:
            break;
        case SELECCION_MENU_PERSIANA_IZQUIERDA:
            // fall through
        case SELECCION_MENU_PERSIANA_CENTRAL:
            // fall through
        case SELECCION_MENU_PERSIANA_DERECHA: {
            uint8_t _localShutterIndex = (SELECCION_MENU_PERSIANA_TO_INDEX(_nextMenu));
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu--;
                    // Sanitize menu transition to left
                    if (_nextMenu < SELECCION_MENU_PERSIANA_IZQUIERDA) {
                        _nextMenu = SELECCION_MENU_INFO;
                    }
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu++;
                    // Sanitize menu transition to right
                    if (_nextMenu > SELECCION_MENU_PERSIANA_DERECHA) {
                        _nextMenu = SELECCION_MENU_JOB_MODE;
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
        case SELECCION_MENU_JOB_MODE: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_PERSIANA_DERECHA;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
                case BUTTON_STATUS_DOWN:
                    _SystemState = SYSTEM_STATE_ACTION_JOB;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_JOB_MODE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_INFO;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH;
                    break;
                case BUTTON_STATUS_DOWN:
                    if (--sleepModeDaySelected <= 0) {
                        sleepModeDaySelected = 31;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    if (++sleepModeDaySelected >= 32) {
                        sleepModeDaySelected = 1;
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_CONFIRM;
                    break;
                case BUTTON_STATUS_DOWN:
                    if (--sleepModeMonthSelected <= 0) {
                        sleepModeMonthSelected = 12;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    if (++sleepModeMonthSelected >= 13) {
                        sleepModeMonthSelected = 1;
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_CONFIRM: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH;
                    break;
                case BUTTON_STATUS_RIGHT:
                    setScheduledDataFlag(
                            sleepModeDaySelected,
                            sleepModeMonthSelected,
                            (SCHEDULED_DATA_FLAG_DONE | SCHEDULED_DATA_FLAG_PENDING),
                            SCHEDULED_DATA_FLAG_PENDING);
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_MONTH;
                    break;
                case BUTTON_STATUS_DOWN:
                    if (--sleepModeDaySelected <= 0) {
                        sleepModeDaySelected = 31;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    if (++sleepModeDaySelected >= 32) {
                        sleepModeDaySelected = 1;
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_MONTH: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_CONFIRM;
                    break;
                case BUTTON_STATUS_DOWN:
                    if (--sleepModeMonthSelected <= 0) {
                        sleepModeMonthSelected = 12;
                    }
                    break;
                case BUTTON_STATUS_UP:
                    if (++sleepModeMonthSelected >= 13) {
                        sleepModeMonthSelected = 1;
                    }
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_CONFIRM: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_MONTH;
                    break;
                case BUTTON_STATUS_RIGHT:
                    setScheduledDataFlag(
                            sleepModeDaySelected,
                            sleepModeMonthSelected,
                            (SCHEDULED_DATA_FLAG_DONE | SCHEDULED_DATA_FLAG_PENDING),
                            0);
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL_CONFIRM;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL_CONFIRM: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL;
                    break;
                case BUTTON_STATUS_RIGHT:
                    setAllScheduledDataFlag(
                            (SCHEDULED_DATA_FLAG_DONE | SCHEDULED_DATA_FLAG_PENDING),
                            SCHEDULED_DATA_FLAG_PENDING);
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL_CONFIRM;
                    break;
            }
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL_CONFIRM: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL;
                    break;
                case BUTTON_STATUS_RIGHT:
                    setAllScheduledDataFlag(
                            (SCHEDULED_DATA_FLAG_DONE | SCHEDULED_DATA_FLAG_PENDING),
                            0);
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
            }
            break;
        }
        case SELECCION_MENU_INFO: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_SLEEP_MODE;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_PERSIANA_IZQUIERDA;
                    break;
                case BUTTON_STATUS_DOWN:
                    _nextMenu = SELECCION_MENU_INFO_WIFI_STATUS;
                    break;
            }
            break;
        }
        case SELECCION_MENU_INFO_WIFI_STATUS: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_INFO;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_INFO_WIFI_SSID;
                    break;
            }
            break;
        }
        case SELECCION_MENU_INFO_WIFI_SSID: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_INFO_WIFI_STATUS;
                    break;
                case BUTTON_STATUS_RIGHT:
                    _nextMenu = SELECCION_MENU_INFO_WIFI_IP;
                    break;
            }
            break;
        }
        case SELECCION_MENU_INFO_WIFI_IP: {
            switch (_localButton) {
                case BUTTON_STATUS_LEFT:
                    _nextMenu = SELECCION_MENU_INFO_WIFI_SSID;
                    break;
            }
            break;
        }
    }
    *_localMenu = _nextMenu;
}

/*
void actionSubirPersiana()
{
    uint8_t _localShutterIndex = (_seleccionMenu - SELECCION_MENU_NONE) - 1;
    if (ShutterData[_localShutterIndex].status != SHUTTER_STATUS_STOPPED)
    {
        PararPersiana(_localShutterIndex);
    }
    else
    {
        subirPersiana(_localShutterIndex);
    }
}

void actionBajarPersiana()
{
    uint8_t _localShutterIndex = (_seleccionMenu - SELECCION_MENU_NONE) - 1;
    if (ShutterData[_localShutterIndex].status != SHUTTER_STATUS_STOPPED)
    {
        PararPersiana(_localShutterIndex);
    }
    else
    {
        bajarPersiana(_localShutterIndex);
    }
}

void actionJobMode()
{
}

void actionIncreaseDaySelected()
{
}

void actionDecreaseDaySelected()
{
}

void actionIncreaseMonthSelected()
{
}

void actionDecreaseMonthSelected()
{
}

void actionRegisterSleepMode()
{
}

// Define a structure to represent menu transitions
struct MenuTransition {
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    void (*actionLeft)();
    void (*actionRight)();
    void (*actionUp)();
    void (*actionDown)();
};

// Define menu transitions for each menu item with associated actions
const struct MenuTransition menuTransitions[] = {
    {SELECCION_MENU_NONE, SELECCION_MENU_NONE, SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL,
NULL, NULL}, {SELECCION_MENU_INFO, SELECCION_MENU_PERSIANA_CENTRAL, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, NULL, actionSubirPersiana, actionBajarPersiana},
    {SELECCION_MENU_PERSIANA_IZQUIERDA, SELECCION_MENU_PERSIANA_DERECHA, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, NULL, actionSubirPersiana, actionBajarPersiana},
    {SELECCION_MENU_PERSIANA_CENTRAL, SELECCION_MENU_JOB_MODE, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, NULL, actionSubirPersiana, actionBajarPersiana},
    {SELECCION_MENU_PERSIANA_DERECHA, SELECCION_MENU_SLEEP_MODE, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, NULL, NULL, actionJobMode}, {SELECCION_MENU_JOB_MODE,
SELECCION_MENU_INFO, SELECCION_MENU_NONE, SELECCION_MENU_SLEEP_MODE_ACTIVATE, NULL, NULL, NULL,
NULL}, {SELECCION_MENU_SLEEP_MODE, SELECCION_MENU_SLEEP_MODE_DEACTIVATE, SELECCION_MENU_NONE,
SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY, NULL, NULL, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_ACTIVATE, SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH,
SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL, actionIncreaseDaySelected,
actionDecreaseDaySelected}, {SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY,
SELECCION_MENU_SLEEP_MODE_ACTIVATE_CONFIRM, SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL,
actionIncreaseMonthSelected, actionDecreaseMonthSelected},
    {SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH, SELECCION_MENU_SLEEP_MODE,
SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, actionRegisterSleepMode, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_ACTIVATE, SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL,
SELECCION_MENU_NONE, SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY, NULL, NULL, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_DEACTIVATE, SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY,
SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL, actionIncreaseDaySelected,
actionDecreaseDaySelected}, {SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY,
SELECCION_MENU_SLEEP_MODE_DEACTIVATE_CONFIRM, SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL,
actionIncreaseMonthSelected, actionDecreaseMonthSelected},
    {SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_MONTH, SELECCION_MENU_SLEEP_MODE,
SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, actionRegisterSleepMode, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_DEACTIVATE, SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL,
SELECCION_MENU_NONE, SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL_CONFIRM, NULL, NULL, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL, SELECCION_MENU_SLEEP_MODE, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, actionRegisterSleepMode, NULL, NULL},
    {SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL, SELECCION_MENU_NONE, SELECCION_MENU_NONE,
SELECCION_MENU_DEACTIVATE_ALL, NULL, NULL, NULL, NULL}, {SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL,
SELECCION_MENU_SLEEP_MODE, SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, actionRegisterSleepMode,
NULL, NULL}, {SELECCION_MENU_SLEEP_MODE, SELECCION_MENU_PERSIANA_IZQUIERDA, SELECCION_MENU_NONE,
SELECCION_MENU_INFO_WIFI_STATUS, NULL, NULL, NULL, NULL}, {SELECCION_MENU_INFO,
SELECCION_MENU_INFO_WIFI_SSID, SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL, NULL, NULL},
    {SELECCION_MENU_INFO_WIFI_STATUS, SELECCION_MENU_INFO_WIFI_IP, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, NULL, NULL, NULL, NULL}, {SELECCION_MENU_INFO_WIFI_SSID, SELECCION_MENU_NONE,
SELECCION_MENU_NONE, SELECCION_MENU_NONE, NULL, NULL, NULL, NULL},
};

// Define a function to handle button presses and menu transitions
void handleButtonFromMenu(uint8_t * _localMenu, uint8_t _localButton) {
    uint8_t _nextMenu = *_localMenu;

    if (_nextMenu < sizeof(menuTransitions) / sizeof(menuTransitions[0])) {
        const struct MenuTransition *transitions = &menuTransitions[_nextMenu];

        switch (_localButton) {
            case BUTTON_STATUS_LEFT:
                if (transitions->actionLeft) {
                    transitions->actionLeft();
                }
                if ( transitions->left != SELECCION_MENU_NONE)
                {
                    _nextMenu = transitions->left;
                }
                break;
            case BUTTON_STATUS_RIGHT:
                if (transitions->actionRight) {
                    transitions->actionRight();
                }
                if ( transitions->right != SELECCION_MENU_NONE)
                {
                    _nextMenu = transitions->right;
                }
                break;
            case BUTTON_STATUS_UP:
                if (transitions->actionUp) {
                    transitions->actionUp();
                }
                if ( transitions->up != SELECCION_MENU_NONE)
                {
                    _nextMenu = transitions->up;
                }
                break;
            case BUTTON_STATUS_DOWN:
                if (transitions->actionDown) {
                    transitions->actionDown();
                }
                if ( transitions->down != SELECCION_MENU_NONE)
                {
                    _nextMenu = transitions->down;
                }
                break;
        }
    }

    *_localMenu = _nextMenu;
}
*/

void actualizarMenuPantalla(uint8_t _localMenu) {
    String lcdBuffer = "";
    switch (_localMenu) {
        case SELECCION_MENU_NONE:
            break;
        case SELECCION_MENU_PERSIANA_IZQUIERDA:
            // fall through
        case SELECCION_MENU_PERSIANA_CENTRAL:
            // fall through
        case SELECCION_MENU_PERSIANA_DERECHA: {
            String namePersiana[] = { " PERSIANA IZQDA ", "PERSIANA CENTRAL", "PERSIANA DERECHA" };
            uint8_t _localShutterIndex = (SELECCION_MENU_PERSIANA_TO_INDEX(_localMenu));

            lcdBuffer += namePersiana[_localShutterIndex];
            lcdBuffer += String("<    ");

            if (ShutterData[_localShutterIndex].status == SHUTTER_STATUS_MOVING_UP) {
                lcdBuffer += LCD_SPECIAL_CHAR_STOP_ARROW;
            } else {
                lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
            }

            lcdBuffer += String("    ");

            if (ShutterData[_localShutterIndex].status == SHUTTER_STATUS_MOVING_DOWN) {
                lcdBuffer += LCD_SPECIAL_CHAR_STOP_ARROW;
            } else {
                lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            }

            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_JOB_MODE: {
            lcdBuffer += String("  MODO TRABAJO  <    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE: {
            lcdBuffer += String("  MODO DORMIR   <    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE: {
            lcdBuffer += String(" ACTIVAR UN DIA ");
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_DAY: {
            lcdBuffer += String("SEL. DIA:  ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_SEL_DATE_MONTH: {
            lcdBuffer += String("SEL. MES:  ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += String("<    ");
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_CONFIRM: {
            lcdBuffer += String("CONFIRMAR  FECHA");
            lcdBuffer += String("<    ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += String("    OK");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE: {
            lcdBuffer += String("CANCELAR UN DIA <    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_DAY: {
            lcdBuffer += String("SEL. DIA:  ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_SEL_DATE_MONTH: {
            lcdBuffer += String("SEL. MES:  ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += String("<    ");
            lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW;
            lcdBuffer += String("    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_CONFIRM: {
            lcdBuffer += String("CONFIRMAR  FECHA");
            lcdBuffer += String("<    ");
            lcdBuffer += makeLcdStringDate(sleepModeDaySelected, sleepModeMonthSelected);
            lcdBuffer += String("    OK");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL: {
            lcdBuffer += String("ACTIVAR  SIEMPRE<    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_ACTIVATE_ALL_CONFIRM: {
            lcdBuffer += String("ACTIVAR SIEMPRE?");
            lcdBuffer += String("<             OK");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL: {
            lcdBuffer += String(" CANCELAR TODO  <    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("     ");
            break;
        }
        case SELECCION_MENU_SLEEP_MODE_DEACTIVATE_ALL_CONFIRM: {
            lcdBuffer += String(" CANCELAR TODO? ");
            lcdBuffer += String("<             OK");
            break;
        }
        case SELECCION_MENU_INFO:
            lcdBuffer += String("  WIFI  STATUS  ");
            lcdBuffer += String("<    ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String(" OK ");
            lcdBuffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
            lcdBuffer += String("    >");
            break;
        case SELECCION_MENU_INFO_WIFI_STATUS:
            lcdBuffer += String(" WIFI ");
            if (ESP8266Utils_is_STA_connected()) {
                lcdBuffer   += String("(STA): ");
                lcdBuffer   += ESP8266Utils_is_STA_online() ? String("ON ") : String("OFF");
                lcdBuffer   += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
                lcdBuffer   += String("  RSSI: ");
                String rssi  = String(ESP8266Utils_get_STA_RSSI());
                for (int i = 0; i < 4 - rssi.length(); i++) {
                    lcdBuffer += String(" ");
                }
                lcdBuffer += rssi;
                lcdBuffer += String("  >");
            } else if (ESP8266Utils_is_AP_created()) {
                lcdBuffer         += String(": AP Mode ");
                lcdBuffer         += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
                lcdBuffer         += String(" Clients : ");
                int _localClients  = ESP8266Utils_get_AP_clients();
                if (_localClients < 10) {
                    lcdBuffer += String("0");
                }
                lcdBuffer += String(_localClients);
                lcdBuffer += String(" >");
            } else {
                lcdBuffer += String(": OFFLINE ");
                lcdBuffer += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
                lcdBuffer += String("              >");
            }
            break;
        case SELECCION_MENU_INFO_WIFI_SSID: {
            static String _locaBuffer;
            static uint8_t _localBufferIndex;
            static uint32_t timeout_ms;
            if (ESP8266Utils_is_STA_connected()) {
                if (((millis() - timeout_ms) > LCD_SLIDE_SPEED_MS) || _locaBuffer == "") {
                    timeout_ms   = millis();
                    _locaBuffer  = "               SSID: ";
                    _locaBuffer += ESP8266Utils_get_STA_SSID();
                    _locaBuffer += String("                ");

                    _localBufferIndex++;
                    if (_localBufferIndex > (_locaBuffer.length() - 16)) {
                        _localBufferIndex = 0;
                    }
                    _locaBuffer = _locaBuffer.substring(_localBufferIndex, _localBufferIndex + 16);
                }
                lcdBuffer += _locaBuffer;
                lcdBuffer += String("<              >");
            } else if (ESP8266Utils_is_AP_created()) {
                if (((millis() - timeout_ms) > LCD_SLIDE_SPEED_MS) || _locaBuffer == "") {
                    timeout_ms   = millis();
                    _locaBuffer  = "               SSID (AP): ";
                    _locaBuffer += ESP8266Utils_get_AP_SSID();
                    _locaBuffer += String(" PWD: ");
                    _locaBuffer += ESP8266Utils_get_AP_PWD();
                    _locaBuffer += String("                ");

                    _localBufferIndex++;
                    if (_localBufferIndex > (_locaBuffer.length() - 16)) {
                        _localBufferIndex = 0;
                    }
                    _locaBuffer = _locaBuffer.substring(_localBufferIndex, _localBufferIndex + 16);
                }
                lcdBuffer += _locaBuffer;
                lcdBuffer += String("<              >");
            } else {
                lcdBuffer += String("  SSID STA & AP ");
                lcdBuffer += String("<  NOT AVAIL.  >");
            }
            break;
        }
        case SELECCION_MENU_INFO_WIFI_IP: {
            static String _locaBuffer;
            static uint8_t _localBufferIndex;
            static uint32_t timeout_ms;
            if (((millis() - timeout_ms) > LCD_SLIDE_SPEED_MS) || _locaBuffer == "") {
                timeout_ms   = millis();
                _locaBuffer  = "               IP: ";
                _locaBuffer += ESP8266Utils_get_hostname();
                _locaBuffer += String("                ");

                _localBufferIndex++;
                if (_localBufferIndex > (_locaBuffer.length() - 16)) {
                    _localBufferIndex = 0;
                }
                _locaBuffer = _locaBuffer.substring(_localBufferIndex, _localBufferIndex + 16);
            }
            lcdBuffer += _locaBuffer;
            lcdBuffer += String("<               ");
            break;
        }
    }
    sendLcdBuffer(lcdBuffer);
}
