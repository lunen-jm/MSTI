#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdlib.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long interval = 1000;

// TODO: add new global variables for your sensor readings and processed data

const int trigPin = D4; // Set the trigPin to GPIO D4
const int echoPin = D5; // Set the echoPin to GPIO D5
const float soundSpeed = 0.034; // Speed of sound in cm/us

// TODO: Change the UUID to your own (any specific one works, but make sure they're different from others'). You can generate one here: https://www.uuidgenerator.net/
#define SERVICE_UUID        "a7733b16-a649-4d34-9d73-32c9bd59bab7"
#define CHARACTERISTIC_UUID "c2536ad7-74aa-4914-a3fc-7ec211b3a073"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

// TODO: add DSP algorithm functions here
// DSP algorithm to filter out readings greater than 30cm
float processReading(float distance) {
    if (distance < 30.0) {
        return distance;
    } else {
        return -1.0; // Return -1 to indicate an invalid reading
    }
}


void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    // TODO: add codes for handling your sensor setup (pinMode, etc.)
    pinMode(trigPin, OUTPUT); // Set the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Set the echoPin as an INPUT
    Serial.println("HC-SR04 Distance Measurement Initialized");

    // TODO: name your device to avoid conflictions
    BLEDevice::init("JadenXIAO_ESP32C3");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setValue("Hello World");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
    // TODO: add codes for handling your sensor readings (analogRead, etc.)

    Serial.println("Loop started"); // Debugging statement

    // Code taken from lab 5, sensorReading.cpp, lines 22-57 
    // I then added in the processing to that codebase

    // Clear the trigPin by setting it LOW
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Trigger the sensor by setting the trigPin HIGH for 10 microseconds
    Serial.println("Triggering sensor");
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds
    Serial.println("Reading echo");
    long duration = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms

    // Check if the duration is valid
    if (duration == 0) {
        Serial.println("No pulse detected");
    } else {
        // Calculate the distance
        float distance = duration * soundSpeed / 2;

        // TODO: use your defined DSP algorithm to process the readings
        // Process the reading using the DSP algorithm
        float processedDistance = processReading(distance);

        // Check if the processed distance is valid
        if (processedDistance != -1.0) {
            // Print the distance to the Serial Monitor
            Serial.print("Distance: ");
            Serial.print(processedDistance);
            Serial.println(" cm");

            if (deviceConnected) {
                // Send new readings to database
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis >= interval) {
                    pCharacteristic->setValue(String(processedDistance).c_str());
                    pCharacteristic->notify();
                    Serial.println("Notify value: " + String(processedDistance));
                    previousMillis = currentMillis; // Update the previousMillis
                }
            }
        } else {
            Serial.println("Distance greater than 30 cm, not sending");
        }
    }
   
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising();  // advertise again
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    delay(1000);
}