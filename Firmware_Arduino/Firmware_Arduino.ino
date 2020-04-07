#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <EEPROM.h>
#include "basic_defines.h"
#include <TimeLib.h>

LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);

int task_button = TASK_UPDATE_LCD;
int selected_shutter = 1;
enum state {GOING_DOWN,STOPPED,GOING_UP};
String shuttername [3] = {"Persiana derecha","Persiana central","Persiana lateral"};
state shutterstate [3] = {STOPPED,STOPPED,STOPPED};
int shutterprevioustime [3] = {0,0,0};
int shuttercurrenttime [3] = {0,0,0};
unsigned long LastTimeButtonPressed = 0xFFFF;
unsigned long LastTimeScreenRefreshed = 0xFFFF;

void setup(){ 
  // delay(5000);
  Serial.begin(9600);
  do{
    delay(1000);
    Wire.begin();
    Wire.beginTransmission(ADDRESS_I2C_LCD);
    tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,500);
  }while(Wire.endTransmission() != 0);
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  byte customArrayChar[5][8] = {{0x00,0x00,0x1F,0x1F,0x0E,0x04,0x00,0x00},{0x00,0x00,0x04,0x0E,0x1F,0x1F,0x00,0x00},{0x00,0x00,0x06,0x0E,0x1E,0x0E,0x06,0x00},{0x00,0x00,0x0C,0x0E,0x0F,0x0E,0x0C,0x00},{0x00,0x0E,0x1F,0x11,0x11,0x1F,0x0E,0x00}};
  for ( int i = 0 ; i < 5 ; i ++ ) lcd.createChar(i, customArrayChar[i]);

  DDRB &= ~bit(DDB0) | ~bit(DDB1) | ~bit(DDB2) | ~bit(DDB3); // Clear the PBX pin
  // PBX (PCINT0-5 pin) is now an input
  PCMSK0 |= bit (PCINT0) | bit (PCINT1) |bit (PCINT2) |bit (PCINT3); // set PCINT0-3 to trigger an interrupt on state change 
  PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
  PCICR |= bit (PCIF0); // set PCIE0 to enable PCMSK0 scan
  
  setTime(21,31,00,4,4,2020); // 12:00 (H24) 9 de Septiembre del 2019
}

void loop(){
    switch (task_button) {
      case PIN_BUTTON_LEFT : {
        selected_shutter = (selected_shutter+1) % 3;
        updateScreen();
        task_button = TASK_UPDATE_LCD;
        break;
      }
      case PIN_BUTTON_CANCEL : {
        updateScreen();
        task_button = TASK_UPDATE_LCD;
        break;
      }
      case PIN_BUTTON_OK : {
        updateScreen();
        task_button = TASK_UPDATE_LCD;
        
        break;
      }
      case PIN_BUTTON_RIGHT : {
        selected_shutter = (selected_shutter < 1) ? 2 : selected_shutter - 1;
        updateScreen();
        task_button = TASK_UPDATE_LCD;
        break;
      }
      case TASK_UPDATE_LCD : {
        if ((millis() - LastTimeButtonPressed > SHUTTER_CICLETIME) && (millis() - LastTimeScreenRefreshed > 1000))  {
          LastTimeButtonPressed
          updateMainScreen();
        }
      }
    }
}

ISR (PCINT0_vect)
{
  if(bitRead(PINB,PIN_BUTTON_LEFT)==0){
    tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
    task_button = PIN_BUTTON_LEFT;
  }
  if(bitRead(PINB,PIN_BUTTON_CANCEL)==0){
    tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
    task_button = PIN_BUTTON_CANCEL;
  }
  if(bitRead(PINB,PIN_BUTTON_OK)==0){
    tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
    task_button = PIN_BUTTON_OK;
  }
  if(bitRead(PINB,PIN_BUTTON_RIGHT)==0){
    tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
    task_button = PIN_BUTTON_RIGHT;
  }
  LastTimeButtonPressed = millis();
}

void updateScreen(){
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.print(shuttername[selected_shutter]);
  lcd.setCursor(1,1);
  lcd.write(LCD_CHAR_ARROWLEFT);
  lcd.setCursor(5,1);
  if(shutterstate[selected_shutter] == GOING_UP){
    lcd.write(LCD_CHAR_STOP);
  }else{
    lcd.write(LCD_CHAR_ARROWUP);
  }
  /*
  lcd.setCursor(7,1);
  lcd.print(shuttercurrenttime[selected_shutter]/1000);
  */
  lcd.setCursor(10,1);
  if(shutterstate[selected_shutter] == GOING_DOWN){
    lcd.write(LCD_CHAR_STOP);
  }else{
    lcd.write(LCD_CHAR_ARROWDOWN);
  }
  lcd.setCursor(14,1);
  lcd.write(LCD_CHAR_ARROWRIGHT);

  LastTimeScreenRefreshed = millis();
}

void updateMainScreen(){
  time_t T = now();
  lcd.clear();
  lcd.setCursor(0, 1);
  if (day(T)<10) lcd.print('0');
  lcd.print(day(T), DEC);
  
  lcd.print('/');
  if (month(T)<10) lcd.print('0');
  lcd.print(month(T), DEC);
  
  lcd.print('/');
  lcd.print(year(T), DEC);
  lcd.print(' ');
  
  lcd.setCursor(4, 0);
  if (hour(T)<10) lcd.print('0');
  lcd.print(hour(T), DEC);
  
  lcd.print(':');
  if (minute(T)<10) lcd.print('0');
  lcd.print(minute(T), DEC);
  
  lcd.print(':');
  if (second(T)<10) lcd.print('0');
  lcd.print(second(T), DEC);
  
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
  LastTimeScreenRefreshed = millis();
  lcd.setBacklight(0);
}
