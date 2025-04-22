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

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

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
}

void loop() {
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // received a packet
      Serial.println("Received packet: ");  // Changed to println for cleaner output
  
      // read packet
      String LoRaData = "";
      while (LoRa.available()) {
        LoRaData += (char)LoRa.read();
      }
      Serial.println(LoRaData);  // Print the full data received
  
      // Extract PPG value from the received data
      int ppgIndex = LoRaData.indexOf("PPG: ");
      if (ppgIndex >= 0) {
        String ppgValue = LoRaData.substring(ppgIndex + 5);
        ppgValue.trim();  // Remove any whitespace
        Serial.print("PPG Value: ");
        Serial.println(ppgValue);
        
        // Output the PPG value for the serial plotter
        Serial.println(ppgValue.toInt()); // Send ONLY the integer value for plotting
      } else {
        Serial.println("PPG data not found in message");
      }
  
      // print RSSI of packet
      Serial.print("RSSI: ");
      Serial.println(LoRa.packetRssi());
    }
  }