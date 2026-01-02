# 🌈 esp8266_mqtt_ir_neopixel

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-black?style=for-the-badge&logo=espressif)
![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-660066?style=for-the-badge&logo=mqtt)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-orange?style=for-the-badge)

## 📖 Overview
**esp8266_mqtt_ir_neopixel** is an advanced IoT Hub designed for smart home integration. It acts as a bridge between your network and physical devices, combining visual feedback (NeoPixels) with infrared control (IR Blaster).

Unlike simple sketches, this project features a **Captive Portal** for WiFi configuration—if it can't connect to your router, it automatically creates a Hotspot so you can enter new credentials via a web page. It also utilizes **Hardware Timer Interrupts** to ensure the system status is monitored without blocking the LED animations.

## 🚀 Key Features
* **🔗 Smart WiFi Provisioning:** Automatic failover to AP Mode ("Unimote Setup") if WiFi connection fails. Credentials are saved to EEPROM.
* **📨 MQTT IR Blaster:** Listens for MQTT commands to transmit raw Infrared signals to TVs, ACs, or other appliances.
* **✨ NeoPixel Controller:** Integrated `Adafruit_NeoPixel` support for complex light animations (Rainbow, Theater Chase).
* **⚡ Hardware Interrupts:** Uses `ESP8266TimerInterrupt` to handle background tasks every 500ms independently of the main loop.

## 🛠️ Hardware Setup
| Component | GPIO Pin | NodeMCU Label | Description |
| :--- | :--- | :--- | :--- |
| **NeoPixel Data** | GPIO 14 | **D5** | Control pin for LED Strip |
| **IR LED** | GPIO 4 | **D2** | Infrared Transmitter (940nm) |
| **Built-in LED** | GPIO 2 | **D4** | Status Indicator |



## 📦 Required Libraries
Ensure these are installed via the Arduino Library Manager:
1.  **Adafruit NeoPixel**
2.  **IRremoteESP8266**
3.  **PubSubClient** (for MQTT)
4.  **ESP8266TimerInterrupt**

## ⚙️ Configuration
1.  **Upload:** Flash the code to your ESP8266.
2.  **Connect:** If it's the first run, connect your phone/PC to the WiFi network **"Unimote Setup"**.
3.  **Configure:** A pop-up (Captive Portal) should appear. If not, go to `192.168.4.1`. Enter your home WiFi SSID and Password.
4.  **Control:** Once connected, the device will listen to the MQTT topic defined in the code (default: `IR_RAW_DATA_TOPIC`).

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.