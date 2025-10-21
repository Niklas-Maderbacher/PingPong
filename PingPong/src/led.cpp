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
  Serial.begin(115200);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  
  FastLED.clear();
  FastLED.show();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'r' || command == 'R') {
      reverseDirection = true;
    } else if (command == 'f' || command == 'F') {
      reverseDirection = false;
    } else if (command == 't' || command == 'T') {
      reverseDirection = !reverseDirection;
    }
    
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  
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
            currentPosition = NUM_LEDS - 1;
          }
        } else {
          currentPosition--;
          if (currentPosition < 0) {
            currentPosition = 0;
          }
        }
        
        previousMillis = currentMillis;
      }
      break;
  }
}