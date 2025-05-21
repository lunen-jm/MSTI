# Lab 4: Gesture Recognition Wand with ESP32 and Edge Impulse

## Overview

This project implements a gesture recognition wand using an ESP32-C3 microcontroller (Seeed Studio Xiao), an MPU6050 accelerometer and gyroscope, and a WS2812 RGB LED. The wand is trained to recognize several distinct gestures ("begin", "fire", "shield", "heal") using a machine learning model developed with Edge Impulse. Upon recognizing a gesture, the wand provides visual feedback by changing the color of the LED.

The primary goal is to continuously monitor sensor data, perform inference using the onboard ML model, and react to detected gestures in real-time.

## Hardware Components

*   **Microcontroller:** Seeed Studio Xiao ESP32-C3
*   **Motion Sensor:** MPU6050 (6-axis accelerometer and gyroscope)
*   **Visual Feedback:** WS2812 RGB LED (Neopixel)
*   **Wiring:** Connected via I2C for the MPU6050 and a digital pin for the LED.

## Software, Libraries, and Tools

*   **Development Environment:** PlatformIO with Visual Studio Code
*   **Firmware:** Custom C++ application (`lab4_platformIO/src/main.cpp`)
*   **Machine Learning:** Edge Impulse
    *   **Model Library:** `wand_better_inferencing` (custom Edge Impulse Arduino library using 6-axis data)
*   **Key Arduino Libraries:**
    *   `Adafruit_MPU6050`: For interfacing with the MPU6050 sensor.
    *   `Adafruit_Unified_Sensor`: Dependency for the MPU6050 library.
    *   `FastLED`: For controlling the WS2812 RGB LED.
*   **Data Collection (Initial Phase):**
    *   Arduino sketch (`gesture_capture/gesture_capture.ino`) for raw data logging.
    *   Python scripts (`gesture_capture/process_gesture_data.py`, `gesture_capture/test_serial.py`) for data processing and serial communication during data collection.

## Functionality

1.  **Initialization:**
    *   The ESP32 initializes the MPU6050 sensor, the FastLED library, and serial communication.
    *   It uses a custom partition table (`huge_app.csv`) to ensure enough flash space for the application and ML model.
2.  **Continuous Data Capture:**
    *   The device continuously samples data from the MPU6050 (accelerometer and gyroscope - 6 axes) at a rate of 100Hz (10ms interval).
    *   Data is collected for a window of 1.2 seconds to gather enough samples for one inference cycle.
3.  **Gesture Inference:**
    *   Once a sufficient number of samples (600 data points: 100 samples * 6 axes) are collected, the `run_classifier` function from the Edge Impulse library is invoked.
    *   The model processes the raw sensor data and outputs classification scores for the predefined gestures.
4.  **LED Feedback:**
    *   If a gesture is detected with a confidence score above a set threshold (e.g., 60%), the `main.cpp` code identifies the gesture:
        *   "begin": White LED
        *   "fire": Red LED
        *   "shield": Blue LED
        *   "heal": Green LED
    *   If no gesture is confidently detected, the LED turns off.
5.  **Loop:** The capture-inference-feedback cycle repeats continuously, allowing the wand to respond to new gestures.

## Project Structure Highlights

*   `Lab 4/` (Root directory: `c:\Users\jaden\Documents\GitHub\MSTI\Q3\TECHIN 515\Lab 4\`)
    *   `README.md`: This file.
    *   `discussion.md`: Project notes and discussion points.
    *   `data/`: Contains subdirectories for gesture data used for training the Edge Impulse model (e.g., `data/begin/`).
    *   `gesture_capture/`:
        *   `gesture_capture.ino`: Arduino sketch used for collecting raw gesture data via serial.
        *   Python scripts (e.g., `process_gesture_data.py`) for processing and managing collected data.
        *   `data/`: CSV files of captured gesture data (e.g., `gesture_capture/data/begin/output_begin_user_1_20250520_143000.csv`).
    *   `images/`: Contains images related to the project (e.g., `features.png`, `modelTest.png`).
    *   `lab4_platformIO/`: The main PlatformIO project for the wand firmware.
        *   `src/main.cpp`: The primary application code running on the ESP32.
        *   `platformio.ini`: PlatformIO project configuration file, defining the board, framework, libraries, and build flags.
        *   `huge_app.csv`: Custom ESP32 partition table to allocate more space for the application and ML model.
        *   `lib/wand_better_inferencing/`: The Edge Impulse Arduino library containing the ML model and necessary SDK files.
        *   `codeChunks/`: Contains various C++ code snippets, likely from different stages of development or for testing specific functionalities.
    *   `wand/wand.ino`: An earlier or alternative Arduino sketch for the wand functionality.

## Setup and Build (PlatformIO Project)

1.  Ensure you have Visual Studio Code with the PlatformIO IDE extension installed.
2.  Open the `Lab 4/lab4_platformIO/` folder as a PlatformIO project.
3.  The necessary libraries (Adafruit MPU6050, FastLED, and the local `wand_better_inferencing`) should be automatically detected or are specified in `platformio.ini`.
4.  Build and upload the project to the Seeed Studio Xiao ESP32-C3.

## Development Notes

*   The project evolved from potentially more complex state-based logic to a simpler, continuous gesture detection loop.
*   Significant effort was made to clean up the project structure, ensuring that PlatformIO uses a single, correct version of the Edge Impulse library.
*   The use of a custom partition table (`huge_app.csv`) is critical for accommodating the size of the TensorFlow Lite model on the ESP32's flash memory.
*   The MPU6050 gyroscope data is converted from radians/second to degrees/second before being fed to the model.

---
*Generated on May 20, 2025.*