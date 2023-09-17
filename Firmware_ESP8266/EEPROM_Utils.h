#pragma once

#include <EEPROM.h>
#include "basic_defines.h"

struct __attribute__ ((__packed__)) EEPROM_Data {
    char ScheduledData[12][31];
    char _ssid_sta[32];
    char _password_sta[32];
    char _ssid_ap[32];
    char _password_ap[32];
    char _openweathermap_appid[64];
    double _openweathermap_lat;
    double _openweathermap_lon;
    uint16_t crc16;
} _storedData;

uint16_t CRC16(const uint8_t * data, uint16_t size){
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < size; i++){
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++){
            if (crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc;
}

void EEPROM_Write(struct EEPROM_Data * data){
    EEPROM.begin(EEPROM_SIZE);
    data->crc16 = CRC16((uint8_t *)data, EEPROM_SIZE - 2);
    // store values in EEPROM
    for (int index = 0; index < EEPROM_SIZE; index++) {
        if( ((char *)data)[index] != EEPROM.read(index) ){
            EEPROM.write(index, ((char *)data)[index]);
        }
    }
    EEPROM.commit();
    EEPROM.end();
}

void EEPROM_Read(struct EEPROM_Data * data){
    EEPROM.begin(EEPROM_SIZE);
    for (int index = 0; index < EEPROM_SIZE; index++){
        ((char *)data)[index] = EEPROM.read(index);
    }
    EEPROM.end();
}

void EEPROM_Check(struct EEPROM_Data * data){
    // This is the very first time the device is powered on
    uint16_t crc16 = CRC16((uint8_t *)data, EEPROM_SIZE - 2);
    if( crc16 != data->crc16 ){
        Serial.println("Corrupted data in EEPROM, writing default values");
        memset(data->ScheduledData, 0x03, sizeof(data->ScheduledData));
        strcpy(data->_ssid_sta, DEFAULT_STA_SSID);
        strcpy(data->_password_sta, DEFAULT_STA_PASSWORD);
        strcpy(data->_ssid_ap, DEFAULT_AP_SSID_AND_PASSWORD);
        strcpy(data->_password_ap, DEFAULT_AP_SSID_AND_PASSWORD);
        strcpy(data->_openweathermap_appid, DEFAULT_OPENWEATHERMAP_APPID);
        data->_openweathermap_lat = DEFAULT_OPENWEATHERMAP_LOCATION_LAT;
        data->_openweathermap_lon = DEFAULT_OPENWEATHERMAP_LOCATION_LON;
        EEPROM_Write(data);
    }
}
