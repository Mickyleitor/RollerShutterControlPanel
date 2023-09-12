#pragma once

#include <stdint.h>

extern volatile uint8_t _rxBuffer[64];
extern volatile uint8_t _rxBufferLength;

int8_t pushByteToRxBuffer(uint8_t data)
{
    if(_rxBufferLength >= sizeof(_rxBuffer)){
        return -1;
    }

    _rxBuffer[_rxBufferLength++] = data;
    return 0;
}

int8_t popByteFromRxBuffer(uint8_t *data)
{
    if(_rxBufferLength == 0){
        return -1;
    }

    *data = _rxBuffer[0];
    for(uint8_t i = 0; i < _rxBufferLength; i++){
        _rxBuffer[i] = _rxBuffer[i + 1];
    }
    _rxBufferLength--;
    return 0;
}