// Domotic Shutter sketch
// This sketch is in development and uses MEGA 2560.
// In the future it should be adapted to ATmega368P

#include <Wire.h>
#include "basic_defines.h"
#include "LowPower.h"

#define BUZZER_TIME_MILLIS 100
#define DEBOUNCE_TIME_MILLIS 100

enum States {
  IDLING,
  SWITCH_RELAY,
  PROCCESS_I2C
} SystemState;

void setup(){
  pinMode(PIN_BUZZER,OUTPUT);
  pinMode(PIN_RF_TX,OUTPUT);
  pinMode(PIN_RF_RX,INPUT);
  pinMode(PIN_RELAY,OUTPUT);
  initButtonsFunction();
  
  Serial.begin(115200);
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
        char c = Wire.read();         // receive byte as a character
        // Serial.println(c);         // print the character
        if(c == 'A'){ 
          tone(PIN_BUZZER,BUZZER_LOW_VOLUME,BUZZER_TIME_MILLIS);
          LastTimeSounded = millis();
        }
        if(c == 'B'){
          tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,BUZZER_TIME_MILLIS);
          LastTimeSounded = millis();
        }
        if(c == 'C'){
          tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,BUZZER_TIME_MILLIS);
          LastTimeSounded = millis();
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
