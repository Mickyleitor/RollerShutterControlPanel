# Roller Shutter Control Panel — Main CPU (ESP8266)

This serves as the primary CPU for the roller shutter control panel; it uses the [ESP8266]
microcontroller and is programmed using the [Arduino IDE].

## Introduction

This firmware runs on an ESP8266 module and acts as the “brain” of a roller shutter control panel.
It drives a 16x2 I²C LCD; reads panel buttons; and commands three shutters through a radio module
via I²C. When Wi-Fi is enabled it connects as a station; fetches the current temperature and the
timezone shift; keeps the clock in sync; and shows status on the LCD. All common settings can be
changed from the panel; no reflash required.

## Proposed Enhancements

1. **AP fallback for first-time setup.** If repeated station connects fail; bring up a temporary AP
   and a tiny config page; go back to STA when saved.
2. **Location and API key in UI.** Add menu items to edit OpenWeatherMap `lat` and `lon` and rotate
   the API key; mask the key in the UI; store in EEPROM.
3. **HTTPS for the web UI** start with a self-signed cert; allow uploading a new certificate for
   encrypted credentials.
4. **Language option** Keep Spanish strings; add English selection in a simple Language menu.
5. **Update radio-module** Update radio-module firmware from ESP8266; and from the web UI or OTA.
6. **Diagnostics** Optional web pages to visualize additional data when it adds value.

[ESP8266]: https://en.wikipedia.org/wiki/ESP8266
[Arduino IDE]: https://www.arduino.cc/en/Main/Software
