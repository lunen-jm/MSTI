#include <Arduino.h>

const int trigPin = D4; // Set the trigPin to GPIO D4
const int echoPin = D5; // Set the echoPin to GPIO D5
const float soundSpeed = 0.034; // Speed of sound in cm/us

void setup() {
  Serial.begin(115200); // Start the Serial communication
  pinMode(trigPin, OUTPUT); // Set the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Set the echoPin as an INPUT
  Serial.println("HC-SR04 Distance Measurement Initialized");
}

void loop() {
  Serial.println("Loop started"); // Debugging statement

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

    // Print the distance to the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Wait for a second before the next measurement
  delay(1000);
}