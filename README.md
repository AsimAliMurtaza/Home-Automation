# Smart Home Automation System

## Overview

This project implements a Smart Home Automation System utilizing the ATmega328P microcontroller programmed in AVR Assembly and the ESP32 WROOM development board programmed in C++ using the Arduino IDE. The objective is to provide users with remote control over 3 lights and 2 fans, including the ability to adjust the speed of fan 2 via a PWM pin. The system also incorporates a Light Dependent Resistor (LDR) module to detect light intensity, and users can control and monitor the system through a mobile phone over WiFi using the ESP32. The communication between the ESP32 and ATmega328P is facilitated by the MQTT protocol, and sensor data is sent to the ThingSpeak server for analysis.

## Key Features

- **Remote Control:**
  - Users can remotely control the state of 3 lights and 2 fans using a mobile phone application.

- **PWM Control:**
  - The system allows for the adjustment of fan 2 speed through Pulse Width Modulation (PWM) control.

- **Light Intensity Detection:**
  - An LDR module is integrated to detect ambient light intensity.

- **Wireless Communication:**
  - The ESP32 facilitates wireless communication, enabling users to control and monitor the system remotely over WiFi.

- **MQTT Protocol:**
  - MQTT is employed for communication between the ESP32 and ATmega328P, ensuring reliable data exchange.

- **ThingSpeak Integration:**
  - Sensor data, including light intensity information, is sent to the ThingSpeak server for analysis and visualization.

## Hardware Components

- **ATmega328P Microcontroller:**
  - Programmed in AVR Assembly language to efficiently control lights, fans, and process data from the LDR module.

- **ESP32 WROOM Development Board:**
  - Programmed in C++ using Arduino IDE for WiFi communication, MQTT protocol implementation, and data transfer to the ThingSpeak server.

- **LDR Module:**
  - Light Dependent Resistor module for detecting ambient light levels.

- **Mobile Phone:**
  - Serves as the user interface for remote control and monitoring.

- **ThingSpeak Server:**
  - Used for data analysis and visualization.

## Programming Languages and Tools

- **ATmega328P:**
  - Programmed in AVR Assembly language for precise hardware control.

- **ESP32:**
  - C++ programmed using Arduino IDE for ease of development and integration.

## How to Use

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/AsimAliMurtaza/Home-Automation.git
   ```

2. **Open the Arduino IDE:**
   - Load the ESP32 program into the Arduino IDE.
   - Select the correct board and port for your ESP32.

3. **Program the ATmega328P:**
   - Use your preferred AVR programming method to upload the Assembly program to the ATmega328P.

4. **Connect the Hardware:**
   - Ensure proper connections between the microcontrollers, LDR module, lights, and fans.

5. **Update Configuration:**
   - Update WiFi credentials, MQTT server details, and other configurations in the code.

6. **Upload and Run:**
   - Upload the programs to the respective microcontrollers.

7. **Monitor on ThingSpeak:**
   - View real-time and historical data on the ThingSpeak server.

8. **Control via Mobile App:**
   - Download and install the mobile app on your phone.
   - Connect to the system over WiFi and control lights and fans remotely.

## Contributions and Issues

Contributions are welcome! If you find any issues or have ideas for improvements, please open an issue or submit a pull request.
