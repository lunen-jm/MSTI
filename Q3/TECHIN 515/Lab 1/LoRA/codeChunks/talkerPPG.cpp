/*********
  Modified from the examples of the Arduino LoRa library
Code from: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

// Define the analog pin connected to the PPG sensor
const int ppgSensorPin = 34; // Replace with the GPIO pin you're using (e.g., GPIO34)

int counter = 0;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xEE);
  Serial.println("LoRa Initializing OK!");
    // (Optional) Configure the analog pin if necessary
  pinMode(ppgSensorPin, INPUT);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
    // Read the analog value from the PPG sensor
  int sensorValue = analogRead(ppgSensorPin);

  // Print the value to the Serial Monitor
  Serial.print("PPG Sensor Value: ");
  Serial.println(sensorValue);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.print(" PPG: ");  // Added space between counter and PPG
  LoRa.print(sensorValue);
  LoRa.endPacket();

  counter++;

  delay(1000);
}