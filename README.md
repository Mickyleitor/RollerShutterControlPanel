# Arduino-Powered Shutter Controller
Custom embedded system to remotely control your shutter, add programmable tasks/alarms and with a LED display device.

![Panel](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterPanel.jpg)

## User Requirements
* User can control the shutters manually via buttons.
* Arduino can interface with shutter via Radio frecuency (HTR108 device), see hacking RF shutters article.
* The system should move the shutters automatically according to an alarms table.
* Four buttons will be used in order to accomplish with HCI compliants (Left/Up/Down/Right).

## Technical limitations
* Space limitations compacted in a 13x6x4 cm box-sized.
* Power consumption by arduino, LCD, buttons and required ICs.
* Logic voltage levels between LCD 5V and Arduino MiniPro 3.3V.

## Finite State Machine Diagrams
### Initialization
![Initialization](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/FSM/Initialization_FSM.jpg)

## Board Schematics
![Schematic](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/KiCad/DomoticShutterSchematic.jpg)

## PCB Board
![Board Front](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardFront.jpg)
![Board Back](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardBack.jpg)