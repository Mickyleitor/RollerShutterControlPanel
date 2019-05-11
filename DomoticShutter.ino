#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <EEPROM.h>
#include <TimeLib.h>

#define PIN_BUZZER 5
#define PIN_RF_TX 2
#define PIN_RF_RX 3
#define PIN_BUTTON_LEFT 0
#define PIN_BUTTON_CANCEL 1
#define PIN_BUTTON_OK 2
#define PIN_BUTTON_RIGHT 3
#define BUZZER_LOW_VOLUME 90000
#define BUZZER_MEDIUM_VOLUME 300
#define BUZZER_HIGH_VOLUME 5000

#define KEY_LENGTH 92
#define KEY_CRC_LENGTH 1
#define DEBUGGING 0

LiquidCrystal_PCF8574 lcd(0x3F);

struct Tcommand {
  char Key [KEY_LENGTH];
  char CRC [KEY_CRC_LENGTH];
};

void setup(){
  int error = -1;
  while(error){
    Wire.begin();
    Wire.beginTransmission(0x3F);
    error = Wire.endTransmission();
    if(error && !DEBUGGING){
      beep_alarm(BUZZER_MEDIUM_VOLUME,300,3);
      delay(5000);
    }else if(DEBUGGING){
      error = 0;
    }
  }
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
    
}

void beep_alarm(unsigned int volume, unsigned long period_ms, int times){
  while(times > 0){
    tone(PIN_BUZZER,volume,period_ms);
    delay(2*period_ms);
    times --;
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

bool readKeyFromEEPROM (int row, char  * ptr){
  char buf;
  for(int address = 0; address < (KEY_LENGTH+KEY_CRC_LENGTH); address ++){
    EEPROM.get(row*(sizeof(Tcommand)) + address, buf);
    ptr[address] = buf;
  }
  return ((char)CRC8(ptr,KEY_LENGTH) == ptr[KEY_LENGTH]);
}

ISR (PCINT0_vect)
{
  if(bitRead(PINB,PIN_BUTTON_LEFT)==0){
    beep_alarm(BUZZER_LOW_VOLUME,100,1);
  }
  if(bitRead(PINB,PIN_BUTTON_CANCEL)==0){
    beep_alarm(BUZZER_LOW_VOLUME,100,1);
  }
  if(bitRead(PINB,PIN_BUTTON_OK)==0){
    beep_alarm(BUZZER_LOW_VOLUME,100,1);
  }
  if(bitRead(PINB,PIN_BUTTON_RIGHT)==0){
    beep_alarm(BUZZER_LOW_VOLUME,100,1);
  }
}
