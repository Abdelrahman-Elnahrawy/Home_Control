# 🏷️ Tags

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-black?style=for-the-badge&logo=espressif)
![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-orange)
![Language: C/C++](https://img.shields.io/badge/Language-C%2B%2B-blue)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-660066?style=for-the-badge&logo=mqtt)
![Status: Demo](https://img.shields.io/badge/Status-Demo-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-orange?style=for-the-badge)

---

# Arduino IoT Relay & Sensor Control
**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![Project Image](./assets/project_image.png)  <!-- Replace with your project image -->

---

## 📌 Description | الوصف

**English:**  
This project allows you to control relays, motors, and read sensor data (DHT11, PIR, buttons, and analog sensors) using an ESP8266 connected via WiFi and MQTT.  
You can remotely toggle devices, monitor temperature, humidity, and other sensors, making it suitable for home automation or IoT applications.

**Arabic:**  
هذا المشروع يتيح لك التحكم في المرحلات، المحركات وقراءة بيانات الحساسات (DHT11، PIR، الأزرار والحساسات التناظرية) باستخدام ESP8266 متصل عبر الواي فاي وMQTT.  
يمكنك التحكم في الأجهزة عن بعد ومراقبة درجة الحرارة والرطوبة والحساسات الأخرى، مما يجعله مناسبًا لتطبيقات المنزل الذكي وإنترنت الأشياء.

---

## 🔧 Features | المميزات

- Control relays, motors, and other actuators remotely | التحكم عن بعد في المرحلات والمحركات والأجهزة الأخرى  
- Read temperature and humidity from DHT11 sensor | قراءة درجة الحرارة والرطوبة من حساس DHT11  
- Detect motion using PIR sensor | كشف الحركة باستخدام حساس PIR  
- Button press detection | الكشف عن ضغط الأزرار  
- MQTT communication for IoT integration | الاتصال عبر MQTT لتكامل إنترنت الأشياء  
- Compatible with ESP8266 boards | متوافق مع لوحات ESP8266  

---

## ⚙️ Hardware Setup | إعداد الهاردوير

- **ESP8266 board**  
- **DHT11 sensor** connected to GPIO0  
- **Relays** connected to GPIO12, GPIO13, GPIO14, GPIO16  
- **Motors or devices** connected to GPIO2 and GPIO15  
- **PIR sensor** connected to GPIO5  
- **Button** connected to GPIO4  
- Optional **analog sensor** connected to A0  

---

## 💻 Software Setup | إعداد السوفتوير

1. Install Arduino IDE  
2. Install the following libraries:  
   - `DHT`  
   - `ESP8266WiFi`  
   - `PubSubClient`  
3. Set your WiFi SSID and password in the code  
4. Set your MQTT broker address  
5. Upload the code to your ESP8266 board  

---

## 🔗 MQTT Topics | مواضيع MQTT

| Topic           | Purpose                  |
|-----------------|-------------------------|
| `esp8266/DHT`   | Send temperature & humidity data | إرسال بيانات الحرارة والرطوبة |
| `esp8266/PIR`   | Motion detection alerts | تنبيهات كشف الحركة |
| `esp8266/BUTTON`| Button press notifications | إشعارات ضغط الأزرار |
| `esp8266/LASER` | Analog sensor alerts | تنبيهات الحساس التناظري |

---

## 📄 License | الترخيص

This project is licensed under the **MIT License** – see the [LICENSE](./LICENSE) file for details.  
هذا المشروع مرخص بموجب **ترخيص MIT** – راجع ملف [LICENSE](./LICENSE) للتفاصيل.

---
