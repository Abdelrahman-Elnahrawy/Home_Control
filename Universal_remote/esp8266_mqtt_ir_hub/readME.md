# 📡 esp8266_mqtt_ir_hub

![Project Status](https://img.shields.io/badge/Status-Active-success?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-ESP8266-blue?style=for-the-badge&logo=espressif)
![Protocol](https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge)
![Hardware](https://img.shields.io/badge/Hardware-IR_LED_Emitter-red?style=for-the-badge)

## 📖 Overview
The **Smart MQTT IR Hub** is a DIY universal remote bridge. It allows you to control non-smart appliances (TVs, Air Conditioners, Stereos) using internet-based commands. It connects to an MQTT broker, listens for specific "Raw IR" payloads, and blasts them via an infrared LED.

## 🚀 Key Features
* **🌐 Self-Healing Connectivity:** Automatically switches to "Access Point Mode" if WiFi or MQTT connection fails, allowing you to re-configure the device via a smartphone.
* **💾 Persistent Storage:** Uses EEPROM to store SSID, Password, and MQTT Broker IP so you never have to hardcode credentials.
* **⚡ Raw IR Processing:** Supports complex IR protocols by handling "Raw" timing arrays, making it compatible with almost any remote control.
* **📱 Captive Portal:** Includes a built-in HTML/CSS web interface for easy initial setup.

## 🛠️ System Architecture
The device operates in two primary states:

### 1. Configuration Mode (AP Mode)
If no WiFi is found, the ESP8266 creates a hotspot named **"Unimote Credentials !"**. When you connect, you are presented with a web form to enter your local network details.


### 2. Operational Mode (STA Mode)
Once configured, it connects to your WiFi and MQTT Broker. 
* **Subscription Topic:** `IR_RAW_DATA_TOPIC`
* **Payload Format:** `IRRAW:500,200,500,200...}`


## 🔌 Hardware Connections


| Component | ESP8266 Pin | Note |
| :--- | :--- | :--- |
| **IR LED (Anode)** | D7 (GPIO 13) | Use a current-limiting resistor or transistor driver |
| **IR LED (Cathode)** | GND | |

## 📡 MQTT Command Examples
To trigger an IR signal, publish a message to the topic `IR_RAW_DATA_TOPIC`:

| Action | Payload Example |
| :--- | :--- |
| **Send Raw Signal** | `IRRAW:3400,1750,450,450,450,1300...}` |
| **Repeat Last Signal** | `}` |

## 📦 Requirements
* **IRremoteESP8266 Library:** For IR signal generation.
* **PubSubClient Library:** For MQTT communication.
* **ESP8266 Board Package:** Installed via Arduino Boards Manager.

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.