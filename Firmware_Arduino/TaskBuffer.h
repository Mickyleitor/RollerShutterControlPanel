/**
 * @file TaskBuffer.h
 * @brief Functions for managing a task buffer.
 */

#pragma once

#include <stdint.h>

#include "basic_defines.h"


extern rscpTaskType _rscpTasks[];
extern uint8_t _rscpTaskCount;

/**
 * @brief Pushes a task onto the task buffer.
 *
 * @param command The command for the task.
 * @param arg Pointer to the argument data.
 * @param argLength Length of the argument data.
 *
 * @return 0 on success, -1 if the task buffer is full.
 */
int8_t pushTaskToBuffer(uint8_t command, uint8_t* arg, uint8_t argLength) {
    if (_rscpTaskCount >= sizeof(_rscpTasks)) {
        return -1;
    }

    _rscpTasks[_rscpTaskCount].command   = command;
    _rscpTasks[_rscpTaskCount].argLength = argLength;
    for (uint8_t i = 0; i < argLength; i++) {
        _rscpTasks[_rscpTaskCount].arg[i] = (uint8_t)arg[i];
    }
    _rscpTaskCount++;
    return 0;
}

/**
 * @brief Pops a task from the task buffer.
 *
 * @param task Pointer to store the popped task.
 *
 * @return 0 on success, -1 if the task buffer is empty.
 */
int8_t popTaskFromBuffer(rscpTaskType* task) {
    if (_rscpTaskCount == 0) {
        return -1;
    }

    task->command   = _rscpTasks[0].command;
    task->argLength = _rscpTasks[0].argLength;
    for (uint8_t i = 0; i < task->argLength; i++) {
        task->arg[i] = _rscpTasks[0].arg[i];
    }
    for (uint8_t i = 0; i < _rscpTaskCount; i++) {
        _rscpTasks[i].command   = _rscpTasks[i + 1].command;
        _rscpTasks[i].argLength = _rscpTasks[i + 1].argLength;
        for (uint8_t j = 0; j < _rscpTasks[i + 1].argLength; j++) {
            _rscpTasks[i].arg[j] = _rscpTasks[i + 1].arg[j];
        }
    }
    _rscpTaskCount--;
    return 0;
}

/**
 * @brief Peeks at the first task in the task buffer without removing it.
 *
 * @param task Pointer to store the peeked task.
 *
 * @return 0 on success, -1 if the task buffer is empty.
 */
int8_t peekTaskFromBuffer(rscpTaskType* task) {
    if (_rscpTaskCount == 0) {
        return -1;
    }

    task->command   = _rscpTasks[0].command;
    task->argLength = _rscpTasks[0].argLength;
    for (uint8_t i = 0; i < task->argLength; i++) {
        task->arg[i] = _rscpTasks[0].arg[i];
    }
    return 0;
}
