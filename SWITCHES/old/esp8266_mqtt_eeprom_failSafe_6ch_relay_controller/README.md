# esp8266_mqtt_eeprom_failSafe_6ch_relay_controller

![Platform](https://img.shields.io/badge/Platform-ESP8266-blue?style=for-the-badge&logo=espressif)
![Protocol](https://img.shields.io/badge/Protocol-MQTT-green?style=for-the-badge&logo=mqtt)
![Stability](https://img.shields.io/badge/Stability-Fail--Safe_Watchdog-red?style=for-the-badge)

## 📖 Project Overview
The **esp8266_mqtt_eeprom_failSafe_6ch_relay_controller** is an industrial-grade IoT node designed for high-availability home automation. It features a robust connectivity watchdog, non-volatile state memory (EEPROM), and a tri-modal logic engine. This controller is engineered to recover automatically from network outages and power failures without manual intervention.

---

## 🚀 Technical Highlights

* **🛡️ Fail-Safe Connectivity Watchdog:** Implements a 300-second supervisor timer. If the link to the MQTT broker or WiFi is severed, the system triggers a soft-reset to clear heap fragmentation and re-establish the connection.
* **💾 EEPROM State Restoration:** All relay states, PWM brightness levels, and Logic Modes are mirrored in EEPROM. Upon power recovery, the system restores the environment to its exact previous state.
* **🌐 Self-Healing Provisioning:** If stored WiFi credentials fail, the device automatically deploys a **Captive Portal** Access Point (`WiFi!RCID:1`). Users can update credentials via a web browser without re-flashing the firmware.
* **🧠 Tri-Modal Automation Engine (LM):**
    * **LM 0 (Manual):** Direct remote control via MQTT or local tactile buttons.
    * **LM 1 (Logic-Toggle):** Autonomous switching based on motion detection with a 5-second hysteresis lockout.
    * **LM 2 (Temporal-Pulse):** Motion-triggered activation with a configurable countdown timer (`LD`).

---

## 🛠️ Hardware Architecture

### Pin Mapping Table


| Component | Pin (GPIO) | Mode | Purpose |
| :--- | :--- | :--- | :--- |
| **Relay 1** | GPIO 4 (D2) | Output | Primary Load (Automation Linked) |
| **Relay 2** | GPIO 5 (D1) | Output | Secondary Load |
| **Relay 3** | GPIO 0 (D3) | Output | Secondary Load |
| **Relay 4** | GPIO 2 (D4) | Output | Secondary Load |
| **Relay 5** | GPIO 15 (D8) | Output | Secondary Load |
| **Relay 6** | GPIO 16 (D0) | **PWM** | Dimmer / Speed Control |
| **PIR Sensor**| GPIO 3 (RX) | Input | Motion Detection (Active LOW) |
| **Buttons** | 14, 12, 13 | Input | 3-Button Navigation Keypad |



---

## 📡 Telemetry & MQTT Protocol

### State Feedback
The device broadcasts a packed telemetry string every 2 seconds to the `ESP_Pub_1` topic:
`R1:1 R2:0 R3:0 R4:0 R5:0 R6:255 T:420 D:0010 LM:1 #142`

* **R1 - R5:** Relay Boolean states.
* **R6:** PWM Intensity (0-255).
* **T:** Raw ADC value from A0 (Sensor Feedback).
* **D:** Automation Delay (`LD`) in seconds.
* **LM:** Current Logic Mode Index.

### Security Alarm

A dedicated alarm flag is published to the broker immediately upon motion detection if the sensor is triggered, allowing for real-time security integration.

---

## 🎮 Local Control Logic
The 3-button keypad manages local overrides using combination logic:
* **Button 3:** Toggles Relay 3.
* **Button 2:** Decrements Logic Mode ($LM$).
* **Button 2 + 3:** Toggles Relay 1 and forces Manual Mode.
* **Button 1 + 2:** Increases PWM intensity for Relay 6 ($+4$ steps).
* **Button 1 + 2 + 3:** Universal Logic Mode cycle.

---

## 👤 Author
* **Abdelrahman Elnahrawy**
* **Project Identity:** `esp8266_mqtt_eeprom_failSafe_6ch_relay_controller`