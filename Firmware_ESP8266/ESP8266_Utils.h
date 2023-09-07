#pragma once

#include <ESP8266WiFi.h>

#define DEFAULT_STA_SSID                                             "YOUR_SSID"
#define DEFAULT_STA_PASSWORD                                     "YOUR_PASSWORD"
#define DEFAULT_AP_SSID_AND_PASSWORD                             "ESP8266Config"

#define DEFAULT_OPENWEATHERMAP_APPID                              "YOUR_API_KEY"
#define DEFAULT_OPENWEATHERMAP_LOCATION_LAT                                  (0)
#define DEFAULT_OPENWEATHERMAP_LOCATION_LON                                  (0)

#define DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(appid, lat, long) \
    "GET /data/2.5/weather?lat=" + \
    String(lat) + \
    "&lon=" + \
    String(long) + \
    "&appid=" + \
    String(appid) + \
    "&units=metric&lang=es HTTP/1.0"

void initTimeFunction() {
  // We want UTC time.
  int dst = 0;
  configTime(0, dst * 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
  while (!time(nullptr)){
    yield();
    delay(1000);
  }
}

bool ConnectWiFi_STA(char * ssid, char * password, int32_t timeout = 10000)
{
    Serial.println("");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED && timeout > 0) 
    {
        delay(100);
        timeout -= 100;
        yield();
    }

    if(timeout <= 0){
        Serial.println("STA Failed");
        return false;
    }

    Serial.println("");
    Serial.print("Iniciado STA:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    initTimeFunction();

    return true;
}

bool ConnectWiFi_AP(char * ssid, char * password, int32_t timeout = 10000)
{ 
   Serial.println("");
   WiFi.mode(WIFI_AP);
    while(!WiFi.softAP(ssid, password) && timeout > 0) 
    {
        delay(100);
        timeout -= 100;
        yield();
    }

    if(timeout <= 0){
        Serial.println("AP Failed");
        return false;
    }

    Serial.println("");
    Serial.print("Iniciado AP:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());

    return true;
}