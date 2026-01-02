#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>

// -------------------- CONFIG --------------------
#define EEPROM_SIZE       96
#define WIFI_CRED_FLAG    0x42
#define ANALOG_PIN        A0
#define TRIAC_PIN         D3
#define ZERO_CROSS_PIN    D8
#define RELAY1_PIN        D5
#define RELAY2_PIN        D6
#define BUTTON1_PIN       D7
#define BUTTON2_PIN       D4

#define MQTT_BRIGHT_TOPIC "smartdimmer/set"
#define MQTT_STATE_TOPIC  "smartdimmer/state"
#define MQTT_RETAIN       true

#define DEBOUNCE_MS       50
#define POT_THRESHOLD     100

// -------------------- GLOBALS --------------------
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

volatile bool zeroCrossDetected = false;
volatile int lastCrossState = 0;
unsigned long zeroCrossCount = 0;
unsigned long lastReport = 0;

int brightness = 0;           // 0-100%
int lastBrightness = 0;
int lastPotValue = 0;
bool mqttOverride = false;

unsigned long lastButtonTime[2] = {0, 0};
bool lastButtonState[2] = {HIGH, HIGH};

String SSID = "";
String PASS = "";

// -------------------- EEPROM --------------------
void saveWiFiCredentials(const String &ssid, const String &pass) {
  EEPROM.write(0, WIFI_CRED_FLAG);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(1 + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(33 + i, i < pass.length() ? pass[i] : 0);
  }
  EEPROM.commit();
}

bool loadWiFiCredentials(String &ssid, String &pass) {
  if (EEPROM.read(0) != WIFI_CRED_FLAG) return false;
  char buffer[33];
  for (int i = 0; i < 32; i++) buffer[i] = EEPROM.read(1 + i);
  buffer[32] = 0;
  ssid = String(buffer);
  for (int i = 0; i < 32; i++) buffer[i] = EEPROM.read(33 + i);
  buffer[32] = 0;
  pass = String(buffer);
  return true;
}

// -------------------- WIFI + MQTT --------------------
void connectWiFi() {
  if (!loadWiFiCredentials(SSID, PASS)) {
    WiFi.softAP("SmartDimmer_Config");
    server.begin();
    Serial.println("SoftAP started. Connect and configure WiFi.");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID.c_str(), PASS.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.softAP("SmartDimmer_Config");
    server.begin();
    Serial.println("Failed to connect to WiFi, SoftAP started.");
  } else {
    Serial.println("Connected! IP: " + WiFi.localIP().toString());
  }
}

void reconnectMQTT() {
  if (!client.connected()) {
    while (!client.connected()) {
      if (client.connect("ESP8266_SmartDimmer")) {
        client.subscribe(MQTT_BRIGHT_TOPIC);
        client.subscribe(MQTT_STATE_TOPIC);
        Serial.println("MQTT connected.");
      } else {
        Serial.print("MQTT failed, rc=");
        Serial.print(client.state());
        Serial.println(" retry in 5s");
        delay(5000);
      }
    }
  }
}

// -------------------- WEB PORTAL --------------------
const char* webPage = R"rawliteral(
<html>
<head>
<title>ESP8266 WiFi Config</title>
<style>
body { font-family: Arial; text-align:center; margin-top:50px; }
input { margin:5px; padding:5px; width:200px;}
button { padding:5px 15px; }
</style>
</head>
<body>
<h2>Configure WiFi</h2>
<form method="POST" action="/save">
SSID:<br><input type="text" name="SSID"><br>
Password:<br><input type="text" name="PASS"><br>
<button type="submit">Save</button>
</form>
</body>
</html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", webPage); }

void handleSave() {
  SSID = server.arg("SSID");
  PASS = server.arg("PASS");
  saveWiFiCredentials(SSID, PASS);
  server.send(200, "text/html", "<h2>Saved! Rebooting...</h2>");
  delay(1000);
  ESP.restart();
}

// -------------------- ZERO CROSS --------------------
void IRAM_ATTR zeroCrossISR() {
  bool state = digitalRead(ZERO_CROSS_PIN);
  if (state != lastCrossState) {
    zeroCrossDetected = true;
    zeroCrossCount++;
  }
  lastCrossState = state;
}

// -------------------- MQTT --------------------
void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
  String val = "";
  for (unsigned int i=0;i<length;i++) val += (char)payload[i];

  if (String(topic) == MQTT_BRIGHT_TOPIC) {
    int b = val.toInt();
    if(b>=0 && b<=100) {
      brightness = b;
      mqttOverride = true;
      Serial.println("MQTT Override Brightness: " + String(brightness));
    }
  }

  if (String(topic) == MQTT_STATE_TOPIC && val == "get") {
    char msg[5];
    snprintf(msg,sizeof(msg), "%d", brightness);
    client.publish(MQTT_STATE_TOPIC, msg, MQTT_RETAIN);
  }
}

// -------------------- BUTTONS --------------------
void checkButton(uint8_t idx, uint8_t pin, uint8_t relay) {
  bool state = digitalRead(pin);
  unsigned long now = millis();
  if (state != lastButtonState[idx] && now - lastButtonTime[idx] > DEBOUNCE_MS) {
    lastButtonState[idx] = state;
    lastButtonTime[idx] = now;
    if (state == LOW) {
      digitalWrite(relay, !digitalRead(relay));
      Serial.printf("Relay %d toggled\n", relay);
    }
  }
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  pinMode(TRIAC_PIN, OUTPUT);
  pinMode(ZERO_CROSS_PIN, INPUT_PULLUP);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCrossISR, CHANGE);

  connectWiFi();

  client.setServer("192.168.1.50", 1883); // MQTT broker
  client.setCallback(handleMQTTMessage);
  reconnectMQTT();

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
  Serial.println("Web server started.");
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();

  if(WiFi.status() == WL_CONNECTED){
    if(!client.connected()) reconnectMQTT();
    client.loop();
  }

  // Potentiometer brightness
  int pot = analogRead(ANALOG_PIN);
  if(!mqttOverride) {
    if(abs(pot - lastPotValue) > POT_THRESHOLD) {
      brightness = map(pot,0,1023,0,100);
      lastBrightness = brightness;
      lastPotValue = pot;
    }
  } else {
    if(abs(pot - lastPotValue) > POT_THRESHOLD) {
      brightness = map(pot,0,1023,0,100);
      lastBrightness = brightness;
      lastPotValue = pot;
      mqttOverride = false;
    }
  }

  // Check buttons
  checkButton(0, BUTTON1_PIN, RELAY1_PIN);
  checkButton(1, BUTTON2_PIN, RELAY2_PIN);

  // TRIAC trigger
  if(zeroCrossDetected){
    int delayTime = map(brightness,0,100,8000,0);
    delayMicroseconds(delayTime);
    digitalWrite(TRIAC_PIN,HIGH);
    delayMicroseconds(50);
    digitalWrite(TRIAC_PIN,LOW);
    zeroCrossDetected = false;
  }

  // Publish zero-cross count every 500ms
  if(millis() - lastReport > 500){
    lastReport = millis();
    if(client.connected()){
      char msg[10];
      snprintf(msg,sizeof(msg), "%lu", zeroCrossCount);
      client.publish("smartdimmer/freq", msg, MQTT_RETAIN);
    }
    zeroCrossCount = 0;
  }
}
