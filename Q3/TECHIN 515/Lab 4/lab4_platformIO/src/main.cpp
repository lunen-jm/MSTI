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

/* 
 * Gesture Recognition Wand with LED Feedback
 * 
 * This program uses an ESP32 with MPU6050 and WS2812 LED to:
 * 1. Continuously monitor for a "begin" gesture using Edge Impulse model
 * 2. When detected, activate the LED to glow white for 2 seconds
 * 3. During this period, detect additional gestures and change LED color accordingly
 */

/* Includes ---------------------------------------------------------------- */
#include <wand_better_inferencing.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <FastLED.h>

// MPU6050 sensor
Adafruit_MPU6050 mpu;

// Define I2C pins - adjust these to match your wiring
#define I2C_SDA 6  // SDA pin on ESP32-C3
#define I2C_SCL 7  // SCL pin on ESP32-C3

// LED configuration
#define LED_PIN 10    // Changed from D10, assuming GPIO 10 for ESP32-C3
#define NUM_LEDS    1     // Number of LEDs
CRGB leds[NUM_LEDS];

// LED Brightness Levels
#define IDLE_BRIGHTNESS 30
#define LISTENING_BRIGHTNESS 200
#define ANIMATION_BRIGHTNESS 180
#define POST_SPELL_BRIGHTNESS 80

// State Durations (milliseconds)
#define LISTENING_DURATION 7000    // Listen for a spell for 7 seconds
#define FIRE_ANIMATION_DURATION 3000
#define HEAL_ANIMATION_DURATION 6000    // Increased from 4000ms
#define SHIELD_ANIMATION_DURATION 6000  // Increased from 4000ms
#define POST_SPELL_DURATION 2000

// Idle Animation Settings
#define IDLE_COLOR_TRANSITION_INTERVAL 2000 // ms for each color in idle cycle
const CRGB idleColors[] = {
    CRGB(50, 100, 100),    // Soft Cyan
    CRGB(255, 182, 193),   // Light Pink
    CRGB(221, 160, 221),   // Light Purple (Plum)
    CRGB(152, 251, 152),   // Pale Green
    CRGB(255, 228, 181)    // Pale Gold (Moccasin)
};
const int numIdleColors = sizeof(idleColors) / sizeof(idleColors[0]);
int currentIdleColorIndex = 0;
unsigned long lastIdleColorChangeTime = 0;

// Wand States
enum WandState {
    IDLE,
    LISTENING_FOR_SPELL,
    FIRE_ANIMATION,
    HEAL_ANIMATION,
    SHIELD_ANIMATION,
    POST_SPELL
};
WandState currentState = IDLE;
unsigned long stateEnterTime = 0;
unsigned long lastAnimationUpdateTime = 0; // For animations

// Sampling and capture variables
#define SAMPLE_RATE_MS 10  // 100Hz sampling rate (10ms between samples)
#define CAPTURE_DURATION_MS 1200  // 1.2 second capture to ensure all 100 samples
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE  // Size of the feature array

// Timing variables
unsigned long lastSampleTime = 0;
unsigned long captureStartTime = 0;
int sampleCount = 0;

// Feature array to store accelerometer data
float features[FEATURE_SIZE];

// Variable to store the last detected gesture - REMOVED
// int lastDetectedGesture = -1; // REMOVED
// bool newGestureDetected = false; // REMOVED

// Confidence threshold for gesture detection
float confidenceThreshold = 0.65;  // Minimum confidence (60%) to accept a gesture (used for FIRE)
float BEGIN_GESTURE_CONFIDENCE_THRESHOLD = 0.40; // Lower threshold for "begin" gesture in IDLE state
float HEAL_GESTURE_CONFIDENCE_THRESHOLD = 0.40;  // Threshold for "heal" gesture
float SHIELD_GESTURE_CONFIDENCE_THRESHOLD = 0.50; // Threshold for "shield" gesture

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

// Forward declaration
void startCapture(); 
void processDetectedGesture(int gestureIndex);
void updateStateMachine();
void animateFire();
void animateHeal();
void animateShield();


/**
 * @brief      Run inference on the captured data and return the detected gesture
 * 
 * @return     Detected gesture index or -1 if no confident detection
 */
