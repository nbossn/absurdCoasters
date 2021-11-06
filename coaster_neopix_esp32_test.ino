#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

int fsrPin = 4;
int ledPin = 2;

const int LED_COUNT = 16;
const float SPEED_FACTOR = 0.008;
const int STEP_DELAY = 5;
Adafruit_NeoPixel strip(LED_COUNT, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(250); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int analogReading = analogRead(fsrPin);
  Serial.print("Force sensor reading = ");
  Serial.print(analogReading); // print the raw analog reading

  if (analogReading < 10)       // from 0 to 9
  {
    Serial.println(" -> no pressure");
  }
  else if (analogReading < 200) // from 10 to 199
  {
    digitalWrite(ledPin, LOW);
  }
  else if (analogReading < 500) // from 200 to 499
  {
    Serial.println(" -> light squeeze");
  }
  else if (analogReading < 800) // from 500 to 799
  {
    Serial.println(" -> medium squeeze");
  }
  else // from 800 to 1023
  {
    Serial.println(" -> big squeeze");
    breathe(SPEED_FACTOR, STEP_DELAY);
  }
}
  
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void breathe(float speedFactor, int stepDelay) {
  for (int i = 0; i < 65535; i++) {
    float intensity = 250 /2.0 * (1.0 + sin(speedFactor * i));
    strip.setBrightness(intensity);
    for (int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 255);
      }
      strip.show();
      delay(stepDelay);
  }
}
