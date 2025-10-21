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

bool currentSwitchState = HIGH; // Start mit nicht-gedrückt (wegen INPUT_PULLUP)
bool oldSwitchState = LOW;

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

  // Initialisiere den letzten Schalterzustand
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
  Serial.println("==================================");
}

void show_end_zone()
{
  leds[0] = CRGB::Blue;
  leds[NUM_LEDS - 1] = CRGB::Blue;
}

void switch_direction()
{
  // Aktuellen Schalterzustand lesen
  currentSwitchState = digitalRead(SWITCH_PIN);

  // Debug-Ausgabe für Schalterzustandsänderungen
  if (currentSwitchState == oldSwitchState)
  {
    Serial.print("Schalterzustand geändert: ");
    Serial.print(currentSwitchState == HIGH ? "HIGH" : "LOW");
    Serial.print(" -> ");
    Serial.print(currentSwitchState == HIGH ? "HIGH" : "LOW");
    Serial.print(" | Millis: ");
    Serial.println(currentMillis);

    if (digitalRead(SWITCH_PIN) == currentSwitchState)
    { // Immer noch gedrückt = gültiger Tastendruck
      reverseDirection = !reverseDirection;
      oldSwitchState = !currentSwitchState;

      if (reverseDirection)
      {
        currentPosition = NUM_LEDS - 1;
      }
      else
      {
        currentPosition = 0;
      }

      running = true;

      Serial.print("Richtung geändert: ");
      Serial.println(reverseDirection ? "Rückwärts" : "Vorwärts");
      Serial.print("Neue Startposition: ");
      Serial.println(currentPosition);
      Serial.println("Lauflicht gestartet");
    }
    else
    {
      Serial.println("Tastendruck war Preller - ignoriert");
    }
  }
}

void light_led()
{
  switch (currentState)
  {
  case RUNLIGHT_ACTIVE:
    if (currentMillis - previousMillis >= interval && running)
    {
      FastLED.clear();

      leds[currentPosition] = CRGB::Red;

      for (int i = 1; i <= 3; i++)
      {
        int trailPos = currentPosition - (reverseDirection ? -i : i);
        if (trailPos >= 0 && trailPos < NUM_LEDS)
        {
          leds[trailPos] = CRGB(255 / (i * 2), 0, 0);
        }
      }

      FastLED.show();

      // Debug-Ausgabe für jede 10. LED-Bewegung
      if (currentPosition % 10 == 0)
      {
        Serial.print("LED Position: ");
        Serial.print(currentPosition);
        Serial.print(" | Richtung: ");
        Serial.print(reverseDirection ? "Rückwärts" : "Vorwärts");
        Serial.print(" | Laufend: ");
        Serial.println(running ? "Ja" : "Nein");
      }

      if (!reverseDirection)
      {
        currentPosition++;
        leds[currentPosition - 1] = CRGB::Black;
        if (currentPosition >= NUM_LEDS - 1)
        {
          currentPosition = NUM_LEDS - 2;
          running = false;
          Serial.println("*** Ende erreicht - Lauflicht gestoppt (Vorwärts) ***");
        }
      }
      else
      {
        currentPosition--;
        leds[currentPosition + 2] = CRGB::Black;
        if (currentPosition < 1)
        {
          currentPosition = 1;
          running = false;
          Serial.println("*** Anfang erreicht - Lauflicht gestoppt (Rückwärts) ***");
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

  show_end_zone();
  switch_direction();
  light_led();

  // Zusätzliche Statusausgabe alle 5 Sekunden
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
    Serial.print("Schalterzustand: ");
    Serial.println(digitalRead(SWITCH_PIN) == HIGH ? "HIGH (nicht gedrückt)" : "LOW (gedrückt)");
    Serial.println("=====================");
  }
}