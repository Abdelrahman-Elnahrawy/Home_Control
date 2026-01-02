# ESP8266 MQTT Three Relay Switch with Hotspot Configuration

![Arduino IDE](https://img.shields.io/badge/ArduinoIDE-Compatible-blue)
![ESP8266](https://img.shields.io/badge/ESP8266-WiFi-yellow)
![MQTT](https://img.shields.io/badge/MQTT-IoT-green)

## 📌 Overview
This project implements a **3-channel relay controller** using **ESP8266** and **MQTT**.
It supports **WiFi auto-configuration via Hotspot**, **physical buttons with interrupts**, and **EEPROM storage** for credentials.

## 🚀 Features
- Control **3 relays** via MQTT
- Physical buttons with hardware interrupts
- Auto **Hotspot Mode** if WiFi or MQTT fails
- Web-based WiFi & MQTT broker configuration
- EEPROM storage (credentials + relay states)
- Works with Arduino IDE

## 🔌 Hardware
- ESP8266 (NodeMCU / ESP-12)
- 3 Relay Module
- 3 Push Buttons

## 🧠 Software
- Arduino IDE
- Libraries:
  - ESP8266WiFi
  - PubSubClient
  - ESP8266WebServer
  - EEPROM

## 📡 MQTT Topics
- **Subscribe:** `buttons/#`
- **Publish:** `SWITCHS_pub`

Example payload:
R1:1
R2:0
R3:1


## 🌐 Hotspot Mode
If WiFi or MQTT connection fails:
- ESP starts an Access Point
- Open browser and configure:
  - WiFi SSID
  - WiFi Password
  - MQTT Broker IP

## 🧩 Pin Mapping
| Function | GPIO | Label |
|--------|------|-------|
| Relay 1 | GPIO5 | D1 |
| Relay 2 | GPIO4 | D2 |
| Relay 3 | GPIO0 | D3 |
| Button 1 | GPIO12 | D6 |
| Button 2 | GPIO13 | D7 |
| Button 3 | GPIO14 | D5 |

## 🛠️ Arduino IDE File Name
## 🛠️ Arduino IDE File Name
esp8266_mqtt_three_relay_switch_with_hotspot_config.ino

css
Copy code

## 📜 License
Open-source – Free to use and modify.