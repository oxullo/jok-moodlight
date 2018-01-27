#include <stdint.h>

#include "leds.h"

static uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
static uint16_t x = random16();
static uint16_t y = random16();
static uint16_t z = random16();

// adapted from https://github.com/FastLED/FastLED/blob/master/examples/Noise/Noise.ino
void noise_render(uint16_t speed, uint16_t scale)
{
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
        }
    }
}
