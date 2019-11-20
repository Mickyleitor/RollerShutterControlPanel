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
time_t T;

void setup(){
  Serial.begin(9600);
  do{
    delay(1000);
    Wire.begin();
    Wire.beginTransmission(ADDRESS_I2C_LCD);
  }while(Wire.endTransmission() != 0);
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  byte customArrayChar[5][8] = {{0x00,0x00,0x1F,0x1F,0x0E,0x04,0x00,0x00},{0x00,0x00,0x04,0x0E,0x1F,0x1F,0x00,0x00},{0x00,0x00,0x06,0x0E,0x1E,0x0E,0x06,0x00},{0x00,0x00,0x0C,0x0E,0x0F,0x0E,0x0C,0x00},{0x00,0x0E,0x1F,0x11,0x11,0x1F,0x0E,0x00}};
  for ( int i = 0 ; i < 5 ; i ++ ) lcd.createChar(i, customArrayChar[i]);

  setTime(14,33,30,9,9,2019); // 12:00 (H24) 9 de Septiembre del 2019
  
  updateMainScreen();
 
  DDRB &= ~bit(DDB0) | ~bit(DDB1) | ~bit(DDB2) | ~bit(DDB3); // Clear the PBX pin
  // PBX (PCINT0-5 pin) is now an input
  PCMSK0 |= bit (PCINT0) | bit (PCINT1) |bit (PCINT2) |bit (PCINT3); // set PCINT0-3 to trigger an interrupt on state change 
  PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
  PCICR |= bit (PCIF0); // set PCIE0 to enable PCMSK0 scan
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
        moveShutter(GOING_UP);
        updateScreen();
        task_button = TASK_UPDATE_LCD;
        break;
      }
      case PIN_BUTTON_OK : {
        moveShutter(GOING_DOWN);
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
          updateMainScreen();
        }
      }
    }
}

// This shouldn't be used inside ISR
void beep_alarm(unsigned int volume, unsigned long period_ms, unsigned int times){
  for(int i = 0; i < times; i ++){
    tone(PIN_BUZZER,volume,period_ms);
    delay(2*period_ms);
  }
}

byte CRC8(const byte *data, size_t dataLength)
{
  byte crc = 0x00;
  while (dataLength--) 
  {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--)
   {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
     {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
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


// Send a RF 'order' to 'shutter'
bool sendCommand(unsigned int shutter, unsigned int order){
  Tcommand command;
  if(shutter < 3 && order < 3){ // Check out of range
    EEPROM.get( ((shutter * 3) + order) * sizeof(struct Tcommand), command);
    if(CRC8(command.Key,KEY_LENGTH) == command.CRC){ // Check true data
      for(int i = 0 ; i < REPEAT_MSG_TIMES ; i ++){
        // Loop bit per bit the data writting the bit state 0-1 to the TX pin
        int bits = 0;
        for(int index = 0; index < KEY_LENGTH ; index++){
          for(bits = 7; bits >= 0; bits--){
            bitWrite(PORT_RF_TX,PIN_RF_TX,(command.Key[index] >> bits) & 1);
            // Delay 460 uS every bit
            delayMicroseconds(490);
          }
        }
      }
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
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
  /*
  lcd.home(); lcd.clear();
  

  lcd.setCursor(6,0);
  lcd.print(hour(T));  
  lcd.print(+ ":") ;
  lcd.print(minute(T));
  // lcd.print(+ ":") ;
  // lcd.print(second(T));
  lcd.setCursor(4,1);
  lcd.print(day(T));
  lcd.print(+ "/") ;
  lcd.print(month(T));
  lcd.print(+ "/") ;
  lcd.print(year(T)); 
  lcd.print( " ") ;

  LastTimeScreenRefreshed = millis();
  lcd.setBacklight(0);
  */
  T = now();
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

void moveShutter(state GOING_TO){
  if(shutterstate[selected_shutter] == GOING_TO){
    if(sendCommand(selected_shutter,STOPPED)){
      shuttercurrenttime[selected_shutter] += ((GOING_TO == GOING_UP) ? -1 : 1)*(millis() - shutterprevioustime[selected_shutter]);
      shutterstate[selected_shutter] = STOPPED;
    }
  }else{
    if(sendCommand(selected_shutter,GOING_TO)){
      if(shutterstate[selected_shutter] == ((GOING_TO == GOING_UP) ? GOING_DOWN : GOING_UP)){
        shuttercurrenttime[selected_shutter] += ((GOING_TO == GOING_UP) ? 1 : -1)*(millis() - shutterprevioustime[selected_shutter]);
      }
      shutterprevioustime[selected_shutter] = millis();
      shutterstate[selected_shutter] = GOING_TO;
    }
  }
  shuttercurrenttime[selected_shutter] = constrain(shuttercurrenttime[selected_shutter], 0, SHUTTER_CICLETIME);
}