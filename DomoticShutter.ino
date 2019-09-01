#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <EEPROM.h>
#include "basic_defines.h"

LiquidCrystal_PCF8574 lcd(ADDRESS_I2C_LCD);

int task_button = -1;

void setup(){
  Serial.begin(9600);
  do{
    delay(1000);
    Wire.begin();
    Wire.beginTransmission(ADDRESS_I2C_LCD);
  }while(Wire.endTransmission() != 0);
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
 
  DDRB &= ~bit(DDB0) | ~bit(DDB1) | ~bit(DDB2) | ~bit(DDB3); // Clear the PBX pin
  // PBX (PCINT0-5 pin) is now an input
  PCMSK0 |= bit (PCINT0) | bit (PCINT1) |bit (PCINT2) |bit (PCINT3); // set PCINT0-3 to trigger an interrupt on state change 
  PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
  PCICR |= bit (PCIF0); // set PCIE0 to enable PCMSK0 scan
}

void loop(){
    switch (task_button) {
      case PIN_BUTTON_LEFT : {
        if(!sendCommand(2,0)) beep_alarm(BUZZER_MEDIUM_VOLUME,100,3);
        task_button = -1;
        break;
      }
      case PIN_BUTTON_CANCEL : {
        if(!sendCommand(2,1)) beep_alarm(BUZZER_MEDIUM_VOLUME,100,3);
        task_button = -1;
        break;
      }
      case PIN_BUTTON_OK : {
        if(!sendCommand(2,2)) beep_alarm(BUZZER_MEDIUM_VOLUME,100,3);
        task_button = -1;
        break;
      }
      case PIN_BUTTON_RIGHT : {
        if(!sendCommand(1,1)) beep_alarm(BUZZER_MEDIUM_VOLUME,100,3);
        task_button = -1;
        break;
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
}

bool sendCommand(unsigned int shutter, unsigned int order){
  Tcommand command;
  if(shutter < 3 && order < 3){
    EEPROM.get( ((shutter * 3) + order) * sizeof(struct Tcommand), command);
    if(CRC8(command.Key,KEY_LENGTH) == command.CRC){
      int bits = 0;
      for(int index = 0; index < KEY_LENGTH ; index++){
        for(bits = 7; bits >= 0; bits--){
          bitWrite(PORT_RF_TX,PIN_RF_TX,(command.Key[index] >> bits) & 1);
          delayMicroseconds(490);
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
