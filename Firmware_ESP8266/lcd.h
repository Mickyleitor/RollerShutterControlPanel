#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include "basic_defines.h"
#include "ESP8266_Utils.h"

extern enum SystemState _SystemState;
extern enum seleccionMenu _seleccionMenu;
extern struct ShutterParameters ShutterData [];

LiquidCrystal_PCF8574 lcd(LCD_I2C_ADDRESS);

void apagarBrilloPantalla()  {
  lcd.setBacklight(0);
}

void encenderBrilloPantalla() {
  lcd.setBacklight(255);
}

String makeLcdStringDate( int sday, int smonth) {
  String date = "";
  if (sday < 10) date += "0";
  date += sday;
  date += "/";
  if (smonth < 10) date += "0";
  date += smonth;
  return date;
}

void seleccionarAnterior() {
  switch (_seleccionMenu) {
    case SELECCION_MENU_PERSIANA_IZQUIERDA : {
        _seleccionMenu = SELECCION_MENU_OPTION_SLEEP_MODE;
        _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
        break;
      };
    case SELECCION_MENU_PERSIANA_CENTRAL : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_IZQUIERDA;
        break;
      };
    case SELECCION_MENU_PERSIANA_DERECHA : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_CENTRAL;
        break;
      };
    case SELECCION_MENU_OPTION_JOB_MODE : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_DERECHA;
        _SystemState = SYSTEM_STATE_SHUTTER_MANAGER;
        break;
    };
    case SELECCION_MENU_OPTION_SLEEP_MODE : {
        _seleccionMenu = SELECCION_MENU_OPTION_JOB_MODE;
        _SystemState = SYSTEM_STATE_MENU_JOB_MODE;
        break;
    };
  }
}
void seleccionarSiguiente() {
  switch (_seleccionMenu) {
    case SELECCION_MENU_PERSIANA_IZQUIERDA : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_CENTRAL;
        break;
      };
    case SELECCION_MENU_PERSIANA_CENTRAL : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_DERECHA;
        break;
      };
    case SELECCION_MENU_PERSIANA_DERECHA : {
        _seleccionMenu = SELECCION_MENU_OPTION_JOB_MODE;
        _SystemState = SYSTEM_STATE_MENU_JOB_MODE;
        break;
      };
    case SELECCION_MENU_OPTION_JOB_MODE : {
        _seleccionMenu = SELECCION_MENU_OPTION_SLEEP_MODE;
        _SystemState = SYSTEM_STATE_MENU_SLEEP_MODE;
        break;
    };
    case SELECCION_MENU_OPTION_SLEEP_MODE : {
        _seleccionMenu = SELECCION_MENU_PERSIANA_IZQUIERDA;
        _SystemState = SYSTEM_STATE_SHUTTER_MANAGER;
        break;
    };
  }
}

int initLCDFunction(int32_t timeout_ms) {
  int status = -1;
  // Search LCD into I2C line:
  while( status != 0 && timeout_ms > 0) {
    Wire.begin(2, 14);
    Wire.beginTransmission(LCD_I2C_ADDRESS);
    status = Wire.endTransmission();
    if (status != 0) {
      // wait 2 seconds for reconnection:
      delay(2000);
      timeout_ms -= 2000;
    }
  }

  if(timeout_ms <= 0) {
    return -1;
  }

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  uint8_t customArrayChar[6][8] =
  {
    /* Flecha izquierda */  {0x00, 0x07, 0x0E, 0x1C, 0x1C, 0x0E, 0x07, 0x00},
    /* Flecha arriba    */  {0x00, 0x04, 0x0E, 0x1F, 0x1B, 0x11, 0x00, 0x00},
    /* Flecha abajo     */  {0x00, 0x00, 0x11, 0x1B, 0x1F, 0x0E, 0x04, 0x00},
    /* Flecha derecha   */  {0x00, 0x1C, 0x0E, 0x07, 0x07, 0x0E, 0x1C, 0x00},
    /* Flecha STOP      */  {0x00, 0x0E, 0x1B, 0x11, 0x11, 0x1B, 0x0E, 0x00},
    /* Flecha arribacan */  {0x04, 0x0E, 0x1F, 0x15, 0x04, 0x04, 0x07, 0x00}
  };
  for( int i = 0 ; i < 6 ; i ++ ){
    lcd.createChar(i, customArrayChar[i]);
  }
  lcd.home(); lcd.clear();
  lcd.print("...INICIANDO...");
  return 0;
}

void mostrarHoraPantalla() {
  time_t now;
  struct tm * timeinfo;
  now = time(&now) + MyWeather.timezoneshift;
  timeinfo = localtime(&now);
  lcd.clear();
  lcd.setCursor(3, 0);
  if ((timeinfo->tm_hour) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_hour), DEC);

  lcd.print(':');
  if ((timeinfo->tm_min) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_min), DEC);
/*
  lcd.print(':');
  if ((timeinfo->tm_sec) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_sec), DEC);
*/
  lcd.setCursor(12, 0);
  if (((int)MyWeather.TemperatureDegree) < 10) lcd.print('0');
  lcd.print(((int)MyWeather.TemperatureDegree), DEC);
  lcd.print((char)223);
  lcd.print('C');

  lcd.setCursor(0, 1);
  if ((timeinfo->tm_mday) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_mday), DEC);

  lcd.print('/');
  if ((timeinfo->tm_mon+1) < 10) lcd.print('0');
  lcd.print((timeinfo->tm_mon+1), DEC);

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
  switch (_SystemState) {
    case SYSTEM_STATE_WAKEUP :
      // fall through
    case SYSTEM_STATE_SHUTTER_MANAGER : {
        if (_seleccionMenu < 3) {
          String namePersiana [] = {" PERSIANA IZQDA ", "PERSIANA CENTRAL", "PERSIANA DERECHA", "  ERROR MENU  "};
          lcd.print(namePersiana[_seleccionMenu]);
          lcd.setCursor(0, 1);
          lcd.print("<");
          lcd.setCursor(5, 1);
          
          if (ShutterData[_seleccionMenu].status == 1) lcd.write(4);
          else lcd.write(1);
          
          lcd.setCursor(10, 1);
          
          if (ShutterData[_seleccionMenu].status == 2) lcd.write(4);
          else lcd.write(2);
          
          lcd.setCursor(15, 1);
          lcd.print(">");
        }
        break;
      }
    case SYSTEM_STATE_MENU_JOB_MODE : {
        lcd.print("  MODO TRABAJO  ");
        lcd.setCursor(0, 1);
        lcd.print("<        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_SLEEP_MODE : {
        lcd.print("  MODO DORMIR   ");
        lcd.setCursor(0, 1);
        lcd.print("<        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE : {   
        lcd.print(" ACTIVAR UN DIA ");
        lcd.setCursor(0, 1);
        lcd.write(5);
        lcd.print("        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE : {
        lcd.print("CANCELAR UN DIA ");
        lcd.setCursor(0, 1);
        lcd.print("<        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE : {
        lcd.print("ACTIVAR  SIEMPRE");
        lcd.setCursor(0, 1);
        lcd.print("<        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_ALL_SLEEP_MODE : {
        lcd.print(" CANCELAR TODO  ");
        lcd.setCursor(0, 1);
        lcd.print("<        OK     ");
        break;
      }
  }
}