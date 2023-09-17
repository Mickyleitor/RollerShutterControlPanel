#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include "basic_defines.h"
#include "ESP8266_Utils.h"

extern enum SystemState _SystemState;
extern enum seleccionMenu _seleccionMenu;
extern struct ShutterParameters ShutterData [];

LiquidCrystal_PCF8574 _lcd(LCD_I2C_ADDRESS);

#define LCD_SPECIAL_CHAR_BASE                                         (char)(10)
#define LCD_SPECIAL_CHAR_LEFT_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 0)
#define LCD_SPECIAL_CHAR_UP_ARROW              (char)(LCD_SPECIAL_CHAR_BASE + 1)
#define LCD_SPECIAL_CHAR_DOWN_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 2)
#define LCD_SPECIAL_CHAR_RIGHT_ARROW           (char)(LCD_SPECIAL_CHAR_BASE + 3)
#define LCD_SPECIAL_CHAR_STOP_ARROW            (char)(LCD_SPECIAL_CHAR_BASE + 4)
#define LCD_SPECIAL_CHAR_UP_ARROW_CAN          (char)(LCD_SPECIAL_CHAR_BASE + 5)

void apagarBrilloPantalla()  {
  _lcd.setBacklight(0);
}

void encenderBrilloPantalla() {
  _lcd.setBacklight(255);
}

void sendLcdBuffer(String line1, String line2) {
  _lcd.home();
  _lcd.clear();
  for(int i = 0 ; i < 16 ; i++) {
    _lcd.setCursor(i, 0);
    if( (line1[i] >= LCD_SPECIAL_CHAR_BASE) && (line1[i] < LCD_SPECIAL_CHAR_BASE + 6)) {
        _lcd.write(line1[i] - LCD_SPECIAL_CHAR_BASE);
    }else{
        _lcd.print(line1[i]);
    }
    _lcd.setCursor(i, 1);
    if( (line2[i] >= LCD_SPECIAL_CHAR_BASE) && (line2[i] < LCD_SPECIAL_CHAR_BASE + 6)) {
        _lcd.write(line2[i] - LCD_SPECIAL_CHAR_BASE);
    }else{
        _lcd.print(line2[i]);
    }
  }
}

bool sendLcdBuffer(String buffer) {
  String line1 = buffer.substring(0, 16);
  String line2 = buffer.substring(16, 32);
  sendLcdBuffer(line1, line2);
  return true;
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

  _lcd.begin(16, 2);
  encenderBrilloPantalla();
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
    _lcd.createChar(i, customArrayChar[i]);
  }
  return 0;
}

void mostrarHoraPantalla() {
  time_t now;
  struct tm * timeinfo;
  now = time(&now) + MyWeather.timezoneshift;
  timeinfo = localtime(&now);
  String buffer = "   ";
  
  if ((timeinfo->tm_hour) < 10){
    buffer += String('0');
  }
  buffer += String(timeinfo->tm_hour);
  buffer += String(':');

  if ((timeinfo->tm_min) < 10){
    buffer += String('0');
  }
  buffer += String(timeinfo->tm_min);
  buffer += String("    ");

  if (((int)MyWeather.TemperatureDegree) < 10){
    buffer += String('0');
  }
  buffer += String((int)MyWeather.TemperatureDegree);
  buffer += (char)223;
  buffer += String("C");

  if ((timeinfo->tm_mday) < 10){
    buffer += String('0');
  }
  buffer += String(timeinfo->tm_mday);
  buffer += String("/");

  if ((timeinfo->tm_mon+1) < 10){
    buffer += String('0');
  }
  buffer += String(timeinfo->tm_mon+1);
  buffer += String("/");

  buffer += String((timeinfo->tm_year) + 1900);
  buffer += String("  ");

  int dayofweek = (timeinfo->tm_wday);
  switch (dayofweek) {
    case 0:
      buffer += String("Dom.");
      break;
    case 1:
      buffer += String("Lun.");
      break;
    case 2:
      buffer += String("Mar.");
      break;
    case 3:
      buffer += String("Mie.");
      break;
    case 4:
      buffer += String("Jue.");
      break;
    case 5:
      buffer += String("Vie.");
      break;
    case 6:
      buffer += String("Sab.");
      break;
    default:
      buffer += String("Err.");
      break;
  }
  sendLcdBuffer(buffer);
}

void actualizarMenuPantalla() {
  String buffer = "";
  switch (_SystemState) {
    case SYSTEM_STATE_WAKEUP :
      // fall through
    case SYSTEM_STATE_SHUTTER_MANAGER : {
        if (_seleccionMenu < 3) {
          String namePersiana [] = {" PERSIANA IZQDA ", "PERSIANA CENTRAL", "PERSIANA DERECHA", "  ERROR MENU  "};
          buffer += namePersiana[_seleccionMenu];
          buffer += String("<    ");
          
          if (ShutterData[_seleccionMenu].status == 1){
            buffer += LCD_SPECIAL_CHAR_STOP_ARROW;
          }else{
            buffer += LCD_SPECIAL_CHAR_UP_ARROW;
          }

          buffer += String("    ");

          if (ShutterData[_seleccionMenu].status == 2){
            buffer += LCD_SPECIAL_CHAR_STOP_ARROW;
          }else{
            buffer += LCD_SPECIAL_CHAR_DOWN_ARROW;
          }

          buffer += String("    >");
        }
        break;
      }
    case SYSTEM_STATE_MENU_JOB_MODE : {
        buffer += String("  MODO TRABAJO  <        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_SLEEP_MODE : {
        buffer += String("  MODO DORMIR   <        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_SLEEP_MODE : {
        buffer += String(" ACTIVAR UN DIA ");
        buffer += LCD_SPECIAL_CHAR_UP_ARROW_CAN;
        buffer += String("        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_SLEEP_MODE : {
        buffer += String("CANCELAR UN DIA <        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_ACTIVATE_ALL_SLEEP_MODE : {
        buffer += String("ACTIVAR  SIEMPRE<        OK    >");
        break;
      }
    case SYSTEM_STATE_MENU_DEACTIVATE_ALL_SLEEP_MODE : {
        buffer += String(" CANCELAR TODO  <        OK     ");
        break;
      }
  }
  if ( buffer != "" ) {
    sendLcdBuffer(buffer);
  }
}