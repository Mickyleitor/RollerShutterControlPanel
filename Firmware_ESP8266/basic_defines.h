#pragma once

#define SLAVE_I2C_ADDRESS                                                 (0x08)
#define LCD_I2C_ADDRESS                                                   (0x3F)

#define DEFAULT_BUZZER_VOLUME                                             (4000)
#define DEFAULT_STA_SSID                                             "YOUR_SSID"
#define DEFAULT_STA_PASSWORD                                     "YOUR_PASSWORD"
#define DEFAULT_AP_SSID_AND_PASSWORD                             "ESP8266Config"
#define DEFAULT_HOSTNAME                                         "esp8266config"

#define OPENWEATHERMAP_HOST                             "api.openweathermap.org"
#define OPENWEATHERMAP_PORT                                                 (80)
#define DEFAULT_OPENWEATHERMAP_APPID                              "YOUR_API_KEY"
#define DEFAULT_OPENWEATHERMAP_LOCATION_LAT                                  (0)
#define DEFAULT_OPENWEATHERMAP_LOCATION_LON                                  (0)

#define HTTP_SERVER_REQUEST_TIMEOUT                                       (2000)
#define WIFI_CONNECTION_TIMEOUT                                          (10000)

#define DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(appid, lat, long) \
    "GET /data/2.5/weather?lat=" + \
    String(lat) + \
    "&lon=" + \
    String(long) + \
    "&appid=" + \
    String(appid) + \
    "&units=metric&lang=es HTTP/1.0"

#define EEPROM_SIZE                                    (sizeof(struct Settings))

enum SystemState {
    SYSTEM_STATE_ENTERING_IDLE,
    SYSTEM_STATE_IDLING,
    SYSTEM_STATE_WAKEUP,
    SYSTEM_STATE_MENU,
};

typedef uint8_t SystemState_t;

enum seleccionMenu {
    SELECCION_MENU_RELOJ,
    SELECCION_MENU_PERSIANA_IZQUIERDA,
    SELECCION_MENU_PERSIANA_CENTRAL,
    SELECCION_MENU_PERSIANA_DERECHA,

    // Starting from here, if there is a button timeout when some menu below is displayed,
    // it will return to the default shutter menu when we wake up again.
    SELECCION_MENU_CONFIG,
    SELECCION_MENU_CONFIG_FECHA_HORA,
    SELECCION_MENU_CONFIG_FECHA_HORA_AJUSTE,
    SELECCION_MENU_CONFIG_VOLUMEN,
    SELECCION_MENU_CONFIG_VOLUMEN_AJUSTE,

    SELECCION_MENU_MAX
};

typedef uint8_t seleccionMenu_t;

// This macro is used to convert the enum to the index of the array
#define SELECCION_MENU_PERSIANA_TO_INDEX(x)     ((x - SELECCION_MENU_RELOJ) - 1)

// Default selection menu. This can't be the SELECCION_MENU_RELOJ
#define DEFAULT_SELECTION_MENU                 (SELECCION_MENU_PERSIANA_CENTRAL)

enum ButtonStatus {
    BUTTON_STATUS_NONE,
    BUTTON_STATUS_LEFT,
    BUTTON_STATUS_UP,
    BUTTON_STATUS_DOWN,
    BUTTON_STATUS_RIGHT
};

enum ShutterStatus {
    SHUTTER_STATUS_STOPPED,
    SHUTTER_STATUS_MOVING_UP,
    SHUTTER_STATUS_MOVING_DOWN,
};

#define NUMBER_OF_SHUTTERS                                                   (3)

struct ShutterParameters {
    enum ShutterStatus status;
    unsigned long lastUpdate;
};
