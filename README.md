# Roller Shutter Control Panel
This embedded control panel manages custom shutters over radio—and a few auxiliary functions. It’s released as open hardware, including 3D-printed parts, electronic modules and a PCB with an ATMega328P microcontroller and ESP8266 Wi-Fi module.

<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/Figures/RollerShutterControlPanel.jpg" width="360"/>
</p>

## Motivation
I built this to automate my motorized shutters at dawn, when the rising sun would beam straight into my bedroom and wake me prematurely. Once I realized I could craft a solution, I expanded it with home-automation features. What began as a personal convenience grew into my Bachelor’s thesis in Electronic Systems Engineering—and, more importantly, a hands-on learning journey. Costs stayed low by reusing existing components and optimizing each part, in true open-hardware spirit.

## Roadmap
The full release lives under the [TFG tag](https://github.com/Mickyleitor/RollerShutterControlPanel/releases/tag/TFG). Published in November 2020 for my Bachelor’s thesis, that version delivered all core features required for the jury: radio-based shutter control; Wi-Fi connectivity; UTC time and weather retrieval; LCD display of time and temperature; and button-driven configuration.

Today the repo serves as a live test bed—but our immediate aim is to simplify and ship a rock-solid solution for real-world use as soon as possible.

## Folder organization
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

## Tools and software used
All tools are free or trial versions to avoid licensing hurdles:  
• KiCAD; Arduino IDE; Google SketchUp (free); GIMP; Cameo Systems Modeler (evaluation); draw.io (online); VSCode.

## License
Released under the CERN Open Hardware Licence.