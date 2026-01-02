/*
 * Project: ESP8266 Analog Button Ladder Relay Hub
 * Author : Abdelrahman Elnahrawy
 * Board  : ESP8266 (NodeMCU / ESP-12)
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* ===================== CONFIG ===================== */
#define DEVICE_ID   101

#define PIR_PIN     2     // D4
#define RELAY1_PIN  12    // D6
#define RELAY2_PIN  13    // D7
#define RELAY3_PIN  14    // D5

#define ADC_PIN     A0

/* ===================== BUTTON CONFIG ===================== */
#define BTN_NONE  0
#define BTN_1     1
#define BTN_2     2
#define BTN_3     3

#define ADC_SAMPLES        8
#define DEBOUNCE_TIME_MS  50
#define BUTTON_SCAN_MS    40

/* ===================== MQTT ===================== */
WiFiClient espClient;
PubSubClient client(espClient);

/* ===================== GLOBALS ===================== */
volatile bool pirTriggered = false;

bool relayState[3] = {false, false, false};

unsigned long lastButtonScan = 0;
unsigned long lastPublish    = 0;

/* ===================== PIR ISR ===================== */
void pirISR() {
  pirTriggered = true;
}

/* ===================== ADC FILTER ===================== */
int readFilteredADC() {
  static uint16_t samples[ADC_SAMPLES];
  static uint8_t index = 0;
  static uint32_t sum = 0;

  sum -= samples[index];
  samples[index] = analogRead(ADC_PIN);
  sum += samples[index];

  index = (index + 1) % ADC_SAMPLES;
  return sum / ADC_SAMPLES;
}

/* ===================== BUTTON DETECTION ===================== */
uint8_t detectButton(int adc) {
  if      (adc > 0   && adc < 200) return BTN_1;
  else if (adc > 350 && adc < 550) return BTN_2;
  else if (adc > 700 && adc < 900) return BTN_3;
  return BTN_NONE;
}

/* ===================== RELAY ===================== */
void toggleRelay(uint8_t idx) {
  relayState[idx] = !relayState[idx];

  uint8_t pin = (idx == 0) ? RELAY1_PIN :
                (idx == 1) ? RELAY2_PIN :
                             RELAY3_PIN;

  digitalWrite(pin, relayState[idx] ? HIGH : LOW);
}

/* ===================== BUTTON FSM ===================== */
void handleButtons() {
  static uint8_t lastBtn = BTN_NONE;
  static uint8_t stableBtn = BTN_NONE;
  static unsigned long stableTime = 0;

  int adc = readFilteredADC();
  uint8_t currentBtn = detectButton(adc);

  if (currentBtn != lastBtn) {
    lastBtn = currentBtn;
    stableTime = millis();
  }

  if (millis() - stableTime >= DEBOUNCE_TIME_MS) {

    if (stableBtn == BTN_NONE && currentBtn != BTN_NONE) {
      stableBtn = currentBtn;
      toggleRelay(stableBtn - 1);
    }

    if (currentBtn == BTN_NONE && stableBtn != BTN_NONE) {
      stableBtn = BTN_NONE;
    }
  }
}

/* ===================== MQTT ===================== */
void publishState() {
  char payload[128];
  snprintf(payload, sizeof(payload),
           "{\"id\":%d,\"r1\":%d,\"r2\":%d,\"r3\":%d,\"pir\":%d}",
           DEVICE_ID,
           relayState[0],
           relayState[1],
           relayState[2],
           pirTriggered);

  client.publish("home/relay/status", payload, true);
  pirTriggered = false;
}

/* ===================== SETUP ===================== */
void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirISR, RISING);

  // WiFi + MQTT init assumed already done
}

/* ===================== LOOP ===================== */
void loop() {
  if (!client.connected()) {
    // reconnect();
  }
  client.loop();

  unsigned long now = millis();

  /* ---- Buttons ---- */
  if (now - lastButtonScan >= BUTTON_SCAN_MS) {
    lastButtonScan = now;
    handleButtons();
  }

  /* ---- PIR Publish ---- */
  if (pirTriggered || (now - lastPublish >= 2000)) {
    lastPublish = now;
    publishState();
  }
}
