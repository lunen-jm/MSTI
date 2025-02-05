#include <Arduino.h>
#include <WiFi.h> // Ensure you include the WiFi library

const char* ssid     = "UW MPSK";
const char* password = "password"; // Replace with your password received from UW MPSK

void printWiFiStatus() {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      Serial.println("WiFi Status: Idle");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("WiFi Status: SSID not available");
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("WiFi Status: Scan completed");
      break;
    case WL_CONNECTED:
      Serial.println("WiFi Status: Connected");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("WiFi Status: Connection failed");
      break;
    case WL_CONNECTION_LOST:
      Serial.println("WiFi Status: Connection lost");
      break;
    case WL_DISCONNECTED:
      Serial.println("WiFi Status: Disconnected");
      break;
    default:
      Serial.println("WiFi Status: Unknown");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(1000);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Limit the number of attempts
    delay(1000);
    Serial.print(".");
    attempts++;
    printWiFiStatus(); // Print WiFi status during each attempt
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.println("Failed to connect to WiFi");
    printWiFiStatus(); // Print final WiFi status
  }
}

void loop() {
  // Test WiFi connection
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi is still connected");
  } else {
    Serial.println("WiFi connection lost");
    printWiFiStatus(); // Print WiFi status if connection is lost
  }
  delay(5000); // Add a delay to avoid flooding the serial output
}