#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 60
#define DATA_PIN D1
#define SWITCH_PIN D2

CRGB leds[NUM_LEDS];

unsigned long previousMillis = 0;
const long interval = 50;
unsigned long currentMillis = 0;

int currentPosition = 0;
bool reverseDirection = false;

bool running = true;

bool currentSwitchState = HIGH;
bool oldSwitchState = LOW;

// Timing window settings
const int TIMING_WINDOW_SIZE = 5; // Number of LEDs at start/end where button must be pressed
const unsigned long TIMING_WINDOW_START = 500; // Start of timing window (ms)
const unsigned long TIMING_WINDOW_END = 2000;  // End of timing window (ms)
unsigned long timingWindowStartTime = 0;
bool inTimingWindow = false;
bool timingWindowActive = false;

enum LEDState
{
  RUNLIGHT_ACTIVE
};
LEDState currentState = RUNLIGHT_ACTIVE;

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  currentSwitchState = digitalRead(SWITCH_PIN);

  FastLED.clear();
  FastLED.show();

  Serial.println("=== LED Lauflicht Test startet ===");
  Serial.println("Setup abgeschlossen");
  Serial.print("Initiale Schalterzustand: ");
  Serial.println(currentSwitchState == HIGH ? "HIGH (nicht gedrückt)" : "LOW (gedrückt)");
  Serial.print("Initiale Richtung: ");
  Serial.println(reverseDirection ? "Rückwärts" : "Vorwärts");
  Serial.print("Aktuelle Position: ");
  Serial.println(currentPosition);
  Serial.print("Timing Fenster: ");
  Serial.print(TIMING_WINDOW_START);
  Serial.print(" - ");
  Serial.print(TIMING_WINDOW_END);
  Serial.println(" ms");
  Serial.print("Timing Bereich: erste/letzte ");
  Serial.print(TIMING_WINDOW_SIZE);
  Serial.println(" LEDs");
  Serial.println("==================================");
}

void checkTimingWindow()
{
  currentSwitchState = digitalRead(SWITCH_PIN);

  // Check if we're entering a timing window position
  if (running && !timingWindowActive) 
  {
    bool nearStart = (!reverseDirection && currentPosition >= NUM_LEDS - TIMING_WINDOW_SIZE);
    bool nearEnd = (reverseDirection && currentPosition < TIMING_WINDOW_SIZE);
    
    if (nearStart || nearEnd) 
    {
      timingWindowActive = true;
      inTimingWindow = false;
      timingWindowStartTime = currentMillis;
      Serial.println("*** Timing Fenster aktiv - bereite Button-Press vor ***");
      Serial.print("Drücke den Button zwischen ");
      Serial.print(TIMING_WINDOW_START);
      Serial.print(" und ");
      Serial.print(TIMING_WINDOW_END);
      Serial.println(" ms für Fortsetzung!");
    }
  }

  // Handle timing window logic
  if (timingWindowActive) 
  {
    unsigned long windowElapsed = currentMillis - timingWindowStartTime;
    
    // Check if we're in the valid timing window
    if (windowElapsed >= TIMING_WINDOW_START && windowElapsed <= TIMING_WINDOW_END) 
    {
      if (!inTimingWindow) 
      {
        inTimingWindow = true;
        Serial.println(">>> TIMING FENSTER JETZT AKTIV - BUTTON DRÜCKEN! <<<");
      }
      
      // Visual feedback for timing window
      if (windowElapsed % 200 < 100) 
      {
        // Blink timing window LEDs in yellow
        for (int i = 0; i < TIMING_WINDOW_SIZE; i++) 
        {
          if (!reverseDirection) 
          {
            leds[NUM_LEDS - 1 - i] = CRGB::Yellow;
          } 
          else 
          {
            leds[i] = CRGB::Yellow;
          }
        }
        FastLED.show();
      }

      // Check for button press during timing window
      if (currentSwitchState == LOW && oldSwitchState == HIGH) 
      {
        // Successful timing!
        reverseDirection = !reverseDirection;
        oldSwitchState = LOW;
        
        if (reverseDirection) 
        {
          currentPosition = NUM_LEDS - 1;
        } 
        else 
        {
          currentPosition = 0;
        }
        
        timingWindowActive = false;
        inTimingWindow = false;
        running = true;
        
        Serial.println("*** ERFOLG! Richtung geändert - Lauflicht geht weiter! ***");
        Serial.print("Neue Richtung: ");
        Serial.println(reverseDirection ? "Rückwärts" : "Vorwärts");
        
        // Clear any timing window visuals
        FastLED.clear();
        return;
      }
    } 
    else if (windowElapsed > TIMING_WINDOW_END) 
    {
      // Timing window expired - stop the runlight
      timingWindowActive = false;
      inTimingWindow = false;
      running = false;
      
      Serial.println("*** FEHLER! Timing Fenster abgelaufen - Lauflicht gestoppt! ***");
      Serial.println("*** Drücke Button um neu zu starten ***");
      
      // Show failure indication (all red briefly)
      for (int i = 0; i < NUM_LEDS; i++) 
      {
        leds[i] = CRGB::Red;
      }
      FastLED.show();
      delay(1000);
      FastLED.clear();
      FastLED.show();
    }
  }

  // Regular button press (outside timing windows) to restart
  if (!running && !timingWindowActive && currentSwitchState == LOW && oldSwitchState == HIGH) 
  {
    // Reset to start
    reverseDirection = false;
    currentPosition = 0;
    running = true;
    oldSwitchState = LOW;
    
    Serial.println("*** Neustart vom Anfang ***");
  }

  // Update old switch state
  if (currentSwitchState == HIGH) 
  {
    oldSwitchState = HIGH;
  }
}

