#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -----------------------------
// OneWire & Temperature Setup
// -----------------------------
#define ONE_WIRE_BUS 5 // D1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// -----------------------------
// WiFi & MQTT Setup
// -----------------------------
const char* ssid = "Na7rawy Limited";
const char* password = "Selim&Nahrawy751";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// -----------------------------
// Variables
// -----------------------------
float t = 0;  // Temperature reading
unsigned long lastMsg = 0;       // Timer for temperature
unsigned long lastMsg1 = 0;      // Timer for PIR
unsigned long lastMsg2 = 0;      // Timer for button
unsigned long lastMsg3 = 0;      // Timer for relay feedback

int R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0;  // Relay states

int value = 0, value1 = 0, value2 = 0, value3 = 0;

int pir = 2;     // D4, PIR sensor
int button = 4;  // D2, push button

int relay1 = 12; // D6
int relay2 = 13; // D7
int relay3 = 14; // D5
int relay4 = 15; // D8
int relay5 = 0;  // D3
int relay6 = 16; // D0

#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];

// -----------------------------
// WiFi Connection
// -----------------------------
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// -----------------------------
// MQTT Callback Function
// -----------------------------
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print(payload[i]);
    }
    Serial.println();

    // -----------------------------
    // Relay Control Commands
    // Example payload: r1n = relay1 ON, r1f = relay1 OFF
    // -----------------------------
    if ((char)payload[0] == 'r') {
        // Relay 1
        if ((char)payload[5] == '1' && (char)payload[7] == 'n') { digitalWrite(relay1, HIGH); R1 = 1; }
        if ((char)payload[5] == '1' && (char)payload[7] == 'f') { digitalWrite(relay1, LOW); R1 = 0; }

        // Relay 2
        if ((char)payload[5] == '2' && (char)payload[7] == 'n') { digitalWrite(relay2, HIGH); R2 = 1; }
        if ((char)payload[5] == '2' && (char)payload[7] == 'f') { digitalWrite(relay2, LOW); R2 = 0; }

        // Relay 3
        if ((char)payload[5] == '3' && (char)payload[7] == 'n') { digitalWrite(relay3, HIGH); R3 = 1; }
        if ((char)payload[5] == '3' && (char)payload[7] == 'f') { digitalWrite(relay3, LOW); R3 = 0; }

        // Relay 4
        if ((char)payload[5] == '4' && (char)payload[7] == 'n') { digitalWrite(relay4, HIGH); R4 = 1; }
        if ((char)payload[5] == '4' && (char)payload[7] == 'f') { digitalWrite(relay4, LOW); R4 = 0; }

        // Relay 5
        if ((char)payload[5] == '5' && (char)payload[7] == 'n') { digitalWrite(relay5, HIGH); R5 = 1; }
        if ((char)payload[5] == '5' && (char)payload[7] == 'f') { digitalWrite(relay5, LOW); R5 = 0; }

        // Relay 6
        if ((char)payload[5] == '6' && (char)payload[7] == 'n') { digitalWrite(relay6, HIGH); R6 = 1; }
        if ((char)payload[5] == '6' && (char)payload[7] == 'f') { digitalWrite(relay6, LOW); R6 = 0; }
    }
}

// -----------------------------
// MQTT Reconnect
// -----------------------------
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            client.subscribe("zabolaa"); // Subscribe to relay commands
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
    // Setup relay pins as OUTPUT
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);
    pinMode(relay5, OUTPUT);
    pinMode(relay6, OUTPUT);

    // Setup PIR and Button pins
    pinMode(pir, INPUT);
    
    // Start temperature sensor
    sensors.begin();

    // Start serial for debug
    Serial.begin(115200);

    // Connect WiFi
    setup_wifi();

    // Setup MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

// -----------------------------
// Main Loop
// -----------------------------
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Read temperature
    sensors.requestTemperatures();
    t = sensors.getTempCByIndex(0);

    unsigned long now = millis();

    // -----------------------------
    // Publish Temperature every 2 seconds
    // -----------------------------
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf(msg, MSG_BUFFER_SIZE, "Temperature: %f  #%d", t, value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/TEMP", msg);
    }

    // -----------------------------
    // Publish PIR alerts every 1 second if motion detected
    // -----------------------------
    if (now - lastMsg1 > 1000 && digitalRead(pir) == HIGH) {
        lastMsg1 = now;
        ++value1;
        snprintf(msg, MSG_BUFFER_SIZE, "ALERT #%d", value1);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/PIR", msg);
    }

    // -----------------------------
    // Publish Button press every 0.7 seconds
    // -----------------------------
    if (now - lastMsg2 > 700 && digitalRead(button) == HIGH) {
        lastMsg2 = now;
        ++value2;
        snprintf(msg, MSG_BUFFER_SIZE, "PRESS NO. #%d", value2);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/BUTTON", msg);
    }

    // -----------------------------
    // Publish Relay Status every 2 seconds
    // -----------------------------
    if (now - lastMsg3 > 2000) {
        lastMsg3 = now;
        ++value3;
        snprintf(msg, MSG_BUFFER_SIZE, "R1:%d R2:%d R3:%d R4:%d R5:%d R6:%d #%d",
                 R1, R2, R3, R4, R5, R6, value3);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/feedback", msg);
    }
}
