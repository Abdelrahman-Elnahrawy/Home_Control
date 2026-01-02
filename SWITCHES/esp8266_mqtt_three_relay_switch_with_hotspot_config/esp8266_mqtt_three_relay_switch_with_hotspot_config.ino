/*
 * Project : ESP8266 MQTT 3-Relay Switch with Hotspot Configuration
 * Board   : ESP8266 (NodeMCU / ESP-12)
 * Features:
 *  - Control 3 relays via MQTT
 *  - Physical buttons with interrupts
 *  - EEPROM storage for WiFi & MQTT broker
 *  - Auto Hotspot mode if WiFi/MQTT fails
 */

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

/* -------------------- Modes -------------------- */
#define HOTSPOT_MODE                0
#define NORMAL_OPERATION_MODE       1

/* -------------------- Timing -------------------- */
#define TRIGGER_DELAY               700   // Debounce time (ms)

/* -------------------- GPIO Mapping -------------------- */
// Relays
uint8_t Switch_1 = 5;   // D1
uint8_t Switch_2 = 4;   // D2
uint8_t Switch_3 = 0;   // D3

// Buttons
uint8_t Button_1 = 12;  // D6
uint8_t Button_2 = 13;  // D7
uint8_t Button_3 = 14;  // D5

/* -------------------- States -------------------- */
bool R1, R2, R3;
bool CurrentMode;

/* -------------------- EEPROM -------------------- */
const int eepromSize = 200;
char storedSSID[32];
char storedPassword[32];
char storedBroker[32];

/* -------------------- WiFi -------------------- */
const char* ssid = "";
const char* password = "";

/* -------------------- MQTT -------------------- */
const char* mqttServer = "";
const int mqttPort = 1883;

const char* sub_Topic = "buttons/#";
const char* pub_Topic = "SWITCHS_pub";

/* -------------------- Globals -------------------- */
unsigned long lastMsg = 0;
unsigned long LostConnectionTimer = 0;
unsigned long last_Trigger_Delay = 0;
char msg[50];
int value = 0;

/* -------------------- Objects -------------------- */
ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/* =========================================================
 *                  Relay Update
 * ========================================================= */
void Update_Switchs() {
  digitalWrite(Switch_1, !R1);
  digitalWrite(Switch_2, !R2);
  digitalWrite(Switch_3, !R3);

  EEPROM.write(101, R1);
  EEPROM.write(102, R2);
  EEPROM.write(103, R3);
  EEPROM.commit();
}

/* =========================================================
 *                  MQTT Callback
 * ========================================================= */
void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Example payload: R1:1
  if (payload[1] == '1') R1 = payload[3] - '0';
  if (payload[1] == '2') R2 = payload[3] - '0';
  if (payload[1] == '3') R3 = payload[3] - '0';

  Update_Switchs();
}

/* =========================================================
 *                  MQTT Connection
 * ========================================================= */
void connectToMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(handleMQTTMessage);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266_MQTT_SWITCH")) {
      mqttClient.subscribe(sub_Topic);
      mqttClient.publish(pub_Topic, "Node Connected");
    } else {
      delay(1000);
    }
  }
}

/* =========================================================
 *                  WiFi Connection
 * ========================================================= */
void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - startTime > 7000) {
      startAccessPoint("Switches Credentials !");
      return;
    }
  }
}

/* =========================================================
 *                  Hotspot Mode
 * ========================================================= */
void startAccessPoint(const char* AP_NAME) {
  CurrentMode = HOTSPOT_MODE;
  LostConnectionTimer = millis();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME);

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
}

/* =========================================================
 *                  Web Pages
 * ========================================================= */
void handleRoot() {
  server.send(200, "text/html", "<h1>ESP8266 MQTT Switch Config</h1>");
}

void handleSave() {
  String s = server.arg("wifi-ssid");
  String p = server.arg("wifi-password");
  String b = server.arg("mqtt-broker");

  for (int i = 0; i < s.length(); i++) EEPROM.write(i, s[i]);
  EEPROM.write(s.length(), '\0');

  for (int i = 0; i < p.length(); i++) EEPROM.write(i + 32, p[i]);
  EEPROM.write(p.length() + 32, '\0');

  for (int i = 0; i < b.length(); i++) EEPROM.write(i + 64, b[i]);
  EEPROM.write(b.length() + 64, '\0');

  EEPROM.commit();
  server.send(200, "text/html", "Saved. Restarting...");
  delay(1000);
  ESP.restart();
}

/* =========================================================
 *                  Interrupts
 * ========================================================= */
IRAM_ATTR void IntCallback_R1() {
  if (millis() - last_Trigger_Delay > TRIGGER_DELAY) {
    R1 = !R1;
    last_Trigger_Delay = millis();
    Update_Switchs();
  }
}

IRAM_ATTR void IntCallback_R2() {
  if (millis() - last_Trigger_Delay > TRIGGER_DELAY) {
    R2 = !R2;
    last_Trigger_Delay = millis();
    Update_Switchs();
  }
}

IRAM_ATTR void IntCallback_R3() {
  if (millis() - last_Trigger_Delay > TRIGGER_DELAY) {
    R3 = !R3;
    last_Trigger_Delay = millis();
    Update_Switchs();
  }
}

/* =========================================================
 *                  Setup & Loop
 * ========================================================= */
void setup() {
  Serial.begin(115200);
  EEPROM.begin(eepromSize);

  pinMode(Switch_1, OUTPUT);
  pinMode(Switch_2, OUTPUT);
  pinMode(Switch_3, OUTPUT);

  pinMode(Button_1, INPUT_PULLUP);
  pinMode(Button_2, INPUT_PULLUP);
  pinMode(Button_3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Button_1), IntCallback_R1, RISING);
  attachInterrupt(digitalPinToInterrupt(Button_2), IntCallback_R2, RISING);
  attachInterrupt(digitalPinToInterrupt(Button_3), IntCallback_R3, RISING);

  connectToWiFi();
  if (WiFi.status() == WL_CONNECTED) connectToMQTT();
}

void loop() {
  if (CurrentMode == HOTSPOT_MODE) {
    server.handleClient();
    return;
  }

  if (!mqttClient.connected()) connectToMQTT();
  mqttClient.loop();
}
