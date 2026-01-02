/*
 * Project: ESP8266 Secure HTTPS Server with mTLS
 * Author: Abdelrahman Elnahrawy
 * Description: A highly secure web server that uses BearSSL to provide 
 * HTTPS and requires a valid client certificate signed by a custom CA.
 */

#include <ESP8266WiFi.h>
#include <time.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

// Initialize the Secure Server on port 443 (Standard HTTPS)
BearSSL::WiFiServerSecure server(443);

// --- Certificates Storage ---
// Public Root CA used to verify incoming clients
const char ca_cert[] PROGMEM = R"EOF(...)EOF"; 

// Server's private key (Keep this secret!)
const char server_private_key[] PROGMEM = R"EOF(...)EOF";

// Server's public certificate shown to the client
const char server_cert[] PROGMEM = R"EOF(...)EOF";

/**
 * Syncs internal ESP8266 clock with NTP servers.
 * Essential for X.509 certificate validation (checking expiration).
 */
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing Time");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime Synced.");
}

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Connected.");

  // Sync clock for SSL validation
  setClock(); 

  // Load Server Identity (Cert + Private Key)
  BearSSL::X509List *serverCertList = new BearSSL::X509List(server_cert);
  BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(server_private_key);
  server.setRSACert(serverCertList, serverPrivKey);

  // Load Trust Anchor (CA) to verify client certificates
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  server.setClientTrustAnchor(serverTrustedCA);

  server.begin();
  Serial.println("HTTPS Server Started on port 443");
}

// Simple HTML Response
static const char *HTTP_RES = "HTTP/1.0 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Secure Connection Verified!</h1></body></html>";

void loop() {
  // Check for encrypted incoming connections
  BearSSL::WiFiClientSecure incoming = server.available();
  
  if (incoming) {
    Serial.println("New Secure Client connected.");
    
    // Wait for the end of the HTTP Request header (\r\n\r\n)
    uint32_t timeout = millis() + 1000;
    while (incoming.connected() && millis() < timeout) {
      if (incoming.available()) {
        String line = incoming.readStringUntil('\n');
        if (line == "\r") break; // End of headers
      }
    }

    // Send encrypted response
    incoming.write((uint8_t*)HTTP_RES, strlen(HTTP_RES));
    incoming.flush();
    incoming.stop();
    Serial.println("Response sent. Connection closed.");
  }
}