#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "leds.h"
#include "LSM6.h"
#include "ballgame.h"

CRGB leds[NUM_LEDS];
LSM6 imu;


void animate_noise(uint16_t speed, uint16_t scale)
{
    static uint8_t ihue=0;
    static uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
    static uint16_t x = random16();
    static uint16_t y = random16();
    static uint16_t z = random16();


    for (int i = 0; i < MAX_DIMENSION; i++) {
        int ioffset = scale * i;

        for (int j = 0; j < MAX_DIMENSION; j++) {
            int joffset = scale * j;
            noise[i][j] = inoise8(x + ioffset, y + joffset, z);
        }
    }
    z += speed;

    for(int i = 0; i < kMatrixWidth; i++) {
        for(int j = 0; j < kMatrixHeight; j++) {
        // We use the value at the (i,j) coordinate in the noise
        // array for our brightness, and the flipped value from (j,i)
        // for our pixel's hue.
        leds[XY(i, j)] = CHSV(noise[j][i], 255, noise[i][j]);

        // You can also explore other ways to constrain the hue used, like below
        // leds[XY(i,j)] = CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]);
        }
    }
    ihue+=1;
}

void matrix()
{
        // move code downward
        // start with lowest row to allow proper overlapping on each column
        for (int8_t row=kMatrixHeight-1; row>=0; row--)
        {
          for (int8_t col=0; col<kMatrixWidth; col++)
          {
            if (leds[XY(col, row)] == CRGB(175,255,175))
            {
              leds[XY(col, row)] = CRGB(27,130,39); // create trail
              if (row < kMatrixHeight-1) leds[XY(col, row+1)] = CRGB(175,255,175);
            }
          }
        }

        // fade all leds
        for(int i = 0; i < NUM_LEDS; i++) {
          if (leds[i].g != 255) leds[i].nscale8(192); // only fade trail
        }

        // check for empty screen to ensure code spawn
        bool emptyScreen = true;
        for(int i = 0; i < NUM_LEDS; i++) {
          if (leds[i])
          {
            emptyScreen = false;
            break;
          }
        }

        // spawn new falling code
        if (random8(3) == 0 || emptyScreen) // lower number == more frequent spawns
        {
          int8_t spawnX = random8(kMatrixWidth);
          leds[XY(spawnX, 0)] = CRGB(175,255,175 );
        }
}

void rain()
{

}

void fadeall()
{
    for (uint16_t i = 0; i < NUM_LEDS; ++i) {
        leds[i].nscale8(250);
    }
}

void cylon()
{
	static uint8_t hue = 0;
	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}

	// Now go in the other direction.
	for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}

void confetti(uint8_t probability)
{
    static uint8_t hue = 0;
    EVERY_N_MILLIS(500) {
        ++hue;
    }
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);

    if (random8(100) < probability) {
        int pos = random16(NUM_LEDS);
        leds[pos] += CHSV( hue + random8(64), 200, 255);
    }
}

void tester()
{
    for (uint8_t i=0 ; i < kMatrixWidth ; ++i) {
        leds[XY(i, i)] = CRGB::Red;
        FastLED.show();
        delay(100);
    }

    for (uint8_t i=0 ; i < kMatrixWidth ; ++i) {
        leds[XY(kMatrixWidth - i - 1, i)] = CRGB::Green;
        FastLED.show();
        delay(100);
    }
    delay(1000);

    FastLED.clear();
    FastLED.show();
}

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    pinMode(13, OUTPUT);

    LEDS.addLeds<WS2811,PIXEL_PIN,GRB>(leds, NUM_LEDS);
    // LEDS.setBrightness(96);

    if (!imu.init()) {
        Serial.println("Horror");
        while (1);
    }
    imu.enableDefault();
    Serial.println("Hellow");

    // tester();
}

void loop()
{
    // static uint32_t tsLastFrame = 0;
    static uint8_t control_value = 100;

    imu.readAcc();

    // speed = max(xn * 100, 0);
    // scale = max(yn * 400, 0);
    //
    // char report[80];
    // snprintf(report, sizeof(report), "A: %3d %3d %3d", (int16_t)(xn * 100), (int16_t)(yn * 100), (int16_t)(zn * 100));
    // oled.clearDisplay();
    // oled.setCursor(0, 0);
    //
    // oled.println(report);
    // oled.display();

     EVERY_N_MILLIS(500) {
         imu.updateOrientation();
         Serial.print("ax=");
         Serial.print(imu.a.x);
         Serial.print(" ay=");
         Serial.print(imu.a.y);
         Serial.print(" az=");
         Serial.print(imu.a.z);
         Serial.print(" orientation=");
         Serial.println(imu.orientation);
     }

//     EVERY_N_MILLIS(5) {
//         ballgame_render();
//         FastLED.show();
//     }
    // EVERY_N_MILLIS(500) {
    //     if (control_value != 5) {
    //         --control_value;
    //         Serial.println(control_value);
    //     }
    // }
    //
//    EVERY_N_MILLIS(33) {
//        matrix();
//        // animate_noise(5, 200);
//        // confetti(control_value);
//        // cylon();
//        FastLED.show();
//    }
    // snprintf(report, sizeof(report), "A: %3d %3d %3d", (int16_t)(imu.a.x * 100), (int16_t)(imu.a.y * 100), (int16_t)(imu.a.z * 100));
    // Serial.println(report);
    // FastLED.delay(1000 / 10.0);
}
