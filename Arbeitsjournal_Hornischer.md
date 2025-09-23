# Arbeitsjournal
~ Kilian Hornischer

### 23.09.2025

**LED Hardware**
- WS2811S-M30-LW67 LED strip (Datasheet: https://www.tme.eu/Document/26d574b43ad9ddaffa4d5bcd140ec145/WS2811.pdf)
- 30 LEDs per meter, using WS2811 protocol
- C++ code to control it

**Set Up Development Environment**
- Started with Arduino IDE 
- Transitioned to PlatformIO
- library installation methods for both environments

**Library Selection & Installation**
- Chose FastLED library (recommended for performance) (fastled/FastLED@^3.6.0)
- Alternative: Adafruit NeoPixel library
- PlatformIO dependency management via `platformio.ini`

**Code Implementation**
- Created examples for LED control
- Externer Power Supply missing to test
