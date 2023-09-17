#pragma once

#include <ESP8266WiFi.h>
#include "basic_defines.h"
#include "EEPROM_Utils.h"

struct WeatherData {
  double SunAzimuth = 0;
  double SunElevation = 0;
  time_t sunriseSecondsUTC = 0;
  time_t sunsetSecondsUTC = 0;
  unsigned long timezoneshift = MYTZ;
  double Cloudiness = 0;
  double TemperatureDegree = 0;
} MyWeather;

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

bool getWeatherDataFunction(){

  double NewAzimuth = -9999;
  double NewElevation = -9999;
  SolarAzEl(time(NULL), _storedData._openweathermap_lat, _storedData._openweathermap_lon, 1, &NewAzimuth, &NewElevation);
  if(NewAzimuth > -9999 && NewElevation > -9999){
    MyWeather.SunAzimuth = NewAzimuth;
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
  String HTTPrequest = DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(_storedData._openweathermap_appid, _storedData._openweathermap_lat, _storedData._openweathermap_lon);
  client.println(HTTPrequest);
  client.println("Host : " + String(OPENWEATHERMAP_HOST));
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
  if(client.find("\"temp\":")){
    double NewTemp = client.readStringUntil(',').toDouble();
    if(NewTemp > 273) NewTemp -= 273.15;
    Serial.print("Temperature: ");
    Serial.println(NewTemp);
    MyWeather.TemperatureDegree = NewTemp;
  }else{
    Serial.println("No Temperature JSON object found");
  }
  if(client.find("\"clouds\":{\"all\":")){
    double NewCloudiness = client.readStringUntil('}').toDouble();
    Serial.print("Cloudiness: ");
    Serial.print(NewCloudiness);
    Serial.println("%");
    MyWeather.Cloudiness = NewCloudiness;
  }else{
    Serial.println("No Cloudiness JSON object found");
  }
  if(client.find("\"sunrise\":")){
    MyWeather.sunriseSecondsUTC = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10)  % (60 * 60 * 24);
    Serial.print("Current Sunrise UTC time: ");
    Serial.print(ctime(&MyWeather.sunriseSecondsUTC));
  }else{
    Serial.println("No sunrise JSON object found");
  }
  if(client.find("\"sunset\":")){
    MyWeather.sunsetSecondsUTC = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10)  % (60 * 60 * 24);
    Serial.print("Current Sunset UTC time: ");
    Serial.print(ctime(&MyWeather.sunsetSecondsUTC));    
  }else{
    Serial.println("No sunsetTime JSON object found");
  }
  if(client.find("\"timezone\":")){
    unsigned long timezoneshift = strtoul(client.readStringUntil(',').c_str(), NULL, 10);
    Serial.print("Timezone shift: ");
    Serial.println(timezoneshift);
    MyWeather.timezoneshift = timezoneshift;
  }else{
    Serial.println("No timezoneshift JSON object found");
  } 
  // Disconnect
  client.stop();
  return true;
}