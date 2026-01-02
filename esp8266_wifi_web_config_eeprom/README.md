# 🌐 esp8266_wifi_credentials_web_config_eeprom

ESP8266 WiFi Credentials Web Configuration using EEPROM, Access Point fallback, and Web Server.

**Author:** Abdelrahman Elnahrawy  
**License:** MIT

![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-green)
![IDE](https://img.shields.io/badge/IDE-ArduinoIDE-orange)
![ArduinoIDE](https://img.shields.io/badge/ArduinoIDE-orange)  
![ESP8266](https://img.shields.io/badge/ESP8266-blue)  
![WiFi](https://img.shields.io/badge/WiFi-lightgrey)  
![HTTP](https://img.shields.io/badge/HTTP-green)  
![EEPROM](https://img.shields.io/badge/EEPROM-yellow)  
![Access-Point](https://img.shields.io/badge/Access--Point-lightblue)  
![Web-Server](https://img.shields.io/badge/Web--Server-red)  
![IoT](https://img.shields.io/badge/IoT-purple)  

---

## 📌 Overview | نظرة عامة

### English
This project allows an ESP8266 to:
- Store WiFi SSID and Password in EEPROM
- Automatically try to connect on boot
- Create an Access Point if connection fails
- Provide a Web Page to enter WiFi credentials
- Save credentials and reboot automatically

### العربي
المشروع يتيح لـ ESP8266:
- تخزين اسم الشبكة وكلمة المرور في EEPROM
- محاولة الاتصال تلقائيًا عند التشغيل
- إنشاء Access Point عند فشل الاتصال
- عرض صفحة ويب لإدخال بيانات الشبكة
- حفظ البيانات وإعادة التشغيل تلقائيًا

---

## ⚙️ Features | المميزات

- WiFi credentials stored in EEPROM
- Automatic reconnect on startup
- Access Point fallback mode
- Embedded HTML configuration page
- No Serial input required
- MDNS support (esp8266.local)

---

## 🧩 Hardware Requirements | المتطلبات

| Component | Description |
|---------|------------|
| ESP8266 | NodeMCU / Wemos D1 Mini |
| USB Cable | Programming & Power |
| PC / Mobile | For configuration page |

---

## 🌐 Network Behavior | سلوك الشبكة

1. Read WiFi credentials from EEPROM
2. Try connecting to saved network
3. If connection fails:
   - Create AP: **ESP help!!**
   - IP: 192.168.4.22
4. User enters SSID & Password via browser
5. Credentials saved to EEPROM
6. ESP8266 restarts automatically

---

## 🧠 Software Logic | منطق البرنامج

- EEPROM separator character: `$`
- SSID stored first, password second
- Web server runs on port 80
- POST endpoint: `/postform/`
- EEPROM size: 512 bytes

---

## 🏷️ Tags

---

## 📄 License

MIT License  
Free for educational and research use.
