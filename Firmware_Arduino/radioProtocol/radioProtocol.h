#ifndef _RADIO_PROTOCOL_H_
#define _RADIO_PROTOCOL_H_

/*! \file **********************************************************************
 *
 *  \brief  Roller Shutter Low Level Radio Protocol
 *
 *  \author MickySim: https://www.mickysim.com
 *
 *  Copyright (c) 2023 MickySim All rights reserved.
 ******************************************************************************/

//---[ Includes ]-------------------------------------------------------------

#include <stdint.h>

#include "../moduleConfigs/radioProtocolConfig.h"

//---[ Macros and Constants ]-------------------------------------------------

#if !defined(RADIOPROTOCOL_SYMBOL_US)
#error RADIOPROTOCOL_SYMBOL_US must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_DELAYUS)
#error RADIOPROTOCOL_DELAYUS must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_PORT_RF_TX)
#error RADIOPROTOCOL_PORT_RF_TX must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_PIN_RF_TX)
#error RADIOPROTOCOL_PIN_RF_TX must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_BITWRITE)
#error RADIOPROTOCOL_BITWRITE must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_BITREAD)
#error RADIOPROTOCOL_BITREAD must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_PREAMBLE_LENGTH)
#error RADIOPROTOCOL_PREAMBLE_LENGTH must be set in radioProtocolConfig.h
#endif

#if !defined(RADIOPROTOCOL_PREAMBLE_PATTERN)
#error RADIOPROTOCOL_PREAMBLE_PATTERN must be set in radioProtocolConfig.h
#endif

//---[ Function Prototypes ]--------------------------------------------------

/**
 * \brief Sends a frame using the radio protocol.
 *
 * This function sends the data in the provided frame using the radio protocol.
 * The frame is transmitted bit by bit, with the specified inter-bit delay.
 *
 * \param frame Pointer to the data frame to be transmitted.
 * \param length_bits Length of the data frame in bits.
 */
static inline void radioProtocol_send_frame(uint8_t *frame, uint32_t length_bits) {

  uint32_t num_bytes = (length_bits + 7) / 8;
  uint8_t *ptr = frame;
  uint8_t data_bit = 0;

  while (ptr < frame + num_bytes) {
    for (int8_t index_bit = 7; index_bit >= 0; index_bit--) {
      if ((ptr - frame) * 8 + index_bit < (int32_t)length_bits) {
        data_bit = (*ptr >> index_bit) & 1;
        RADIOPROTOCOL_BITWRITE(RADIOPROTOCOL_PORT_RF_TX, RADIOPROTOCOL_PIN_RF_TX, data_bit);
        RADIOPROTOCOL_DELAYUS(RADIOPROTOCOL_SYMBOL_US);
      }
    }
    ptr++;
  }
}

/**
 * @brief Receives a frame using the radio protocol.
 * 
 * This function receives a frame using the radio protocol.
 * The frame is received bit by bit, with the specified inter-bit delay.
 * 
 * @param frame Pointer to the data frame to be received.
 * @param length_bits Length of the data frame in bits.
 * @param timeout_us Timeout in microseconds.
 * @return uint32_t Number of bits received.
 */
static inline uint32_t radioProtocol_receive_frame(uint8_t *frame, uint32_t length_bits, uint32_t timeout_us) {

    uint64_t data = 0;
    uint64_t data_mask = (1ULL << RADIOPROTOCOL_PREAMBLE_LENGTH) - 1; // Use bit-shift to calculate the data_mask
    uint8_t *ptr = frame;
    uint32_t received_bits = 0;

    // Search for preamble pattern with a timeout
    while (timeout_us > 0 && ((data & data_mask) != (RADIOPROTOCOL_PREAMBLE_PATTERN & data_mask))) {
        data = (data << 1) | (RADIOPROTOCOL_BITREAD(RADIOPROTOCOL_PORT_RF_RX, RADIOPROTOCOL_PIN_RF_RX) & 1);
        RADIOPROTOCOL_DELAYUS(RADIOPROTOCOL_SYMBOL_US);
        timeout_us -= RADIOPROTOCOL_SYMBOL_US;
    }

    if (timeout_us > 0) {
        // Preamble found, receive the data
        do {
            uint8_t data_byte = 0;
            for (int8_t index_bit = 7; index_bit >= 0; index_bit--) {
                data_byte |= (RADIOPROTOCOL_BITREAD(RADIOPROTOCOL_PORT_RF_RX, RADIOPROTOCOL_PIN_RF_RX) & 1) << index_bit;
                RADIOPROTOCOL_DELAYUS(RADIOPROTOCOL_SYMBOL_US);
            }
            received_bits += 8;
            *ptr = data_byte;
            ptr++;
        } while (received_bits < length_bits);
    }

    return received_bits;
}

#endif // _RADIO_PROTOCOL_H_
