# ⛽ arduino_mq2_gas_detector

![Project Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)
![Hardware](https://img.shields.io/badge/Sensor-MQ--2_Gas-orange?style=for-the-badge)
![Safety](https://img.shields.io/badge/Application-Safety_System-red?style=for-the-badge)

## 📖 Overview
The **arduino_mq2_gas_detector** is an environmental safety project designed to detect combustible gases and smoke (such as LPG, Propane, Hydrogen, and Methane). Using an MQ-2 sensor, the system provides real-time feedback through a serial monitor and a visual RGB LED indicator.



## 🚀 Key Features
* **📊 Multi-Level Sensing:** Categorizes gas concentration into three distinct color-coded safety zones.
* **📈 Data Mapping:** Converts the raw 10-bit Arduino ADC values into a more manageable 0-255 range for easier threshold calibration.
* **🚨 Visual Alarm:** Uses a Common Cathode/Anode RGB LED to provide instant status updates without needing a computer screen.
* **🛠️ Simple Calibration:** Easily adjustable software thresholds to suit different environments (kitchen vs. workshop).

## 🛠️ Hardware Setup
### Wiring Table
| Component | Pin (Arduino) | Description |
| :--- | :--- | :--- |
| **MQ-2 VCC** | 5V | Power supply |
| **MQ-2 GND** | GND | Ground |
| **MQ-2 AO** | A0 | Analog output signal |
| **RGB Red Pin** | D2 | Red Channel control |
| **RGB Green Pin** | D7 | Green Channel control |
| **RGB Blue Pin** | D5 | Blue Channel control |



## ⚙️ How it Works
1. **Sensor Preheating:** The MQ-2 contains a internal heater. It requires a short "warm-up" period (24-48 hours for first use, 30s for daily use) to provide accurate readings.
2. **Detection:** As gas concentration increases, the sensor's conductivity rises, resulting in a higher voltage output at the Analog pin.
3. **Indicator Logic:**
   - **Green (21-30):** Trace amounts detected.
   - **Blue (31-60):** Warning; moderate gas presence.
   - **Red (> 60):** Danger; high concentration detected.

## ⚠️ Important Safety Notice
This project is for educational and prototyping purposes only. It should **not** be used as a primary safety device in life-critical environments. Always use certified, commercially manufactured gas detectors for home safety.

## 👤 Author
* **Abdelrahman Elnahrawy**
* GitHub: [@Abdelrahman-Elnahrawy](https://github.com/Abdelrahman-Elnahrawy)

## ⚖️ License
This project is licensed under the **MIT License**.