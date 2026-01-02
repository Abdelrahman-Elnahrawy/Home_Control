/**
 * PROJECT: ESP8266 Global GPIO Validator
 * AUTHOR: Abdelrahman Elnahrawy
 * DESCRIPTION: Simultaneously blinks all major GPIO pins to verify hardware 
 * integrity. Useful for testing new development boards.
 */

// Array of all available GPIOs on a standard ESP-12E/NodeMCU
const int targetPins[] = {16, 5, 4, 0, 2, 14, 12, 13, 15, 3, 1};
const int pinCount = sizeof(targetPins) / sizeof(targetPins[0]);

// Timing Configuration (ms)
const int ON_TIME  = 1000;
const int OFF_TIME = 2000;

void setup() {
  // Initialize all pins as OUTPUT in a single loop
  for (int i = 0; i < pinCount; i++) {
    pinMode(targetPins[i], OUTPUT);
    // Ensure they start HIGH (Off for active-low LEDs)
    digitalWrite(targetPins[i], HIGH);
  }
}

void loop() {
  // 1. Turn all LEDs/Pins ON (Active Low)
  for (int i = 0; i < pinCount; i++) {
    digitalWrite(targetPins[i], LOW);
  }
  delay(ON_TIME);

  // 2. Turn all LEDs/Pins OFF
  for (int i = 0; i < pinCount; i++) {
    digitalWrite(targetPins[i], HIGH);
  }
  delay(OFF_TIME);
}