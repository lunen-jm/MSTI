#include <Arduino.h>
#include <WiFi.h> // Ensure you include the WiFi library

void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(1000);

  // Scan for available networks
  Serial.println("Scanning for available networks...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.println("Networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}

void loop() {
  // Nothing to do here
}