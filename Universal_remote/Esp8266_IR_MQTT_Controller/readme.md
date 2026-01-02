# 🌐 esp8266_ir_mqtt_controller

ESP8266 IR Controller with MQTT integration and web-based WiFi configuration.

**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-green)
![IDE](https://img.shields.io/badge/IDE-ArduinoIDE-orange)
![Status](https://img.shields.io/badge/Status-Prototype-lightgrey)

---

## 📌 Overview | نظرة عامة

**English**  
This project uses an ESP8266 to send IR signals controlled via MQTT:  
- WiFi configuration through web form  
- IR sending using IRremoteESP8266 library  
- MQTT publish/subscribe for IR commands  
- Auto-reconnect and fail-safe restart  

**العربي**  
المشروع يستخدم ESP8266 لإرسال إشارات IR بالتحكم عبر MQTT:  
- إعداد شبكة WiFi عبر صفحة ويب  
- إرسال IR باستخدام مكتبة IRremoteESP8266  
- النشر والاشتراك في MQTT لإرسال واستقبال أوامر IR  
- إعادة الاتصال التلقائي وإعادة تشغيل الجهاز عند الحاجة  

---

## ⚙️ Features | المميزات

- Configurable WiFi credentials stored in EEPROM  
- MQTT publish/subscribe support  
- IR transmission via pin 13  
- Automatic reconnection and ESP restart if disconnected  

---

## 🧩 Hardware Requirements | المتطلبات

| Component | Description |
|-----------|------------|
| ESP8266 Board | NodeMCU / Wemos D1 mini |
| IR LED | Connected to GPIO 13 |
| MQTT Broker | Example: broker.emqx.io |
| USB Cable | Power & Serial |
| Optional: Serial Monitor | Debug messages |

---

## 🧠 Software Logic | منطق البرنامج

1. Connect to stored WiFi or start hotspot for configuration  
2. Connect to MQTT broker and subscribe to `IR_Sub` topic  
3. Receive IR command sequences via MQTT  
4. Send IR commands via IR LED on GPIO 13  
5. Publish confirmation messages via MQTT  
6. Restart ESP automatically if disconnected for 5 minutes  

---

## 🏷️ Tags

![ESP8266](https://img.shields.io/badge/Platform-ESP8266-blue)  
![ArduinoIDE](https://img.shields.io/badge/IDE-ArduinoIDE-orange)  
![MQTT](https://img.shields.io/badge/MQTT-lightgrey)  
![IR](https://img.shields.io/badge/IR-purple)  
![EEPROM](https://img.shields.io/badge/EEPROM-red)  

---

## 📄 License

MIT License - provided by Abdelrahman Elnahrawy for educational and research purposes.
