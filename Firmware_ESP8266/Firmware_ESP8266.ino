// Domotic Shutter sketch master by Mickyleitor
// This sketch is in development and uses ESP13-WROOM-02.
//
// Libraries used and need to be installed from Arduino IDE
// - Shield manager ESP8266 by ESP8266 Community version 2.6.3 : http://arduino.esp8266.com/stable/package_esp8266com_index.json
// - NTPClient by Fabrice Weinberg 3.2.0 : https://github.com/arduino-libraries/NTPClient
// - LiquidCrystal_PCF8574 by mathertel 1.2.0  : https://github.com/mathertel/LiquidCrystal_PCF8574
// - LinkedList by Luis Llamas version 1.0.0  : https://github.com/luisllamasbinaburo/Arduino-LinkedList


#include <Ticker.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include "basic_defines.h"
#include "CriticalData.h" // No incluir en proyecto final
#include "SolarAzEl.h"


enum State {
  IDLE,
  SHUTTERMANAGER,
  MODE_JOB_TASK,
  MODE_SLEEP_TASK,
  MODE_ON_SLEEP_TASK,
  MODE_OFF_SLEEP_TASK
} SystemState ;

enum ButtonInput {
  NONE,
  LEFT,
  UP,
  DOWN,
  RIGHT
} currentButton;

enum MenuSeleccion {
  PERSIANA_IZQUIERDA,
  PERSIANA_CENTRAL,
  PERSIANA_DERECHA,
  OPTION
} seleccionMenu ;

struct WeatherData {
  double SunAzimuth = 0;
  double SunElevation = 0;
  time_t sunriseTimeSecondsUTC = 0;
  time_t sunsetTimeSecondsUTC = 0;
  unsigned long timezoneshift = MYTZ;
  double Cloudiness = 0;
  double TemperatureDegree = 0;
} MyWeather;

struct ShutterParameters {
  int status = 0;
  int LastMoved = 0;
} ShutterData [3];

struct ScheduledParameters {
  bool scheduled = false;
  bool isDone = false;
} ScheduledData[365] ;

int ScheduledDataResetValue = 0;

Ticker TimeOutTask, ButtonsISRTask,SystemFunctionTask;
LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);

void goIdleState() {
  Serial.println("Buttons Time Out");
  SystemState = IDLE;
};

void setup() {
  SystemState = IDLE;
  seleccionMenu = PERSIANA_CENTRAL;
  currentButton = NONE;
  Serial.begin(115200);
  initLCDFunction();
  initWifiFunction();
  initTimeFunction();
  initButtonsFunction();
  checkSlaveConnection();
  getWeatherDataFunction();
}

