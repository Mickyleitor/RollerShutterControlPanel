#pragma once

#include <stdint.h>

extern uint8_t _rxBuffer[64];
extern uint8_t _rxBufferLength;

typedef struct rscpTaskDef {
   uint8_t command;
   uint8_t arg[64];
   uint8_t argLength;
} rscpTaskType;

extern rscpTaskType _rscpTasks[4];
extern uint8_t _rscpTaskCount;

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

int8_t pushTaskToBuffer(uint8_t command, uint8_t * arg, uint8_t argLength)
{
    if(_rscpTaskCount >= sizeof(_rscpTasks)){
        return -1;
    }

    _rscpTasks[_rscpTaskCount].command = command;
    _rscpTasks[_rscpTaskCount].argLength = argLength;
    for(uint8_t i = 0; i < argLength; i++){
        _rscpTasks[_rscpTaskCount].arg[i] = arg[i];
    }
    _rscpTaskCount++;
    return 0;
}

int8_t popTaskFromBuffer(rscpTaskType * task)
{
    if(_rscpTaskCount == 0){
        return -1;
    }

    task->command = _rscpTasks[0].command;
    task->argLength = _rscpTasks[0].argLength;
    for(uint8_t i = 0; i < task->argLength; i++){
        task->arg[i] = _rscpTasks[0].arg[i];
    }
    for(uint8_t i = 0; i < _rscpTaskCount; i++){
        _rscpTasks[i].command = _rscpTasks[i + 1].command;
        _rscpTasks[i].argLength = _rscpTasks[i + 1].argLength;
        for(uint8_t j = 0; j < _rscpTasks[i + 1].argLength; j++){
            _rscpTasks[i].arg[j] = _rscpTasks[i + 1].arg[j];
        }
    }
    _rscpTaskCount--;
    return 0;
}

int8_t peekTaskFromBuffer(rscpTaskType * task)
{
    if(_rscpTaskCount == 0){
        return -1;
    }

    task->command = _rscpTasks[0].command;
    task->argLength = _rscpTasks[0].argLength;
    for(uint8_t i = 0; i < task->argLength; i++){
        task->arg[i] = _rscpTasks[0].arg[i];
    }
    return 0;
}