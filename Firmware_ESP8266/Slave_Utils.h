#pragma once

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "error.h"
#include "basic_defines.h"

#define SLAVE_I2C_ADDRESS                                                   (0x08)

static inline uint16_t rscpGetCrcCallback(uint8_t *data, uint8_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++){
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++){
            if (crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc;
}

void sendCommandToSlave(char type) {
  // Wire.beginTransmission(SLAVE_I2C_ADDRESS); // transmit to device #8
  // Wire.write(1);        // This should wake up the device
  // Wire.write(type);        // sends five bytes
  // Wire.endTransmission();    // stop transmitting
}

void sendRollerCommand(int persiana, int comando) {
  char type = 1 << (persiana + 2) | (0x3  & comando);
  sendCommandToSlave(type);
}

void checkSlaveConnection(LiquidCrystal_PCF8574 * mylcd) {
  uint32_t txBufferIndex = 0;
  uint8_t txBuffer[64];
  
  // Fill txBuffer
  txBuffer[txBufferIndex++] = 0xAA;
  txBuffer[txBufferIndex++] = 0x03;
  txBuffer[txBufferIndex++] = 0x0A;
  txBuffer[txBufferIndex++] = 0x01; // No data

  uint16_t crc = rscpGetCrcCallback(&txBuffer[1], txBufferIndex - 1);
  txBuffer[txBufferIndex++] = (crc >> 8) & 0xFF;
  txBuffer[txBufferIndex++] = (crc & 0xFF);

  Serial.println("Sending...");
  Serial.print("txBuffer : ");
  for (int i = 0; i < txBufferIndex; i++) {
      Serial.print(txBuffer[i], HEX);
      Serial.print(" ");
  }
  Serial.println();

  Wire.beginTransmission(SLAVE_I2C_ADDRESS); // transmit to device #8
  Wire.write(txBuffer, txBufferIndex);
  Wire.endTransmission();    // stop transmitting

  uint8_t rxBuffer[64];
  uint32_t rxBufferIndex = 0;
  uint8_t rxExpectedLength = 13;

  Wire.requestFrom(SLAVE_I2C_ADDRESS, (int)rxExpectedLength);

  unsigned long timeOut = millis();
  while (Wire.available() > 0) {
      rxBuffer[rxBufferIndex++] = Wire.read();
      yield();
  }
  Serial.println("rxBufferIndex: " + String(rxBufferIndex));
  Serial.println("rxExpectedLength: " + String(rxExpectedLength));
  Serial.print("rxBuffer : ");
  for (int i = 0; i < rxBufferIndex; i++) {
      Serial.print(rxBuffer[i], HEX);
      Serial.print(" ");
  }
  Serial.println();
  errorHandler(mylcd, FATAL_ERROR_CODE_SLAVE_HARDWARE);
  return ;
/*
  // Wire.requestFrom(SLAVE_I2C_ADDRESS, (int)rxExpectedLength);
  unsigned long timeOut = millis();
  while (rxBufferIndex < rxExpectedLength) {
      if (Wire.available() > 0) {
          rxBuffer[rxBufferIndex++] = Wire.read();
      }
      if ( (millis() - timeOut) > 5000) {
          errorHandler(mylcd, FATAL_ERROR_CODE_SLAVE_HARDWARE);
      }
      yield();
  }
  Serial.println("rxBufferIndex: " + String(rxBufferIndex));
  Serial.println("rxExpectedLength: " + String(rxExpectedLength));
  Serial.print("rxBuffer : ");
  for (int i = 0; i < rxBufferIndex; i++) {
      Serial.print(rxBuffer[i], HEX);
      Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < 3; i++) {
    sendCommandToSlave(COMMAND_BUZZER_HIGH_VOLUME);
    delay(200);
    yield();
  }
  */
}