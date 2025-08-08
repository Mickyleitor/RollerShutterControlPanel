#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "basic_defines.h"

static unsigned long EEPROM_Scheduled_Write_At    = 0;
static unsigned long EEPROM_Scheduled_Write_Delay = 0;
static bool EEPROM_Scheduled_Write_Enabled        = false;

struct WifiSettings {
    char ssid_sta[64];
    char password_sta[64];
    bool wifi_enabled;
    bool ota_enabled;
};

struct OpenWeatherMapSettings {
    char appid[64];
    double lat;
    double lon;
};

struct BuzzerSettings {
    uint32_t general_volume; // Volume
    bool isEnabled;          // True if buzzer is enabled
};

struct Settings {
    struct WifiSettings wifiSettings;
    struct OpenWeatherMapSettings openWeatherMapSettings;
    struct BuzzerSettings buzzerSettings;
    uint16_t crc16;
} settings;

static_assert(sizeof(Settings) % 4 == 0, "Settings must be aligned to 4 bytes");

uint16_t CRC16(const uint8_t* data, uint16_t size) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < size; i++) {
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc  ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void EEPROM_Schedule_Write(unsigned long delayMs) {
    EEPROM_Scheduled_Write_At      = millis();
    EEPROM_Scheduled_Write_Delay   = delayMs;
    EEPROM_Scheduled_Write_Enabled = true;
}

void EEPROM_Write(Settings* data) {
    if (!data) {
        return;
    }
    noInterrupts();
    EEPROM.begin(sizeof(Settings));

    Settings copy = *data;
    copy.crc16    = 0;

    static uint8_t buffer[sizeof(Settings)];
    memcpy(buffer, &copy, sizeof(Settings));
    copy.crc16 = CRC16(buffer, sizeof(Settings) - sizeof(copy.crc16));
    memcpy(buffer, &copy, sizeof(Settings));

    for (size_t i = 0; i < sizeof(Settings); ++i) {
        if (EEPROM.read(i) != buffer[i]) {
            EEPROM.write(i, buffer[i]);
        }
        delayMicroseconds(1); // Allow other tasks to run
    }

    EEPROM.commit();
    EEPROM.end();
    interrupts();
}

void EEPROM_Handler() {
    if (EEPROM_Scheduled_Write_Enabled) {
        if ((millis() - EEPROM_Scheduled_Write_At) >= EEPROM_Scheduled_Write_Delay) {
            EEPROM_Scheduled_Write_Enabled = false;
            EEPROM_Write(&settings);
        }
    }
}

bool EEPROM_Read(Settings* out) {
    if (!out) {
        return false;
    }

    EEPROM.begin(sizeof(Settings));

    static uint8_t buffer[sizeof(Settings)];
    for (size_t i = 0; i < sizeof(Settings); ++i) {
        buffer[i] = EEPROM.read(i);
        yield();
    }
    EEPROM.end();

    Settings copy;
    memcpy(&copy, buffer, sizeof(Settings));

    uint16_t stored_crc = copy.crc16;
    copy.crc16          = 0;

    memcpy(buffer, &copy, sizeof(Settings));
    uint16_t calc_crc = CRC16(buffer, sizeof(Settings) - sizeof(copy.crc16));

    if (stored_crc != calc_crc) {
        Serial.println(
                "EEPROM data CRC mismatch. Expected: " + String(stored_crc)
                + ", Calculated: " + String(calc_crc));
        return false;
    }

    *out = copy;
    return true;
}

void EEPROM_Write_Default(Settings* data) {
    if (data == nullptr) {
        return;
    }

    Settings defaultSettings = { 0 };
    strcpy(defaultSettings.wifiSettings.ssid_sta, DEFAULT_STA_SSID);
    strcpy(defaultSettings.wifiSettings.password_sta, DEFAULT_STA_PASSWORD);
    defaultSettings.wifiSettings.wifi_enabled = DEFAULT_WIFI_ENABLED;
    defaultSettings.wifiSettings.ota_enabled  = DEFAULT_WIFI_OTA_ENABLED;
    strcpy(defaultSettings.openWeatherMapSettings.appid, DEFAULT_OPENWEATHERMAP_APPID);
    defaultSettings.openWeatherMapSettings.lat    = DEFAULT_OPENWEATHERMAP_LOCATION_LAT;
    defaultSettings.openWeatherMapSettings.lon    = DEFAULT_OPENWEATHERMAP_LOCATION_LON;
    defaultSettings.buzzerSettings.general_volume = DEFAULT_BUZZER_VOLUME;
    defaultSettings.buzzerSettings.isEnabled      = DEFAULT_BUZZER_ENABLED;
    EEPROM_Write(&defaultSettings);
    *data = defaultSettings;
}

void EEPROM_Begin(Settings* data) {
    if (data == nullptr) {
        return;
    }
    if (!EEPROM_Read(data)) {
        Serial.println("EEPROM data not valid, writing default settings.");
        EEPROM_Write_Default(data);
    }
}
