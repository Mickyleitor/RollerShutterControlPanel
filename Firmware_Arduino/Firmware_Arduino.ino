// Domotic Shutter sketch
// This sketch is in development and uses MEGA 2560.
// In the future it should be adapted to ATmega368P

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "WiFiEsp.h"
#include "WiFiEspUdp.h"
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <NTPClient.h> // From https://github.com/MHotchin/NTPClient
#include "basic_defines.h"
#include "wifidata.h" // No incluir en proyecto final

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

#define NTP_REFRESH_RATE 600
#define SUNRISE_REFRESH_RATE 30 // 43200

time_t initTime,sunriseTime;
AlarmId sunriseTaskID;

int status = -1;
int failed = 0;
int cloudiness = -1;
int temperature = -1;

LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);

void setup(){
  Serial.begin(9600);
  initLCDFunction();
  initWifiFunction();
  initTime = getTimeFunction();
  getWeatherFunction();
  // getSunriseTimeFunction();
  // getCloudsFunction();
  // Every 10 min get a the time from NTP server
  // Alarm.timerRepeat(NTP_REFRESH_RATE, getTimeFunction);
  // Get sunrise time every 12 hours (this should be changed to everyday at midnight)
  // Alarm.timerRepeat(SUNRISE_REFRESH_RATE, getSunriseTimeFunction);
}

void loop(){
  /*
  Serial.println("-START TIME-");
  updateSerialScreen(initTime);
  Serial.println("------------");
  Serial.println("--INTERNAL--");
  updateSerialScreen(now());
  Serial.println("------------");
  Serial.println("---SUNRISE--");
  updateSerialScreen(sunriseTime);
  Serial.println("------------");
  
  // tone(PIN_BUZZER,25000,25);

  */
  updateMainScreen();
  Alarm.delay(1000);
}
void initLCDFunction(){
  status = -1;
  failed = 0;
  // Search LCD into I2C line:
  while ( status != 0 ) {
    Wire.begin();
    Wire.beginTransmission(ADDRESS_I2C_LCD);
    status = Wire.endTransmission();

    if(failed > 5){
      while(true){
        tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,500);
        delay(1000);
      }
    }else if(status != 0){
      // wait 1 seconds for reconnection:
      delay(5000);
    }
    failed++;
  }
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  int customArrayChar[6][8] = {{0x00,0x00,0x1F,0x1F,0x0E,0x04,0x00,0x00},{0x00,0x00,0x04,0x0E,0x1F,0x1F,0x00,0x00},{0x00,0x00,0x06,0x0E,0x1E,0x0E,0x06,0x00},{0x00,0x00,0x0C,0x0E,0x0F,0x0E,0x0C,0x00},{0x00,0x0E,0x1F,0x11,0x11,0x1F,0x0E,0x00},{0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00}};
  for ( int i = 0 ; i < 6 ; i ++ ) lcd.createChar(i, customArrayChar[i]);
  lcd.home(); lcd.clear();
  lcd.print("...INICIANDO...");
}

void initWifiFunction(){
  status = WL_IDLE_STATUS;
  failed = 0;
  
  Serial1.begin(2400);
  WiFi.init(&Serial1);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    lcd.home(); lcd.clear();
    lcd.print("  Tarjeta WIFI  ");
    lcd.setCursor(0,1);
    lcd.println(" NO  ENCONTRADA ");
    // don't continue:
    while (true);
  }
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED ) {
    status = WiFi.begin(ssid,password);

    if(failed > 5 ){
      lcd.home(); lcd.clear();
      lcd.print("Red no conectada");
      lcd.setCursor(0,1);
      lcd.print(ssid);
      while(true){
        tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,1000);
        delay(1500);
      }
    }else if(status != WL_CONNECTED){
      // wait 10 seconds for reconnection:
      lcd.home(); lcd.clear();
      lcd.print("Reconectando a:");
      lcd.setCursor(0,1);
      lcd.print(ssid);
      delay(10000);
    }
    failed++;
  }
}

