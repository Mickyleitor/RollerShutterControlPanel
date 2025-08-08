# Roller Shutter Control Panel

Embedded wall panel for motorized shutters over radio; with LCD; buttons; Wi-Fi via ESP8266; clock;
and temperature. Open hardware with 3D-printed parts; off-the-shelf modules; and a PCB using an
ATMega328P plus ESP8266.

<p align="center">
  <img
    src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/Figures/RollerShutterControlPanel.jpg"
    width="360"
  />
</p>

## Motivation and origins

Built to stop sunrise from waking me early; first a simple wall controller; then my 2020 Bachelor’s
thesis; now an open build others can replicate; learn from; and adapt. Also a testbed for new ideas.

## Project status; what it does today

Actively used and stable. Local controller for **three** shutters; buttons to move; LCD for time and
date; current temperature from the internet; on-device Wi-Fi setup; ESP8266 firmware updates over
the air. No cloud required.

## Current functionality

- **Three shutters**; Left; Center; Right; actions: Up; Down; Stop at any time.
- **16×2 I²C LCD**; Shutter management, clock on idle and configuration menu.
- **Current temperature**; periodic OpenWeatherMap request; hides value when unavailable.
- **On-device Wi-Fi setup**; scan networks; choose SSID; enable or disable Wi-Fi and OTA;
- **Auto reconnect**; for both wifi attemps and weather requests.
- **Time and timezone editor**; set time and timezone.
- **OTA updates** for the ESP8266 using ArduinoOTA; handled in the background.
- **Buzzer feedback**; adjustable volume; enable or disable; accept and error tones.
- **Debug pages**; soft reset counter; current SSID with link status; device IP.

## Repository layout

```
Docs/                     Documents
├─ Figures/               Diagrams, schematics, screenshots
├─ Files/                 Raw docs (.docx, .pdf, etc.)
└─ Photos/                Prototyping photos

Firmware_Arduino/         Arduino MCU source
Firmware_ESP8266/         ESP8266 source
Firmware_Processing_HCI/  Processing-IDE files

PCB_Design/               KiCAD board files
├─ Libraries/             Custom symbols
└─ Modules/               Footprints & 3D models

3D_Design/                Panel case models
```


## Tools

Free or evaluation to avoid licensing issues:  
• KiCAD; Arduino IDE; Google SketchUp (free); GIMP; Cameo Systems Modeler (evaluation);  
  draw.io (online); VSCode.

## License

CERN Open Hardware Licence.
