/*
 * Project: Professional ESP8266 IoT Home Controller
 * Author: Abdelrahman Elnahrawy
 * Features: 
 * - Multi-Mode Logic (Manual, Motion-Switch, Motion-Timer)
 * - Automatic EEPROM state recovery for all relays
 * - Fail-safe auto-restart on connection loss
 * - MQTT Alarm publishing for security
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// --- System Configuration ---
String ID = "1";
const char* mqtt_server = "192.168.1.123";
const char* Pub = "ESP_Pub_1";
const char* Sub = "ESP_Sub_1";

// --- Pin Mapping ---
int relayPins[] = {4, 5, 0, 2, 15, 16}; // R1-R6
int PIR_PIN     = 3;  // Motion Sensor
int KEY_PINS[]  = {14, 12, 13}; 

// --- Variables ---
bool R1, R2, R3, R4, R5;
int R6, T, LM;
unsigned long T1, LD, lastMsg, temo = 0;
bool chk = false;

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);

void setup() {
  EEPROM.begin(512);
  
  // Restore States from EEPROM
  R1 = EEPROM.read(101);
  R2 = EEPROM.read(102);
  R3 = EEPROM.read(103);
  R4 = EEPROM.read(104);
  R5 = EEPROM.read(105);
  R6 = EEPROM.read(106);
  LM = EEPROM.read(111);
  
  // Initialize Pins
  for(int i=0; i<5; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], (i==0 ? R1 : (i==1 ? R2 : (i==2 ? R3 : (i==3 ? R4 : R5)))));
  }
  pinMode(relayPins[5], OUTPUT);
  analogWrite(relayPins[5], R6);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  if(chk) http_forum_load();
  else connectMqTT();
}

void loop() {
  yield();
  server.handleClient();
  
  // Fail-safe Connection Watchdog
  if (!client.connected() && !chk) {
    if (temo == 0) temo = millis();
    if (millis() - temo > 300000) ESP.restart(); // Restart if offline for 5 mins
    connectMqTT();
  } else {
    temo = 0;
  }

  client.loop();
  handleLogicModes();
}

void handleLogicModes() {
  bool motion = digitalRead(PIR_PIN) == LOW; // Adjusted for your sensor logic
  
  if (LM == 1) { // Mode 1: Motion Switch
    if(motion) digitalWrite(relayPins[0], HIGH);
  } 
  else if (LM == 2) { // Mode 2: Motion Timer
    if (motion) {
      T1 = millis();
      R1 = HIGH;
    }
    if (millis() - T1 > LD * 1000) R1 = LOW;
    digitalWrite(relayPins[0], R1);
  }
}