// Domotic Shutter sketch
// This sketch is in development and uses MEGA 2560.
// In the future it should be adapted to ATmega368P

#include <Wire.h>
#include "basic_defines.h"

void setup(){
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,100);
  delay(500);
  tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,100);
  delay(500);
  tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
}

void loop(){
  
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    if(c == 'A') tone(PIN_BUZZER,BUZZER_LOW_VOLUME,100);
    if(c == 'B') tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,100);
    if(c == 'C') tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,100);
  }
}
