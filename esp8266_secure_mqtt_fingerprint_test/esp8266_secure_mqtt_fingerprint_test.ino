/*
 * Project: ESP8266 Secure MQTT with TLS Fingerprint
 * Author: Abdelrahman Elnahrawy
 * Description: Connects to a secure MQTT broker using SSL/TLS. 
 * It verifies the server's identity via a SHA1 Fingerprint and toggles 
 * the onboard LED based on received MQTT messages.
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- Network Configuration ---
const char* ssid = "Na7rawy Limited";
const char* password = "Selim&Nahrawy751";
const char* mqtt_server = "NA7RAWY_PC.local"; // Local mDNS or IP address

// --- Security Configuration ---
// SHA1 Fingerprint of the MQTT Broker's SSL Certificate
static const char *fingerprint PROGMEM = "D1 A3 E7 C1 EB FE 61 DC EA EE D5 A6 80 A1 4C DD 62 F0 FF BD";

// --- Global Objects ---
WiFiClientSecure espClient;
PubSubClient client(espClient);

/**
 * Handles incoming WiFi connections.
 */
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * MQTT Callback: Triggered when a message arrives on a subscribed topic.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // If '1' is received, turn LED ON (Active Low)
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW); 
    client.publish("gBridge/u4537/d14818/onoff/set", "1");
  } else {
    digitalWrite(BUILTIN_LED, HIGH); 
    client.publish("gBridge/u4537/d14818/onoff/set", "0");
  }
}

/**
 * Reconnect logic for secure MQTT.
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting Secure MQTT connection...");
    
    // Generate a unique Client ID
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    // Attempt connection with credentials
    if (client.connect(clientId.c_str(), "gbridge-uaername", "gbridge-mqtt_password")) {
      Serial.println("connected");
      client.subscribe("gBridge/u4537/d14818/onoff");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); // Start with LED OFF
  
  Serial.begin(115200);
  
  setup_wifi();

  // Set the SHA1 fingerprint for TLS verification
  espClient.setFingerprint(fingerprint);
  
  // Port 8883 is the standard port for MQTT over TLS
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}