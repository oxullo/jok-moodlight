#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "leds.h"
#include "LSM6.h"
#include "ballgame.h"
#include "rain.h"
#include "confetti.h"

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

void flashlight()
{
    fill_solid(leds, NUM_LEDS, CRGB::White);
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
    static IMUOrientation currentOrientation = IMUORIENTATION_UNKNOWN;
    imu.readAcc();

    EVERY_N_MILLIS(500) {
        IMUOrientation newOrientation = imu.getOrientation();

        if (newOrientation != currentOrientation && newOrientation != IMUORIENTATION_UNKNOWN) {
            currentOrientation = newOrientation;
        }
    }

    switch (currentOrientation) {
        case IMUORIENTATION_VERTICAL_NORMAL:
            EVERY_N_MILLIS(16) {
                rain_render();
                FastLED.show();
            }
            break;

        case IMUORIENTATION_VERTICAL_90CW:
            EVERY_N_MILLIS(33) {
                confetti_render(30);
                FastLED.show();
            }
            break;

        case IMUORIENTATION_VERTICAL_90CCW:
            EVERY_N_MILLIS(16) {
                animate_noise(5, 100);
                FastLED.show();
            }
            break;

        case IMUORIENTATION_VERTICAL_180:
            break;

        case IMUORIENTATION_HORIZONTAL_TOP:
            EVERY_N_MILLIS(5) {
                ballgame_render();
                FastLED.show();
            }
            break;

        case IMUORIENTATION_HORIZONTAL_BOTTOM:
            EVERY_N_MILLIS(33) {
                flashlight();
                FastLED.show();
            }
            break;

        case IMUORIENTATION_UNKNOWN:
            EVERY_N_MILLIS(66) {
                FastLED.clear();
                FastLED.show();
            }
            break;
    }

//     EVERY_N_MILLIS(500) {
//         Serial.print("ax=");
//         Serial.print(imu.a.x);
//         Serial.print(" ay=");
//         Serial.print(imu.a.y);
//         Serial.print(" az=");
//         Serial.print(imu.a.z);
//         Serial.print(" orientation=");
//         Serial.println(imu.getOrientation());
//         imu.debugOrientationCounters();
//     }


}
