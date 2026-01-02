# Home_Control 🏠
**A Modular, Industrial-Grade IoT Ecosystem for Smart Home Automation**

![Device](https://img.shields.io/badge/Device-ESP8266-000000?style=for-the-badge&logo=espressif)
![Security](https://img.shields.io/badge/Security-MQTTS_SSL-red?style=for-the-badge)
![DSP](https://img.shields.io/badge/DSP-Kalman_Filter-blue?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-blue?style=for-the-badge&logo=github)

A comprehensive suite of production-ready IoT nodes designed for a high-security, high-reliability smart home environment. This ecosystem features advanced **Signal Processing (Kalman Filters)**, **SSL/TLS encrypted MQTTS**, and **Captive Portal** network recovery.

---

## 📡 Network Resilience & Persistence Logic

A key feature of this repository is the "Zero-Interruption" logic, ensuring the home remains functional even during network or power failures.

### 1. Connection Loss & Auto-Hotspot (Captive Portal)
The system monitors the WiFi and MQTT connection states in the background. If the connection to the local router is lost or the credentials are invalid:
* **Trigger:** The device automatically switches from `STATION` mode to `AP_STA` (Access Point) mode.
* **Captive Portal:** It hosts a local web server (Hotspot). When a user connects to the "Home_Control_Node" SSID, a configuration page automatically pops up.
* **Recovery:** Users can scan for new WiFi networks and update MQTT broker details via the browser without needing to connect the device to a PC.

### 2. EEPROM Memory Management
To ensure the house doesn't "reset" after a power flicker, the system utilizes the ESP8266's **EEPROM (Flash-emulated memory)**:
* **State Saving:** Every time a relay is toggled or a dimmer level is adjusted, the new value is committed to a specific EEPROM address.
* **Boot Recovery:** Upon startup, the `void setup()` function reads these addresses first, restoring all appliances to their last known state before attempting to connect to WiFi.
* **Network Storage:** WiFi credentials and MQTT Fingerprints are also stored in EEPROM, allowing the device to remember the network across reboots.

---

## 🚀 Core Engineering Highlights

* **📈 Noise Reduction (Kalman Filter):** Implements Linear Quadratic Estimation (Kalman Filter) to stabilize noisy analog readings from MQ gas sensors, ensuring precision and eliminating false alarms.
* **🔒 Hardened Security:** Communication is secured via **MQTTS with SSL Fingerprint verification** and **mTLS (Mutual TLS)** for server-to-client authentication.

---

## 🛠 Project Catalog

### 🔒 Security & Gateway Nodes
* **Secure Gateway (Imhotep):** A professional-grade serial bridge to secure legacy hardware communications.
* **Secure Server mTLS:** Implementation of Mutual TLS for high-security device-to-server handshakes.
* **WiFi Web Config (EEPROM):** The central manager for network resilience and local web-interface configuration.

### ⚠️ Safety & Environmental Monitoring
* **Multi-Gas Kalman Monitor:** Integrated **MQ-9 (CO & Combustible)**, **MQ-7 (CO)**, and **MQ-5 (LPG)** sensors, processed through a **Kalman Filter**.
* **Precision Climate (DS18B20):** High-accuracy temperature monitoring using the **Dallas 1-Wire protocol**.

### 💡 Power, Lighting & Motion
* **Smart Dimmer:** AC phase-cutting controller using zero-cross detection for flicker-free dimming.
* **PC Power Controller:** Remote Desktop PC management with status feedback via USB 5V telemetry.
* **MQTT Motor Controller:** Remote control for DC/Stepper motors with speed and direction logic.

### 📺 Universal Remote Hubs
* **Universal Remote Suite:** Includes **IR MQTT Controller**, **MQTT IR Hub**, and **MQTT IR RGB**. These nodes bridge the gap between "dumb" IR-based appliances and the MQTT ecosystem.

### 🔌 Multi-Channel Switch Suites
* **6-Relay Automation Node:** Features **PIR motion sensors** and physical buttons for local/remote hybrid control.
* **FailSafe Controller:** A relay node specifically optimized for EEPROM state persistence.

---

## 🔌 Hardware Architecture & Components



| Category | Components Used |
| :--- | :--- |
| **Microcontrollers** | NodeMCU V3 (ESP8266-12E) |
| **Sensors** | MQ-9, MQ-7, MQ-5, DS18B20, DHT22, PIR HC-SR501 |
| **Persistence** | ESP8266 Internal Flash (EEPROM Emulation) |
| **Connectivity** | WiFi 802.11 b/g/n, MQTTS (Secure) |

---

## 👤 Author
**Abdelrahman Elnahrawy** *Embedded Systems Engineer | IoT Security Enthusiast*

[![Github](https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/Abdelrahman-Elnahrawy)

---

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.