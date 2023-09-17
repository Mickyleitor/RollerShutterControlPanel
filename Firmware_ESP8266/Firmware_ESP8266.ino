// Domotic Shutter sketch master by Mickyleitor
// This sketch is in development and uses ESP13-WROOM-02.
//
// Libraries used and need to be installed from Arduino IDE
// - Shield manager ESP8266 by ESP8266 Community version 3.1.2 : http://arduino.esp8266.com/stable/package_esp8266com_index.json
// - NTPClient by Fabrice Weinberg 3.2.0 : https://github.com/arduino-libraries/NTPClient
// - LiquidCrystal_PCF8574 by mathertel 2.2.0  : https://github.com/mathertel/LiquidCrystal_PCF8574


#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "basic_defines.h"
#include "SolarAzEl.h"
#include "error.h"
#include "buttons.h"
#include "lcd.h"

#include "ESP8266_Utils.h"
#include "EEPROM_Utils.h"
#include "Slave_Utils.h"

#include "rscpProtocol/rscpProtocol.h"

int ScheduledDataResetValue = 0;
enum SystemState _SystemState = SYSTEM_STATE_WIFI_STATION_CONNECTED;
enum seleccionMenu _seleccionMenu = SELECCION_MENU_PERSIANA_CENTRAL;
struct ShutterParameters ShutterData[3] = {0};
Ticker TimeOutTask, SystemFunctionTask;

void goIdleState(void) {
  Serial.println("Buttons Time Out");
  _SystemState = SYSTEM_STATE_ENTERING_IDLE;
};

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Master inicializado");
  EEPROM_Read(&_storedData); // Comment this line to reset the EEPROM
  EEPROM_Check(&_storedData);

  if( initLCDFunction(10000) < 0){
    errorHandler(&lcd, FATAL_ERROR_CODE_LCD_INIT_FAILED);
  }

  lcd.setCursor(0, 1);
  lcd.print("Conectando Wifi");
  if( !ConnectWiFi_STA(_storedData._ssid_sta, _storedData._password_sta, 10000) ){
    Serial.print("Error connecting to SSID: ");
    Serial.println(_storedData._ssid_sta);
    if( !ConnectWiFi_AP(_storedData._ssid_ap, _storedData._password_ap, 10000) ){
      Serial.print("Error creating AP with SSID: ");
      Serial.println(_storedData._ssid_ap);
      errorHandler(&lcd, FATAL_ERROR_CODE_WIFI_AP_FAILED);
    }else{
      Serial.print("AP created with SSID: ");
      Serial.println(_storedData._ssid_ap);
      _SystemState = SYSTEM_STATE_WIFI_ACCESS_POINT_OPENED;
    }
  }
}

