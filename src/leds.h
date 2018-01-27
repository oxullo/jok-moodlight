#ifndef LEDS_H
#define LEDS_H

#include <FastLED.h>
#include <stdint.h>

const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

#define PIXEL_PIN    A0

inline uint16_t XY(uint8_t x, uint8_t y)
{
    uint16_t xc = min(max(x, 0), kMatrixWidth - 1);
    uint16_t yc = min(max(y, 0), kMatrixHeight - 1);

    return (yc * kMatrixWidth) + xc;
}

extern CRGB leds[];

#endif
