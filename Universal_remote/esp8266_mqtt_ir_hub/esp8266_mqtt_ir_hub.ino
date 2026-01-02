/*
 * Project: ESP8266 Smart MQTT IR Hub (Universal Remote)
 * Author: Abdelrahman Elnahrawy
 * Description: An IoT hub that receives IR raw codes via MQTT and sends 
 * them through an IR LED. Includes a fallback Hotspot mode for WiFi setup.
 */

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// --- Configuration ---
#define kIrLed 13 // D7 on most NodeMCU boards
IRsend irsend(kIrLed); 

#define HOTSPOT_MODE 0
#define NORMAL_OPERATION_MODE 1

// Macro to detect the start of a Raw IR payload
#define FIRST_PAYLOAD_CHECK (payload[0]=='I'&&payload[1]=='R'&&payload[2]=='R'&&payload[3]=='A'&&payload[4]=='W'&&payload[5]==':') 

// --- Global Variables ---
char storedSSID[32], storedPassword[32], storedBroker[32];
const char *ssid = "", *password = "", *mqttServer = "";
const int mqttPort = 1883;
const int eepromSize = 200;

bool CurrentMode;
uint16_t values[512]; // Buffer for IR Raw data
unsigned long lastMsg = 0;
unsigned long LostConnectionTimer;

ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/**
 * Converts a segment of the byte payload into an unsigned integer.
 * Used for parsing CSV IR codes.
 */
unsigned int PayloadToInt(byte* payload, unsigned int index, unsigned int CommaIndex) {
    unsigned int elementvalue = 0;
    for (int i = index; i < CommaIndex; i++) {
        elementvalue += (payload[i] - '0') * pow(10, (CommaIndex - i - 1));
    }
    return elementvalue;
}

/**
 * MQTT Callback: Parses incoming IR signals or repeat commands.
 */
void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
    static unsigned int index, CommaIndex, count;
    
    // Command: Repeat last IR signal
    if (length < 2 && payload[0] == '}') {
        irsend.sendRaw(values, 512, 38);
        return;
    }

    // Command: New Raw IR Data sequence
    if (FIRST_PAYLOAD_CHECK) {
        count = 0;
        memset(values, 0, sizeof(values)); // Clear old data
        index = 7; // Skip "IRRAW:"
        CommaIndex = index;

        while (index < length) {
            while (payload[CommaIndex] != ',' && (CommaIndex + 1) < length) {
                CommaIndex++;
            }
            if (CommaIndex > length) break;
            
            values[count] = PayloadToInt(payload, index, CommaIndex);
            count++;
            index = CommaIndex + 1;
            CommaIndex = index;
        }
    }
    
    // Send signal if payload ends with '}'
    if (payload[length - 1] == '}') {
        irsend.sendRaw(values, 512, 38);
    }
}

// ... (Rest of logic: WiFi Connect, MQTT Connect, and Web Server handlers)

void setup() {
    Serial.begin(115200);
    EEPROM.begin(eepromSize);
    irsend.begin();

    // Load Credentials from EEPROM
    for (int i = 0; i < 32; ++i) {
        storedSSID[i] = EEPROM.read(i);
        storedPassword[i] = EEPROM.read(i + 32);
        storedBroker[i] = EEPROM.read(i + 64);
    }

    if (strlen(storedSSID) > 1) {
        ssid = storedSSID;
        password = storedPassword;
        mqttServer = storedBroker;
        // ... Logic to try connection
    } else {
        // startAccessPoint("Unimote Setup");
    }
}

void loop() {
    if (CurrentMode == NORMAL_OPERATION_MODE) {
        if (!mqttClient.connected()) {
            // Reconnect logic
        }
        mqttClient.loop();
    } else {
        server.handleClient();
        // Auto-restart after 3 mins of inactivity in setup mode
        if (millis() - LostConnectionTimer > 180000) ESP.restart();
    }
}