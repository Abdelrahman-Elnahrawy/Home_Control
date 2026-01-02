# 🏷️ Tags

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-black?style=for-the-badge&logo=espressif)
![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-orange)
![Language: C/C++](https://img.shields.io/badge/Language-C%2B%2B-blue)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-660066?style=for-the-badge&logo=mqtt)
![Status: Demo](https://img.shields.io/badge/Status-Demo-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-orange?style=for-the-badge)

---

# ESP8266 WiFi-MQTT Motor Controller
**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![Project Image](./assets/project_image.png)  <!-- Replace with your project image -->

---

## 📌 Description | الوصف

**English:**  
This project allows controlling DC motors using an ESP8266 via MQTT.  
It can automatically switch between **normal operation mode** (connecting to WiFi & MQTT broker) and **hotspot configuration mode** if the WiFi credentials are missing or connection is lost.  
Users can configure WiFi and MQTT broker via a web interface hosted on the ESP8266.  

**Arabic:**  
هذا المشروع يتيح التحكم في محركات DC باستخدام ESP8266 عبر بروتوكول MQTT.  
يمكنه التبديل تلقائيًا بين **وضع التشغيل العادي** (الاتصال بالواي فاي وخادم MQTT) و**وضع الهوت سبوت للتكوين** إذا كانت بيانات الواي فاي مفقودة أو تم فقد الاتصال.  
يمكن للمستخدمين إعداد بيانات الواي فاي وخادم MQTT عبر واجهة ويب مستضافة على ESP8266.  

---

## 🔧 Features | المميزات

- Control DC motors remotely via MQTT | التحكم عن بعد في المحركات عبر MQTT  
- Automatically switch to hotspot mode for configuration | التبديل تلقائيًا إلى وضع الهوت سبوت للتكوين  
- Save WiFi and MQTT credentials in EEPROM | حفظ بيانات الواي فاي وخادم MQTT في الذاكرة الدائمة  
- Web interface for easy configuration | واجهة ويب لتسهيل الإعداد  
- Compatible with ESP8266 boards | متوافق مع لوحات ESP8266  

---

## ⚙️ Hardware Setup | إعداد الهاردوير

- **ESP8266 board**  
- **DC Motors** connected to GPIO5, GPIO4, GPIO14, GPIO12  
- Power supply suitable for ESP8266 and motors  

---

## 💻 Software Setup | إعداد السوفتوير

1. Install Arduino IDE  
2. Install the following libraries:  
   - `ESP8266WiFi`  
   - `EEPROM`  
   - `ESP8266WebServer`  
   - `PubSubClient`  
3. Upload the code to your ESP8266 board  
4. First time, the ESP will start a hotspot for configuration  
5. Connect to the hotspot, open the web page, and enter WiFi & MQTT broker info  

---

## 🔗 MQTT Topics | مواضيع MQTT

| Topic              | Purpose                        |
|--------------------|--------------------------------|
| `MOTOR_CONTROL`    | Commands to control motors    | إرسال أوامر التحكم في المحركات  
| `IR_RAW_DATA_TOPIC`| Publish status / debug data   | إرسال بيانات الحالة أو اختبار  

---

## 📄 License | الترخيص

This project is licensed under the **MIT License** – see the [LICENSE](./LICENSE) file for details.  
هذا المشروع مرخص بموجب **ترخيص MIT** – راجع ملف [LICENSE](./LICENSE) للتفاصيل.

---
