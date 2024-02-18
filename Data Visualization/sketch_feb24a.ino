#include <FastLED.h>
#define NUM_LEDS 66

CRGB leds[NUM_LEDS];
CRGB colors[] = {CRGB::Green, CRGB::Yellow, CRGB::Orange, CRGB::Red, CRGB::Purple, CRGB::Green};


#define PIN 5

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  Serial.begin(9600);
}

int n = 0;

void loop() {
  int pInd = (11 * (n - 1) + 66) % 66;
  for(int i = 0; i < 11; i++) {
    leds[i + pInd] = CRGB(0,0,0);
    leds[(i + pInd + 11) % 66] = colors[n%6];
  }
  FastLED.show();
  delay(500);
  n++;
}
