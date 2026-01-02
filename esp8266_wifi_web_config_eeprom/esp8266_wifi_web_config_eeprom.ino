/*
  Project Name:
  ESP8266 WiFi Credential Manager with Web Configuration & EEPROM Storage

  Description:
  - ESP8266 tries to connect to WiFi using credentials stored in EEPROM
  - If connection fails, it starts an Access Point
  - Provides a web page to enter SSID & Password
  - Saves credentials in EEPROM
  - Restarts and retries WiFi connection automatically
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

/* -------------------- Global Variables -------------------- */

// EEPROM stored string: "SSID$PASSWORD$"
String eepromData;
String USR;     // WiFi SSID
String PASS;    // WiFi Password
int separatorIndex; // Index of '$' separator

ESP8266WebServer server(80);

/* Static IP for SoftAP */
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

/* -------------------- HTML Page -------------------- */
const String postForms = "<html>\
<head>\
<title>ESP8266 WiFi Setup</title>\
<style>\
body { background-color: #cccccc; font-family: Arial; color: #000088; }\
</style>\
</head>\
<body>\
<h1>Enter WiFi Credentials</h1><br>\
<form method=\"post\" action=\"/postform/\">\
<input type=\"text\" name=\"SSID\" value=\"SSID\"><br>\
<input type=\"text\" name=\"PASSWORD\" value=\"PASSWORD\"><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

/* -------------------- Web Handlers -------------------- */

// Root page (WiFi configuration page)
void handleRoot() {
  server.send(200, "text/html", postForms);
}

// Handle POST request (save SSID & Password)
void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String newUSR = server.arg(0);
  String newPASS = server.arg(1);

  // Store as "SSID$PASSWORD$"
  eepromData = newUSR + "$" + newPASS + "$";

  for (int i = 0; i < eepromData.length(); i++) {
    EEPROM.write(i, eepromData[i]);
  }
  EEPROM.commit();

  server.send(200, "text/plain", "Credentials saved. Rebooting...");

  delay(500);
  WiFi.softAPdisconnect(true);
  delay(1000);
  ESP.restart();
}

/* -------------------- WiFi Setup -------------------- */

void setup_wifi() {
  USR = "";
  PASS = "";

  // Read SSID from EEPROM
  for (int i = 0; (char)EEPROM.read(i) != '$'; i++) {
    USR += (char)EEPROM.read(i);
    separatorIndex = i;
    if (i > 50) break;
  }

  // Read PASSWORD from EEPROM
  for (int i = separatorIndex + 2; (char)EEPROM.read(i) != '$'; i++) {
    PASS += (char)EEPROM.read(i);
    if (i > 100) break;
  }

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.begin(USR, PASS);

  Serial.println("Connecting to WiFi...");
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");

    // If WiFi fails → Start Access Point
    if (i > 15) {
      Serial.println("\nStarting configuration AP");
      WiFi.softAP("ESP help!!");
      Serial.print("AP IP: ");
      Serial.println(WiFi.softAPIP());
      return;
    }
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

/* -------------------- Arduino Setup -------------------- */

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  delay(1000);
  setup_wifi();

  // mDNS
  if (MDNS.begin("esp8266")) {
    Serial.println("mDNS started (http://esp8266.local)");
  }

  server.on("/", handleRoot);
  server.on("/postform/", handleForm);

  server.begin();
  Serial.println("HTTP server started");
}

/* -------------------- Main Loop -------------------- */

void loop() {
  server.handleClient();
}
