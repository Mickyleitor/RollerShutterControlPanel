// Domotic Shutter sketch by Mickyleitor
// This sketch is in development and uses ATmega368P.
// 
// Libraries used and need to be installed from Arduino IDE
// - Low-Power by rocketscream/ version 1.6.0 : https://github.com/rocketscream/Low-Power
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
// - First three MSB bits indicates a buzzer or switch change command.
// The folling fields are only valid if the first bit is high.
// 0x0 : Low volume
// 0x1 : Medium volume
// 0x2 : Maximum volume
// 0x3 : Switch on/off volume
// 0x4 : Indicates a buzzer command is issued.
// The folling fields are only valid if the first bit is low
// 0x0 : Intentionally unused
// 0x1 : Switch ON relay
// 0x2 : Switch OFF relay
// 0x3 : Change the state of the switch relay

#include <Wire.h>
#include "basic_defines.h"
#include "LowPower.h"
#include "remote.h"

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
  
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
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
      // Pendiente hacer modo bajo consumo
      // LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_ON);
      delay(10);
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
        Serial.print("Received :");
        Serial.println(cmd,BIN);
        // Process buzzer commands
        if((cmd & FLAG_BIT_BUZZER) == FLAG_BIT_BUZZER){
          Serial.println("Buzzer command");
          // Switch on/off volume
          if( (cmd & FLAG_MASK_BUZZER) == COMMAND_BUZZER_CHANGE ){
            Serial.println("Buzzer switch on/off");
            buzzer_running = 1 ^ buzzer_running;
          }else{
            // If the buzzer is not silenced..
            // Sound the buzzer at low,medium,high volume.
            if(buzzer_running){
              if( (cmd & FLAG_MASK_BUZZER) == COMMAND_BUZZER_LOW_VOLUME ){
                Serial.println("Buzzer low volume");
                tone(PIN_BUZZER,BUZZER_LOW_VOLUME,BUZZER_TIME_MILLIS);
              }
              if( (cmd & FLAG_MASK_BUZZER) == COMMAND_BUZZER_MEDIUM_VOLUME ){
                Serial.println("Buzzer medium volume");
                tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,BUZZER_TIME_MILLIS);
              }
              if( (cmd & FLAG_MASK_BUZZER) == COMMAND_BUZZER_HIGH_VOLUME ){
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
        // Process Switch relay commands
        if(((cmd & FLAG_BIT_BUZZER) == 0) && ((cmd & FLAG_MASK_LIGHT) >= 1)){
          Serial.println("Switch relay command");
          if( (cmd & FLAG_MASK_LIGHT) == COMMAND_LIGHT_ON ){
            Serial.println("Switch ON relay");
            digitalWrite(PIN_RELAY,HIGH);
          }
          if( (cmd & FLAG_MASK_LIGHT) == COMMAND_LIGHT_OFF ){
            Serial.println("Switch OFF relay");
            digitalWrite(PIN_RELAY,LOW);
          }
          if( (cmd & FLAG_MASK_LIGHT) == COMMAND_LIGHT_CHANGE ){
            Serial.println("Change the state of the switch relay");
            digitalWrite(PIN_RELAY,!digitalRead(PIN_RELAY));
          }
        }
      }
      // Necesitamos esperar, al menos, un tiempo minimo de BUZZER_TIME_MILLIS
      // Para que al volver a IDLING no se duerma mientras siguen saltando
      // las interrupciones del Timer2 usado por tone()
      while( abs(millis() - LastTimeSounded) <= BUZZER_TIME_MILLIS );
      // Volvemos a IDLING pues ya hemos procesado todas las tareas
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

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(0x3); // respond with message of 1 byte
  // as expected by master
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
