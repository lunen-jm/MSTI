/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* Includes ---------------------------------------------------------------- */
#include <test_wand_inferencing.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <FastLED.h> // Add FastLED library for LED control

// MPU6050 sensor
Adafruit_MPU6050 mpu;

// LED configuration
#define LED_PIN     D10   // LED pin (adjust if necessary for your board)
#define NUM_LEDS    1     // Number of LEDs
CRGB leds[NUM_LEDS];

// Gesture state machine
bool waitingForSpell = false;  // Becomes true after 'begin' gesture is detected
unsigned long lastGestureTime = 0;
#define GESTURE_TIMEOUT_MS 10000  // Reset to waiting state after 10 seconds of no gesture

// Sampling and capture variables
#define SAMPLE_RATE_MS 10  // 100Hz sampling rate (10ms between samples)
#define CAPTURE_DURATION_MS 1000  // 1 second capture
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE  // Size of the feature array

// Capture state variables
bool capturing = false;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;

// Feature array to store accelerometer data
float features[FEATURE_SIZE];

/**
 * @brief      Copy raw feature data in out_ptr
 *             Function called by inference library
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

void print_inference_result(ei_impulse_result_t result);

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
    
    // Initialize FastLED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(150);  // Set brightness (0-255)
    
    // Turn off LED initially
    leds[0] = CRGB::Black;
    FastLED.show();
    
    Serial.println("MPU6050 initialized successfully");
    Serial.println("Ready for gesture detection - make a 'begin' gesture to start");
}

/**
 * @brief      Start the gesture capture process
 */
void startCapture() {
    sample_count = 0;
    capturing = true;
    capture_start_time = millis();
    last_sample_time = millis();
    
    // Dim the current LED color to indicate we're capturing
    // If we're in waiting for spell mode, dim the white light
    // If we're in regular mode, briefly flash the LED
    if (waitingForSpell) {
        leds[0].fadeToBlackBy(150); // Dim current color
    } else {
        leds[0] = CRGB(20, 20, 20); // Slight glow to indicate capturing
    }
    FastLED.show();
}

/**
 * @brief      Capture accelerometer data for inference
 */
void capture_accelerometer_data() {
    if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
        last_sample_time = millis();
        
        // Get accelerometer data
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        // Store data in features array (x, y, z, x, y, z, ...)
        if (sample_count < FEATURE_SIZE / 3) {
            int idx = sample_count * 3;
            features[idx] = a.acceleration.x;
            features[idx + 1] = a.acceleration.y;
            features[idx + 2] = a.acceleration.z;
            sample_count++;
        }
          // Check if capture duration has elapsed
        if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
            capturing = false;
            Serial.println("Capture complete");
            
            // Restore LED to appropriate state
            if (waitingForSpell) {
                leds[0] = CRGB::White;  // Restore white light if waiting for spell
            } else {
                leds[0] = CRGB::Black;  // Turn off LED if in idle mode
            }
            FastLED.show();
            
            // Run inference on captured data
            run_inference();
        }
    }
}

/**
 * @brief      Run inference on the captured data
 */
void run_inference() {
    // Check if we have enough data
    if (sample_count * 3 < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        Serial.println("ERROR: Not enough data for inference");
        return;
    }
    
    ei_impulse_result_t result = { 0 };

    // Create signal from features array
    signal_t features_signal;
    features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    features_signal.get_data = &raw_feature_get_data;

    // Run the classifier
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    if (res != EI_IMPULSE_OK) {
        Serial.print("ERR: Failed to run classifier (");
        Serial.print(res);
        Serial.println(")");
        return;
    }

    // Print inference result
    print_inference_result(result);
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    // Auto-start capture if not already capturing
    static unsigned long lastAutoCaptureTime = 0;
    if (!capturing && (millis() - lastAutoCaptureTime > 3000)) {  // Check every 3 seconds
        lastAutoCaptureTime = millis();
        
        // Start a capture automatically
        // Only print message if in spell waiting mode (to reduce console spam)
        if (waitingForSpell) {
            Serial.println("Listening for spell gesture...");
        } else {
            Serial.println("Listening for begin gesture...");
        }
        startCapture();
    }
    
    // Capture data if in capturing mode
    if (capturing) {
        capture_accelerometer_data();
    }
    
    // Check for gesture timeout (reset to non-spell mode after timeout period)
    if (waitingForSpell && (millis() - lastGestureTime > GESTURE_TIMEOUT_MS)) {
        Serial.println("Gesture timeout - ready for 'begin' gesture again");
        waitingForSpell = false;
        leds[0] = CRGB::Black;  // Turn off LED
        FastLED.show();
    }
}

void print_inference_result(ei_impulse_result_t result) {    // Find the prediction with highest confidence
    float max_value = 0;
    int max_index = -1;
    
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_value) {
            max_value = result.classification[i].value;
            max_index = i;
        }
    }
    
    // Set confidence threshold for gesture detection
    const float CONFIDENCE_THRESHOLD = 0.7;  // 70% confidence required
    
    // Only process gestures with high enough confidence
    if (max_index != -1 && max_value >= CONFIDENCE_THRESHOLD) {
        String gesture = ei_classifier_inferencing_categories[max_index];
        Serial.print("Detected gesture: ");
        Serial.print(gesture);
        Serial.print(" (");
        Serial.print(max_value * 100);
        Serial.println("%)");
        
        // Update last gesture time
        lastGestureTime = millis();
        
        // Process the detected gesture
        if (gesture == "begin") {
            // Begin gesture detected - enter spell casting mode
            waitingForSpell = true;
            Serial.println("Begin gesture detected - ready for spell!");
            leds[0] = CRGB::White;  // White light for begin gesture
            FastLED.show();
        }
        else if (waitingForSpell) {            // We're waiting for a spell and detected another gesture
            if (gesture == "fire") {
                Serial.println("Fire spell cast!");
                
                // Fire effect animation (red with flicker)
                for (int i = 0; i < 10; i++) {
                    leds[0] = CRGB(255, 50 + random(50), random(40));
                    FastLED.show();
                    delay(50);
                }
                leds[0] = CRGB::Red;  // Settle on red
                FastLED.show();
            }
            else if (gesture == "shield") {
                Serial.println("Shield spell cast!");
                
                // Shield effect animation (building blue)
                for (int i = 0; i < 255; i += 25) {
                    leds[0] = CRGB(0, 0, i);
                    FastLED.show();
                    delay(20);
                }
                leds[0] = CRGB::Blue;  // Full blue
                FastLED.show();
            }
            else if (gesture == "heal") {
                Serial.println("Heal spell cast!");
                
                // Heal effect animation (green pulse)
                for (int i = 0; i < 3; i++) {
                    leds[0] = CRGB(0, 255, 0);
                    FastLED.show();
                    delay(100);
                    leds[0] = CRGB(0, 100, 0);
                    FastLED.show();
                    delay(100);
                }
                leds[0] = CRGB::Green;  // Settle on green
                FastLED.show();
            }
            // Reset waiting state (need another begin gesture)
            waitingForSpell = false;
        }
    }
    else if (max_index != -1) {
        // Low confidence detection
        Serial.print("Low confidence detection: ");
        Serial.print(ei_classifier_inferencing_categories[max_index]);
        Serial.print(" (");
        Serial.print(max_value * 100);
        Serial.println("%)");
    }
}