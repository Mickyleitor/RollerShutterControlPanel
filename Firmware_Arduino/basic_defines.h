#pragma once

/**
 * @file basic_defines.h
 * @brief Contains various macros and definitions used in the project.
 */

// General macros

/** @brief Timeout value for entering idle state in milliseconds. */
#define IDLE_TIMEOUT_MS                                                  (10000)

/** @brief Timeout value for I2C communication in milliseconds. */
#define I2C_TIMEOUT_MS                                                    (1000)

// Macros for PIN and PORTs

/** @brief Pin number for the user button. */
#define PIN_BUTTON_USER                                                      (2)

/** @brief Debounce time for button presses in milliseconds. */
#define DEBOUNCE_TIME_MILLIS                                               (100)

// Buzzer

/** @brief Pin number for the buzzer. */
#define PIN_BUZZER                                                           (5)

// Relay

/** @brief Pin number for the relay. */
#define PIN_RELAY                                                            (7)

/** @brief Pin number for I2C SCL. */
#define PIN_I2C_SCL                                                         (A5)

/** @brief Pin number for I2C SDA. */
#define PIN_I2C_SDA                                                         (A4)

// Macros for buzzer volume selections

/** @brief Low volume setting for the buzzer. */
#define BUZZER_LOW_VOLUME                                                (17000)

/** @brief Medium volume setting for the buzzer. */
#define BUZZER_MEDIUM_VOLUME                                               (300)

/** @brief High volume setting for the buzzer. */
#define BUZZER_HIGH_VOLUME                                                (5000)

// Macros for LCD handing

/** @brief I2C slave address for LCD communication. */
#define I2C_SLAVE_ADDRESS                                                    (8)

/**
 * @brief Structure to define an RSCP task.
 */
typedef struct rscpTaskDef {
    uint8_t command;   /**< RSCP command. */
    uint8_t arg[64];   /**< Argument data. */
    uint8_t argLength; /**< Length of the argument data. */
} rscpTaskType;

/** @brief Array to store RSCP tasks. */
rscpTaskType _rscpTasks[4];

/** @brief Number of RSCP tasks in the array. */
uint8_t _rscpTaskCount = 0;
