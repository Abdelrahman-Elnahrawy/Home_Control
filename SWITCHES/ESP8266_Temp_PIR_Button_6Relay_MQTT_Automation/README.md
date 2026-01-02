# 🌡️ ESP8266 MQTT Temperature & Home Automation System (Clean Version)
نظام التحكم المنزلي وقياس درجة الحرارة باستخدام ESP8266 و MQTT (نسخة محسنة)

**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![ESP8266](https://img.shields.io/badge/Platform-ESP8266-blue)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-green)
![Status](https://img.shields.io/badge/Status-Prototype-orange)

---

## 📌 Overview | نظرة عامة

**English**  
This project uses an ESP8266 microcontroller to monitor temperature using a DS18B20 sensor, detect motion via a PIR sensor, detect button presses, and control 6 relays via MQTT messages. The system communicates with a MQTT broker to publish sensor data and receive relay commands remotely.

**العربي**  
المشروع يستخدم ESP8266 لقياس درجة الحرارة باستخدام حساس DS18B20، واكتشاف الحركة عبر حساس PIR، واكتشاف ضغط الأزرار، والتحكم في 6 ريليه عن طريق رسائل MQTT. النظام يتواصل مع بروكر MQTT لنشر البيانات واستقبال أوامر التحكم عن بعد.

---

## ⚙️ Features | المميزات

- Measure temperature using DS18B20 sensor  
- Detect motion (PIR) and button press events  
- Control 6 relays via MQTT commands  
- Publish temperature, PIR, button, and relay status periodically  
- WiFi connectivity with MQTT broker (e.g., test.mosquitto.org)  

- قياس درجة الحرارة باستخدام DS18B20  
- اكتشاف الحركة (PIR) وضغط الأزرار  
- التحكم في 6 ريليه عن طريق رسائل MQTT  
- نشر درجة الحرارة والحركة وحالة الأزرار والريليه بشكل دوري  
- الاتصال بشبكة WiFi و بروكر MQTT (مثل test.mosquitto.org)

---

## 🧩 Hardware Requirements | المتطلبات العتادية

| Component | المكوّن |
|---------|---------|
| ESP8266 Board | NodeMCU / Wemos D1 mini / compatible |
| DS18B20 Temperature Sensor | حساس درجة الحرارة DS18B20 |
| PIR Motion Sensor | حساس الحركة PIR |
| Push Button | زر ضغط |
| 6 Relays | 6 ريليه للتحكم بالأجهزة |
| Resistors & Wires | مقاومات وأسلاك توصيل |
| MQTT Broker | بروكر MQTT (مثال: test.mosquitto.org) |

---

## 🔌 Pin Connections | توصيل الحساسات و الريليه

| Device | ESP8266 Pin |
|--------|-------------|
| DS18B20 | D1 (GPIO5) |
| PIR Sensor | D4 (GPIO2) |
| Button | D2 (GPIO4) |
| Relay 1 | D6 (GPIO12) |
| Relay 2 | D7 (GPIO13) |
| Relay 3 | D5 (GPIO14) |
| Relay 4 | D8 (GPIO15) |
| Relay 5 | D3 (GPIO0) |
| Relay 6 | D0 (GPIO16) |

---

## 🧠 Software Logic | منطق البرنامج

1️⃣ **WiFi & MQTT Connection**  
- Connects ESP8266 to a WiFi network  
- Connects to MQTT broker and subscribes to topic `"zabolaa"`  

2️⃣ **Relay Control via MQTT**  
- Receives commands like `r1n` or `r1f` to turn relays ON or OFF  
- Updates internal relay state variables `R1` to `R6`

3️⃣ **Sensor Readings & Publishing**  
- Temperature measured every 2 seconds and published to `"esp8266/TEMP"`  
- PIR motion detected every 1 second and published to `"esp8266/PIR"`  
- Button press detected every 0.7 seconds and published to `"esp8266/BUTTON"`  
- Relay status published every 2 seconds to `"esp8266/feedback"`

4️⃣ **Non-blocking Timing**  
- Uses `millis()` to handle periodic publishing and avoid delays  
- Ensures multiple events are handled concurrently without blocking the main loop

---

## 🏷️ Tags

```
ESP8266
MQTT
Home-Automation
IoT
DS18B20
PIR-Sensor
Relay-Control
NodeMCU
Wemos
Temperature-Monitoring
Wireless-IoT
```
---

## 📄 License
MIT License - provided by Abdelrahman Elnahrawy for educational and research purposes.
