#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  int analogValueA0 = analogRead(A0); // Read the analog value from pin A0
  int analogValueA1 = analogRead(A1); // Read the analog value from pin A1

  // Print the analog values to the serial monitor
  Serial.print("Analog value from A0 (VOUT1): ");
  Serial.println(analogValueA0);
  Serial.print("Analog value from A1 (VOUT2): ");
  Serial.println(analogValueA1);

  delay(1000); // Wait for 1 second before reading again
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}