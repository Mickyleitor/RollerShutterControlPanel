#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "error.h"
#include "basic_defines.h"

#define SLAVE_I2C_ADDRESS                                                   (0x08)

void sendCommandToSlave(char type) {
  Wire.beginTransmission(SLAVE_I2C_ADDRESS); // transmit to device #8
  Wire.write(1);        // This should wake up the device
  Wire.write(type);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
}

void sendRollerCommand(int persiana, int comando) {
  char type = 1 << (persiana + 2) | (0x3  & comando);
  sendCommandToSlave(type);
}

void checkSlaveConnection(LiquidCrystal_PCF8574 * mylcd) {
  bool slaveStatusRunning = false;
  Wire.requestFrom(SLAVE_I2C_ADDRESS, 1);
  unsigned long timeOut = millis();
  while (!slaveStatusRunning) {
    if (Wire.available() > 0) {
      char c = Wire.read();
      if (c != 0x3){
        Wire.requestFrom(SLAVE_I2C_ADDRESS, 1);
      } 
      else slaveStatusRunning = true;
    }
    if ( (millis() - timeOut) > 5000) {
        errorHandler(mylcd, FATAL_ERROR_CODE_SLAVE_HARDWARE);
    }
    yield();
  }
  for (int i = 0; i < 3; i++) {
    sendCommandToSlave(COMMAND_BUZZER_HIGH_VOLUME);
    delay(200);
    yield();
  };
}