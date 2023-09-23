# Roller Shutter Control Panel Slave CPU

The Roller Shutter Control Panel Slave CPU is a vital component of the larger Domotic Shutter Control System. It operates as an ATmega368P microcontroller, serving as a subordinate CPU within the system.

## Overview

This CPU is dedicated to interfacing with the primary control unit (main CPU) using the I2C communication protocol as the physical layer. However, the communication between the main CPU and the slave CPU relies on the RSCP (Roller Shutter Control Panel) stack protocol. Its primary function is to receive commands and data from the main CPU via the RSCP protocol and execute various control tasks related to roller shutter operation, relay switching, and buzzer control.

## Key Features

- **RSCP Protocol**: The slave CPU communicates with the main CPU using the RSCP (Roller Shutter Control Panel) stack protocol that is implemented on top of the I2C physical layer.

- **Roller Shutter Control**: It possesses the capability to command the movement of roller shutters, including actions such as opening, closing, and stopping.

- **Relay Switching**: The CPU has the capability to control relays, enabling the activation or deactivation of connected devices or circuits.

- **Buzzer Control**: The inclusion of buzzer control functionality allows for audible alerts or notifications.