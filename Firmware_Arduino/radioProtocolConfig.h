#ifndef __RADIO_PROTOCOL_CONFIG_H_INCLUDED__
#define __RADIO_PROTOCOL_CONFIG_H_INCLUDED__

#include "CriticalData.h"

#define RADIOPROTOCOL_REMOTE_PACKET_LENGTH  (CRITICAL_DATA_REMOTE_PACKET_LENGTH)
#define RADIOPROTOCOL_SYMBOL_US                    (CRITICAL_DATA_REMOTE_SYMBOL)

#define RADIOPROTOCOL_PORT_RF_TX                                         (PORTB)
#define RADIOPROTOCOL_PIN_RF_TX                                              (1)
#define RADIOPROTOCOL_PINNUMBER_RF_TX                                        (9)

#define RADIOPROTOCOL_DELAYUS(US) \
    delayMicroseconds(US)

#define RADIOPROTOCOL_BITWRITE(PORT, PIN, VALUE) \
    bitWrite(PORT, PIN, VALUE)

#endif // __RADIO_PROTOCOL_CONFIG_H_INCLUDED__
