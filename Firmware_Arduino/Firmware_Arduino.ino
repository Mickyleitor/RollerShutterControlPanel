// Domotic Shutter sketch
// This sketch is in development and uses ATmega368P.
// 
// Current protocol for I2C messages with master is as follows:
// - Two first LSB bits indicates roller action.
// 0x0 : Stop movement
// 0x1 : Up movement
// 0x2 : Down movement
// - 3,4,5 LSB bits indicates the roller shutter which will do the action.
// 0x1 : Left roller shutter
// 0x2 : Center roller shutter
// 0x4 : Right roller shutter
// - First three MSB bits indicates a buzzer command.
// 0x0 : Low volume
// 0x1 : Medium volume
// 0x2 : Maximum volume
// 0x3 : Switch on/off volume
// 0x4 : Indicates a buzzer command is issued.

#include <Wire.h>
#include "basic_defines.h"
#include "LowPower.h"
#include "somfy.h"

#define BUZZER_TIME_MILLIS 100
#define DEBOUNCE_TIME_MILLIS 100

int buzzer_running = 1;

enum States {
  IDLING,
  SWITCH_RELAY,
  PROCCESS_I2C
} SystemState;

void setup(){
  pinMode(PIN_BUZZER,OUTPUT);
  pinMode(PIN_RF_TX,INPUT); // Free 433 Mhz channel now.
  pinMode(PIN_RF_RX,INPUT);
  pinMode(PIN_RELAY,OUTPUT);
  initButtonsFunction();
  
  Serial.begin(9600);
  Wire.begin(I2C_SLAVE);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,BUZZER_TIME_MILLIS);
  delay(200);
  tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,BUZZER_TIME_MILLIS);
  delay(200);
  tone(PIN_BUZZER,BUZZER_LOW_VOLUME,BUZZER_TIME_MILLIS);
  SystemState = IDLING;
}

void loop(){
  switch( SystemState ) {
    case IDLING : {
      delay(10); // Aqui se debería ir a bajo consumo...
      break;
    }
    case SWITCH_RELAY : {
      digitalWrite(PIN_RELAY,digitalRead(PIN_BUTTON_USER));
      Serial.println("Boton pulsado");
      Serial.flush();
      SystemState = IDLING;
      break;
    }
    case PROCCESS_I2C : {
      unsigned long LastTimeSounded = 0;
      while (Wire.available()) {
        char cmd = Wire.read();         // receive byte as a character
        // Process buzzer commands
        if((cmd & 0x80) == 0x80){
          Serial.println("Buzzer command");
          // Switch on/off volume
          if( (cmd & 0xE0) == 0xE0 ){
            Serial.println("Buzzer switch on/off");
            buzzer_running = 1 ^ buzzer_running;
          }else{
            // If the buzzer is not silenced..
            // Sound the buzzer at low,medium,high volume.
            if(buzzer_running){
              if( (cmd & 0xE0) == 0x80 ){
                Serial.println("Buzzer low volume");
                tone(PIN_BUZZER,BUZZER_LOW_VOLUME,BUZZER_TIME_MILLIS);
              }
              if( (cmd & 0xE0) == 0xA0 ){
                Serial.println("Buzzer medium volume");
                tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,BUZZER_TIME_MILLIS);
              }
              if( (cmd & 0xE0) == 0xC0 ){
                Serial.println("Buzzer high volume");
                tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,BUZZER_TIME_MILLIS);
              }
              LastTimeSounded = millis();
            }
          }
        }
        // Check the flag mask regarding roller shutter according to protocol
        if( (cmd & 0x1C) >= 0x4){
          Serial.println("Roller shutter command");
          for( int roller = 0 ; roller < 3 ; roller ++){
            // If the flag mask is activated
            if( ((cmd >> (2+roller)) & 1) == 1){
              sendCommand(roller,(cmd & 0x3),2);
            }
          }
        }
      }
      // Necesitamos esperar, al menos, un tiempo minimo de BUZZER_TIME_MILLIS
      // Para que al volver a IDLING no se duerma mientras siguen saltando
      // las interrupciones del Timer2 usado por tone()
      while( abs(millis() - LastTimeSounded) <= BUZZER_TIME_MILLIS);
      SystemState = IDLING;
      break;
    }
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  SystemState = PROCCESS_I2C;
}

void isrButton(){
  detachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER));
  TCNT1 = 0;   // Reset counter to 0
  TCCR1B = bit(WGM12) | bit (CS12);   // CTC, scale to clock / 256  
}

ISR(TIMER1_COMPA_vect)
{
  SystemState = SWITCH_RELAY;
  TCCR1B = 0;
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER),isrButton,FALLING);
}

void initButtonsFunction(){
  pinMode(PIN_BUTTON_USER,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER),isrButton,FALLING);

  // set up Timer 1
  TCCR1A = 0;          // normal operation
  TCCR1B = 0;
  // TCCR1B = bit(WGM12) | bit (CS12);   // CTC, scale to clock / 1024
  OCR1A =  ( ( F_CPU * DEBOUNCE_TIME_MILLIS ) / (256 * 2 * 1000)) - 1;       // compare A register value 
  TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
}
