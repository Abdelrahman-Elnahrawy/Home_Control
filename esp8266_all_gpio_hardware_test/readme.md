# 📶 ESP8266 GPIO Output Test & Blink Pattern

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-black?style=for-the-badge&logo=espressif)
![Status](https://img.shields.io/badge/Status-Diagnostic-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Author](https://img.shields.io/badge/Author-Abdelrahman--Elnahrawy-orange?style=for-the-badge)

## 📖 Overview
This project is a **Hardware Diagnostic Utility** designed for ESP8266-based boards (like NodeMCU or Wemos D1 Mini). 

Because the printed pin labels (e.g., **D1, D2**) often do not match the internal GPIO numbers used in code, this sketch iterates through the digital pins, triggering high/low signals and blink patterns. It is an essential tool for verifying pin mapping and hardware integrity.

## 🔌 Pin Mapping Reference
This sketch helps verify the standard NodeMCU mapping used in the code:

| Board Label | Internal GPIO | Special Function |
| :--- | :--- | :--- |
| **D0** | GPIO 16 | Wake / User LED |
| **D1** | GPIO 5 | I2C SCL |
| **D2** | GPIO 4 | I2C SDA |
| **D3** | GPIO 0 | Flash Button (Boot) |
| **D4** | GPIO 2 | TXD1 / Built-in LED |
| **D5** | GPIO 14 | SPI CLK |
| **D6** | GPIO 12 | SPI MISO |
| **D7** | GPIO 13 | SPI MOSI |
| **D8** | GPIO 15 | SPI CS / Boot |

## ⚙️ How It Works
The script runs a diagnostic loop:
* **Initialization:** Configures all defined pins as `OUTPUT` mode.
* **Sequential Test:** Cycles each pin **HIGH** then **LOW** with a specific delay.
* **Serial Logging:** Prints the active GPIO number to the Serial Monitor so you can cross-reference what you see on the board.

## 🛠️ Hardware Setup
* **Microcontroller:** ESP8266 (NodeMCU v1.0 / Wemos D1 Mini).
* **Testing:** Connect an LED (with a 220Ω resistor) to the pin you wish to test, or use a multimeter to check for a 3.3V signal.

## 👤 Author
* **Abdelrahman Elnahrawy** * GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.
