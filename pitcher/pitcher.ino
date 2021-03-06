//#include <OctoWS2811.h>
#include <FastLED.h>
#include <Arduino.h>
#include <HCSR04.h>

const int numPins = 1;
byte pinList[numPins] = {10};
const int ledsPerStrip = 144;
const int numStrips = 1;
unsigned int led = 0;
const int cooling = 55;
const int sparking = 120;
const int fadeAmount = 255;
const int PATIENCE = 5000;
const int TIME_UP = 5000;
const int TIME_DOWN = 2000;
const int BRIGHT_MAX = 255;
const int BRIGHT_MIN = 1;
int currentY = BRIGHT_MAX;

CRGB leds[numStrips * ledsPerStrip];

UltraSonicDistanceSensor distanceSensor(0, 1);
float data = 0.0;
bool reverseDirection = false;
unsigned long someone_was_near = 0;
unsigned long time_now = 0;


// These buffers need to be large enough for all the pixels.
// The total number of pixels is "ledsPerStrip * numPins".
// Each pixel needs 3 bytes, so multiply by 3.  An "int" is
// 4 bytes, so divide by 4.  The array is created using "int"
// so the compiler will align it to 32 bit memory.
//DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
//int drawingMemory[ledsPerStrip * numPins * 3 / 4];

void setup()
{
  FastLED.addLeds<numStrips, WS2812B, 10, GRB>(leds, ledsPerStrip);
  FastLED.setBrightness(100);
  Serial.println("setup");
}

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[ledsPerStrip];
  int y = 255;
  int logy = 0;
  unsigned long t = 0;

// Step 1.  Cool down every cell a little
  for( int i = 0; i < ledsPerStrip; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / ledsPerStrip) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= ledsPerStrip - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < sparking ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  time_now = millis();
  t = time_now - someone_was_near; // elapsed time

  Serial.println(t);
  if (t > 0 && t < TIME_DOWN) {
    y = ((BRIGHT_MAX - BRIGHT_MIN) / (float)TIME_DOWN) * t;
  } else if (t > 0 && t < TIME_DOWN + PATIENCE) {
    y = BRIGHT_MAX - BRIGHT_MIN;
  } else if (t > TIME_DOWN + PATIENCE && t < TIME_DOWN + PATIENCE + TIME_UP) {
    y = BRIGHT_MAX - ((BRIGHT_MAX - BRIGHT_MIN) / (float)TIME_UP) * (t - PATIENCE - TIME_DOWN);
  } else if (t > TIME_DOWN + PATIENCE + TIME_UP) {
    y = 1;
  }
  Serial.println("log(y)=");
  logy = 46 * log(y);
  Serial.println(logy);
  //Serial.println(y);
    
  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < ledsPerStrip; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    if( reverseDirection ) {
      pixelnumber = (ledsPerStrip-1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;

    if (logy > 64)
      leds[pixelnumber].fadeToBlackBy(random(logy, BRIGHT_MAX));
    else
      leds[pixelnumber].fadeToBlackBy(logy);
  }

  FastLED.show();

}

void loop() {
  //delay(50);
  
  //Serial.println("Fire");
  // put your main code here, to run repeatedly:

  data = distanceSensor.measureDistanceCm();  
  if (data > 2.0) {
    Serial.println(data);
    someone_was_near = millis();
  }
  
  Fire2012();
  //unsigned int ii = 0;

  /*if (data > 2.0) {
    Serial.println(data);
    delay(100);
  }*/

  /*if (msec >= 45) {
    if (data > 0.0) {
      for (ii = 0; ii < ledsPerStrip; ++ii) {
        leds.setPixel(led, 255 * data * 3, 0, 0);
      }
      //Serial.println(255 * data * 3);
    }
    
    msec = 0;
    leds.setPixel(led, 0, 100, 0);
    ++led;
    if (led > 1) {
      //leds.setPixel(led-2, 0, 0, 0);
    } else if (led >= ledsPerStrip) {
      led = 0;
    }
    //Serial.println(led);
  }
  Serial.println("loop");
  delay(100);*/
  FastLED.show();
}
