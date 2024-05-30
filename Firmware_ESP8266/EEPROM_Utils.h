#pragma once

#include <EEPROM.h>

#include "basic_defines.h"


struct __attribute__((__packed__)) WifiSettings {
    char ssid_sta[64];
    char password_sta[64];
    char ssid_ap[64];
    char password_ap[64];
    char hostname[64];
};

struct __attribute__((__packed__)) OpenWeatherMapSettings {
    char appid[64];
    double lat;
    double lon;
};

struct __attribute__((__packed__)) Settings {
    char ScheduledData[12][31];
    struct WifiSettings wifiSettings;
    struct OpenWeatherMapSettings openWeatherMapSettings;
    uint16_t crc16;
} settings;

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

void EEPROM_Write(struct Settings* data) {
    EEPROM.begin(EEPROM_SIZE);
    data->crc16 = CRC16((uint8_t*)data, EEPROM_SIZE - 2);
    // store values in EEPROM
    for (int index = 0; index < EEPROM_SIZE; index++) {
        if (((char*)data)[index] != EEPROM.read(index)) {
            EEPROM.write(index, ((char*)data)[index]);
        }
    }
    EEPROM.commit();
    EEPROM.end();
}

void EEPROM_Read(struct Settings* data) {
    EEPROM.begin(EEPROM_SIZE);
    for (int index = 0; index < EEPROM_SIZE; index++) {
        ((char*)data)[index] = EEPROM.read(index);
    }
    EEPROM.end();
}

void EEPROM_Check(struct Settings* data) {
    // This is the very first time the device is powered on
    uint16_t crc16 = CRC16((uint8_t*)data, EEPROM_SIZE - 2);
    if (crc16 != data->crc16) {
        Serial.println("Corrupted data in EEPROM, writing default values");
        memset(data->ScheduledData, 0x03, sizeof(data->ScheduledData));
        strcpy(data->wifiSettings.hostname, DEFAULT_HOSTNAME);
        strcpy(data->wifiSettings.ssid_sta, DEFAULT_STA_SSID);
        strcpy(data->wifiSettings.password_sta, DEFAULT_STA_PASSWORD);
        strcpy(data->wifiSettings.ssid_ap, DEFAULT_AP_SSID_AND_PASSWORD);
        strcpy(data->wifiSettings.password_ap, DEFAULT_AP_SSID_AND_PASSWORD);
        strcpy(data->openWeatherMapSettings.appid, DEFAULT_OPENWEATHERMAP_APPID);
        data->openWeatherMapSettings.lat = DEFAULT_OPENWEATHERMAP_LOCATION_LAT;
        data->openWeatherMapSettings.lon = DEFAULT_OPENWEATHERMAP_LOCATION_LON;
        EEPROM_Write(data);
    }
}

void EEPROM_Begin(struct Settings* data) {
    EEPROM_Read(data); // Comment this line to reset the EEPROM
    EEPROM_Check(data);
    Serial.println("EEPROM size : " + String(EEPROM_SIZE));
}
