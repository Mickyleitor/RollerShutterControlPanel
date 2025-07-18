#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "EEPROM_Utils.h"
#include "SolarAzEl.h"
#include "basic_defines.h"


#define HTTP_SERVER_PING_ADDRESS                                       "1.1.1.1"
#define HTTP_SERVER_PING_INTERVAL_MS                                     (10000)

extern struct Settings settings;

ESP8266WebServer server(80);

struct WeatherData {
    double SunAzimuth           = 0;
    double SunElevation         = 0;
    time_t sunriseSecondsUTC    = 0;
    time_t sunsetSecondsUTC     = 0;
    unsigned long timezoneshift = MYTZ;
    double Cloudiness           = 255;
    double TemperatureDegree    = 65535;
} MyWeather;

void InitServer() {
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "Hello from ESP8266");
    });
    server.begin();
    MDNS.addService("http", "tcp", 80);
}

bool initTime(int32_t timeout_ms = 10000) {
    // We want UTC time.
    int dst = 0;
    configTime(0, dst * 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
    while (!time(nullptr) && timeout_ms > 0) {
        yield();
        delay(1000);
        timeout_ms -= 1000;
    }

    return (timeout_ms > 0);
}

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

    MDNS.begin(hostname);

    Serial.println("");
    Serial.print("Iniciado STA:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    if (!initTime()) {
        Serial.println("Failed to get time from NTP server");
        return false;
    }

    InitServer();

    return true;
}

bool ESP8266Utils_Connect_AP(
        const char* ssid,
        const char* password,
        const char* hostname,
        int32_t timeout_ms = 10000) {
    Serial.println("");
    WiFi.mode(WIFI_AP);
    while (!WiFi.softAP(ssid, password) && timeout_ms > 0) {
        delay(100);
        timeout_ms -= 100;
        yield();
    }

    if (timeout_ms <= 0) {
        Serial.println("AP Failed");
        return false;
    }

    MDNS.begin(hostname);

    Serial.println("");
    Serial.print("Iniciado AP:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());

    InitServer();

    return true;
}

void ESP8266Utils_handleWifi() {
    server.handleClient();
    MDNS.update();
}

bool ESP8266Utils_is_STA_connected() { return WiFi.isConnected(); }

bool ESP8266Utils_is_STA_online() {
    if (!ESP8266Utils_is_STA_connected()) {
        return false;
    }
    static bool isOnline;
    static uint32_t timeout_ms;
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

bool ESP8266Utils_is_AP_created() { return (WiFi.getMode() == WIFI_AP); }

String ESP8266Utils_get_AP_SSID() { return WiFi.softAPSSID(); }

String ESP8266Utils_get_AP_PWD() { return WiFi.softAPPSK(); }

uint8_t ESP8266Utils_get_AP_clients() { return WiFi.softAPgetStationNum(); }

String ESP8266Utils_get_hostname() {
    if (MDNS.isRunning()) {
        return (String(settings.wifiSettings.hostname) + String(".local"));
    } else {
        return WiFi.localIP().toString();
    }
}

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

    double NewAzimuth   = -9999;
    double NewElevation = -9999;
    SolarAzEl(
            time(NULL),
            myData->openWeatherMapSettings.lat,
            myData->openWeatherMapSettings.lon,
            1,
            &NewAzimuth,
            &NewElevation);
    if (NewAzimuth > -9999 && NewElevation > -9999) {
        MyWeather.SunAzimuth   = NewAzimuth;
        MyWeather.SunElevation = NewElevation;
    }
    Serial.print("SunAzimuth :");
    Serial.print(MyWeather.SunAzimuth);
    Serial.print(" SunElevation : ");
    Serial.println(MyWeather.SunElevation);

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
    if (client.find("\"clouds\":{\"all\":")) {
        double NewCloudiness = client.readStringUntil('}').toDouble();
        Serial.print("Cloudiness: ");
        Serial.print(NewCloudiness);
        Serial.println("%");
        MyWeather.Cloudiness = NewCloudiness;
    } else {
        Serial.println("No Cloudiness JSON object found");
    }
    if (client.find("\"sunrise\":")) {
        MyWeather.sunriseSecondsUTC
                = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10) % (60 * 60 * 24);
        Serial.print("Current Sunrise UTC time: ");
        Serial.print(ctime(&MyWeather.sunriseSecondsUTC));
    } else {
        Serial.println("No sunrise JSON object found");
    }
    if (client.find("\"sunset\":")) {
        MyWeather.sunsetSecondsUTC
                = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10) % (60 * 60 * 24);
        Serial.print("Current Sunset UTC time: ");
        Serial.print(ctime(&MyWeather.sunsetSecondsUTC));
    } else {
        Serial.println("No sunsetTime JSON object found");
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
