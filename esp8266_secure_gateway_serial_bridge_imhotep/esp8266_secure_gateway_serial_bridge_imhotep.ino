/**
 * PROJECT: Imhotep Secure IoT Gateway
 * AUTHOR: Abdelrahman Elnahrawy
 * DESCRIPTION: Secure MQTT Bridge (TLS 8883) with Serial-to-MQTT translation,
 * EEPROM state persistence, and Captive Portal provisioning.
 */

#include <ESP8266WiFi.h>
#include <time.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// --- Configuration & Identity ---
ESP8266WebServer server(80);
const unsigned int MAX_MESSAGE_LENGTH = 100;
IPAddress local_IP(192, 168, 1, 124);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

String ID = "TsesT";
const char* Pub = "ESP_Pub_TsesT";
const char* Sub = "ESP_Sub_TsesT";
const char* mqtt_server = "NA7RAWY_PC.local";
const char* mqtt_username = "home";
const char* mqtt_password = "password_here"; // Placeholder for privacy

// --- Pin Mapping ---
int wifierror = 2;   // GPIO2 (D4) - Blue LED
int mqtterror = 5;   // GPIO5 (D1) - orange LED
int htspt     = 4;   // GPIO4 (D2) - green LED
int HS        = 13;  // GPIO13 (D7) - Hardware Mode Switch

// --- Global State Variables ---
bool R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11;
int PWMD0, PWMD1, PWMD2, PWMA0, PWMA1, PWMA2;
int LM = 0, LD = 10000, LDR = 0, T = 0, MSG_NO = 0;
unsigned long lastMsg = 0;
bool chk = 0; // 0 = AP Mode, 1 = Station Mode

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE (160)
char msg[MSG_BUFFER_SIZE];

// --- HTML UI Assets ---
String postForms = "..."; // (Your existing HTML/CSS string here)

// SSL Fingerprint (Update this periodically)
static const char *fingerprint PROGMEM = "D1 A3 E7 C1 EB FE 61 DC EA EE D5 A6 80 A1 4C DD 62 F0 FF BD";

// ================================================================
// SYSTEM UTILITIES
// ================================================================

void hotspot() {
  yield();
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("WiFi!RCID: " + ID);
}

/**
 * Loads WiFi Credentials from EEPROM (Addresses 0-100)
 */
void setup_wifi() {
  yield();
  String USR = "", PASS = "";
  int f = 0;

  for (int n = 0; (char)EEPROM.read(n) != '^'; n++) {
    USR += (char)EEPROM.read(n);
    f = n;
    if (n > 50) break;
  }
  for (int n = f + 2; (char)EEPROM.read(n) != '^'; n++) {
    PASS += (char)EEPROM.read(n);
    if (n > 100) break;
  }
  WiFi.begin(USR, PASS);
  WiFi.mode(WIFI_STA);
}

// ================================================================
// MQTT & NETWORK HANDLERS
// ================================================================

void connectMqTT() {
  if (!client.connected()) {
    String clientId = "RC" + ID;
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      client.publish(Pub, "Imhotep Secure Gateway Online");
      client.subscribe(Sub);
    }
  }
}

/**
 * Parses incoming MQTT payloads and updates Hardware/EEPROM
 */
void callback(char* topic, byte* payload, unsigned int length) {
  // EEPROM Remote Update Trigger
  if ((char)payload[0] == '~') {
    for (int n = 1; payload[n] != '~'; n++) {
      EEPROM.write(n - 1, (char)payload[n]);
    }
    EEPROM.commit();
    ESP.restart();
  }

  // Relay & PWM Logic (Condensed for readability)
  // ... (Your existing if/else logic for R0-R11 and PWMA/D)
  
  EEPROM.commit();
  yield();
}

// ================================================================
// WEB SERVER HANDLERS
// ================================================================

void handleForm() {
  String USR = server.arg(0);
  String PASS = server.arg(1);
  String storage = USR + "^" + PASS + "^";

  for (int n = 0; n < storage.length(); n++) {
    EEPROM.write(n, storage[n]);
  }
  EEPROM.commit();
  server.send(200, "text/plain", "Credentials Saved. Rebooting...");
  delay(1000);
  ESP.restart();
}

// ================================================================
// MAIN SETUP
// ================================================================

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  
  pinMode(HS, INPUT);
  pinMode(wifierror, OUTPUT);
  pinMode(mqtterror, OUTPUT);
  pinMode(htspt, OUTPUT);

  // Load States from EEPROM
  R0 = EEPROM.read(100); R1 = EEPROM.read(101); // ... etc
  LM = EEPROM.read(118);
  
  // Hardware Mode Selection via GPIO13
  if (digitalRead(HS) == HIGH) {
    chk = true;
    setup_wifi();
    espClient.setFingerprint(fingerprint);
    client.setServer(mqtt_server, 8883);
    client.setCallback(callback);
    digitalWrite(htspt, LOW);
  } else {
    chk = false;
    hotspot();
    MDNS.begin("esp8266");
    server.on("/", []() { server.send(200, "text/html", postForms); });
    server.on("/postform/", handleForm);
    server.begin();
    digitalWrite(htspt, HIGH);
  }
}

// ================================================================
// MAIN LOOP
// ================================================================

void loop() {
  // 1. MQTT Keep-alive & Maintenance
  if (chk) {
    if (!client.connected()) {
      digitalWrite(mqtterror, HIGH);
      connectMqTT();
    } else {
      digitalWrite(mqtterror, LOW);
      client.loop();
    }
  }

  // 2. Serial Bridge Logic (UART to MQTT)
  while (Serial.available() > 0) {
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    char inByte = Serial.read();

    if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
      message[message_pos++] = inByte;
    } else {
      message[message_pos] = '\0';
      // Process Serial Command...
      client.publish(Pub, (const uint8_t*)message, message_pos, true);
      message_pos = 0;
    }
  }

  // 3. Status Reporting & Watchdogs
  if (millis() - lastMsg > 2000) {
    lastMsg = millis();
    if (client.connected()) {
      client.publish(Pub, "Heartbeat: Active");
    }
  }

  // 4. Mode Switching & UI
  if (chk != digitalRead(HS)) ESP.restart();
  if (!chk) server.handleClient();
  
  digitalWrite(wifierror, (WiFi.status() == WL_CONNECTED) ? LOW : HIGH);
  MDNS.update();
}