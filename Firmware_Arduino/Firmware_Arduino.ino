/**
 * @file Domotic Shutter sketch by Mickyleitor
 * @brief This sketch is in development and uses ATmega368P.
 *
 * @details
 * Libraries used and need to be installed from Arduino IDE:
 * - Low-Power by rocketscream/ version 1.6.0: https://github.com/rocketscream/Low-Power
 *
 * Current protocol for I2C messages with master is addressed by RSCP (Roller Shutter Control Panel)
 * protocol.
 */

#include <Wire.h>

#include "LowPower.h"
#include "TaskBuffer.h"
#include "basic_defines.h"
#include "moduleConfigs/radioProtocolConfig.h"
#include "remote.h"
#include "rscpProtocol/rscpProtocol.h"


/**
 * @enum SystemState
 * @brief Enumeration of system states.
 */
enum SystemState {
    LOW_POWER,   ///< Low power state
    IDLE,        ///< Idle state
    RUNNING,     ///< Running state
    SWITCH_RELAY ///< Switch relay state
};

SystemState currentSystemState = RUNNING;

bool isBuzzerRunning = false;

/**
 * @brief Setup function for Arduino.
 */
void setup() {
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(RADIOPROTOCOL_PINNUMBER_RF_TX, INPUT);
    pinMode(RADIOPROTOCOL_PINNUMBER_RF_RX, INPUT);
    pinMode(PIN_RELAY, OUTPUT);
    initButtonsFunction();

    Serial.begin(115200);
    Serial.println("Slave initialized");

    Wire.begin(I2C_SLAVE_ADDRESS);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    currentSystemState = RUNNING;
}

/**
 * @brief Main loop for Arduino.
 */
void loop() {
    static uint32_t timeout_ms;
    rscpTaskType task;

    switch (currentSystemState) {
        case LOW_POWER: {
            Serial.println("Entering low-power mode...");
            Serial.flush();
            LowPower.idle(
                    SLEEP_FOREVER,
                    ADC_OFF,
                    TIMER2_OFF,
                    TIMER1_ON,
                    TIMER0_OFF,
                    SPI_OFF,
                    USART0_OFF,
                    TWI_ON);
            currentSystemState = RUNNING;
            break;
        }
        case SWITCH_RELAY: {
            digitalWrite(PIN_RELAY, digitalRead(PIN_BUTTON_USER));
            Serial.println("Button pressed");
            currentSystemState = RUNNING;
            break;
        }
        case RUNNING: {
            if (peekTaskFromBuffer(&task) < 0) {
                timeout_ms         = millis();
                currentSystemState = IDLE;
                break;
            }

            bool removeTask = true;

            switch (task.command) {
                case RSCP_CMD_SET_SHUTTER_ACTION: {
                    struct RSCP_Arg_rollershutter* arg = (struct RSCP_Arg_rollershutter*)task.arg;
                    if (arg->shutter <= 2 && arg->action <= 3) {
                        for (int retries = 0; retries < arg->retries; retries++) {
                            sendCommand(arg->shutter, arg->action - 1);
                        }
                    }
                    break;
                }
                case RSCP_CMD_SET_SWITCH_RELAY: {
                    struct RSCP_Arg_switchrelay* arg = (struct RSCP_Arg_switchrelay*)task.arg;
                    digitalWrite(PIN_RELAY, (arg->status == RSCP_DEF_SWITCH_RELAY_ON) ? HIGH : LOW);
                    break;
                }
                case RSCP_CMD_SET_BUZZER_ACTION: {
                    struct RSCP_Arg_buzzer_action* arg = (struct RSCP_Arg_buzzer_action*)task.arg;
                    switch (arg->action) {
                        case RSCP_DEF_BUZZER_ACTION_ON: {
                            removeTask = false;
                            if (isBuzzerRunning) {
                                if ((millis() - timeout_ms) > arg->duration_ms) {
                                    noTone(PIN_BUZZER);
                                    isBuzzerRunning = false;
                                    removeTask      = true;
                                    break;
                                }
                                break;
                            }
                            tone(PIN_BUZZER, arg->volume, arg->duration_ms);
                            timeout_ms      = millis();
                            isBuzzerRunning = true;
                            break;
                        }
                        case RSCP_DEF_BUZZER_ACTION_OFF: {
                            noTone(PIN_BUZZER);
                            isBuzzerRunning = false;
                            break;
                        }
                    }
                    break;
                }
            }

            if (removeTask) {
                popTaskFromBuffer(&task);
            }

            break;
        }
        case IDLE: {
            if ((millis() - timeout_ms) > IDLE_TIMEOUT_MS) {
                currentSystemState = LOW_POWER;
            }
            break;
        }
    }
}

/**
 * @brief Function that executes whenever data is received from the master.
 *
 * @param howMany Number of bytes received.
 */
void receiveEvent(int howMany) {
    // Read all bytes from I2C later in requestEvent
}

/**
 * @brief Function that executes whenever data is requested by the master.
 */
void requestEvent() {
    int8_t err;
    if ((err = rscpHandle(I2C_TIMEOUT_MS)) < 0) {
        Serial.println("[ERROR] rscpHandle: " + String(err));
    }
    currentSystemState = RUNNING;
}

/**
 * @brief Interrupt service routine for the button.
 */
void isrButton() {
    // Detach the interrupt to avoid retriggering during debounce
    detachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER));

    // Reset Timer 1 counter to 0 and start counting (CTC mode, scale to clock / 256)
    TCNT1  = 0;
    TCCR1B = bit(WGM12) | bit(CS12);
}

/**
 * @brief Interrupt service routine for Timer 1 Compare A Match.
 */
ISR(TIMER1_COMPA_vect) {
    currentSystemState = SWITCH_RELAY;

    // Stop Timer 1 (clear prescaler) and reattach the button interrupt on changing edge
    TCCR1B = 0;
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER), isrButton, CHANGE);
}

/**
 * @brief Initialize button functions.
 */
void initButtonsFunction() {
    pinMode(PIN_BUTTON_USER, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_USER), isrButton, CHANGE);

    // Set up Timer 1 for debounce
    TCCR1A = 0; // Normal operation
    TCCR1B = 0; // Stop the timer initially

    // Calculate the compare A register value for debounce time
    uint32_t debounce_cycles = (F_CPU / (256UL * 2 * 1000));
    OCR1A                    = (debounce_cycles * DEBOUNCE_TIME_MILLIS) - 1;

    // Enable the interrupt on Compare A Match (Timer 1)
    TIMSK1 = bit(OCIE1A);
}
