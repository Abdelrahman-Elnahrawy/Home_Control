/*
 * Project: Advanced ESP8266 Smart Home Controller
 * Features: 6-Relay Control, MQTT, WiFi Captive Portal, Motion Sensing, EEPROM Persistence
 * Author: Abdelrahman Elnahrawy
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// --- Pin Definitions ---
const int relayPins[] = {4, 5, 0, 2, 15, 16}; // Pins for Relay 1 to 6
const int motionSensor = 3;
const int buttonPins[] = {14, 12, 13}; // KP_1, KP_2, KP_3

// --- Logic Variables ---
int logicMode = 0; // 0: Manual, 1: Sensor Auto, 2: Timer Auto
unsigned long lightDelay = 10000;
bool relayStates[6];

// --- Web & MQTT Config ---
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server = "192.168.1.123";

/**
 * setup_wifi: Attempts to connect using EEPROM credentials.
 * If failure, launches Access Point mode (Captive Portal).
 */
void setup_wifi() {
    String storedSSID = "";
    String storedPASS = "";
    
    // Read SSID until delimiter '^'
    for (int i = 0; i < 50; i++) {
        char c = EEPROM.read(i);
        if (c == '^') break;
        storedSSID += c;
    }
    // Logic to connect...
    WiFi.begin(storedSSID.c_str(), "storedPassHere");
    
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.softAP("SmartHub_Setup");
        // Start Web Server for user input
    }
}

/**
 * callback: Processes incoming MQTT commands.
 * Example payload: "R1:1" to turn on relay 1.
 */
void callback(char* topic, byte* payload, unsigned int length) {
    // Parsing logic for Relay states and Logic Modes
    if (payload[0] == 'R' && payload[1] == '1') {
        bool state = (payload[3] == '1');
        digitalWrite(relayPins[0], state);
        EEPROM.write(101, state);
        EEPROM.commit();
    }
}
/**
 * reconnect: Attempts to reconnect to the MQTT broker 
 * until a connection is established.
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, resubscribe to topics
      client.subscribe("inTopic"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {
    Serial.begin(115200);
    EEPROM.begin(512);
    
    // Initialize Pins
    for(int i=0; i<6; i++) {
        pinMode(relayPins[i], OUTPUT);
        relayStates[i] = EEPROM.read(101 + i);
        digitalWrite(relayPins[i], relayStates[i]);
    }
    
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    server.handleClient();
    if (!client.connected()) reconnect();
    client.loop();

    // Logic for Motion Sensor interaction
    if (logicMode == 2) { // Auto-Timer Mode
        if (digitalRead(motionSensor) == HIGH) {
            digitalWrite(relayPins[0], HIGH); // Turn on lights
            // Reset timer...
        }
    }
}