time_t getTimeFunction(){
  WiFiEspUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
  timeClient.setUpdateInterval(1);
  while(!timeClient.update()){
    delay(1000);
  } 
  setTime(timeClient.getEpochTime());
  timeClient.end();
  return now();
}

void updateMainScreen(){
  time_t T  = now();
  if(isCESTtimezone()){
    T += 7200;
  }else{
    T += 3600;
  }
  lcd.clear();
  lcd.setCursor(1, 0);
  if (hour(T)<10) lcd.print('0');
  lcd.print(hour(T), DEC);
  
  lcd.print(':');
  if (minute(T)<10) lcd.print('0');
  lcd.print(minute(T), DEC);
  
  lcd.print(':');
  if (second(T)<10) lcd.print('0');
  lcd.print(second(T), DEC);
  
  lcd.setCursor(12, 0);
  lcd.print(temperature-273);
  lcd.write(LCD_CHAR_DEGREE);
  lcd.print('C');
  
  lcd.setCursor(0, 1);
  if (day(T)<10) lcd.print('0');
  lcd.print(day(T), DEC);
  
  lcd.print('/');
  if (month(T)<10) lcd.print('0');
  lcd.print(month(T), DEC);
  
  lcd.print('/');
  lcd.print(year(T), DEC);
  lcd.print(' ');
  
  lcd.setCursor(12, 1);
  int dayofweek = weekday(T);
  switch(dayofweek){
    case 1:
      lcd.print("Dom.");
      break;
    case 2:
      lcd.print("Lun.");
      break;
    case 3:
      lcd.print("Mar.");
    break;
    case 4:
      lcd.print("Mie.");
      break;
    case 5:
      lcd.print("Jue.");
      break;
    case 6:
      lcd.print("Vie.");
      break;
    case 0:
      lcd.print("Sab.");
      break;
  }
}

