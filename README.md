# Arduino-Powered Shutter Controller
Custom embedded system to remotely control your shutter, add programmable tasks/alarms and with a LED display device.

<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterPanel.jpg"/>
</p>

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
<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/FSM/Initialization_FSM.jpg"/>
</p>

## Board Schematics
<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/KiCad/DomoticShutterSchematic.jpg"/>
</p>
## PCB Board
<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardFront.jpg"/>
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardBack.jpg"/>
</p>

## EEPROM Data registers

| Memory Address |         Function        |  Payload Length  |  CRC length  |
|:--------------:|:-----------------------:|:----------------:|:------------:|
|     0x0000     |    Left Up Key command  |     92 bytes     |    1 byte    |
|     0x005C     |   Left Stop Key command |     92 bytes     |    1 byte    |
|     0x00B9     |  Left Down Key command  |     92 bytes     |    1 byte    |
|     0x0115     |    Center Up Key code   |     92 bytes     |    1 byte    |
|     0x0171     | Center Stop Key command |     92 bytes     |    1 byte    |
|     0x01CD     | Center Down Key command |     92 bytes     |    1 byte    |
|     0x0229     |   Right Up Key command  |     92 bytes     |    1 byte    |
|     0x0285     |  Right Stop Key command |     92 bytes     |    1 byte    |
|     0x02E1     |  Right Down Key command |     92 bytes     |    1 byte    |