void light_led()
{
  switch (currentState)
  {
  case RUNLIGHT_ACTIVE:
    if (currentMillis - previousMillis >= interval && running && !timingWindowActive)
    {
      FastLED.clear();

      // Main LED
      leds[currentPosition] = CRGB::Red;

      // Trail effect
      for (int i = 1; i <= 3; i++)
      {
        int trailPos = currentPosition - (reverseDirection ? -i : i);
        if (trailPos >= 0 && trailPos < NUM_LEDS)
        {
          leds[trailPos] = CRGB(255 / (i * 2), 0, 0);
        }
      }

      // Highlight timing window area
      if (running) 
      {
        for (int i = 0; i < TIMING_WINDOW_SIZE; i++) 
        {
          if (!reverseDirection && currentPosition >= NUM_LEDS - TIMING_WINDOW_SIZE) 
          {
            leds[NUM_LEDS - 1 - i] = CRGB::Blue; // Blue for upcoming timing window
          } 
          else if (reverseDirection && currentPosition < TIMING_WINDOW_SIZE) 
          {
            leds[i] = CRGB::Blue; // Blue for upcoming timing window
          }
        }
      }

      FastLED.show();

      if (currentPosition % 10 == 0)
      {
        Serial.print("LED Position: ");
        Serial.print(currentPosition);
        Serial.print(" | Richtung: ");
        Serial.print(reverseDirection ? "Rückwärts" : "Vorwärts");
        Serial.print(" | Laufend: ");
        Serial.print(running ? "Ja" : "Nein");
        Serial.print(" | Timing Fenster: ");
        Serial.println(timingWindowActive ? "Aktiv" : "Inaktiv");
      }

      // Move position
      if (!reverseDirection)
      {
        currentPosition++;
        if (currentPosition >= NUM_LEDS)
        {
          currentPosition = NUM_LEDS; // Set to end to trigger timing window
        }
      }
      else
      {
        currentPosition--;
        if (currentPosition < 0)
        {
          currentPosition = -1; // Set to before start to trigger timing window
        }
      }

      previousMillis = currentMillis;
    }
    break;
  }
}

void loop()
{
  currentMillis = millis();

  checkTimingWindow();
  light_led();

  static unsigned long lastStatusReport = 0;
  if (currentMillis - lastStatusReport >= 5000)
  {
    lastStatusReport = currentMillis;
    Serial.println("=== Status Report ===");
    Serial.print("Aktuelle Position: ");
    Serial.println(currentPosition);
    Serial.print("Richtung: ");
    Serial.println(reverseDirection ? "Rückwärts" : "Vorwärts");
    Serial.print("Lauflicht aktiv: ");
    Serial.println(running ? "Ja" : "Nein");
    Serial.print("Timing Fenster aktiv: ");
    Serial.println(timingWindowActive ? "Ja" : "Nein");
    if (timingWindowActive) 
    {
      Serial.print("Timing verbleibend: ");
      Serial.print(TIMING_WINDOW_END - (currentMillis - timingWindowStartTime));
      Serial.println(" ms");
    }
    Serial.print("Schalterzustand: ");
    Serial.println(digitalRead(SWITCH_PIN) == HIGH ? "HIGH (nicht gedrückt)" : "LOW (gedrückt)");
    Serial.println("=====================");
  }
}