int run_inference() {
    // Check if we have enough data
    if (sampleCount * 6 < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        // Serial.println("ERROR: Not enough data for inference"); // Optional: uncomment for debugging
        return -1;
    }
    
    ei_impulse_result_t result = { 0 };

    // Create signal from features array
    ei::signal_t features_signal;
    features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    features_signal.get_data = &raw_feature_get_data;
    
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    if (res != EI_IMPULSE_OK) {
        Serial.print("ERR: Failed to run classifier (");
        Serial.print(res);
        Serial.println(")");
        return -1;
    }

    // Find the prediction with highest confidence
    float maxValue = 0;
    int maxIndex = -1;
    
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > maxValue) {
            maxValue = result.classification[i].value;
            maxIndex = i;
        }
    }
    
    const char* detectedGestureName = ei_classifier_inferencing_categories[maxIndex];
    bool confidentDetection = false;
    float currentThreshold = confidenceThreshold; // Default to general threshold

    if (currentState == IDLE && strcmp(detectedGestureName, "begin") == 0) {
        currentThreshold = BEGIN_GESTURE_CONFIDENCE_THRESHOLD;
    } else if (currentState == LISTENING_FOR_SPELL) {
        if (strcmp(detectedGestureName, "heal") == 0) {
            currentThreshold = HEAL_GESTURE_CONFIDENCE_THRESHOLD;
        } else if (strcmp(detectedGestureName, "shield") == 0) {
            currentThreshold = SHIELD_GESTURE_CONFIDENCE_THRESHOLD;
        } // For "fire", it will use the default confidenceThreshold (0.60)
    }

    if (maxValue >= currentThreshold) {
        confidentDetection = true;
    }
    
    if (confidentDetection) {
        Serial.print("Detected: ");
        Serial.print(detectedGestureName);
        Serial.print(" (");
        Serial.print(maxValue * 100);
        Serial.println("%)");

        // Filter gestures based on current state (already handled in processDetectedGesture, but good for clarity here)
        if (currentState == IDLE) {
            if (strcmp(detectedGestureName, "begin") == 0) {
                return maxIndex;
            }
        } else if (currentState == LISTENING_FOR_SPELL) {
            if (strcmp(detectedGestureName, "fire") == 0 ||
                strcmp(detectedGestureName, "heal") == 0 ||
                strcmp(detectedGestureName, "shield") == 0) {
                return maxIndex;
            }
        }
        // If gesture is not relevant to current state for action, treat as no detection for state change
        // This logic is now primarily in processDetectedGesture, run_inference just reports a confident detection
        // Serial.println("Gesture detected, but not relevant for immediate action in this state or not the primary expected one.");
        return maxIndex; // Return the index if confident, let processDetectedGesture decide
    } else {
        // Serial.print("No confident gesture detected. Best guess: "); // Optional: uncomment for debugging
        // Serial.print(ei_classifier_inferencing_categories[maxIndex]); // Optional: uncomment for debugging
        // Serial.print(" ("); // Optional: uncomment for debugging
        // Serial.print(maxValue * 100); // Optional: uncomment for debugging
        // Serial.println("%)"); // Optional: uncomment for debugging
        return -1;
    }
}

/**
 * @brief      Start capturing accelerometer data for gesture recognition
 */
