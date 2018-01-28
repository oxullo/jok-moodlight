#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "leds.h"
#include "LSM6.h"
#include "animator.h"
#include "ballgame.h"
#include "rain.h"
#include "confetti.h"
#include "noise.h"
#include "swirl.h"
#include "amdx.h"
#include "banner.h"


CRGB leds[NUM_LEDS];
LSM6 imu;
Animator *animators_map[7];


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

#ifdef SERIAL_DEBUG
    Serial.begin(115200);
#endif

    LEDS.addLeds<WS2811,PIXEL_PIN,GRB>(leds, NUM_LEDS);

    Wire.begin();
    if (!imu.init()) {
        while (1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(1000);
        }
    }
    imu.enableDefault();

    animators_map[IMUORIENTATION_VERTICAL_NORMAL] = &rain;
    animators_map[IMUORIENTATION_VERTICAL_90CW] = &confetti;
    animators_map[IMUORIENTATION_VERTICAL_90CCW] = &noise;
    animators_map[IMUORIENTATION_VERTICAL_180] = &swirl;
    animators_map[IMUORIENTATION_HORIZONTAL_TOP] = &ballgame;
    animators_map[IMUORIENTATION_HORIZONTAL_BOTTOM] = NULL;
    animators_map[IMUORIENTATION_UNKNOWN] = NULL;

    // tester();
    play_alogo();
}

void loop()
{
    static IMUOrientation currentOrientation = IMUORIENTATION_UNKNOWN;
    static uint32_t ts_lastframe = 0;

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

            if (animators_map[currentOrientation]) {
                animators_map[currentOrientation]->reset();
            }
        }
    }

    Animator *current_animator = animators_map[currentOrientation];

    if (current_animator && millis() - ts_lastframe > current_animator->frame_delay) {
        ts_lastframe = millis();
        current_animator->render();
    }

#ifdef SERIAL_DEBUG
     EVERY_N_MILLIS(500) {
         Serial.print("ax=");
         Serial.print(imu.a.x);
         Serial.print(" ay=");
         Serial.print(imu.a.y);
         Serial.print(" az=");
         Serial.print(imu.a.z);
         Serial.print(" orientation=");
         Serial.println(imu.getOrientation());
         imu.debugOrientationCounters();
     }
#endif

}