void getWeatherFunction(){
  WiFiEspClient client;
  // Connect to HTTP server
  client.setTimeout(10000);
  if (!client.connect("api.openweathermap.org", 80)) {
    Serial.println("Connection failed");
    // Disconnect
    client.stop();
    return -1;
  }

  // Send HTTP request
  String HTTPrequest = "GET /data/2.5/weather?lat=36.7167615&lon=-4.4115726&appid=";
  HTTPrequest += appid;
  HTTPrequest += " HTTP/1.0";
  client.println(HTTPrequest);
  client.println("Host: api.openweathermap.org");
  client.println("Connection: close");
  if (client.println() == 0) {
    Serial.println("Failed to send request");
    // Disconnect
    client.stop();
    return -1;
  }
  // Skip HTTP headers
  if (!client.find("\r\n\r\n")) {
    Serial.println("Invalid response");
    // Disconnect
    client.stop();
    return -1;
  }
  if(client.find("\"temp\":")){
    temperature = client.readStringUntil(',').toInt();
  }else{
    Serial.println("No Temperature JSON object found");
  }
  if(client.find("\"clouds\":{\"all\":")){
    cloudiness = client.readStringUntil('}').toInt();
  }else{
    Serial.println("No Cloudiness JSON object found");
  }
  if(client.find("\"sunrise\":")){
    sunriseTime = strtoul(client.readStringUntil(',').c_str(), NULL, 10);
    // Update the time of the Sunrise Task
    Alarm.free(sunriseTaskID);
    // A delay is needed to let the object proccess the free function
    Alarm.delay(1);
    // create a new alarm with the new time
    // sunriseTaskID = Alarm.alarmRepeat(hour(sunriseTime),minute(sunriseTime),second(sunriseTime), sunriseTaskFunction);
  }else{
    Serial.println("No sunrise JSON object found");
  }
  // Disconnect
  client.stop();
}
/*

time_t getSunriseTimeFunction(){
  // Connect to HTTP server
  client.setTimeout(10000);
  if (!client.connect("api.sunrise-sunset.org", 80)) {
    Serial.println("Connection failed");
    return -1;
  }

  // Send HTTP request
  client.println("GET /json?lat=36.7167615&lng=-4.4115726 HTTP/1.0");
  client.println("Host: api.sunrise-sunset.org");
  client.println("Connection: close");
  if (client.println() == 0) {
    Serial.println("Failed to send request");
    // Disconnect
    client.stop();
    return -1;
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println("Invalid response");
    // Disconnect
    client.stop();
    return -1;
  }
  mensaje = "";
  while(mensaje != "sunrise" && client.available()){
    mensaje = client.readStringUntil('"');
  }
  
  if(!client.available()){
    Serial.println("No sunrise JSON object");
    // Disconnect
    client.stop();
    return -1;
  }
  
  client.readStringUntil('"');
  int hora = client.readStringUntil(':').toInt();
  int minuto = client.readStringUntil(':').toInt();
  int segundo = client.readStringUntil(' ').toInt();

  // Calculate the seconds passed since the 1 Jan.
  sunriseTime = hora * 3600 + minuto * 60 + segundo; 

  while(client.available()){
    client.read();
  }
  
  // Disconnect
  client.stop();

  // Only for test purposes
  sunriseTime = now() + 8;

  // Update the time of the Sunrise Task
  Alarm.free(sunriseTaskID);
  // A delay is needed to let the object proccess the free function
  Alarm.delay(1);
  // create a new alarm with the new time
  // sunriseTaskID = Alarm.alarmRepeat(hour(sunriseTime),minute(sunriseTime),second(sunriseTime), sunriseTaskFunction);

  tone(PIN_BUZZER,BUZZER_LOW_VOLUME-10000,1000);

  return sunriseTime;
}

void getCloudsFunction(){
  // Connect to HTTP server
  client.setTimeout(10000);
  if (!client.connect("api.openweathermap.org", 80)) {
    Serial.println("Connection failed");
    return -1;
  }

  // Send HTTP request
  String HTTPrequest = "GET /data/2.5/weather?lat=36.7167615&lon=-4.4115726&appid=";
  HTTPrequest += appid;
  HTTPrequest += " HTTP/1.0";
  client.println("Host: api.openweathermap.org");
  client.println("Connection: close");
  if (client.println() == 0) {
    Serial.println("Failed to send request");
    // Disconnect
    client.stop();
    return -1;
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println("Invalid response");
    // Disconnect
    client.stop();
    return -1;
  }
  mensaje = "";
  while(mensaje != "sunrise" && client.available()){
    mensaje = client.readStringUntil('"');
  }
  
  if(!client.available()){
    Serial.println("No sunrise JSON object");
    // Disconnect
    client.stop();
    return -1;
  }
  
  client.readStringUntil('"');
  int hora = client.readStringUntil(':').toInt();
  int minuto = client.readStringUntil(':').toInt();
  int segundo = client.readStringUntil(' ').toInt();

  while(client.available()){
    client.read();
  }
  
  // Disconnect
  client.stop();

  tone(PIN_BUZZER,BUZZER_LOW_VOLUME-10000,1000);

  return -1;
}
*/
bool isCESTtimezone(){
  if(month() >= 3 && month() <= 10){
    if(month() == 3){
      if(day() >= 29)
        return true;
      else
        return false;
    }else if(month() == 10){
      if(day() < 25)
        return true;
      else
        return false;
    }else{
      return true;
    }
  }
  return false;
}
/*
void sunriseTaskFunction(){
  bool thisDayIsDeactivated = false;
  for(int i = 0 ; i < sunriseTasks ; i++){
    if(month(sunriseTasksArray[i]) == month()){
      if(day(sunriseTasksArray[i]) == day()){
        thisDayIsDeactivated = true;
        Serial.println("This day there is no Sunrise Function");
        break;
      }
    }
  }
  if(!thisDayIsDeactivated){
    Serial.println("This day there is Sunrise Function\nWake up!");
    tone(PIN_BUZZER,BUZZER_LOW_VOLUME,1000);
    delay(1000);
  }
}
*/
