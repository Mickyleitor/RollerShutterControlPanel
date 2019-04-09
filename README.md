# DomoticShutter
Arduino-Controlled Shutter with programmable tasks and LED display device.

![Panel](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterPanel.jpg)

## User Requirements
* User can control the shutters manually via buttons.
* Arduino can interface with shutter via wired or/and RF (the best solution).
* Arduino can program tasks like move shutters at a previously set time.
* Four buttons will be used in order to accomplish with HCI compliants (Left/Up/Down/Right).

## Technical limitations
* Space limitations compacted in a 13x6x4 cm box-sized
* Power consumption by arduino, Relays, LCD, buttons and required ICs
* Interrupts available on Arduino Minipro (More buttons than PINs)
* Logic voltage levels between LCD 5V and Arduino MiniPro 3.3V

## PCB Board

![Board Front](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardFront.jpg)

![Board Back](https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/3Dmodel/DomoticShutterBoardBack.jpg)