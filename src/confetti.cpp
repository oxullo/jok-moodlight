#include <stdint.h>

#include "leds.h"


void confetti_render(uint8_t probability)
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
