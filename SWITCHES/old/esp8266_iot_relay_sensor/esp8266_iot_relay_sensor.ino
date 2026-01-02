#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -------------------- DHT Sensor Setup --------------------
#define DHTPIN 0         // GPIO pin connected to DHT11
#define DHTTYPE DHT11    // DHT 11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// -------------------- WiFi and MQTT Setup --------------------
const char* ssid = "Na7rawy Limited";             // WiFi SSID
const char* password = "Selim&Nahrawy751";       // WiFi password
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

// -------------------- Timing & MQTT Message Buffers --------------------
unsigned long lastMsg = 0;
unsigned long lastMsg1 = 0;
unsigned long lastMsg2 = 0;
unsigned long lastMsg3 = 0;
#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];

// -------------------- Device Pins --------------------
int value = 0, value1 = 0, value2 = 0, value3 = 0;
int S = 0;
int pir = 5;         // PIR sensor pin
int button = 4;      // Button pin
int M_P1 = 2;        // Device 1
int M_P2 = 15;       // Device 2
int relay1 = 12;
int relay2 = 13;
int relay3 = 14;
int relayH = 16;

// -------------------- WiFi Connection Function --------------------
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

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// -------------------- MQTT Callback Function --------------------
String callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Relay 1 Control
    if ((char)payload[0] == 'r' && (char)payload[5] == '1' && (char)payload[7] == 'n') digitalWrite(relay1, HIGH);
    if ((char)payload[0] == 'r' && (char)payload[5] == '1' && (char)payload[7] == 'f') digitalWrite(relay1, LOW);

    // Relay 2 Control
    if ((char)payload[0] == 'r' && (char)payload[5] == '2' && (char)payload[7] == 'n') digitalWrite(relay2, HIGH);
    if ((char)payload[0] == 'r' && (char)payload[5] == '2' && (char)payload[7] == 'f') digitalWrite(relay2, LOW);

    // Relay 3 Control
    if ((char)payload[0] == 'r' && (char)payload[5] == '3' && (char)payload[7] == 'n') digitalWrite(relay3, HIGH);
    if ((char)payload[0] == 'r' && (char)payload[5] == '3' && (char)payload[7] == 'f') digitalWrite(relay3, LOW);

    // Device 1 Control
    if ((char)payload[0] == 'M' && (char)payload[3] == '1' && (char)payload[5] == 'n') digitalWrite(M_P1, HIGH);
    if ((char)payload[0] == 'M' && (char)payload[3] == '1' && (char)payload[5] == 'f') digitalWrite(M_P1, LOW);

    // Device 2 Control
    if ((char)payload[0] == 'M' && (char)payload[3] == '2' && (char)payload[5] == 'n') digitalWrite(M_P2, HIGH);
    if ((char)payload[0] == 'M' && (char)payload[3] == '2' && (char)payload[5] == 'f') digitalWrite(M_P2, LOW);

    // Relay H Control
    if ((char)payload[0] == 'r' && (char)payload[5] == 'H' && (char)payload[7] == 'n') digitalWrite(relayH, HIGH);
    if ((char)payload[0] == 'r' && (char)payload[5] == 'H' && (char)payload[7] == 'f') digitalWrite(relayH, LOW);

    return ""; // Return placeholder (not used)
}

// -------------------- MQTT Reconnect --------------------
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            client.publish("zabola", "hello world");
            client.subscribe("zabolaa");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// -------------------- Arduino Setup --------------------
void setup() {
    Serial.begin(115200);

    // Configure pins
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(M_P1, OUTPUT);
    pinMode(M_P2, OUTPUT);
    pinMode(relayH, OUTPUT);
    pinMode(pir, INPUT);
    pinMode(button, INPUT);

    // Initialize sensors and WiFi
    dht.begin();
    setup_wifi();

    // Setup MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

// -------------------- Arduino Main Loop --------------------
void loop() {
    // Read DHT sensor values
    int h = dht.readHumidity();
    int t = dht.readTemperature();
    int f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Reconnect MQTT if disconnected
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();

    // The following block is commented out for now (optional MQTT publishing)
    /*
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf(msg, MSG_BUFFER_SIZE, "Humidity:%d  Temperature: %d  #%d", h, t, value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/DHT", msg);
    }

    if (now - lastMsg1 > 1000 && digitalRead(pir) == HIGH) {
        lastMsg1 = now;
        ++value1;
        snprintf(msg, MSG_BUFFER_SIZE, "ALERT #%d", value1);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/PIR", msg);
    }

    if (now - lastMsg2 > 700 && digitalRead(button) == HIGH) {
        lastMsg2 = now;
        ++value2;
        snprintf(msg, MSG_BUFFER_SIZE, "PRESS NO. #%d", value2);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/BUTTON", msg);
    }

    if (now - lastMsg3 > 300 && analogRead(A0) < 60) {
        lastMsg3 = now;
        ++value3;
        snprintf(msg, MSG_BUFFER_SIZE, "LASER PASS NO. #%d", value3);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("esp8266/LASER", msg);
    }
    */
}
