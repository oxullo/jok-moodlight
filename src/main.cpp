#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "leds.h"
#include "LSM6.h"
#include "ballgame.h"
#include "rain.h"
#include "confetti.h"
#include "noise.h"
#include "swirl.h"
#include "amdx.h"
#include "banner.h"


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

void play_alogo()
{
    FastLED.clear();
    for (uint8_t x=0 ; x < kMatrixWidth ; ++x) {
        uint8_t column = pgm_read_byte(&(alogo[x]));

        for (uint8_t y=0 ; y < kMatrixHeight ; ++y) {
            if (column & (1 << y)) {
                leds[XY(x, y)] = CHSV(0, 0, 100);
            }
        }
    }

    FastLED.show();
    delay(2000);
}

void play_banner()
{
    FastLED.clear();

    for (uint16_t pos = 0 ; pos < BANNER_COLS - 7 ; ++pos) {
        FastLED.clear();
        for (uint8_t x=0 ; x < kMatrixWidth ; ++x) {
            uint8_t column = pgm_read_byte(&(banner[x+pos]));

            for (uint8_t y=0 ; y < kMatrixHeight ; ++y) {
                if (column & (1 << y)) {
                    if (pos == BANNER_COLS - 8) {
                        leds[XY(x, y)] = CRGB(190, 0, 0);
                    } else {
                        leds[XY(x, y)] = CHSV(0, 0, 100);
                    }
                }
            }
        }

        FastLED.show();
        if (pos == 0) {
            delay(1000);
        } else {
            delay(66);
        }
    }
    delay(1000);
}

void flashlight()
{
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);

    LEDS.addLeds<WS2811,PIXEL_PIN,GRB>(leds, NUM_LEDS);

    Wire.begin();
    if (!imu.init()) {
        while (1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(1000);
        }
    }
    imu.enableDefault();

    // tester();
    play_alogo();
}

void loop()
{
    static IMUOrientation currentOrientation = IMUORIENTATION_UNKNOWN;
    imu.update();

    if (imu.hasBeenKnocked()) {
        play_banner();
    }

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
                rain.render();
            }
            break;

        case IMUORIENTATION_VERTICAL_90CW:
            EVERY_N_MILLIS(33) {
                confetti.render();
            }
            break;

        case IMUORIENTATION_VERTICAL_90CCW:
            EVERY_N_MILLIS(16) {
                noise.render();
            }
            break;

        case IMUORIENTATION_VERTICAL_180:
            EVERY_N_MILLIS(33) {
                swirl.render();
            }
            break;

        case IMUORIENTATION_HORIZONTAL_TOP:
            EVERY_N_MILLIS(5) {
                ballgame.render();
            }
            break;

        case IMUORIENTATION_HORIZONTAL_BOTTOM:
            EVERY_N_MILLIS(33) {
                flashlight();
            }
            break;

        case IMUORIENTATION_UNKNOWN:
//            EVERY_N_MILLIS(66) {
//                FastLED.clear();
//                FastLED.show();
//            }
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
