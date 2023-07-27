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

#include "radioProtocolConfig.h"

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
static inline void radioProtocol_send_frame(uint8_t *frame, int32_t length_bits) {

  uint32_t num_bytes = (length_bits + 7) / 8;
  uint8_t *ptr = frame;
  uint8_t data_bit = 0;

  while (ptr < frame + num_bytes) {
    for (int8_t index_bit = 7; index_bit >= 0; index_bit--) {
      if ((ptr - frame) * 8 + index_bit < length_bits) {
        data_bit = (*ptr >> index_bit) & 1;
        RADIOPROTOCOL_BITWRITE(RADIOPROTOCOL_PORT_RF_TX, RADIOPROTOCOL_PIN_RF_TX, data_bit);
        RADIOPROTOCOL_DELAYUS(RADIOPROTOCOL_SYMBOL_US);
      }
    }
    ptr++;
  }
}

#endif // _RADIO_PROTOCOL_H_
