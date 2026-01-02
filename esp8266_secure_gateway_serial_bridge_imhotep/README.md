# esp8266_secure_gateway_serial_bridge_imhotep

![Security](https://img.shields.io/badge/Security-SSL--TLS_8883-red?style=for-the-badge&logo=espressif)
![Protocol](https://img.shields.io/badge/Mode-Serial--to--MQTT_Bridge-orange?style=for-the-badge)
![UI](https://img.shields.io/badge/UI-Imhotep_Captive_Portal-blue?style=for-the-badge)

## 📖 Overview
The **esp8266_secure_gateway_serial_bridge_imhotep** is a sophisticated IoT communication hub. It serves as a secure bridge between legacy Serial (UART) devices and modern MQTT brokers using **MQTTS (Secure MQTT over TLS)**. Featuring the "Imhotep" diagnostic interface, it allows for seamless hardware state recovery via EEPROM and dual-mode operation (Station vs. Hotspot) controlled by a physical toggle.

---

## 🚀 Advanced Technical Features

* **🔐 Encrypted Communication:** Utilizes `WiFiClientSecure` with **Fingerprint Verification** and **X.509 Certificate** support for secure communication over port 8883.
* **📟 Serial-to-MQTT Bridge:** Decodes complex incoming Serial strings (at 9600 baud) and translates them into MQTT publications. This allows non-WiFi microcontrollers (like Arduino Uno/Pro Mini) to participate in a secure IoT ecosystem.
* **💾 High-Density State Persistence:** Manages a large array of variables (12 Relays: $R_0$-$R_{11}$, 6 PWM channels, and automation timers) with persistent EEPROM mirroring.
* **🌐 Imhotep Provisioning Engine:** A customized CSS/HTML captive portal for network provisioning. Includes a hardware-selectable Hotspot mode via GPIO 13.
* **🔄 Self-Correction & Watchdog:** Real-time monitoring of WiFi and MQTT status with visual LED error indicators for diagnostic troubleshooting in the field.

---

## 🛠️ Hardware Architecture

### Diagnostic Indicators
| Indicator | Pin (GPIO) | Description |
| :--- | :--- | :--- |
| **WiFi Error** | GPIO 2 | Low = Connected / High = Disconnected |
| **MQTT Error** | GPIO 5 | Low = Broker Linked / High = Error |
| **Hotspot Active**| GPIO 4 | High = AP Mode Active |
| **Toggle Switch** | GPIO 13 (D7) | Physical switch to toggle Station/AP mode |

### Virtual State Memory Mapping
* **Relays:** 12 Channels ($R_0$ to $R_{11}$)
* **PWM Channels:** 3 Digital PWM ($PWMD_0$-$PWMD_2$), 3 Analog-derived ($PWMA_0$-$PWMA_2$)
* **Automation:** Logic Mode ($LM$) and Load Delay ($LD$)

---

## 📡 Protocol Specification

### Serial Interface (UART 9600)
The gateway parses incoming Serial messages using a custom delimiter logic. 
* **Command Format:** `[ID]:[Value]` (e.g., `R1:1` or `PWMD0:128`)
* **EEPROM Write:** Commands wrapped in `~` (e.g., `~SSID^PASS~`) trigger an EEPROM update and system reboot.

### MQTT Telemetry
Securely publishes status heartbeats to the `ESP_Pub_TsesT` topic:
`me5a fa4e5a` (System Alive Signal)

---

## 🔧 Installation & Setup
1. **SSL Configuration:** Ensure the `fingerprint` variable matches your MQTT broker's SSL certificate.
2. **Provisioning:** - Toggle **GPIO 13** to `LOW` to enter Hotspot mode.
   - Connect to `WiFi!RCID:TsesT`.
   - Browse to `192.168.1.124` to configure network credentials.
3. **Hardware Deployment:** Toggle **GPIO 13** to `HIGH` to initiate Secure MQTT connection.

---

## 👤 Author
* **Abdelrahman Elnahrawy**
* **Project Name:** `esp8266_secure_gateway_serial_bridge_imhotep`