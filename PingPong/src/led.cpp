#include <Arduino.h>
#include <FastLED.h>

const int numLeds = 8;
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
+  for (int i = 0; i < numLeds; i++) {
    for (int j = 0; j < numLeds; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    digitalWrite(ledPins[i], HIGH);
    delay(100);
  }
  
  for (int i = numLeds - 1; i >= 0; i--) {
    for (int j = 0; j < numLeds; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    digitalWrite(ledPins[i], HIGH);
    delay(100);
  }
}
