#ifndef __RADIO_PROTOCOL_CONFIG_H_INCLUDED__
#define __RADIO_PROTOCOL_CONFIG_H_INCLUDED__

#include "CriticalData.h"

#define RADIOPROTOCOL_PREAMBLE_PATTERN                 (REMOTE_PREAMBLE_PATTERN)
#define RADIOPROTOCOL_PREAMBLE_LENGTH                   (REMOTE_PREAMBLE_LENGTH)
#define RADIOPROTOCOL_REMOTE_PACKET_LENGTH                (REMOTE_PACKET_LENGTH)
#define RADIOPROTOCOL_SYMBOL_US                               (REMOTE_SYMBOL_US)
#define RADIOPROTOCOL_INTERFRAME_PATTERN             (REMOTE_INTERFRAME_PATTERN)

#define RADIOPROTOCOL_PORT_RF_TX                                         (PORTB)
#define RADIOPROTOCOL_PIN_RF_TX                                              (1)
#define RADIOPROTOCOL_PINNUMBER_RF_TX                                        (9)

#define RADIOPROTOCOL_PORT_RF_RX                                          (PIND)
#define RADIOPROTOCOL_PIN_RF_RX                                              (3)
#define RADIOPROTOCOL_PINNUMBER_RF_RX                                        (3)

#define RADIOPROTOCOL_DELAYUS(US) \
    delayMicroseconds(US)

#define RADIOPROTOCOL_BITWRITE(PORT, PIN, VALUE) \
    bitWrite(PORT, PIN, VALUE)

#define RADIOPROTOCOL_BITREAD(PORT, PIN) \
    bitRead(PORT, PIN)

#endif // __RADIO_PROTOCOL_CONFIG_H_INCLUDED__
