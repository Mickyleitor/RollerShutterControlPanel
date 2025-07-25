#pragma once

#include <ESP8266WiFi.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"

#define HTTP_SERVER_PING_ADDRESS                                       "1.1.1.1"
#define HTTP_SERVER_PING_INTERVAL_MS                                     (10000)

#define TEMPERATURE_DEGREE_INVALID                                       (65535)

extern struct Settings settings;

struct WeatherData {
    unsigned long timezoneshift = 0;
    double TemperatureDegree    = TEMPERATURE_DEGREE_INVALID;
} MyWeather;

bool ESP8266Utils_Connect_STA(
        const char* ssid,
        const char* password,
        const char* hostname,
        int32_t timeout_ms = 10000) {
    Serial.println("");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED && timeout_ms > 0) {
        delay(100);
        timeout_ms -= 100;
        yield();
    }

    if (timeout_ms <= 0) {
        Serial.println("STA Failed");
        return false;
    }

    Serial.println("");
    Serial.print("Iniciado STA:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    return true;
}

bool ESP8266Utils_is_STA_connected() { return WiFi.isConnected(); }

bool ESP8266Utils_is_STA_online() {
    if (!ESP8266Utils_is_STA_connected()) {
        return false;
    }
    static bool isOnline;
    static uint32_t timeout_ms = 0;
    if ((millis() - timeout_ms) > HTTP_SERVER_PING_INTERVAL_MS) {
        timeout_ms = millis();
        isOnline   = true;
        WiFiClient client;
        // Ping the server to see if it is online
        client.setTimeout(HTTP_SERVER_REQUEST_TIMEOUT);
        if (!client.connect(HTTP_SERVER_PING_ADDRESS, 80)) {
            isOnline = false;
        }
        client.stop();
    }
    return isOnline;
}

int8_t ESP8266Utils_get_STA_RSSI() { return WiFi.RSSI(); }

String ESP8266Utils_get_STA_SSID() { return WiFi.SSID(); }

bool ESP8266Utils_update_WeatherData(struct Settings* myData) {
    if (!ESP8266Utils_is_STA_online()) {
        Serial.println("No internet connection");
        return false;
    }

    String default_appid = DEFAULT_OPENWEATHERMAP_APPID;
    if ((String(myData->openWeatherMapSettings.appid) == default_appid)
        || (String(myData->openWeatherMapSettings.appid) == String(""))) {
        Serial.println("Invalid Weather API key : " + String(myData->openWeatherMapSettings.appid));
        return false;
    }

    WiFiClient client;
    // Connect to HTTP server
    client.setTimeout(HTTP_SERVER_REQUEST_TIMEOUT);
    if (!client.connect(OPENWEATHERMAP_HOST, OPENWEATHERMAP_PORT)) {
        Serial.println("Connection failed");
        // Disconnect
        client.stop();
        return false;
    }

    // Send HTTP request
    String HTTPrequest = DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(
            myData->openWeatherMapSettings.appid,
            myData->openWeatherMapSettings.lat,
            myData->openWeatherMapSettings.lon);
    client.println(HTTPrequest);
    client.println("Host: " + String(OPENWEATHERMAP_HOST));
    client.println("Connection: close");
    if (client.println() == 0) {
        Serial.println("Failed to send request");
        // Disconnect
        client.stop();
        return false;
    }
    // Skip HTTP headers
    if (!client.find("\r\n\r\n")) {
        Serial.println("Invalid response");
        // Disconnect
        client.stop();
        return false;
    }
    if (client.find("\"temp\":")) {
        double NewTemp = client.readStringUntil(',').toDouble();
        if (NewTemp > 273) {
            NewTemp -= 273.15;
        }
        Serial.print("Temperature: ");
        Serial.println(NewTemp);
        MyWeather.TemperatureDegree = NewTemp;
    } else {
        Serial.println("No Temperature JSON object found");
    }
    if (client.find("\"timezone\":")) {
        unsigned long timezoneshift = strtoul(client.readStringUntil(',').c_str(), NULL, 10);
        Serial.print("Timezone shift: ");
        Serial.println(timezoneshift);
        MyWeather.timezoneshift = timezoneshift;
    } else {
        Serial.println("No timezoneshift JSON object found");
    }

    // Disconnect
    client.stop();
    return true;
}