void loop() {
  // Serial.println(digitalRead(2));
  switch (SystemState) {
    case IDLE : {
        apagarBrilloPantalla();
        mostrarHoraPantalla();
        TimeOutTask.detach();
        TimeOutTask.attach(UPDATE_SCREEN_SECONDS, mostrarHoraPantalla);
        while (SystemState == IDLE) {
          if (buttonPressed() != NONE) {
            SystemState = SHUTTERMANAGER;
          }
          yield();
        }
        encenderBrilloPantalla();
        if (seleccionMenu == OPTION) SystemState = MODE_JOB_TASK;
        actualizarMenuPantalla();
        break;
      }
    case SHUTTERMANAGER : {
        switch (buttonPressed()) {
          case LEFT : {
              seleccionarAnterior();
              if (seleccionMenu == OPTION) {
                SystemState = MODE_JOB_TASK;
                actualizarMenuPantalla();
              }
              else actualizarMenuPantalla();
              break;
            }
          case RIGHT : {
              seleccionarSiguiente();
              if (seleccionMenu == OPTION) {
                SystemState = MODE_JOB_TASK;
                actualizarMenuPantalla();
              }
              else actualizarMenuPantalla();
              break;
            }
          case UP : {
              if (ShutterData[seleccionMenu].status != 0) {
                PararPersiana(seleccionMenu);
              } else {
                subirPersiana(seleccionMenu);
              }
              actualizarMenuPantalla();
              break;
            }
          case DOWN : {
              if (ShutterData[seleccionMenu].status != 0) {
                PararPersiana(seleccionMenu);
              } else {
                bajarPersiana(seleccionMenu);
              }
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case MODE_JOB_TASK : {
        switch (buttonPressed()) {
          case UP : {
              activarModoTrabajo();
              break;
            }
          case DOWN : {
              SystemState = MODE_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }
          case LEFT : {
              SystemState = SHUTTERMANAGER;
              seleccionMenu = PERSIANA_DERECHA;
              actualizarMenuPantalla();
              break;
            }
          case RIGHT : {
              SystemState = SHUTTERMANAGER;
              seleccionMenu = PERSIANA_IZQUIERDA;
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case MODE_SLEEP_TASK : {
        switch (buttonPressed()) {
          case LEFT : {
              SystemState = SHUTTERMANAGER;
              seleccionMenu = PERSIANA_DERECHA;
              actualizarMenuPantalla();
              break;
            }
          case UP : {
              SystemState = MODE_JOB_TASK;
              actualizarMenuPantalla();
              break;
            }
          case RIGHT : {
              SystemState = MODE_ON_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case MODE_ON_SLEEP_TASK : {
        switch (buttonPressed()) {
          case RIGHT : {
              procesoActivarTarea();
              actualizarMenuPantalla();
              break;
            }
          case DOWN : {
              SystemState = MODE_OFF_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }
          case LEFT : {
              SystemState = MODE_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }

        }
        break;
      }
    case MODE_OFF_SLEEP_TASK : {
        switch (buttonPressed()) {
          case RIGHT : {
              procesoDesactivarTarea();
              actualizarMenuPantalla();
              break;
            }
          case UP : {
              SystemState = MODE_ON_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }
          case LEFT : {
              SystemState = MODE_SLEEP_TASK;
              actualizarMenuPantalla();
              break;
            }

        }
        break;
      }
  }
  yield();
}

void deBounceFunction() {
  currentButton = NONE;
  if (digitalRead(4) == LOW) {
    currentButton = LEFT;
  } else if (digitalRead(5) == LOW) {
    currentButton = DOWN;
  } else if (digitalRead(12) == LOW) {
    currentButton = UP;
  } else if (digitalRead(13) == LOW) {
    currentButton = RIGHT;
  }
  ButtonsISRTask.detach();
}
void IRAM_ATTR isrButtons() {
  ButtonsISRTask.detach();
  ButtonsISRTask.attach_ms(DEBOUNCE_TIME_MILLIS, deBounceFunction);
}
int buttonPressed() {
  while ( Serial.available() ) {
    char c = (char)Serial.read();
    if ( c == '1' ) currentButton = LEFT;
    if ( c == '2' ) currentButton = UP;
    if ( c == '3' ) currentButton = DOWN;
    if ( c == '4' ) currentButton = RIGHT;
  }
  if (currentButton != NONE) {
    Serial.print("------- [DEBUG] PULSADO : ");
    Serial.println(currentButton);
    TimeOutTask.detach();
    TimeOutTask.attach(TO_IDLE_SECONDS, goIdleState);
  }
  int buttonPress = currentButton;
  currentButton = NONE;
  return buttonPress;
}
void seleccionarAnterior() {
  switch (seleccionMenu) {
    case PERSIANA_IZQUIERDA : {
        seleccionMenu = OPTION;
        break;
      };
    case PERSIANA_CENTRAL : {
        seleccionMenu = PERSIANA_IZQUIERDA;
        break;
      };
    case PERSIANA_DERECHA : {
        seleccionMenu = PERSIANA_CENTRAL;
        break;
      };
    case OPTION : {
        seleccionMenu = PERSIANA_DERECHA;
        break;
      };
  }
}
void seleccionarSiguiente() {
  switch (seleccionMenu) {
    case PERSIANA_IZQUIERDA : {
        seleccionMenu = PERSIANA_CENTRAL;
        break;
      };
    case PERSIANA_CENTRAL : {
        seleccionMenu = PERSIANA_DERECHA;
        break;
      };
    case PERSIANA_DERECHA : {
        seleccionMenu = OPTION;
        break;
      };
    case OPTION : {
        seleccionMenu = PERSIANA_IZQUIERDA;
        break;
      };
  }
}
void apagarBrilloPantalla()  {
  lcd.setBacklight(0);
};
void encenderBrilloPantalla() {
  lcd.setBacklight(255);
};
void subirPersiana(int persiana) {
  ShutterData[persiana].status = 1;
  ShutterData[persiana].LastMoved = millis();
  sendRollerCommand(persiana, 1);
  SystemFunctionTask.detach();
  SystemFunctionTask.attach_ms(SHUTTER_DURATION_MILLIS,SystemFunctionManager);
};
void bajarPersiana(int persiana) {
  ShutterData[persiana].status = 2;
  ShutterData[persiana].LastMoved = millis();
  sendRollerCommand(persiana, 2);
  SystemFunctionTask.detach();
  SystemFunctionTask.attach_ms(SHUTTER_DURATION_MILLIS,SystemFunctionManager);
};
void PararPersiana(int persiana) {
  ShutterData[persiana].status = 0;
  sendRollerCommand(persiana, 0);
};

void SystemFunctionManager(){
  SystemFunctionTask.detach();
  SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS,SystemFunctionManager);
  
  // Check Roller Shutter status
  for(int index = 0; index < 3 ; index ++ ){
    if(ShutterData[index].status != 0){
      if( abs(millis() - ShutterData[index].LastMoved) > SHUTTER_DURATION_MILLIS){
        ShutterData[index].status = 0;
      }else{
        SystemFunctionTask.detach();
        SystemFunctionTask.attach_ms(SHUTTER_DURATION_MILLIS,SystemFunctionManager);
      }
    }
  }  
  time_t nowSecondsUTC = time(NULL) % (60 * 60 * 24);
  if(nowSecondsUTC >= MyWeather.sunriseTimeSecondsUTC){
    time_t now = time(NULL);
    struct tm * timeinfo;
    timeinfo = gmtime(&now);
    resetScheduledData(timeinfo);
    
    int DaysYear = timeinfo->tm_yday;
    if(DaysYear > 58 && isYearLeap(timeinfo->tm_year + 1900)) DaysYear--;
    if(ScheduledData[DaysYear].scheduled && !ScheduledData[DaysYear].isDone){
      ScheduledData[DaysYear].isDone = true;
      activarModoDormir();
    }
  }
  Serial.println("System Manager updated");
}

void resetScheduledData(struct tm * timeinfo){
  if(timeinfo->tm_yday == 0 && ScheduledDataResetValue == 0){
    for(int index = 0; index < 365 ; index ++ ) ScheduledData[index].isDone = false;
    ScheduledDataResetValue = 1;
  }else if(timeinfo->tm_yday != 0 && ScheduledDataResetValue != 0){
    ScheduledDataResetValue = 0;
  }
}

bool isYearLeap(int year){
  if (year%400 == 0) // Exactly divisible by 400 e.g. 1600, 2000
    return true;
  else if (year%100 == 0) // Exactly divisible by 100 and not by 400 e.g. 1900, 2100
    return false;
  else if (year%4 == 0) // Exactly divisible by 4 and neither by 100 nor 400 e.g. 2020
    return true;
  else // Not divisible by 4 or 100 or 400 e.g. 2017, 2018, 2019
    return false;
}

void activarModoDormir() {
  Serial.println("Modo trabajo activado");
  sendCommandToSlave(COMMAND_BUZZER_MEDIUM_VOLUME);
  bajarPersiana(PERSIANA_DERECHA);
  delay(500);
  bajarPersiana(PERSIANA_CENTRAL);
};

void activarModoTrabajo() {
  Serial.println("Modo trabajo activado");
  sendCommandToSlave(COMMAND_BUZZER_MEDIUM_VOLUME);
  getWeatherDataFunction();
  time_t nowSecondsUTC = time(NULL) % (60 * 60 * 24);
  // According to Requirements the current time should be between sunrise and sunset time.
  if((MyWeather.sunriseTimeSecondsUTC <= nowSecondsUTC) && (nowSecondsUTC <= MyWeather.sunsetTimeSecondsUTC)){
    // According to Requirements there is no need if the Cloudiness is less than 75%
    if(MyWeather.Cloudiness < 75){
      // Both shutter lowered when 60 < SunAzimuth < 260
      if( 60 < MyWeather.SunAzimuth  && MyWeather.SunAzimuth < 260){
        bajarPersiana(PERSIANA_DERECHA);
        delay(500);
        bajarPersiana(PERSIANA_CENTRAL);
        delay(500);
        sendCommandToSlave(COMMAND_LIGHT_ON);
      // Center shutter only lower when 100 < SunAzimuth < 230
      }else if( 100 < MyWeather.SunAzimuth  && MyWeather.SunAzimuth < 230){
        bajarPersiana(PERSIANA_CENTRAL);
      }
    }
  }
};

int getWeatherDataFunction(){

  double NewAzimuth = -9999;
  double NewElevation = -9999;
  SolarAzEl(time(NULL), MY_LAT, MY_LON, 1, &NewAzimuth, &NewElevation);
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
  client.setTimeout(10000);
  if (!client.connect("api.openweathermap.org", 80)) {
    Serial.println("Connection failed");
    // Disconnect
    client.stop();
    return -1;
  }

  // Send HTTP request
  String HTTPrequest = MyHTTPrequest;
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
    MyWeather.sunriseTimeSecondsUTC = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10)  % (60 * 60 * 24);
    Serial.print("Current Sunset Local time: ");
    Serial.print(ctime(&MyWeather.sunriseTimeSecondsUTC));
  }else{
    Serial.println("No sunrise JSON object found");
  }
  if(client.find("\"sunset\":")){
    MyWeather.sunsetTimeSecondsUTC = (time_t)strtoul(client.readStringUntil(',').c_str(), NULL, 10)  % (60 * 60 * 24);
    Serial.print("Current Sunset Local time: ");
    Serial.print(ctime(&MyWeather.sunsetTimeSecondsUTC));    
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
  return 0;
}

int procesoSeleccionarFecha(int & sday, int & smonth){
  int SleepTaskState = 0;
  bool FechaConfirmada = false;
  int daysOfMonths [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  while (SystemState == MODE_ON_SLEEP_TASK || SystemState == MODE_OFF_SLEEP_TASK)
  {
    switch (SleepTaskState) {
      case 0 : {
          lcd.clear(); lcd.home();
          lcd.print("SEL. FECHA ");
          lcd.setCursor(11, 0);
          mostrarFechaPantalla(sday, smonth);
          lcd.setCursor(0, 1);
          lcd.print("<    +    -  Mes");
          SleepTaskState = 1;
          break;
        }
      case 1 : {
          switch (buttonPressed()) {
            case UP : {
                if ( (sday + 1) > daysOfMonths[smonth - 1]) {
                  if (smonth == 12) smonth = 1;
                  else smonth++;
                  sday = 1;
                } else sday += 1;
                lcd.setCursor(11, 0);
                mostrarFechaPantalla(sday, smonth);
                break;
              }
            case DOWN : {
                if ( (sday - 1) == 0) {
                  if (smonth == 1) smonth = 12;
                  else smonth--;
                  sday = daysOfMonths[smonth - 1];
                } else sday -= 1;
                lcd.setCursor(11, 0);
                mostrarFechaPantalla(sday, smonth);
                break;
              }
            case LEFT : {
                SystemState = MODE_SLEEP_TASK;
                break;
              }
            case RIGHT : {
                SleepTaskState = 2;
                lcd.setCursor(0, 1);
                lcd.print("Dia  +    -   OK");
                break;
              }
          }
          break;
        }
      case 2 : {
          switch (buttonPressed()) {
            case UP : {
                if (smonth == 12) smonth = 1;
                else smonth++;
                if (sday > daysOfMonths[smonth - 1]) sday = daysOfMonths[smonth - 1];
                lcd.setCursor(11, 0);
                mostrarFechaPantalla(sday, smonth);
                break;
              }
            case DOWN : {
                if (smonth == 1) smonth = 12;
                else smonth--;
                if (sday > daysOfMonths[smonth - 1]) sday = daysOfMonths[smonth - 1];
                lcd.setCursor(11, 0);
                mostrarFechaPantalla(sday, smonth);
                break;
              }
            case LEFT : {
                lcd.setCursor(0, 1);
                lcd.print("<    +    -  Mes");
                SleepTaskState = 1;
                break;
              }
            case RIGHT : {
                lcd.clear(); lcd.home();
                lcd.print("CONFIRMAR  FECHA");
                lcd.setCursor(0, 1);
                lcd.print("<    ");
                mostrarFechaPantalla(sday, smonth);
                lcd.print("    OK");
                SleepTaskState = 3;
                break;
              }
          }
          break;
        }
      case 3 : {
          switch (buttonPressed()) {
            case LEFT : {
                SleepTaskState = 0;
                break;
              }
            case RIGHT : {
                // Guardar fecha
                FechaConfirmada = true;
                SystemState = MODE_SLEEP_TASK;
                break;
              }
          }
          break;
        }
    }
    yield();
  }
  return FechaConfirmada;
};

void mostrarFechaPantalla( int sday, int smonth) {
  if (sday < 10) lcd.print('0');
  lcd.print(sday, DEC);
  lcd.print("/");
  if (smonth < 10) lcd.print('0');
  lcd.print(smonth, DEC);
}

void procesoDesactivarTarea() {
  int newday = 1, newmonth = 1;
  if (procesoSeleccionarFecha(newday, newmonth)) {
    Serial.println("Desactivar tarea en fecha seleccionada");
    
    Serial.print(newday);
    Serial.print("-");
    Serial.println(newmonth);
  }
}

void procesoActivarTarea() {
  int newday = 1, newmonth = 1;
  if (procesoSeleccionarFecha(newday, newmonth)) {
    Serial.println("Activar tarea en fecha seleccionada");
    
    Serial.print(newday);
    Serial.print("-");
    Serial.println(newmonth);
  }
}

void mostrarHoraPantalla() {
  time_t now;
  struct tm * timeinfo;
  now = time(&now) + MYTZ;
  timeinfo = localtime(&now);
  lcd.clear();
  lcd.setCursor(1, 0);
  if ((timeinfo->tm_hour) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_hour), DEC);

  lcd.print(':');
  if ((timeinfo->tm_min) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_min), DEC);

  lcd.print(':');
  if ((timeinfo->tm_sec) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_sec), DEC);

  lcd.setCursor(12, 0);
  if (((int)MyWeather.TemperatureDegree) < 10) lcd.print('0');
  lcd.print(((int)MyWeather.TemperatureDegree), DEC);
  lcd.print((char)223);
  lcd.print('C');

  lcd.setCursor(0, 1);
  if ((timeinfo->tm_mday) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_mday), DEC);

  lcd.print('/');
  if ((timeinfo->tm_mon) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_mon), DEC);

  lcd.print('/');
  lcd.print((timeinfo->tm_year) + 1900, DEC);
  lcd.print(' ');

  lcd.setCursor(12, 1);
  int dayofweek = (timeinfo->tm_wday);
  switch (dayofweek) {
    case 0:
      lcd.print("Dom.");
      break;
    case 1:
      lcd.print("Lun.");
      break;
    case 2:
      lcd.print("Mar.");
      break;
    case 3:
      lcd.print("Mie.");
      break;
    case 4:
      lcd.print("Jue.");
      break;
    case 5:
      lcd.print("Vie.");
      break;
    case 6:
      lcd.print("Sab.");
      break;
    default:
      lcd.print("Err.");
      break;
  }
  // Serial.println("Hora pantalla actualizada");
}

void actualizarMenuPantalla() {
  lcd.clear(); lcd.home();
  switch (SystemState) {
    case SHUTTERMANAGER : {
        if (seleccionMenu < 3) {
          String namePersiana [] = {" PERSIANA IZQDA ", "PERSIANA CENTRAL", "PERSIANA DERECHA", "  ERROR MENU  "};
          lcd.print(namePersiana[seleccionMenu]);
          lcd.setCursor(0, 1);
          lcd.print("<");
          lcd.setCursor(5, 1);

          if (ShutterData[seleccionMenu].status == 1) lcd.write(4);
          else lcd.write(1);

          lcd.setCursor(10, 1);

          if (ShutterData[seleccionMenu].status == 2) lcd.write(4);
          else lcd.write(2);

          lcd.setCursor(15, 1);
          lcd.print(">");
        }
        break;
      }
    case MODE_JOB_TASK : {
        lcd.print("  MODO TRABAJO  ");
        lcd.setCursor(0, 1);
        lcd.print("<   OK   SIG.  >");
        break;
      }
    case MODE_SLEEP_TASK : {
        lcd.print("  MODO DORMIR   ");
        lcd.setCursor(0, 1);
        lcd.print("<  ANT.       OK");
        break;
      }
    case MODE_ON_SLEEP_TASK : {
        lcd.print("ACT. MODO DORMIR");
        lcd.setCursor(0, 1);
        lcd.print("<        SIG. OK");
        break;
      }
    case MODE_OFF_SLEEP_TASK : {
        lcd.print("DES. MODO DORMIR");
        lcd.setCursor(0, 1);
        lcd.print("<  ANT.       OK");
        break;
      }
    default : {
        lcd.print("   ERROR MENU   ");
        break;
      }
  }
}

int initLCDFunction() {
  int status = -1;
  int failed = 0;
  // Search LCD into I2C line:
  while ( status != 0 ) {
    Wire.begin(2, 14);
    Wire.beginTransmission(ADDRESS_I2C_LCD);
    status = Wire.endTransmission();

    if (failed > 5) {
      return -1;
    } else if (status != 0) {
      // wait 5 seconds for reconnection:
      delay(5000);
    }
    failed++;
  }

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  int customArrayChar[6][8] =
  {
    /* Flecha izquierda */  {0x00, 0x07, 0x0E, 0x1C, 0x1C, 0x0E, 0x07, 0x00},
    /* Flecha arriba    */  {0x00, 0x04, 0x0E, 0x1F, 0x1B, 0x11, 0x00, 0x00},
    /* Flecha abajo     */  {0x00, 0x00, 0x11, 0x1B, 0x1F, 0x0E, 0x04, 0x00},
    /* Flecha derecha   */  {0x00, 0x1C, 0x0E, 0x07, 0x07, 0x0E, 0x1C, 0x00},
    /* Flecha STOP      */  {0x00, 0x0E, 0x1B, 0x11, 0x11, 0x1B, 0x0E, 0x00},
    /* Vacio            */  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
  };
  for ( byte i = 0 ; i < 6 ; i ++ ) lcd.createChar(i, customArrayChar[i]);
  lcd.home(); lcd.clear();
  lcd.print("...INICIANDO...");
  return 0;
}

void initButtonsFunction() {
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  attachInterrupt(4, isrButtons, FALLING);
  attachInterrupt(5, isrButtons, FALLING);
  attachInterrupt(12, isrButtons, FALLING);
  attachInterrupt(13, isrButtons, FALLING);
}

void initWifiFunction() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID, MyPassword);
  lcd.setCursor(0, 1);
  lcd.print("Conectando Wifi");
  while (WiFi.status() != WL_CONNECTED) delay(1000);    
}

void initTimeFunction() {
  // We want UTC time.
  int dst = 0;
  configTime(0, dst * 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
  while (!time(nullptr)){
    yield();
    delay(1000);
  }
}

void sendCommandToSlave(char type) {
  Wire.beginTransmission(ADDRESS_I2C_SLAVE); // transmit to device #8
  Wire.write(1);        // This should wake up the device
  Wire.write(type);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
}

void sendRollerCommand(int persiana, int comando) {
  char type = 1 << (persiana + 2) | (0x3  & comando);
  sendCommandToSlave(type);
}

void checkSlaveConnection() {
  bool slaveStatusRunning = false;
  Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
  unsigned long timeOut = millis();
  while (!slaveStatusRunning) {
    if (Wire.available() > 0) {
      char c = Wire.read();
      if (c != 0x3) Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
      else slaveStatusRunning = true;
    }
    if ( (millis() - timeOut) > 5000) {
      lcd.home(); lcd.clear();
      lcd.print(" ERROR CONEXION ");
      lcd.setCursor(0, 1);
      lcd.print(" DISP. HARDWARE ");
      Wire.requestFrom(ADDRESS_I2C_SLAVE, 1);
      timeOut = millis();
    }
    yield();
  }
  for (int i = 0; i < 3; i++) {
    sendCommandToSlave(COMMAND_BUZZER_HIGH_VOLUME);
    delay(200);
    yield();
  };
}
