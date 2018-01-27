#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "leds.h"
#include "LSM6.h"
#include "ballgame.h"
#include "rain.h"
#include "confetti.h"
#include "noise.h"


CRGB leds[NUM_LEDS];
LSM6 imu;


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
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);

    Wire.begin();

    pinMode(LED_BUILTIN, OUTPUT);

    LEDS.addLeds<WS2811,PIXEL_PIN,GRB>(leds, NUM_LEDS);

    if (!imu.init()) {
        Serial.println("Horror");
        while (1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(1000);
        }
    }
    imu.enableDefault();
    Serial.println("Hellow");

    // tester();
}

void loop()
{
    static IMUOrientation currentOrientation = IMUORIENTATION_UNKNOWN;
    imu.readAcc();

    EVERY_N_MILLIS(100) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

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
                noise_render(5, 100);
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
