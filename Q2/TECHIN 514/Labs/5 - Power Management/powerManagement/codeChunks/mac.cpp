// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
#include <Arduino.h>
#include <WiFi.h>

void setup(){
  WiFi.begin();
  Serial.begin(115200);
  while(!Serial);
  delay(1000);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){
	Serial.println(WiFi.macAddress());
	delay(1000);
}