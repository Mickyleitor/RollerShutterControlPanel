# Roller Shutter Control Panel
- This project is an embedded system control panel to manage custom shutters, lights and some other minor functions.

The Roller Shutter Control Panel is released as “open hardware” involving 3d printed parts, electronic devices and a PCB board with an ATMega328P microcontroller and ESP8266 Wifi Module.

<p align="center">
  <img src="https://raw.githubusercontent.com/Mickyleitor/DomoticShutter/master/Docs/Figures/RollerShutterControlPanel.jpg" width=360/>
</p>

# Motivation
This personal project was initially developed due the need to close my own motor-powered shutters at a certain time of the day, specially at dawn, as the sun points directly to my head (and even more annoying to my eyes) when I'm still sleeping. I thought I was able to create a solution to this so I wanted to expand the functionality and add some domotic features.

After some time this became a Bachelor's Degree Final Project as part as my Degree in Electronic Systems Engineering.

But the most important aspect of this, it's the experience acquired in the learning process by doing this.

Being an open project where anyone can be part of the design team; anyone can bring knowledge, comments and ideas for developing other improved Arduino compatible board or mechanical changes, improvements and any kind of contribution. The cost has been optimised as much as possible to accomodate any budget, reuse components that I already have and pursue the real open hardware’s spirit.


# Folder organization
The folder structure has been divided as follows.
<pre><code>
- Docs                     <-- Documents file folder
-|- Figures                <-- Diagrams, schematics, screenshots
-|- Files                  <-- Raw documents like .docx, .pdf,...
-|- Photos                 <-- Raw and/or early prototyping photos
- Firmware_Arduino         <-- Source code for Arduino MCU firmware
- Firmware_ESP8266         <-- Source code for ESP8266 firmware
- Firmware_Processing_HCI  <-- Source code for Processing IDE for UCD
-|- data
- PCB_Design               <-- PCB Board design source files (KiCAD)
-|- Libraries              <-- Custom symbol libraries
-|- Modules                <-- Custom footprint and 3D Models libraries 
-|-|- packages3d
-|-|- Project_libraries.pretty
- 3D_Design                <-- 3D model source file for custom panel case
</code></pre> 

# Tools and software used
The tools and software used in this project are completely free (or trial period), to avoid the typical problems of licenses that usually give the proprietary software.
* KiCAD
* Arduino IDE
* Google Sketchup (free version)
* draw.io
* GIMP
* Cameo Systems Modeler (Evaluation version)

# License
This project is released under the CERN Open Hardware Licence.