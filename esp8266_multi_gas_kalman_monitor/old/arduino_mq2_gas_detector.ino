/*
 * Project: MQ-2 Gas Detector with RGB Status Indicator
 * Author: Abdelrahman Elnahrawy
 * Description: Monitors air quality using an MQ-2 sensor.
 * Categorizes gas levels into Three stages:
 * - Low (Green): Safe
 * - Medium (Blue): Warning
 * - High (Red): Critical Alert
 */

// --- RGB LED Pins ---
#define R_PIN 2
#define G_PIN 7
#define B_PIN 5

// --- Sensor Pin ---
#define MQ2_PIN A0

int gasLevel;

void setup() {
  Serial.begin(9600); // Standard serial communication

  pinMode(MQ2_PIN, INPUT);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
}

void loop() {
  // Read analog value (0-1023) and map to 8-bit range (0-255)
  int rawValue = analogRead(MQ2_PIN);
  gasLevel = map(rawValue, 0, 1023, 0, 255);

  // --- Logic for Gas Level Indication ---

  if (gasLevel > 20 && gasLevel <= 30) {
    // Stage 1: Initial Detection (Green)
    setRGB(LOW, HIGH, LOW); 
  } 
  else if (gasLevel > 30 && gasLevel <= 60) {
    // Stage 2: Rising Levels (Blue)
    setRGB(LOW, LOW, HIGH);
  } 
  else if (gasLevel > 60) {
    // Stage 3: Critical Levels (Red)
    setRGB(HIGH, LOW, LOW);
  } 
  else {
    // No significant gas detected (All OFF)
    setRGB(LOW, LOW, LOW);
  }

  Serial.print("Mapped Gas Level: ");
  Serial.println(gasLevel);
  delay(500);
}

/**
 * Helper function to manage RGB LED states efficiently
 */
void setRGB(int red, int green, int blue) {
  digitalWrite(R_PIN, red);
  digitalWrite(G_PIN, green);
  digitalWrite(B_PIN, blue);
}