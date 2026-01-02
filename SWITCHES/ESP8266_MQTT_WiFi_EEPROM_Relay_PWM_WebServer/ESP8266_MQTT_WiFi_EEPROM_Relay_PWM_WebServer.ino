#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Web server setup
ESP8266WebServer server(80);

// Static IP configuration for hotspot mode
IPAddress local_IP(192,168,4,22);
IPAddress gateway  (192,168,4,9);
IPAddress subnet   (255,255,255,0);

// Variables for WiFi credentials and device ID
String eeprom;
String ID ="2";
const char* Pub = "ESP_Pub_2";
const char* Sub = "ESP_Sub_2";
String USR;
String PASS;
int f;
int mq = 1;

// MQTT setup
const char* mqtt_server = "192.168.1.123";
const char* mqtt_username = "username";
const char* mqtt_password = "password";
WiFiClient espClient;
PubSubClient client(espClient);

// General variables
int MSG_NO = 0;
int T = 0;
int LM = 0;
int S = 0;
int T1 = 0;
int LD = 10000;

// Timing variables
unsigned long temo       = 0;
unsigned long lastMsg    = 0;
unsigned long button_time= 0;
unsigned long temp_time  = 0;
unsigned long mqtt_time  = 0;
unsigned long alrm       = 0;
unsigned long TtT        = 0;

// Relay and PWM states
bool sun    = 0;
bool R0     = 0;  bool R1     = 0;  bool R2     = 0;
bool R3     = 0;  bool R4     = 0;  bool R5     = 0;
bool R6     = 0;  bool R7     = 0;  bool R8     = 0;
bool R9     = 0;  bool R10    = 0;  bool R11    = 0;

int PWM0 = 0; int PWM1 = 0; int PWM2 = 0;
int PWM3 = 0; int PWM4 = 0; int PWM5 = 0;

// EEPROM & WiFi flags
int pn = 0; // password start byte in EEPROM
bool chk = 0; // 1 = hotspot mode, 0 = WiFi mode

#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];

// HTML form for WiFi credentials
const String postForms = "<html>\
<head>\
<title>ESP8266 Web Server POST handling</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>Enter the WiFi Username and Password</h1><br>\
<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
<input type=\"text\" name=\"SSID\" value=\"SSID\"><br>\
<input type=\"text\" name=\"PASSWORD\" value=\"PASSWORD\"><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

// Handle root page
void handleRoot() {
  yield();
  server.send(200, "text/html", postForms);
}

// Setup WiFi with EEPROM or fallback to hotspot
void setup_wifi() {
  yield();
  USR = "";
  PASS = "";

  // Read WiFi credentials from EEPROM
  for(int n=0; (char)EEPROM.read(n)!='^'; n++){
    USR += (char)EEPROM.read(n);
    f = n;
    yield();
    if (n > 50) break;
  }
  for(int n=f+2; (char)EEPROM.read(n)!='^'; n++){
    PASS += (char)EEPROM.read(n);
    yield();
    if (n > 100) break;
  }

  WiFi.begin(USR, PASS);
  WiFi.mode(WIFI_STA);
  chk = true;

  // Attempt to connect WiFi
  for(unsigned long temp_time = millis(); WiFi.status() != WL_CONNECTED; yield()) {
    chk = false;
    if (millis() - temp_time > 7000) {
      // Failed: configure hotspot
      WiFi.softAPConfig(local_IP, gateway, subnet);
      WiFi.softAP("MQTT!RCID: "+ID);
      chk = true;
      break;
    }
  }
}

// Handle WiFi POST form
void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String USR = "";
    String PASS = "";
    USR += server.arg(0);
    PASS += server.arg(1);

    eeprom = USR + "^" + PASS + "^";
    for(int n=0; n < eeprom.length(); n++) {
      yield();
      EEPROM.write(n, eeprom[n]);
    }
    EEPROM.commit();
    server.send(200,"text/plain", "On the way");
    WiFi.softAPdisconnect(true);
    ESP.restart();
  }
  yield();
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  yield();
  // ... (code to process MQTT messages for relays and PWM as before) ...
}

// Connect to MQTT server
void connectMqTT() {
  if (!client.connected()) {
    String clientId = "RC"+ID;
    if (client.connect((char*) clientId.c_str())) {
      yield();
      client.publish(Pub, "slave is on");
      client.subscribe(Sub);
      yield();
    } else {
      yield();
    }
  }
}

// Start HTTP server for form
void http_forum_load() {
  if (MDNS.begin("esp8266")) yield();
  server.on("/", handleRoot);
  server.on("/postform/", handleForm);
  server.begin();
}

// Setup function
void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);

  // Load relays & settings from EEPROM
  R1 = EEPROM.read(101); R2 = EEPROM.read(102); R3 = EEPROM.read(103);
  R4 = EEPROM.read(104); R5 = EEPROM.read(105); R6 = EEPROM.read(106);
  LD = (EEPROM.read(107)*1000) + (EEPROM.read(108)*100) + (EEPROM.read(109)*10) + EEPROM.read(110);
  LM = EEPROM.read(111);
  sun = EEPROM.read(112);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (chk == true) {
    http_forum_load();
  } else {
    connectMqTT();
  }
}

// Main loop
void loop() {
  yield();
  server.handleClient();

  if(!client.connected() && chk == false) {
    if (millis() - lastMsg < 20000) connectMqTT();
    else if(mq==1) {
      WiFi.softAPConfig(local_IP, gateway, subnet);
      WiFi.softAP("MQTT!RCID: "+ID);
      mq = 2;
      mqtt_time = millis();
      http_forum_load();
    }
  }

  client.loop();

  // Restart ESP if WiFi or MQTT disconnected for too long
  if ((WiFi.status() != WL_CONNECTED || !client.connected()) && temo == 0) temo = millis();
  if (client.connected() && temo > 0) temo = 0;
  if (millis() - temo > 300000) ESP.restart();

  // Publish MQTT status periodically
  if (chk == false && client.connected() && millis() - lastMsg > 2000) {
    lastMsg = millis();
    ++MSG_NO;
    snprintf(msg, MSG_BUFFER_SIZE, "R1:%d R2:%d R3:%d R4:%d R5:%d R6:%03d T:%03d D:%04d LM:%d  #%ld ", R1,R2,R3,R4,R5,R6,T,LD,LM,MSG_NO);
    client.publish(Pub, msg);
  }
}