void loop() {
  // Serial.println(digitalRead(2));
  switch (_SystemState) {
    case SYSTEM_STATE_WIFI_ACCESS_POINT_OPENED : {
      static int indexDisplay1;
      static int indexDisplay2;
      static uint32_t timerMs;
      String displayString1 = "           " + String(_storedData._ssid_ap) + "          ";
      String displayString2 = "                Password: " + String(_storedData._password_ap) + " IP: " + String(WiFi.softAPIP().toString()) + "               "; 

      if ( (millis() - timerMs) > 250 ) {
        timerMs = millis();
        lcd.home(); lcd.clear();
        lcd.print("SSID:" + displayString1.substring(indexDisplay1, indexDisplay1 + 11));
        lcd.setCursor(0, 1);
        lcd.print(displayString2.substring(indexDisplay2, indexDisplay2 + 16));
        indexDisplay1++;
        indexDisplay2++;
        if (indexDisplay1 > displayString1.length() - 10) indexDisplay1 = 0;
        if (indexDisplay2 > displayString2.length() - 16) indexDisplay2 = 0;
      }
      break;
    }
    case SYSTEM_STATE_WIFI_STATION_CONNECTED : {
      // Before ACCESS_POINT_OPENED it makes no sense to do all this bottom functions
      initButtonsFunction();
      checkSlaveConnection(&lcd);
      getWeatherDataFunction();
      SystemFunctionTask.detach();
      SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS,SystemFunctionManager);
      _SystemState = SYSTEM_STATE_ENTERING_IDLE;
      break;
    }
    case SYSTEM_STATE_ENTERING_IDLE : {
        apagarBrilloPantalla();
        mostrarHoraPantalla();
        TimeOutTask.detach();
        TimeOutTask.attach(UPDATE_SCREEN_SECONDS, mostrarHoraPantalla);
        _SystemState = SYSTEM_STATE_IDLING;
        break;
      }
    case SYSTEM_STATE_IDLING : {
        if(buttonPressed() != BUTTON_STATE_NONE){
          _SystemState = SYSTEM_STATE_WAKEUP;
        }
        break;
      }
    case SYSTEM_STATE_WAKEUP : {
        // Update Weather condition every wake up?
        // getWeatherDataFunction();
        encenderBrilloPantalla();
        _SystemState = SYSTEM_STATE_SHUTTER_MANAGER;
        if (_seleccionMenu == SELECCION_MENU_OPTION_JOB_MODE){
          _SystemState = SYSTEM_STATE_MENU_JOB_MODE;
        }
        if (_seleccionMenu == SELECCION_MENU_OPTION_SLEEP_MODE){
          _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
        }
        actualizarMenuPantalla();
        break;
      }
    case SYSTEM_STATE_SHUTTER_MANAGER : {
        switch (buttonPressed()) {
          case BUTTON_STATE_LEFT : {
              seleccionarAnterior();
              actualizarMenuPantalla();
              break;
            }
          case BUTTON_STATE_RIGHT : {
              seleccionarSiguiente();
              actualizarMenuPantalla();
              break;
            }
          case BUTTON_STATE_UP : {
              if (ShutterData[_seleccionMenu].status != 0) {
                PararPersiana(_seleccionMenu);
              } else {
                subirPersiana(_seleccionMenu);
              }
              actualizarMenuPantalla();
              break;
            }
          case BUTTON_STATE_DOWN : {
              if (ShutterData[_seleccionMenu].status != 0) {
                PararPersiana(_seleccionMenu);
              } else {
                bajarPersiana(_seleccionMenu);
              }
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case SYSTEM_STATE_MENU_JOB_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_DOWN : {
			        apagarBrilloPantalla();
              delay(200);
              encenderBrilloPantalla();
              activarModoTrabajo();
              break;
          }
          case BUTTON_STATE_LEFT : {
              seleccionarAnterior();
              actualizarMenuPantalla();
              break;
            }
          case BUTTON_STATE_RIGHT : {
              seleccionarSiguiente();
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case SYSTEM_STATE_MENU_SLEEP_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_LEFT : {
              seleccionarAnterior();
              actualizarMenuPantalla();
              break;
            }
          case BUTTON_STATE_DOWN : {
              _SystemState = SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_RIGHT : {
              seleccionarSiguiente();
              actualizarMenuPantalla();
              break;
          }
        }
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_DOWN : {
              procesoActivarTarea();
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_RIGHT : {
              _SystemState = SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_LEFT : {
              _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_DOWN : {
              procesoDesactivarTarea();
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_RIGHT : {
              _SystemState = SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_LEFT : {
              _SystemState = SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
          }
        }
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_DOWN : {
              int snumber = -1;
              if(procesoConfirmarFecha(snumber,snumber)){
                for(int mes = 0; mes < 12 ; mes++){
                  for(int dia = 0; dia < 31 ; dia ++ ){
                    _storedData.ScheduledData[mes][dia] |= 0x1;
                  }
                }
                EEPROM_Write(&_storedData);
              }
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_LEFT : {
              _SystemState = SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_RIGHT : {
              _SystemState = SYSTEM_STATE_MENU_DEACTIVATE_ALL_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_ALL_SLEEP_MODE : {
        switch (buttonPressed()) {
          case BUTTON_STATE_DOWN : {
              int snumber = -2;
              if(procesoConfirmarFecha(snumber,snumber)){
                for(int mes = 0; mes < 12 ; mes++){
                  for(int dia = 0; dia < 31 ; dia ++ ){
                    _storedData.ScheduledData[mes][dia] &= 0x2;
                  }
                }
                EEPROM_Write(&_storedData);
              }
              actualizarMenuPantalla();
              break;
          }
          case BUTTON_STATE_LEFT : {
              _SystemState = SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE;
              actualizarMenuPantalla();
              break;
            }
        }
        break;
      }
    case SYSTEM_STATE_SLEEP_MANAGER : {
        Serial.println("Modo dormir activado");
        EEPROM_Write(&_storedData);
        struct RSCP_Arg_buzzer_action buzzerAction;
        buzzerAction.action = RSCP_DEF_BUZZER_ACTION_ON;
        buzzerAction.volume = 300;
        buzzerAction.duration_ms = 500;
        rscpSendAction(RSCP_CMD_SET_BUZZER_ACTION, (uint8_t *)&buzzerAction, sizeof(buzzerAction), 1000);
        delay(200);
        bajarPersiana(SELECCION_MENU_PERSIANA_DERECHA);
        delay(200);
        bajarPersiana(SELECCION_MENU_PERSIANA_CENTRAL);
        _SystemState = SYSTEM_STATE_ENTERING_IDLE;
		    EEPROM_Read(&_storedData);
        break;
      }
  }
}

void subirPersiana(int persiana) {
  ShutterData[persiana].status = 1;
  ShutterData[persiana].LastMoved = millis();
  struct RSCP_Arg_rollershutter arg;
  arg.action = RSCP_DEF_SHUTTER_ACTION_UP;
  arg.shutter = persiana;
  arg.retries = 3;
  (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t *)&arg, sizeof(arg), 1000);
  SystemFunctionTask.detach();
  SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS,SystemFunctionManager);
}

void bajarPersiana(int persiana) {
  ShutterData[persiana].status = 2;
  ShutterData[persiana].LastMoved = millis();
  struct RSCP_Arg_rollershutter arg;
  arg.action = RSCP_DEF_SHUTTER_ACTION_DOWN;
  arg.shutter = persiana;
  arg.retries = 3;
  (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t *)&arg, sizeof(arg), 1000);
  SystemFunctionTask.detach();
  SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS,SystemFunctionManager);
}

void PararPersiana(int persiana) {
  ShutterData[persiana].status = 0;
  struct RSCP_Arg_rollershutter arg;
  arg.action = RSCP_DEF_SHUTTER_ACTION_STOP;
  arg.shutter = persiana;
  arg.retries = 3;
  (void)rscpSendAction(RSCP_CMD_SET_SHUTTER_ACTION, (uint8_t *)&arg, sizeof(arg), 1000);
}

void SystemFunctionManager(){
  SystemFunctionTask.detach();
  SystemFunctionTask.attach(SYSTEM_MANAGER_SECONDS,SystemFunctionManager);
  
  // Check Roller Shutter status
  for(int index = 0; index < 3 ; index ++ ){
    // Is this roller(supposedly) not stopped?
    if(ShutterData[index].status != 0){
      // Is the total duration of the movement time out for this roller?
      int ShutterDuration = millis() - ShutterData[index].LastMoved;
      if( abs(ShutterDuration) > SHUTTER_DURATION_SECONDS){
        // This roller is for sure reached the limit.
        ShutterData[index].status = 0;
        if(_SystemState == SYSTEM_STATE_SHUTTER_MANAGER){
          actualizarMenuPantalla();
        } 
      }else{
        // Check again next time
        SystemFunctionTask.detach();
        SystemFunctionTask.attach(SHUTTER_DURATION_SECONDS,SystemFunctionManager);
      }
    }
  } 
  // Check if time is ahead from sunrise time.
  time_t nowSecondsUTC = time(NULL) + MyWeather.timezoneshift;
  if( (nowSecondsUTC % (60 * 60 * 24)) >= (MyWeather.sunriseSecondsUTC+MyWeather.timezoneshift)){
    struct tm * timenow;
    timenow = gmtime(&nowSecondsUTC);
    // Check if a reset of the data is needed
    resetScheduledData(timenow);
    // Check if today there is a scheduled task and isn't done yet.
    if(_storedData.ScheduledData[timenow->tm_mon][timenow->tm_mday-1] == 0x1){
      // Switch to true the notification flag
      _storedData.ScheduledData[timenow->tm_mon][timenow->tm_mday-1] = 0x3;
      _SystemState = SYSTEM_STATE_SLEEP_MANAGER;
    }
    
  }
  Serial.println("System Manager updated");
}

void resetScheduledData(struct tm * timenow){
  if(timenow->tm_yday == 0 && ScheduledDataResetValue == 0){
    for(int mes = 0; mes < 12 ; mes++){
      for(int dia = 0; dia < 31 ; dia ++ ){
        _storedData.ScheduledData[mes][dia] &= 0x1;
      }
    }
    EEPROM_Write(&_storedData);
    ScheduledDataResetValue = 1;
  }else if(timenow->tm_yday != 0 && ScheduledDataResetValue != 0){
    ScheduledDataResetValue = 0;
  }
}

void activarModoTrabajo() {
  Serial.println("Modo trabajo activado");
  struct RSCP_Arg_buzzer_action buzzerAction;
  buzzerAction.action = RSCP_DEF_BUZZER_ACTION_ON;
  buzzerAction.volume = 300;
  buzzerAction.duration_ms = 500;
  rscpSendAction(RSCP_CMD_SET_BUZZER_ACTION, (uint8_t *)&buzzerAction, sizeof(buzzerAction), 1000);
  getWeatherDataFunction();
  time_t nowSecondsUTC = time(NULL) % (60 * 60 * 24);
  // According to Requirements the current time should be between sunrise and sunset time.
  if((MyWeather.sunriseSecondsUTC <= nowSecondsUTC) && (nowSecondsUTC <= MyWeather.sunsetSecondsUTC)){
    // According to Requirements there is no need if the Cloudiness is less than 75%
    if(MyWeather.Cloudiness < 75){
      // Both shutter lowered when 60 < SunAzimuth < 260
      if( 60 < MyWeather.SunAzimuth  && MyWeather.SunAzimuth < 260){
        bajarPersiana(SELECCION_MENU_PERSIANA_DERECHA);
        delay(500);
        bajarPersiana(SELECCION_MENU_PERSIANA_CENTRAL);
        delay(500);
        struct RSCP_Arg_switchrelay switchRelayArg;
        switchRelayArg.status = RSCP_DEF_SWITCH_RELAY_ON;
        rscpSendAction(RSCP_CMD_SET_SWITCH_RELAY, (uint8_t *)&switchRelayArg, sizeof(switchRelayArg), 1000);
      // Center shutter only lower when 100 < SunAzimuth < 230
      }else if( 100 < MyWeather.SunAzimuth  && MyWeather.SunAzimuth < 230){
        bajarPersiana(SELECCION_MENU_PERSIANA_CENTRAL);
      }
    }
  }
};

int procesoConfirmarFecha(int & sday, int & smonth){
  bool ConfirmationState = false;
  lcd.clear(); lcd.home();
  if(sday == -1){
    lcd.print("ACTIVAR SIEMPRE?");
    lcd.setCursor(0, 1);
    lcd.print("<             OK");
  }else if(sday == -2){
    lcd.print(" CANCELAR TODO? ");
    lcd.setCursor(0, 1);
    lcd.print("<             OK");
  }else{
    lcd.print("CONFIRMAR  FECHA");
    lcd.setCursor(0, 1);
    lcd.print("<             OK");
    lcd.setCursor(5, 1);
    lcd.print(makeLcdStringDate(sday, smonth));
  }
  while ((_SystemState != SYSTEM_STATE_ENTERING_IDLE || _SystemState != SYSTEM_STATE_IDLING) && _SystemState != SYSTEM_STATE_MENU_SLEEP_MODE)
  {
    switch (buttonPressed()) {
      case BUTTON_STATE_LEFT : {
          _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
          break;
        }
      case BUTTON_STATE_RIGHT : {
          ConfirmationState = true;
          _SystemState= SYSTEM_STATE_MENU_SLEEP_MODE;
          break;
        }
    }
    yield();
  }
  return ConfirmationState;
};

int procesoSeleccionarFecha(int & sday, int & smonth){
  int SleepTaskState = 0;
  bool FechaConfirmada = false;
  int daysOfMonths [] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  while ((_SystemState != SYSTEM_STATE_ENTERING_IDLE || _SystemState != SYSTEM_STATE_IDLING) && _SystemState != SYSTEM_STATE_MENU_SLEEP_MODE)
  {
    switch (SleepTaskState) {
      case 0 : {
          lcd.clear(); lcd.home();
          lcd.print("SEL. FECHA ");
          lcd.setCursor(11, 0);
          lcd.print(makeLcdStringDate(sday, smonth));
          lcd.setCursor(0, 1);
          lcd.write(5);
          lcd.print("    +    -    >");
          SleepTaskState = 1;
          break;
        }
      case 1 : {
          switch (buttonPressed()) {
            case BUTTON_STATE_UP : {
                if ( (sday + 1) > daysOfMonths[smonth - 1]) {
                  if (smonth == 12) smonth = 1;
                  else smonth++;
                  sday = 1;
                } else sday += 1;
                lcd.setCursor(11, 0);
                lcd.print(makeLcdStringDate(sday, smonth));
                break;
              }
            case BUTTON_STATE_DOWN : {
                if ( (sday - 1) == 0) {
                  if (smonth == 1) smonth = 12;
                  else smonth--;
                  sday = daysOfMonths[smonth - 1];
                } else sday -= 1;
                lcd.setCursor(11, 0);
                lcd.print(makeLcdStringDate(sday, smonth));
                break;
              }
            case BUTTON_STATE_LEFT : {
                _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
                break;
              }
            case BUTTON_STATE_RIGHT : {
                SleepTaskState = 2;
                lcd.setCursor(0, 1);
                lcd.print("<    +    -   OK");
                break;
              }
          }
          break;
        }
      case 2 : {
          switch (buttonPressed()) {
            case BUTTON_STATE_UP : {
                if (smonth == 12) smonth = 1;
                else smonth++;
                if (sday > daysOfMonths[smonth - 1]) sday = daysOfMonths[smonth - 1];
                lcd.setCursor(11, 0);
                lcd.print(makeLcdStringDate(sday, smonth));
                break;
              }
            case BUTTON_STATE_DOWN : {
                if (smonth == 1) smonth = 12;
                else smonth--;
                if (sday > daysOfMonths[smonth - 1]) sday = daysOfMonths[smonth - 1];
                lcd.setCursor(11, 0);
                lcd.print(makeLcdStringDate(sday, smonth));
                break;
              }
            case BUTTON_STATE_LEFT : {
                SleepTaskState = 0;
                break;
              }
            case BUTTON_STATE_RIGHT : {
                FechaConfirmada = procesoConfirmarFecha(sday,smonth);
                if(!FechaConfirmada) SleepTaskState = 0;
                break;
              }
          }
          break;
        }
    }
    yield();
  }
  return FechaConfirmada;
}

void procesoDesactivarTarea() {
  int sday = 1, smonth = 1;
  if (procesoSeleccionarFecha(sday, smonth)) {
    Serial.println("Desactivar tarea en fecha seleccionada");
    _storedData.ScheduledData[smonth-1][sday-1] &= 0x2;
    Serial.print(sday);
    Serial.print("-");
    Serial.println(smonth);
    EEPROM_Write(&_storedData);
  }
}

void procesoActivarTarea() {
  int sday = 1, smonth = 1;
  if (procesoSeleccionarFecha(sday, smonth)) {
    Serial.println("Activar tarea en fecha seleccionada");
    _storedData.ScheduledData[smonth-1][sday-1] = 0x1;
    Serial.print(sday);
    Serial.print("-");
    Serial.println(smonth);
    EEPROM_Write(&_storedData);
  }
}