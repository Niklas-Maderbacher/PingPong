#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 60
#define DATA_PIN D3

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
  leds[0] = CRGB::Red;
}