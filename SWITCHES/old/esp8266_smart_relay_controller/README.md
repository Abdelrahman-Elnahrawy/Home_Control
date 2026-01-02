# 🏠 ESP8266 Smart Relay Controller

![Platform](https://img.shields.io/badge/Platform-ESP8266-blue?style=for-the-badge&logo=espressif)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge)
![Home Automation](https://img.shields.io/badge/Category-Home_Automation-green?style=for-the-badge)

## 📖 Overview
This project is an industrial-grade **Smart Home Hub** based on the ESP8266. It manages up to 6 relays, supports PWM dimming on the 6th channel, and integrates a motion sensor for automated lighting. It is designed to work seamlessly with **Home Assistant** via MQTT.



## 🚀 Key Features
* **📶 Captive Portal Provisioning:** If WiFi credentials are lost or the router changes, the device creates its own Hotspot. Users can connect via phone and enter new WiFi details through a web interface.
* **🧠 Triple Logic Modes (LM):**
  1. **Manual (0):** Fully controlled via MQTT or physical buttons.
  2. **Sensor-Triggered (1):** Lights turn on when motion is detected and it is dark.
  3. **Timed-Auto (2):** Lights turn on with motion and remain on for a user-defined delay (`LD`).
* **💾 State Memory:** Uses EEPROM to store relay states and user settings (like brightness and timers) so they survive a power outage.
* **🔘 Physical Keypad Support:** Supports a 3-button input system for manual overrides and mode switching.

## 🛠️ Hardware Setup
### Connection Map


| Component | ESP8266 Pin | Function |
| :--- | :--- | :--- |
| **Relay 1** | GPIO 4 (D2) | Main Lighting |
| **Relay 6** | GPIO 16 (D0) | PWM / Dimmer |
| **Motion Sensor** | GPIO 3 (RX) | PIR Input |
| **Buttons** | GPIO 14, 12, 13 | Manual Control Keypad |
| **LDR/Analog** | A0 | Light level sensing |

## ⚙️ Software Logic Flow
1. **Boot:** Load relay states from EEPROM.
2. **Connectivity:** Try WiFi. If fails after 7 seconds, start AP "MQTT!RCID:1".
3. **Main Loop:**
   - Listen for MQTT commands from Home Assistant.
   - Check motion sensor if `Logic Mode` is 1 or 2.
   - Read physical keypad for manual overrides.

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.