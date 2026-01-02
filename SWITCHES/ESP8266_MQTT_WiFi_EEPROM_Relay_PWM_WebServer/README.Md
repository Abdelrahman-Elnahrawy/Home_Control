# 🌐 ESP8266_MQTT_EEPROM_WebServer_PWM_Relays
نظام ESP8266 للتحكم بالأجهزة عبر MQTT، مع حفظ الإعدادات في EEPROM، صفحة ويب لتحديث الواي فاي، والتحكم في PWM و 12 ريليه

**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![ESP8266](https://img.shields.io/badge/Platform-ESP8266-blue)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-green)
![Status](https://img.shields.io/badge/Status-Prototype-orange)

---

## 📌 Overview | نظرة عامة

**English**  
This project uses an ESP8266 to:  
- Connect to WiFi using saved credentials from EEPROM  
- Fall back to SoftAP mode and web page for WiFi configuration if needed  
- Communicate with MQTT broker to receive commands and publish statuses  
- Control 6 relays and 6 PWM channels  
- Persist relay states, PWM values, and configuration in EEPROM  
- Handle automatic restart on connection failure  

**العربي**  
المشروع يستخدم ESP8266 للقيام بالتالي:  
- الاتصال بالواي فاي باستخدام بيانات مخزنة في EEPROM  
- تشغيل نقطة وصول SoftAP وصفحة ويب لإعداد الواي فاي إذا فشل الاتصال  
- التواصل مع بروكر MQTT لتلقي الأوامر ونشر الحالات  
- التحكم في 6 ريليه و 6 مخرجات PWM  
- حفظ حالات الريليه، قيم PWM، والإعدادات في EEPROM  
- إعادة التشغيل تلقائياً عند فشل الاتصال  

---

## ⚙️ Features | المميزات

- EEPROM-based persistence for WiFi credentials, relay states, and PWM values  
- WebServer POST form for WiFi configuration  
- MQTT publish/subscribe for relays and PWM control  
- Automatic reconnect to WiFi and MQTT  
- Non-blocking code using `millis()` for timing  
- Flexible relay and PWM control via MQTT messages  

---

## 🧩 Hardware Requirements | المتطلبات العتادية

| Component | المكوّن |
|---------|---------|
| ESP8266 Board | NodeMCU / Wemos D1 mini |
| Relays | 12 ريليه للتحكم بالأجهزة |
| PWM-controlled Devices | 6 مخرجات PWM |
| Push Button / Sensors | حسب الحاجة |
| Resistors & Wires | مقاومات وأسلاك توصيل |
| MQTT Broker | بروكر MQTT (مثال: 192.168.1.123) |

---

## 🔌 Pin Connections | توصيل الحساسات و الريليه

> يجب تعديل حسب المشروع، أمثلة:  

| Device | ESP8266 Pin |
|--------|-------------|
| Relay 1 | GPIO 12 |
| Relay 2 | GPIO 13 |
| Relay 3 | GPIO 14 |
| Relay 4 | GPIO 15 |
| Relay 5 | GPIO 0 |
| Relay 6 | GPIO 16 |
| PWM 0 | GPIO 5 |
| PWM 1 | GPIO 4 |
| PWM 2 | GPIO 0 |
| PWM 3 | GPIO 2 |
| PWM 4 | GPIO 14 |
| PWM 5 | GPIO 12 |

---

## 🧠 Software Logic | منطق البرنامج

1. Read WiFi credentials from EEPROM  
2. Connect to WiFi or fallback to SoftAP mode  
3. Launch WebServer for configuration in SoftAP mode  
4. Connect to MQTT broker and subscribe to relay/PWM topics  
5. Parse incoming MQTT payload to update:  
   - Relays R0-R5 ON/OFF  
   - PWM0-PWM5 values (0-255)  
   - Timers and modes  
6. Store updated states in EEPROM  
7. Auto-reconnect on WiFi or MQTT failure, with system restart if needed  

---

## 🏷️ Tags

ESP8266  
MQTT  
EEPROM  
WebServer  
PWM  
Home-Automation  
IoT  
Relay-Control  
NodeMCU  
Wemos  
Wireless-IoT  

---

## 📄 License
MIT License - provided by Abdelrahman Elnahrawy for educational and research purposes.
