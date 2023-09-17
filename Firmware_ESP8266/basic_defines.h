#pragma once

#define SLAVE_I2C_ADDRESS                                                 (0x08)
#define LCD_I2C_ADDRESS                                                   (0x3F)

#ifndef TZ_Europe_Madrid
#define TZ_Europe_Madrid 2 * 3600
#endif
#define MYTZ TZ_Europe_Madrid
#define UPDATE_SCREEN_SECONDS 60
#define SHUTTER_DURATION_SECONDS 27
#define SYSTEM_MANAGER_SECONDS 60 * 5

// Flag mask macro for I2C message
#define FLAG_BIT_BUZZER 0x80
#define COMMAND_BUZZER_LOW_VOLUME (FLAG_BIT_BUZZER | 0x0)
#define COMMAND_BUZZER_MEDIUM_VOLUME (FLAG_BIT_BUZZER | 0x20)
#define COMMAND_BUZZER_HIGH_VOLUME (FLAG_BIT_BUZZER | 0x40)
#define COMMAND_BUZZER_CHANGE (FLAG_BIT_BUZZER | 0x60)
#define COMMAND_LIGHT_ON 0x20
#define COMMAND_LIGHT_OFF 0x40
#define COMMAND_LIGHT_CHANGE 0x60

#define FLAG_MASK_BUZZER ( FLAG_BIT_BUZZER | COMMAND_BUZZER_LOW_VOLUME |COMMAND_BUZZER_MEDIUM_VOLUME | COMMAND_BUZZER_HIGH_VOLUME)
#define FLAG_MASK_LIGHT ( COMMAND_LIGHT_ON | COMMAND_LIGHT_OFF | COMMAND_LIGHT_CHANGE )

#define RSCP_TIMEOUT_MS                                                   (1000)

#define DEFAULT_STA_SSID                                             "YOUR_SSID"
#define DEFAULT_STA_PASSWORD                                     "YOUR_PASSWORD"
#define DEFAULT_AP_SSID_AND_PASSWORD                             "ESP8266Config"

#define OPENWEATHERMAP_HOST                             "api.openweathermap.org"
#define OPENWEATHERMAP_PORT                                                 (80)
#define DEFAULT_OPENWEATHERMAP_APPID                              "YOUR_API_KEY"
#define DEFAULT_OPENWEATHERMAP_LOCATION_LAT                                  (0)
#define DEFAULT_OPENWEATHERMAP_LOCATION_LON                                  (0)
#define HTTP_SERVER_REQUEST_TIMEOUT                                      (10000)

#define DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(appid, lat, long) \
    "GET /data/2.5/weather?lat=" + \
    String(lat) + \
    "&lon=" + \
    String(long) + \
    "&appid=" + \
    String(appid) + \
    "&units=metric&lang=es HTTP/1.0"

#define EEPROM_SIZE                                 (sizeof(struct EEPROM_Data))

enum SystemState {
    SYSTEM_STATE_WIFI_ACCESS_POINT_OPENED,
    SYSTEM_STATE_WIFI_STATION_CONNECTED,
    SYSTEM_STATE_ENTERING_IDLE,
    SYSTEM_STATE_IDLING,
    SYSTEM_STATE_WAKEUP,
    SYSTEM_STATE_SHUTTER_MANAGER,
    SYSTEM_STATE_SLEEP_MANAGER,

    // This below are menu states (in the future they will be moved to a new file)
    SYSTEM_STATE_MENU_JOB_MODE,
    SYSTEM_STATE_MENU_SLEEP_MODE,
    SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE,
    SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE,
    SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE,
    SYSTEM_STATE_MENU_DEACTIVATE_ALL_SLEEP_MODE,
};

enum seleccionMenu {
    SELECCION_MENU_PERSIANA_IZQUIERDA,
    SELECCION_MENU_PERSIANA_CENTRAL,
    SELECCION_MENU_PERSIANA_DERECHA,
    SELECCION_MENU_OPTION_JOB_MODE,
    SELECCION_MENU_OPTION_SLEEP_MODE,
};

enum ButtonState {
    BUTTON_STATE_NONE,
    BUTTON_STATE_LEFT,
    BUTTON_STATE_UP,
    BUTTON_STATE_DOWN,
    BUTTON_STATE_RIGHT
};

struct ShutterParameters {
  int status;
  int LastMoved;
};