// ***************************************************************************
// Somfy module for Arduino Mini Pro by Mickyleitor
// 
// This header need a non-included keydata.h file which contains
// the frames and authentication data hidden due to security reasons.
//
// ***************************************************************************

#include "keydata.h"

#define SOMFY_SYMBOL 490 // symbol width in microseconds

bool sendCommand(int persiana,int comando){
  if(persiana > 2 || comando > 2 ) return false;
  byte KeyPacket [SOMFY_PACKET_LENGTH];
  // First we need gather the data from the PROGMEM.
  for(int index = 0 ; index < SOMFY_PACKET_LENGTH ; index ++)
  {
    KeyPacket[index] = pgm_read_byte(&(KeyCommands[persiana][comando][index]));
  }
  // Iterate the array and bits using double loop.
  int bits = 0;
  for(int index = 0; index < SOMFY_PACKET_LENGTH ; index++){
    for(bits = 7; bits >= 0; bits--){
      bitWrite(PUERTO_TX,PIN_RF_TX,(KeyPacket[index] >> bits) & 1);
      delayMicroseconds(SOMFY_SYMBOL);
    }
  }
  // Free the 433 Mhz channel making it a floating port
  pinMode(PIN_RF_TX,INPUT);
  // This delay shouldn't be removed to allow inter-frame separation.
  delay(10);
  return true;
}

bool sendCommand(int persiana,int comando,int intentos){
  if(persiana > 2 || comando > 2 ) return false;
  for(int counter = 0 ; counter < intentos; counter ++){
    sendCommand(persiana,comando);
  }
  return true;
}
