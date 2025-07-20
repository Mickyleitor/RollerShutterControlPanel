# Roller Shutter Control Panel Main CPU

This serves as the primary CPU for the roller shutter control panel, based on the [ESP8266](https://en.wikipedia.org/wiki/ESP8266) microcontroller and programmed using the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

## Introduction

The main CPU handles several critical tasks, including displaying appropriate content on the LCD, processing user input from buttons, and communicating via I2C with the radio module to control the roller shutters. Additionally, it can connect to a Wi-Fi network, enabling data retrieval from the internet for functions like displaying weather forecasts and time. This data is crucial for specific user requirements, such as scheduling shutter control based on time and weather conditions.

## Roadmap

Make the solution simpler!! There are many features that are not strictly necessary for the roller shutter control panel. I want a working solution soon and will remove unnecessary features to achieve that. In order to do that, I will only need the following features:

- Control of the roller shutters via the radio module.
- Connect to a Wi-Fi network.
- Get current time and weather forecast from the internet.
- Display the current time and temperature on the LCD when idle.
- Allowing the user to change the time UTC and the Wi-Fi network settings via LCD buttons.

## Proposed Enhancements

Here are several potential enhancements to consider:

1. **Dynamic Wi-Fi Configuration**:
   - It would be beneficial to allow users to update their Wi-Fi network settings without reprogramming the microcontroller. This could involve creating a web server on the microcontroller with a user-friendly web interface for configuring Wi-Fi settings. The microcontroller would act as an access point if it's not connected to a Wi-Fi station , allowing users to connect and make necessary adjustments.

2. **Access Point SSID/Password Update**:
   - Providing the ability to update the SSID and password of the access point, particularly when the device cannot connect to the user's Wi-Fi network. Displaying this information on the LCD would be helpful.

3. **HTTPS Implementation**:
   - Implement HTTPS on the microcontroller's web server to encrypt Wi-Fi settings transmission, preventing plain text transmission. This can be achieved using a self-signed certificate, requiring user acceptance in their browser. 
   Additionally, consider to whether to permanently disable this option once the user uploads a new certificate for enhanced security. This measure safeguards against potential attackers connected to the user access point intercepting traffic.

4. **OTA Firmware Updates**:
   - Exploring the possibility of updating the microcontroller's firmware without requiring a physical connection to a computer. This could involve using a web server on the microcontroller and a web interface for uploading new firmware. It's worth considering whether to erase all stored data on the microcontroller after a firmware update to prevent the new firmware from accessing old data. This step would enhance security and thwart potential attackers from reading sensitive information like Wi-Fi settings.

5. **Radio Module Key Updates**:
   - While not the highest priority, it may be beneficial to provide a method for updating the keys of the radio module without requiring a computer connection. It's important to note that these keys are stored in the radio module, not the microcontroller.

6. **GPS Coordinates and UTC Time Updates**:
   - Consider adding functionality for updating GPS coordinates and UTC time to enhance internal calculations and improve system accuracy.

7. **Weather Forecast API Key Rotation**:
   - Explore options for updating the API key used for weather forecast services while maintaining the security of the stored key. Avoid displaying the stored key on the microcontroller.

8. **Additional Web Interface Features**:
   - While not a top priority, other data could potentially be visualized through the web interface, enhancing the user experience.

9. **User Menu**:
   - Implement a user menu to display essential Wi-Fi settings, such as the SSID of the station or access point (excluding the password), and the microcontroller's IP address or domain name.

These enhancements can help improve the functionality, security, and user-friendliness of the roller shutter control system. Prioritizing and addressing them based on their significance and potential security implications is essential for a robust and secure solution.