/*
 * Project: Multi-Gas & Temperature Monitor (ESP8266 Version)
 * Hardware: NodeMCU/Wemos D1 Mini + ADS1115 (Required for 4 Analog Inputs)
 * Author: Abdelrahman Elnahrawy (Fixed by Gemini)
 */

#include <ESP8266WiFi.h>       // مكتبة الواي فاي الخاصة بـ ESP8266
#include <ESP8266WebServer.h>  // مكتبة السيرفر الخاصة بـ ESP8266
#include <EEPROM.h>
#include <PubSubClient.h>
#include <SimpleKalmanFilter.h>
#include <Adafruit_ADS1X15.h>  
#include <Wire.h>

// --- Definitions ---
#define HOTSPOT_MODE 0
#define NORMAL_OPERATION_MODE 1
#define EEPROM_SIZE 200

// --- Objects ---
ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Adafruit_ADS1115 ads; // إنشاء كائن للتعامل مع قطعة ADS1115

// --- Globals ---
bool CurrentMode;
char storedSSID[32];
char storedPassword[32];
char storedBroker[32];

// WiFi & MQTT Variables
const char* ssid = "";
const char* password = "";
const char* mqttServer = "";
const int mqttPort = 1883;

const char* sub_Topic = "environment/control";
const char* pub_Topic = "environment/data";

unsigned long lastSensorRead = 0;
const long interval = 5000; // قراءة كل 5 ثواني (بدون delay)

// --- Kalman Filters ---
// SimpleKalmanFilter(e_mea, e_est, q)
SimpleKalmanFilter mq2Kalman(2, 2, 0.01);
SimpleKalmanFilter mq5Kalman(2, 2, 0.01);
SimpleKalmanFilter mq9Kalman(2, 2, 0.01);
SimpleKalmanFilter lm35Kalman(2, 2, 0.01);

