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
#include "Utils.h"

#include "moduleConfigs/radioProtocolConfig.h"

#include "rscpProtocol/rscpProtocol.h"

int buzzer_running = 1;
uint8_t _rxBuffer[64];
uint8_t _rxBufferLength;

enum States {
  SYSTEMSTATE_LOWPOWER,
  SYSTEMSTATE_IDLE,
  SYSTEMSTATE_RUNNING,
  SYSTEMSTATE_SWITCH_RELAY,
  SYSTEMSTATE_PROCCESS_I2C,
} SystemState;

rscpTaskType _rscpTasks[4];
uint8_t _rscpTaskCount = 0;

void setup(){
  pinMode(PIN_BUZZER,OUTPUT);
  pinMode(RADIOPROTOCOL_PINNUMBER_RF_TX,INPUT); // Free 433 Mhz channel now.
  pinMode(RADIOPROTOCOL_PINNUMBER_RF_RX,INPUT);
  pinMode(PIN_RELAY,OUTPUT);
  initButtonsFunction();
  
  Serial.begin(115200);
  Serial.println("Esclavo iniciado");
  Wire.begin(I2C_SLAVE_ADDRESS);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  // tone(PIN_BUZZER,BUZZER_HIGH_VOLUME,BUZZER_TIME_MILLIS);
  // delay(200);
  // tone(PIN_BUZZER,BUZZER_MEDIUM_VOLUME,BUZZER_TIME_MILLIS);
  // delay(200);
  // tone(PIN_BUZZER,BUZZER_LOW_VOLUME,BUZZER_TIME_MILLIS);
  SystemState = SYSTEMSTATE_RUNNING;
}

void loop(){
  static uint32_t timeout_ms;
  static rscpTaskType task;
  switch( SystemState ) {
    case SYSTEMSTATE_LOWPOWER : {
      // Pendiente hacer modo bajo consumo
      Serial.println("Entering lower power mode...");
      Serial.flush();
      LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_ON, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_ON);
      SystemState = SYSTEMSTATE_RUNNING;
      break;
    }
    case SYSTEMSTATE_SWITCH_RELAY : {
      digitalWrite(PIN_RELAY,digitalRead(PIN_BUTTON_USER));
      Serial.println("Boton pulsado");
      SystemState = SYSTEMSTATE_RUNNING;
      break;
    }
    case SYSTEMSTATE_PROCCESS_I2C : {
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
      SystemState = SYSTEMSTATE_RUNNING;
      break;
    }
    case SYSTEMSTATE_RUNNING : {
      int8_t ret = rscpHandle(1000);
      Serial.print("rscpHandle = ");
      Serial.println(ret);
      // Check if there is any data to be processed
      if( peekTaskFromBuffer(&task) < 0 ){
        // If there is no data to be processed, we go to IDLE state
        timeout_ms = millis();
        SystemState = SYSTEMSTATE_IDLE;
        break;
      }

      bool removeTask = true;

      // If there is data to be processed, we process it
      switch(task.command) {
        case RSCP_CMD_SET_SHUTTER_ACTION : {
          struct RSCP_Arg_rollershutter * arg = (struct RSCP_Arg_rollershutter *)task.arg;
          if ( arg->shutter > 2 || arg->action > 3 ) {
            break;
          }
          for(int retries = 0; retries < arg->retries; retries++){
            sendCommand(arg->shutter,arg->action - 1);
          }
          break;
        }
        case RSCP_CMD_SET_SWITCH_RELAY : {
          struct RSCP_Arg_switchrelay * arg = (struct RSCP_Arg_switchrelay *)task.arg;
          digitalWrite(PIN_RELAY, (arg->status == RSCP_DEF_SWITCH_RELAY_ON) ? HIGH : LOW);
          break;
        }
        case RSCP_CMD_SET_BUZZER_ACTION : {
          struct RSCP_Arg_buzzer_action * arg = (struct RSCP_Arg_buzzer_action *)task.arg;
          static bool buzzer_is_running;
          switch (arg->action){
            case RSCP_DEF_BUZZER_ACTION_ON: {
              removeTask = false;
              if ( buzzer_is_running ) {
                if ( (timeout_ms - millis()) > arg->duration_ms ) {
                  noTone(PIN_BUZZER);
                  buzzer_is_running = false;
                  removeTask = true;
                  break;
                }
                break;
              }
              buzzer_is_running = true;
              tone(PIN_BUZZER, arg->volume, arg->duration_ms);
              timeout_ms = millis();
              break;
            }
            case RSCP_DEF_BUZZER_ACTION_OFF: {
              noTone(PIN_BUZZER);
              buzzer_is_running = false;
              break;
            }
          }
          break;
        }
      }

      if( removeTask ){
        popTaskFromBuffer(&task);
      }

      break;
    }
    case SYSTEMSTATE_IDLE : {
      if ( (millis() - timeout_ms) > 10000 ) {
        SystemState = SYSTEMSTATE_LOWPOWER;
      }
      break;
    }
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  Serial.print("rxBuffer : ");
  while( Wire.available() ) {
    uint8_t data = Wire.read();    // receive byte as a character
    if( pushByteToRxBuffer(data) < 0 ){
      Serial.println("Buffer overflow");
      break;
    }
    Serial.print(data,HEX);
    Serial.print(" ");
  }
  Serial.println();
  SystemState = SYSTEMSTATE_RUNNING;
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  int8_t ret = rscpHandle(1000);
  Serial.print("rscpHandle = ");
  Serial.println(ret);
  // SystemState = SYSTEMSTATE_RUNNING;
}

void isrButton() {
  // Detach the interrupt to avoid retriggering during debounce
  detachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER));

  // Reset Timer 1 counter to 0 and start counting (CTC mode, scale to clock / 256)
  TCNT1 = 0;
  TCCR1B = bit(WGM12) | bit(CS12);
}

ISR(TIMER1_COMPA_vect) {
  SystemState = SYSTEMSTATE_SWITCH_RELAY;

  // Stop Timer 1 (clear prescaler) and reattach the button interrupt on falling edge
  TCCR1B = 0;
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER), isrButton, CHANGE);
}

void initButtonsFunction() {
  // Initialize button pin as INPUT_PULLUP
  pinMode(PIN_BUTTON_USER, INPUT_PULLUP);

  // Attach the falling-edge interrupt for the button
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER), isrButton, CHANGE);

  // Set up Timer 1 for debounce
  TCCR1A = 0;  // normal operation
  TCCR1B = 0;  // stop the timer initially

  // Calculate the compare A register value for debounce time
  // Using F_CPU / (256 * 2 * 1000) to avoid integer overflow
  uint32_t debounce_cycles = (F_CPU / (256UL * 2 * 1000));  // Ensure F_CPU is treated as 32-bit
  OCR1A = (debounce_cycles * DEBOUNCE_TIME_MILLIS) - 1;

  // Enable the interrupt on Compare A Match (Timer 1)
  TIMSK1 = bit(OCIE1A);
}
