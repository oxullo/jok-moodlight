#include <stdint.h>
#include <string.h>

#include "leds.h"

static int8_t hotspots[kMatrixWidth];

void rain_init()
{
    memset(hotspots, -1, sizeof(hotspots));
}

void rain_render()
{
    fadeToBlackBy(leds, NUM_LEDS, 20);

    for (uint8_t i = 0 ; i < kMatrixWidth ; ++i) {
        if (hotspots[i] != -1) {
            leds[XY(i, hotspots[i])] = CRGB::Blue;
            ++hotspots[i];

            if (hotspots[i] == kMatrixHeight) {
                hotspots[i] = -1;
            }
        }
    }

    if (random8(100) > 90) {
        hotspots[random8(kMatrixWidth)] = 0;
    }
}
