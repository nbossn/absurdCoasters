#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// IO pins
int fsrPin = 4;
int ledPin = 2;

// num LEDs in neo_pix strip
const int LED_COUNT = 16;

// const for the breathe effect
const float SPEED_FACTOR = 0.008;
const int STEP_DELAY = 5;

// const for rainbow effect
unsigned long speedInterval = 100;
unsigned long lastUpdate = 0;
unsigned long intervals [] = {20, 20, 50, 100, 30};
int fadeStep = 0;
int numberOfCases = 3;

Adafruit_NeoPixel strip(LED_COUNT, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  wipe();
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(250); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  // read fsr and switch state based off range
  static int pattern = 0, lastReading;
  Serial.println(pattern);
  int analogReading = analogRead(fsrPin);
  if (analogReading < 10) {
    Serial.println(analogReading);
    pattern = 0;
    fadeStep = 0;
    delay(50);
  }
  else if (analogReading < 3000) {
    Serial.println(analogReading);
    pattern = 1;
    fadeStep = 0;
    delay(50);
  }
  else {
    Serial.println(analogReading);
    pattern = 2;
    fadeStep = 0;
    delay(50);
  }
  // update millis value from last state change
  lastReading = analogReading;
  if (millis() - lastUpdate > speedInterval) {
    updatePattern(pattern);
  }
}

void updatePattern(int pat) {
  switch (pat) {
    case 0:
      wipe();
      strip.show();
      break;
    case 1:
      stateRainbow();
      break;
    case 2:
      stateBreathe(SPEED_FACTOR);
      break;
  }
}

void rainbow(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      int pixelHue = firstPixelHue + (i * 65536 / strip.numPixels());
      // strip.gamma32 for truer color values in HSV
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void stateRainbow() {
  static uint16_t j = 0;
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    j++;
    if (j >= 256 * 5) {
      j = 0;
    }
    lastUpdate = millis();
  }
}

void breathe(float speedFactor, int stepDelay) {
  for (int i = 0; i < 65535; i++) {
    float intensity = 250 / 2.0 * (1.0 + sin(speedFactor * i));
    strip.setBrightness(intensity);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }
    strip.show();
    delay(stepDelay);
  }
}

void stateBreathe(float speedFactor) {
  static uint16_t j = 0;
  for (int i = 0; i <  65535; i++) {
    float intensity = 250 / 2.0 * (1.0 + sin(speedFactor * i));
    strip.setBrightness(intensity);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }

    strip.show();
    j++;
    if (j >= 256 * 5) {
      j = 0;
    }
    lastUpdate = millis();
  }
}

void colorWipe(uint32_t c) { // modified from Adafruit example to make it a state machine
  static int i = 0;
  strip.setPixelColor(i, c);
  strip.show();
  i++;
  if (i >= strip.numPixels()) {
    i = 0;
    wipe(); // blank out strip
  }
  lastUpdate = millis(); // time for next change to the display
}


void wipe() { // clear all LEDs
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}
