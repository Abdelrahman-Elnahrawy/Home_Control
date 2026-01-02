/*
 * Project: MQTT NeoPixel IR Gateway
 * Author: Abdelrahman Elnahrawy
 * Description: An ESP8266 based IoT hub that controls NeoPixels and sends IR signals via MQTT.
 * Includes Captive Portal for WiFi configuration and Hardware Timer Interrupts.
 */

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>

// --- Hardware Timer Interrupts ---
#define TIMER_INTERRUPT_DEBUG      0
#define _TIMERINTERRUPT_LOGLEVEL_  1
#include "ESP8266TimerInterrupt.h"

// --- Hardware Definitions ---
#define NEOPIXEL_PIN    14      // D5 on NodeMCU
#define IR_LED_PIN      4       // D2 on NodeMCU
#define BUILTIN_LED     2       // D4 on NodeMCU (Active LOW)
#define NUM_PIXELS      30
#define BRIGHTNESS      50

// --- Objects Initialization ---
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
ESP8266Timer ITimer;
ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
IRsend irSender(IR_LED_PIN);

// --- Global Variables ---
const int eepromSize = 100;
volatile uint32_t lastMillis = 0;
int statusLed = 1; // Used for toggling LED in interrupt

// --- Default WiFi & MQTT Settings ---
String ssid_str = "";
String pass_str = "";

// UPDATE THESE WITH YOUR MQTT DETAILS
const char* mqttServer = "MQTT_BROKER_IP";
const int mqttPort = 1883;
const char* mqttUser = "MQTT_USERNAME";
const char* mqttPassword = "MQTT_PASSWORD";
const char* mqttTopic = "IR_RAW_DATA_TOPIC";

// ================================================================
//                      INTERRUPT ROUTINE
// ================================================================
// This runs every 500ms via Hardware Timer
void IRAM_ATTR Server_Update_Loop() {
    digitalWrite(BUILTIN_LED, statusLed);  // Toggle Built-in LED
    statusLed = !statusLed;
    // Note: Removed 'break;' as it is invalid in a void function
}

// ================================================================
//                        MQTT FUNCTIONS
// ================================================================

void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, mqttTopic) == 0) {
        Serial.println("Received IR raw data");
        
        // Example: Treat payload as raw data stream
        // Note: Real implementation needs parsing payload bytes to uint16_t array
        for (unsigned int i = 0; i < length; i++) {
            // irSender.sendRaw(rawData, dataLength, 38); // Example usage
            delay(50); 
        }
        Serial.println("IR transmission complete");
    }
}

void connectToMQTT() {
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(handleMQTTMessage);

    unsigned long MQTT_Connection_TIMER = millis();
    
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        
        if (mqttClient.connect("ESP8266_Unimote", mqttUser, mqttPassword)) {
            Serial.println("Connected!");
            mqttClient.subscribe(mqttTopic);
        } else {
            Serial.print("Failed rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Retrying in 1s...");
            delay(1000);
        }
        
        // Timeout after 7 seconds
        if (millis() - MQTT_Connection_TIMER > 7000) {
            Serial.println("MQTT Failed -> Starting Hotspot Mode");
            startAccessPoint("Unimote MQTT Error");
            break;
        }
    }
}

// ================================================================
//                  WIFI & WEB SERVER FUNCTIONS
// ================================================================

void startAccessPoint(String AP_NAME) {
    WiFi.mode(WIFI_AP);
    // Convert String to char array for softAP
    char apNameChar[30];
    AP_NAME.toCharArray(apNameChar, 30);
    
    WiFi.softAP(apNameChar);
    Serial.print("Access Point Started IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.begin();
}

void handleRoot() {
    String html = "<html><body>";
    html += "<h1>Unimote Configuration</h1>";
    html += "<form method='POST' action='/save'>";
    html += "SSID: <input type='text' name='ssid'><br>";
    html += "Password: <input type='password' name='password'><br>";
    html += "<input type='submit' value='Save & Reboot'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
}

void handleSave() {
    String s = server.arg("ssid");
    String p = server.arg("password");

    // Save to EEPROM
    for (int i = 0; i < s.length(); ++i) EEPROM.write(i, s[i]);
    EEPROM.write(s.length(), '\0'); 

    for (int i = 0; i < p.length(); ++i) EEPROM.write(i + 32, p[i]);
    EEPROM.write(p.length() + 32, '\0'); 
    
    EEPROM.commit();
    server.send(200, "text/html", "<h1>Saved. Restarting...</h1>");
    delay(1000);
    ESP.restart();
}

void connectToWiFi() {
    unsigned long WIFI_TIMER = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_str, pass_str);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - WIFI_TIMER > 7000) {
            Serial.println("\nWiFi Failed -> Starting Hotspot");
            startAccessPoint("Unimote Setup");
            return; // Exit function
        }
    }
    Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());
}

// ================================================================
//                        NEOPIXEL EFFECTS
// ================================================================

uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
    uint16_t i, j;
    for(j=0; j<256; j++) {
        for(i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

// ================================================================
//                          MAIN SETUP
// ================================================================
void setup() {
    Serial.begin(115200);
    EEPROM.begin(eepromSize);
    pinMode(BUILTIN_LED, OUTPUT);

    // Initialize NeoPixel
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show(); 

    // Read Credentials
    char storedSSID[32];
    char storedPass[32];
    for (int i = 0; i < 32; ++i) storedSSID[i] = EEPROM.read(i);
    for (int i = 0; i < 32; ++i) storedPass[i] = EEPROM.read(i + 32);

    if (strlen(storedSSID) > 0) {
        ssid_str = String(storedSSID);
        pass_str = String(storedPass);
        connectToWiFi();
    } else {
        startAccessPoint("Unimote Setup");
    }

    if (WiFi.status() == WL_CONNECTED) {
        connectToMQTT();
    }

    irSender.begin();

    // Start Hardware Timer (500ms interval)
    if (ITimer.attachInterruptInterval(500 * 1000, Server_Update_Loop)) {
        Serial.println("Timer Started");
    } else {
        Serial.println("Timer Error");
    }
}

// ================================================================
//                           MAIN LOOP
// ================================================================
void loop() {
    // Only handle web server if in AP mode or if specifically needed
    if (WiFi.status() != WL_CONNECTED) {
        server.handleClient();
    } else {
        mqttClient.loop();
    }
    
    // Example: Run effect if connected
    // rainbow(20); 
}