# 🔒 esp8266_secure_server_mtls

![ESP8266](https://img.shields.io/badge/ESP8266-BearSSL-black?style=for-the-badge&logo=espressif)
![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![Security](https://img.shields.io/badge/Security-mTLS_Verified-red?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-blue?style=for-the-badge)

## 📖 Overview
The **esp8266_secure_server_mtls** is a high-security implementation of an HTTPS web server. Unlike standard web servers that only use passwords, this project implements **Mutual TLS (mTLS)**. 

This means the server will reject any connection that does not present a valid **Client Certificate** signed by the trusted Root Certificate Authority (CA) stored in the ESP8266 memory. This provides industrial-grade security for sensitive IoT control interfaces.

## 🚀 Key Features
* **🔐 BearSSL Integration:** Full hardware-accelerated encryption on port 443.
* **🛡️ Client Certificate Authentication:** Only "known" devices with a valid certificate can access the server.
* **⏰ NTP Time Sync:** Automatically synchronizes with global time servers to ensure X.509 certificate validity.
* **📜 RSA Support:** Uses 2048-bit RSA keys for robust data protection.

## 🛠️ Security Architecture
The security handshake follows these steps:
1. **Time Sync:** The ESP8266 fetches current time via NTP.
2. **Server Identity:** The server presents its `server_cert` to the client.
3. **Client Identity:** The server requests a certificate from the client and validates it against the `ca_cert`.
4. **Encryption:** Once verified, a secure tunnel is established.



## ⚠️ Important Note
The certificates provided in the code are for **demonstration purposes only**. For a production environment, you must generate your own private keys and certificates using OpenSSL.

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.