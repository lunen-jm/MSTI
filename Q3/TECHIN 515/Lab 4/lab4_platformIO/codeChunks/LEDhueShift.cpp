#include <FastLED.h>

#define LED_PIN     D10
#define NUM_LEDS    1

CRGB leds[NUM_LEDS];
uint8_t hue = 0;  // Variable to keep track of the current hue

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(150);  // Set brightness (0-255)
}

void loop() {
  // Fill the LED with a rainbow color that cycles
  leds[0] = CHSV(hue, 255, 255);  // Full saturation and value, changing hue
  
  // Show the colors
  FastLED.show();
  
  // Increment the hue to cycle through the rainbow
  hue++;  // This will automatically wrap around at 255 back to 0
  
  // Small delay to control animation speed
  delay(15);  // Lower values make the colors change faster
}