void startCapture() {
    sampleCount = 0;
    captureStartTime = millis();
    lastSampleTime = millis();
    Serial.println("Starting gesture capture...");
    Serial.print("Required data points: ");
    Serial.println(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
}

/**
 * @brief      Process a detected gesture based on the current wand state
 */
void processDetectedGesture(int gestureIndex) {
    const char* gestureName = ei_classifier_inferencing_categories[gestureIndex];
    Serial.print("Gesture processed: ");
    Serial.println(gestureName);

    switch (currentState) {
        case IDLE:
            if (strcmp(gestureName, "begin") == 0) {
                Serial.println("State transition: IDLE -> LISTENING_FOR_SPELL");
                currentState = LISTENING_FOR_SPELL;
                stateEnterTime = millis();
                // LED will be updated by updateStateMachine
                // Start capture is implicitly handled by continuous capture logic while in listening states
            }
            break;
        case LISTENING_FOR_SPELL:
            if (strcmp(gestureName, "fire") == 0) {
                Serial.println("State transition: LISTENING_FOR_SPELL -> FIRE_ANIMATION");
                currentState = FIRE_ANIMATION;
                stateEnterTime = millis();
                lastAnimationUpdateTime = millis();
            } else if (strcmp(gestureName, "heal") == 0) {
                Serial.println("State transition: LISTENING_FOR_SPELL -> HEAL_ANIMATION");
                currentState = HEAL_ANIMATION;
                stateEnterTime = millis();
                lastAnimationUpdateTime = millis();
            } else if (strcmp(gestureName, "shield") == 0) {
                Serial.println("State transition: LISTENING_FOR_SPELL -> SHIELD_ANIMATION");
                currentState = SHIELD_ANIMATION;
                stateEnterTime = millis();
                lastAnimationUpdateTime = millis();
            }
            // If a spell is cast, capture will stop due to state change in captureAccelerometerData
            break;
        default:
            // Ignore gestures in other states
            break;
    }
}

/**
 * @brief      Capture accelerometer data for inference
 */
void captureAccelerometerData() {
    // Only capture and infer if in IDLE or LISTENING_FOR_SPELL state
    if (!(currentState == IDLE || currentState == LISTENING_FOR_SPELL)) {
        return; // Do nothing if not in a listening state
    }

    if (millis() - lastSampleTime >= SAMPLE_RATE_MS) {
        lastSampleTime = millis();
        
        // Get accelerometer and gyroscope data
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        // Store data in features array (ax, ay, az, gx, gy, gz, ax, ay, az, gx, gy, gz, ...)
        if (sampleCount < FEATURE_SIZE / 6) {
            int idx = sampleCount * 6;
            // Accelerometer data
            features[idx] = a.acceleration.x;
            features[idx + 1] = a.acceleration.y;
            features[idx + 2] = a.acceleration.z;
            // Gyroscope data - convert from rad/s to deg/s by multiplying with 57.295779513 (180/PI)
            features[idx + 3] = g.gyro.x * 57.295779513;
            features[idx + 4] = g.gyro.y * 57.295779513;
            features[idx + 5] = g.gyro.z * 57.295779513;
            sampleCount++;
        }
        
        // Check if capture duration has elapsed
        if (millis() - captureStartTime >= CAPTURE_DURATION_MS) {
            int detectedGesture = run_inference();
            
            if (detectedGesture != -1) {
                // Process the gesture based on current state
                processDetectedGesture(detectedGesture);
            }
            
            // If still in a listening state, start a new capture immediately
            if (currentState == IDLE || currentState == LISTENING_FOR_SPELL) {
                startCapture();
            } else {
                // Serial.println("Capture paused (not in IDLE or LISTENING_FOR_SPELL state).");
            }
        }
    }
}

/**
 * @brief      Fire spell LED animation
 */
void animateFire() {
    unsigned long currentTime = millis();
    // Cycle red/orange quickly for the first 2/3 of the duration
    if (currentTime - stateEnterTime < (FIRE_ANIMATION_DURATION * 2 / 3)) {
        if ((currentTime / 100) % 2 == 0) {
            leds[0] = CRGB::Red;
        } else {
            leds[0] = CRGB::OrangeRed;
        }
    } else { // Dim for the last 1/3
        uint8_t brightness = map(currentTime - (stateEnterTime + (FIRE_ANIMATION_DURATION * 2 / 3)),
                                 0, FIRE_ANIMATION_DURATION / 3, 255, 0);
        leds[0] = CRGB(brightness, 0, 0); // Dimming red
    }
    FastLED.show();
}

/**
 * @brief      Heal spell LED animation
 */
void animateHeal() {
    // Pulse shades of green
    uint8_t greenVal = beatsin8(30, 80, 255, 0, 0); // BPM, min, max, timebase, phase
    leds[0] = CRGB(0, greenVal, greenVal / 3); // Green with a hint of lighter green/cyan
    FastLED.show();
}

/**
 * @brief      Shield spell LED animation
 */
void animateShield() {
    // Fade between two darker blues
    uint8_t blendAmount = beatsin8(20); // 20 BPM for slow fade
    leds[0] = blend(CRGB(0, 0, 220), CRGB(0, 0, 120), blendAmount); // Changed from silvery-light blue
    FastLED.show();
}

/**
 * @brief      Manages state transitions and actions
 */
void updateStateMachine() {
    unsigned long currentTime = millis();

    switch (currentState) {
        case IDLE:
            FastLED.setBrightness(IDLE_BRIGHTNESS);
            if (currentTime - lastIdleColorChangeTime > IDLE_COLOR_TRANSITION_INTERVAL) {
                lastIdleColorChangeTime = currentTime;
                currentIdleColorIndex = (currentIdleColorIndex + 1) % numIdleColors;
            }
            leds[0] = idleColors[currentIdleColorIndex];
            FastLED.show();
            // Gesture detection handled by captureAccelerometerData
            // Transition to LISTENING_FOR_SPELL is handled by processDetectedGesture
            break;

        case LISTENING_FOR_SPELL:
            FastLED.setBrightness(LISTENING_BRIGHTNESS);
            leds[0] = CRGB::White; // Bright white
            FastLED.show();
            // Gesture detection handled by captureAccelerometerData
            // Transition to ANIMATION is handled by processDetectedGesture
            if (currentTime - stateEnterTime > LISTENING_DURATION) {
                Serial.println("Listening timed out. State transition: LISTENING_FOR_SPELL -> IDLE");
                currentState = IDLE;
                stateEnterTime = millis();
                currentIdleColorIndex = 0; // Reset idle animation
                lastIdleColorChangeTime = millis();
                startCapture(); // Ensure capture restarts
            }
            break;

        case FIRE_ANIMATION:
            FastLED.setBrightness(ANIMATION_BRIGHTNESS);
            animateFire();
            if (currentTime - stateEnterTime > FIRE_ANIMATION_DURATION) {
                Serial.println("State transition: FIRE_ANIMATION -> POST_SPELL");
                currentState = POST_SPELL;
                stateEnterTime = millis();
            }
            break;

        case HEAL_ANIMATION:
            FastLED.setBrightness(ANIMATION_BRIGHTNESS);
            animateHeal();
            if (currentTime - stateEnterTime > HEAL_ANIMATION_DURATION) {
                Serial.println("State transition: HEAL_ANIMATION -> POST_SPELL");
                currentState = POST_SPELL;
                stateEnterTime = millis();
            }
            break;

        case SHIELD_ANIMATION:
            FastLED.setBrightness(ANIMATION_BRIGHTNESS);
            animateShield();
            if (currentTime - stateEnterTime > SHIELD_ANIMATION_DURATION) {
                Serial.println("State transition: SHIELD_ANIMATION -> POST_SPELL");
                currentState = POST_SPELL;
                stateEnterTime = millis();
            }
            break;

        case POST_SPELL:
            FastLED.setBrightness(POST_SPELL_BRIGHTNESS);
            leds[0] = CRGB::LightGoldenrodYellow; // Soft white / pale yellow
            FastLED.show();
            if (currentTime - stateEnterTime > POST_SPELL_DURATION) {
                Serial.println("State transition: POST_SPELL -> IDLE");
                currentState = IDLE;
                stateEnterTime = millis();
                currentIdleColorIndex = 0; // Reset idle animation
                lastIdleColorChangeTime = millis();
                startCapture(); // Ensure capture restarts for "begin"
            }
            break;
    }
}


/**
 * @brief      Arduino setup function
 */
void setup() {
    // Initialize serial
    Serial.begin(115200);
    
    // Initialize I2C with specific pins
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Initialize MPU6050 with retry
    Serial.println("Initializing MPU6050...");
    
    // Try several times to initialize the MPU6050
    bool mpuInitialized = false;
    for (int attempt = 0; attempt < 5; attempt++) {
        if (mpu.begin(0x68, &Wire)) {
            mpuInitialized = true;
            break;
        }
        Serial.print("MPU6050 initialization attempt ");
        Serial.print(attempt + 1);
        Serial.println(" failed, retrying...");
        delay(100);
    }
    
    if (!mpuInitialized) {
        Serial.println("Failed to find MPU6050 chip after multiple attempts");
        Serial.println("Check connections and restart device");
        while (1) {
            delay(100);
        }
    }
    
    // Configure MPU6050 
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    // Initialize FastLED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    // Initial brightness will be set by IDLE state in updateStateMachine
    
    // Initialize state machine
    currentState = IDLE;
    stateEnterTime = millis();
    currentIdleColorIndex = 0;
    lastIdleColorChangeTime = millis(); // Initialize for idle animation
    Serial.println("Wand initialized. Current state: IDLE");
    FastLED.setBrightness(IDLE_BRIGHTNESS); // Set initial brightness
    // The first color of idle animation will be set in the first call to updateStateMachine()
    
    Serial.println("MPU6050 initialized successfully");
    Serial.println("Wand active! Waiting for 'begin' gesture.");
    
    // Start gesture monitoring
    startCapture();
}

/**
 * @brief      Arduino main function
 */
void loop() {
    // Capture accelerometer data if in a listening state
    captureAccelerometerData();
    
    // Update state machine (handles LED updates, timeouts, and state transitions)
    updateStateMachine();
    
    // Small delay to keep things stable, can be adjusted or removed
    // delay(5); // Optional: if needed for stability, but FastLED animations might prefer no delay
}