/* 
 * MPU6050 Gesture Data Capture
 * 
 * This simplified code captures accelerometer and gyroscope data from an MPU6050 sensor
 * and sends it over serial in a format compatible with the process_gesture_data.py script.
 * 
 * Hardware: ESP32 with MPU6050 and optional WS2812 LED
 */

/* Includes ---------------------------------------------------------------- */
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <FastLED.h> // Add FastLED library for LED control (optional)

// MPU6050 sensor
Adafruit_MPU6050 mpu;

// LED configuration (optional)
#define LED_PIN     D10   // LED pin (adjust if necessary for your board)
#define NUM_LEDS    1     // Number of LEDs
CRGB leds[NUM_LEDS];

// Capture state variables
bool capturing = false;
bool edgeImpulseMode = false;   // Flag for Edge Impulse data forwarder mode
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
#define SAMPLE_RATE_MS 10       // 100Hz sampling rate (10ms between samples)
#define CAPTURE_DURATION_MS 1000  // 1 second capture
#define START_MARKER "-,-,-,-,-,-" // Start marker with 6 data points (ax,ay,az,gx,gy,gz)

/**
 * @brief      Arduino setup function
 */
void setup()
{
    // Initialize serial
    Serial.begin(115200);
    
    // Initialize MPU6050
    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }
    
    // Configure MPU6050 - match settings with gesture_capture.ino
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    // Initialize FastLED (optional)
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(150);  // Set brightness (0-255)
    
    // Turn off LED initially
    leds[0] = CRGB::Black;
    FastLED.show();
    
    Serial.println("MPU6050 initialized successfully");
    Serial.println("Ready to capture gesture data - send 'o' to start capture");
}

/**
 * @brief      Start the gesture capture process
 */
void startCapture() {
    capturing = true;
    capture_start_time = millis();
    last_sample_time = millis();
    
    // Only send start marker if not in Edge Impulse mode
    if (!edgeImpulseMode) {
        // Send start marker to indicate beginning of data stream
        Serial.println(START_MARKER);
        Serial.println("Starting capture (will run for 1 second)");
    }
    
    // Optional: Change LED color to indicate capturing
    leds[0] = CRGB(20, 20, 20); // Slight glow to indicate capturing
    FastLED.show();
}

/**
 * @brief      Stop the gesture capture process
 */
void stopCapture() {
    capturing = false;
    
    // Only send completion messages if not in Edge Impulse mode
    if (!edgeImpulseMode) {
        Serial.println("Capture complete (1 second)");
        Serial.print("\n\n\n\n"); // Add extra newlines for the Python script to detect
    }
    
    // Optional: Turn off LED when done
    leds[0] = CRGB::Black;
    FastLED.show();
}

/**
 * @brief      Capture sensor data (accelerometer and gyroscope)
 */
void capture_sensor_data() {
    if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
        last_sample_time = millis();
        
        // Get accelerometer and gyroscope data
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        // Output data in the format expected by the current mode
        if (edgeImpulseMode) {
            // Format for Edge Impulse: just raw values with commas
            Serial.print(a.acceleration.x);
            Serial.print(",");
            Serial.print(a.acceleration.y);
            Serial.print(",");
            Serial.print(a.acceleration.z);
            Serial.print(",");
            Serial.print(g.gyro.x);
            Serial.print(",");
            Serial.print(g.gyro.y);
            Serial.print(",");
            Serial.println(g.gyro.z); // Note: println instead of print+"\n"
        }
        else {
            // Format for process_gesture_data.py
            Serial.print(a.acceleration.x);
            Serial.print(",");
            Serial.print(a.acceleration.y);
            Serial.print(",");
            Serial.print(a.acceleration.z);
            Serial.print(",");
            Serial.print(g.gyro.x);
            Serial.print(",");
            Serial.print(g.gyro.y);
            Serial.print(",");
            Serial.print(g.gyro.z);
            Serial.print("\n");
        }
        
        // Check if capture duration has elapsed (only if not in Edge Impulse mode)
        if (!edgeImpulseMode && millis() - capture_start_time >= CAPTURE_DURATION_MS) {
            stopCapture();
        }
    }
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    // Check for serial commands
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'o') {
            // Start capture when 'o' is received
            startCapture();
        } else if (cmd == 'p') {
            // Stop capture when 'p' is received
            stopCapture();
        } else if (cmd == 'e') {
            // Toggle Edge Impulse mode when 'e' is received
            edgeImpulseMode = !edgeImpulseMode;
            if (edgeImpulseMode) {
                Serial.println("Edge Impulse mode enabled");
                // In Edge Impulse mode, we should be continuously capturing
                startCapture();
            } else {
                Serial.println("Edge Impulse mode disabled");
                stopCapture();
            }
        }
    }
    
    // Capture data if in capturing mode
    if (capturing) {
        capture_sensor_data();
    }
    
    // In Edge Impulse mode, we never stop capturing
    if (edgeImpulseMode && !capturing) {
        startCapture();
    }
}