#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 60
#define DATA_PIN D1

CRGB leds[NUM_LEDS];

unsigned long previousMillis = 0;
const long interval = 50;

int currentPosition = 0;
bool reverseDirection = false;

enum LEDState {
  RUNLIGHT_ACTIVE
};
LEDState currentState = RUNLIGHT_ACTIVE;

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  
  FastLED.clear();
  FastLED.show();
}

void loop() {
  unsigned long currentMillis = millis();
  
  switch (currentState) {
    case RUNLIGHT_ACTIVE:
      if (currentMillis - previousMillis >= interval) {
        FastLED.clear();
        
        leds[currentPosition] = CRGB::Red;
        
        for (int i = 1; i <= 3; i++) {
          int trailPos = currentPosition - i;
          if (trailPos >= 0) {
            leds[trailPos] = CRGB(255/(i*2), 0, 0);
          }
        }
        
        FastLED.show();
        
        if (!reverseDirection) {
          currentPosition++;
          if (currentPosition >= NUM_LEDS) {
            reverseDirection = true;
            currentPosition = NUM_LEDS - 2;
          }
        } else {
          currentPosition--;
          if (currentPosition < 0) {
            reverseDirection = false;
            currentPosition = 1;
          }
        }
        
        previousMillis = currentMillis;
      }
      break;
  }
}