#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/* ===================== Network Configuration ===================== */
ESP8266WebServer server(80);

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway (192, 168, 4, 9);
IPAddress subnet  (255, 255, 255, 0);

/* ===================== EEPROM & Credentials ===================== */
String EEPROM_Data;
String SSID;
String PASSWORD;
int    splitIndex;

/* ===================== Device Configuration ===================== */
String DEVICE_ID = "1";
int mqtt_fallback = 1;

/* ===================== MQTT Configuration ===================== */
const char* mqtt_server   = "192.168.1.123";
const char* mqtt_username = "username";
const char* mqtt_password = "password";

const char* PUB_TOPIC = "PC-response";
const char* SUB_TOPIC = "PC-force";

WiFiClient espClient;
PubSubClient client(espClient);

/* ===================== Timing Variables ===================== */
unsigned long lastMsg      = 0;
unsigned long lastStatus   = 0;
unsigned long pressStart   = 0;
unsigned long releaseTime  = 0;
unsigned long restartTimer = 0;
unsigned long mqttTimer    = 0;

/* ===================== Control Flags ===================== */
bool wifiConfigured = false;
bool buttonPressed  = false;
int  shutdownTime   = -1;
int  msgCounter     = 0;

/* ===================== GPIO Pins ===================== */
#define POWER_PIN   5
#define POWER_IN    13
#define RESET_PIN   4

/* ===================== MQTT Buffer ===================== */
#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];

/* ===================== HTML Configuration Page ===================== */
const String postForms =
"<html>\
<head>\
<title>ESP8266 Configuration</title>\
<style>\
body { background-color: #cccccc; font-family: Arial; color: #000088; }\
</style>\
</head>\
<body>\
<h1>Enter WiFi Credentials</h1>\
<form method='post' action='/postform/'>\
<input type='text' name='SSID' value='SSID'><br>\
<input type='text' name='PASSWORD' value='PASSWORD'><br>\
<input type='submit' value='Submit'>\
</form>\
</body>\
</html>";

/* ===================== Web Handlers ===================== */
void handleRoot() {
  server.send(200, "text/html", postForms);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String ssid = server.arg(0);
  String pass = server.arg(1);

  EEPROM_Data = ssid + "^" + pass + "^";
  for (int i = 0; i < EEPROM_Data.length(); i++) {
    EEPROM.write(i, EEPROM_Data[i]);
  }
  EEPROM.commit();

  server.send(200, "text/plain", "Credentials Saved, Restarting...");
  WiFi.softAPdisconnect(true);
  ESP.restart();
}

/* ===================== WiFi Setup ===================== */
void setup_wifi() {
  SSID = "";
  PASSWORD = "";

  for (int i = 0; (char)EEPROM.read(i) != '^'; i++) {
    SSID += (char)EEPROM.read(i);
    splitIndex = i;
    if (i > 50) break;
  }

  for (int i = splitIndex + 2; (char)EEPROM.read(i) != '^'; i++) {
    PASSWORD += (char)EEPROM.read(i);
    if (i > 100) break;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttempt > 7000) {
      WiFi.softAPConfig(local_IP, gateway, subnet);
      WiFi.softAP("WiFi!PCCID:" + DEVICE_ID);
      wifiConfigured = true;
      return;
    }
    delay(1);
  }

  wifiConfigured = false;
}

/* ===================== MQTT Callback ===================== */
void callback(char* topic, byte* payload, unsigned int length) {

  if (payload[0] == 'c' && payload[1] == 'a') {
    shutdownTime = -1;
    client.publish(PUB_TOPIC, "Shutdown cancelled");
  }

  if (payload[0] == 'R' && payload[1] == 'e') {
    digitalWrite(RESET_PIN, LOW);
    pressStart = millis();
    releaseTime = 2000;
    buttonPressed = true;
  }

  if (payload[0] == 'S' && payload[1] == 'h') {
    digitalWrite(POWER_PIN, LOW);
    pressStart = millis();
    releaseTime = 2000;
    buttonPressed = true;
  }

  if (payload[0] == 'F' && payload[1] == 'o') {
    digitalWrite(POWER_PIN, LOW);
    pressStart = millis();
    releaseTime = 11000;
    buttonPressed = true;
  }

  if (payload[0] == 'i' && payload[1] == 'n') {
    shutdownTime =
      (payload[3] - '0') * 10000 +
      (payload[4] - '0') * 1000 +
      (payload[5] - '0') * 100 +
      (payload[6] - '0') * 10 +
      (payload[7] - '0');
  }
}

/* ===================== MQTT Reconnect ===================== */
void reconnect() {
  if (!client.connected()) {
    String clientId = "PCCID:" + DEVICE_ID;
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      client.subscribe(SUB_TOPIC);
      client.publish(PUB_TOPIC, "Slave Online");
    }
  }
}

/* ===================== HTTP Server ===================== */
void startWebServer() {
  MDNS.begin("esp8266");
  server.on("/", handleRoot);
  server.on("/postform/", handleForm);
  server.begin();
}

/* ===================== Setup ===================== */
void setup() {
  pinMode(POWER_IN, INPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(POWER_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);

  EEPROM.begin(512);
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (wifiConfigured) startWebServer();
  else reconnect();
}

/* ===================== Loop ===================== */
void loop() {
  server.handleClient();
  client.loop();

  if (!client.connected() && !wifiConfigured) reconnect();

  if (shutdownTime >= 0 && millis() - lastMsg > 1000) {
    lastMsg = millis();
    shutdownTime--;
    snprintf(msg, MSG_BUFFER_SIZE, "Shutdown in %d seconds", shutdownTime);
    client.publish(PUB_TOPIC, msg);

    if (shutdownTime == 0) {
      digitalWrite(POWER_PIN, LOW);
      pressStart = millis();
      releaseTime = 2000;
      buttonPressed = true;
    }
  }

  if (buttonPressed && millis() - pressStart > releaseTime) {
    digitalWrite(POWER_PIN, HIGH);
    digitalWrite(RESET_PIN, HIGH);
    buttonPressed = false;
  }
}