// --- HTML Page (Raw String) ---
const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML><html><head>
<meta charset="UTF-8"><title>Config</title>
<style>
body{display:flex;justify-content:center;align-items:center;height:100vh;background:#f2f2f2;font-family:Arial,sans-serif;}
.container{width:300px;padding:20px;background:#fff;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,0.2);}
input{width:90%;padding:10px;margin:5px 0;border:1px solid #ccc;border-radius:4px;}
button{width:100%;padding:10px;background:#4CAF50;color:#fff;border:none;border-radius:4px;cursor:pointer;}
button:hover{background:#45a049;}
h2{text-align:center;}
</style></head><body>
<div class="container">
<h2>Setup ESP8266</h2>
<form action="/save" method="POST">
<label>WiFi SSID:</label><input type="text" name="ssid">
<label>Password:</label><input type="password" name="pass">
<label>MQTT IP:</label><input type="text" name="broker">
<button type="submit">Save & Restart</button>
</form></div></body></html>
)rawliteral";

// --- Function Prototypes ---
void handleRoot();
void handleSave();
void startAccessPoint();
void connectToWiFi();
void reconnectMQTT();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  
  // تشغيل الـ ADS1115
  // SCL -> D1, SDA -> D2
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115! Check wiring.");
    while (1); // توقف هنا إذا لم يتم توصيل القطعة
  }

  // قراءة البيانات المحفوظة
  for (int i = 0; i < 32; ++i) {
    storedSSID[i] = EEPROM.read(i);
    storedPassword[i] = EEPROM.read(i + 32);
    storedBroker[i] = EEPROM.read(i + 64);
  }
  storedSSID[31] = '\0';
  storedPassword[31] = '\0';
  storedBroker[31] = '\0';

  // تحديد المود
  if (strlen(storedSSID) > 1) {
    CurrentMode = NORMAL_OPERATION_MODE;
    ssid = storedSSID;
    password = storedPassword;
    mqttServer = storedBroker;
    
    connectToWiFi();
    
    if(WiFi.status() == WL_CONNECTED){
      mqttClient.setServer(mqttServer, mqttPort);
      mqttClient.setCallback([](char* topic, byte* payload, unsigned int length) {
        Serial.print("Msg: "); Serial.println(topic);
      });
    }
  } else {
    startAccessPoint();
  }
}

void loop() {
  // --- Mode 1: Access Point (Configuration) ---
  if (CurrentMode == HOTSPOT_MODE) {
    server.handleClient();
  } 
  // --- Mode 2: Normal Operation ---
  else {
    // 1. MQTT Connection Handling
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();

    // 2. Sensor Reading (Non-blocking)
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorRead >= interval) {
      lastSensorRead = currentMillis;
      
      // قراءة القيم من ADS1115 (Channels 0, 1, 2, 3)
      int16_t adc0 = ads.readADC_SingleEnded(0); // MQ-2
      int16_t adc1 = ads.readADC_SingleEnded(1); // MQ-5
      int16_t adc2 = ads.readADC_SingleEnded(2); // MQ-9
      int16_t adc3 = ads.readADC_SingleEnded(3); // LM35

      // Kalman Filter
      float fMQ2 = mq2Kalman.updateEstimate((float)adc0);
      float fMQ5 = mq5Kalman.updateEstimate((float)adc1);
      float fMQ9 = mq9Kalman.updateEstimate((float)adc2);
      float fTemp = lm35Kalman.updateEstimate((float)adc3);

      // تحويل القراءات إلى فولت (ADS1115 default gain is +/- 6.144V -> 1 bit = 0.1875mV)
      // Multiplier = 0.0001875F
      float vMQ2 = fMQ2 * 0.0001875;
      float vMQ5 = fMQ5 * 0.0001875;
      float vMQ9 = fMQ9 * 0.0001875;
      float vTemp = fTemp * 0.0001875;

      // --- معادلات الغاز التقريبية ---
      float lpgPPM = 2.5 * vMQ2 * 1000; // ضربنا في 1000 لأن الفولت صغير
      float naturalGasPPM = 3.0 * vMQ5 * 1000;
      float coPPM = 1.44 * (3.3 - vMQ9) * 100; 
      float tempC = vTemp * 100.0; // LM35: 10mV per degree C -> V * 100

      // Debug Print
      Serial.printf("T:%.1fC | LPG:%.0f | NG:%.0f | CO:%.0f\n", tempC, lpgPPM, naturalGasPPM, coPPM);

      // Publish to MQTT
      if (mqttClient.connected()) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), 
          "{\"temp\":%.2f, \"lpg\":%.2f, \"ng\":%.2f, \"co\":%.2f}", 
          tempC, lpgPPM, naturalGasPPM, coPPM);
        mqttClient.publish(pub_Topic, buffer);
      }
    }
  }
}

// --- Helper Functions ---

void startAccessPoint() {
  CurrentMode = HOTSPOT_MODE;
  Serial.println("Starting AP Mode...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266_Gas_Setup", "12345678");
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
}

void connectToWiFi() {
  Serial.print("Connecting to: "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500); Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Failed -> AP Mode");
    startAccessPoint();
  }
}

void reconnectMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (mqttClient.connect("ESP8266_GasNode")) {
      Serial.println("MQTT Connected");
      mqttClient.publish(pub_Topic, "Device Online");
      mqttClient.subscribe(sub_Topic);
    }
  }
}

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSave() {
  String n_ssid = server.arg("ssid");
  String n_pass = server.arg("pass");
  String n_broker = server.arg("broker");

  if(n_ssid.length() > 0 && n_broker.length() > 0) {
    for (int i = 0; i < 96; i++) EEPROM.write(i, 0);

    for (int i = 0; i < n_ssid.length(); ++i) EEPROM.write(i, n_ssid[i]);
    for (int i = 0; i < n_pass.length(); ++i) EEPROM.write(i + 32, n_pass[i]);
    for (int i = 0; i < n_broker.length(); ++i) EEPROM.write(i + 64, n_broker[i]);

    EEPROM.commit();
    server.send(200, "text/html", "<h1>Saved! Restarting...</h1>");
    delay(1000);
    ESP.restart();
  } else {
    server.send(200, "text/html", "<h1>Error: Missing Fields</h1>");
  }
}