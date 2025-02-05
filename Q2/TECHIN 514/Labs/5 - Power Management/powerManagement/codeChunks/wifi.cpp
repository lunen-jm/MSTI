#include <WiFi.h> // Ensure you include the WiFi library

const char* ssid     = "UW MPSK";
const char* password = "password"; // Replace with your password received from UW MPSK

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
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void loop() {
  // Test WiFi connection
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi is still connected");
  } else {
    Serial.println("WiFi connection lost");
  }
  delay(5000); // Add a delay to avoid flooding the serial output
}