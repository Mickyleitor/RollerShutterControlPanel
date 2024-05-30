// ***************************************************************************
// Remote RF module (potentially Somfy) for Arduino Mini Pro by Mickyleitor
//
// This header need a non-included CriticalData.h file which contains
// the frames and authentication data hidden due to security reasons.
//
// The CriticalData.h creates a byte multi-dimensional array of
// 3x3xREMOTE_PACKET_LENGTH stored in PROGMEM
// 3 arrays for each Roller Shutter. Each ones with REMOTE_PACKET_LENGTH length
//
// An example of an empty array is as follows
//
// const byte KeyCommands [3][3][SOMFY_PACKET_LENGTH] PROGMEM =
// {
//                         {
// /* COMANDO_STOP_LEFT    */  {},
// /* COMANDO_UP_LEFT      */  {},
// /* COMANDO_DOWN_LEFT    */  {}
//                         },
//                         {
// /* COMANDO_STOP_CENTER  */  {},
// /* COMANDO_UP_CENTER    */  {},
// /* COMANDO_DOWN_CENTER  */  {}
//                         },
//                         {
// /* COMANDO_STOP_RIGHT   */  {},
// /* COMANDO_UP_RIGHT     */  {},
// /* COMANDO_DOWN_RIGHT   */  {}
//                         }
// };
//
// ***************************************************************************

#include "moduleConfigs/radioProtocolConfig.h"
#include "radioProtocol/radioProtocol.h"


bool sendCommand(int persiana, int comando) {
    if (persiana > 2 || comando > 2) {
        return false;
    }
    pinMode(RADIOPROTOCOL_PINNUMBER_RF_TX, OUTPUT);
    uint8_t KeyPacket[RADIOPROTOCOL_REMOTE_PACKET_LENGTH];
    // First we need gather the data from the PROGMEM.
    for (int index = 0; index < RADIOPROTOCOL_REMOTE_PACKET_LENGTH; index++) {
        KeyPacket[index] = pgm_read_byte(&(KeyCommands[persiana][comando][index]));
    }
    radioProtocol_send_frame(KeyPacket, RADIOPROTOCOL_REMOTE_PACKET_LENGTH * 8);
    // Free the 433 Mhz channel making it a floating port
    pinMode(RADIOPROTOCOL_PINNUMBER_RF_TX, INPUT);
    // This delay shouldn't be removed to allow inter-frame separation.
    delay(10);
    return true;
}

bool sendCommand(int persiana, int comando, int intentos) {
    if (persiana > 2 || comando > 2) {
        return false;
    }
    for (int counter = 0; counter < intentos; counter++) {
        sendCommand(persiana, comando);
    }
    return true;
}
