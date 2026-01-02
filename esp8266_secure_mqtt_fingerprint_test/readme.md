# 🔒 esp8266_secure_mqtt_fingerprint

![ESP8266](https://img.shields.io/badge/Platform-ESP8266-blue?style=for-the-badge&logo=espressif)
![Security](https://img.shields.io/badge/Security-TLS_Fingerprint-red?style=for-the-badge)
![MQTT](https://img.shields.io/badge/Protocol-MQTT_Secure-orange?style=for-the-badge)

## 📖 Overview
The **esp8266_secure_mqtt_fingerprint_test** project demonstrates how to implement encrypted IoT communication. While standard MQTT sends data in plain text (port 1883), this project uses **MQTTS** (port 8883) to ensure that control commands and credentials are encrypted.

It uses **Fingerprint Verification**, a lightweight method for the ESP8266 to verify that it is talking to the correct server without the overhead of full CA (Certificate Authority) chain validation.



## 🚀 Key Features
* **🔐 TLS 1.2 Encryption:** Protects your WiFi passwords and MQTT credentials from being sniffed on the network.
* **🛡️ Fingerprint Authentication:** Hardcodes the SHA1 hash of the server's certificate to prevent "Man-in-the-Middle" (MITM) attacks.
* **🔄 gBridge Integration:** Pre-configured for gBridge-style topic structures, enabling easy integration with Google Assistant or smart home platforms.
* **💡 Visual Feedback:** Uses the onboard LED to indicate the status of the "OnOff" characteristic received from the broker.

## 🛠️ Security Architecture
The security flow involves:
1. **WiFi STA Mode:** Connects to the local network.
2. **Fingerprint Match:** When connecting to `NA7RAWY_PC.local`, the ESP8266 checks the server's certificate against the `PROGMEM` fingerprint.
3. **Encrypted Tunnel:** A secure socket is opened via `WiFiClientSecure`.
4. **Credential Exchange:** Username and Password are sent only *after* the tunnel is secure.



## 📋 Technical Requirements
* **MQTT Port:** 8883 (SSL/TLS).
* **Library:** `PubSubClient` by Nick O'Leary.
* **Board:** ESP8266 (NodeMCU, Wemos D1 Mini, or ESP-01).

## ⚠️ Maintenance Note
SSL/TLS Fingerprints are tied to specific certificates. **If your MQTT server's certificate is renewed or changed, you must update the `fingerprint` string in the code**, or the connection will be rejected by the ESP8266 for security reasons.

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.