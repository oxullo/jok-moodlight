#include "leds.h"
#include "LSM6.h"

#include "ballgame.h"

Ballgame::Ballgame() :
        vx(0),
        vy(0),
        ballx(3),
        bally(3)
{

}

void Ballgame::reset()
{
    ballx = 3;
    bally = 3;
}

void Ballgame::render()
{
    int8_t bmx = (int8_t)ballx, bmy = (int8_t)bally;

    vx *= 0.99;
    vy *= 0.99;

    if ((bmx < kMatrixWidth - 1 && imu.a.x > 0) || (bmx > 0 && imu.a.x < 0)) {
        vx += imu.a.x * 0.01;
    }

    if ((bmy < kMatrixHeight - 1 && imu.a.y > 0) || (bmy > 0 && imu.a.y < 0)) {
        vy += imu.a.y * 0.01;
    }

    ballx = ballx + vx;
    bally = bally + vy;
    bmx = (int8_t)ballx;
    bmy = (int8_t)bally;

    if (bmx > kMatrixWidth - 1 || bmx < 0) {
        vx = -vx * 0.5;

        if (bmx > kMatrixWidth - 1) {
            ballx = kMatrixWidth - 1;
        } else {
            ballx = 0;
        }
    }

    if (bmy > kMatrixHeight - 1 || bmy < 0) {
        vy = -vy * 0.5;

        if (bmy > kMatrixHeight - 1) {
            bally = kMatrixHeight - 1;
        } else {
            bally = 0;
        }
    }

    bmx = (int8_t)ballx;
    bmy = (int8_t)bally;

    fadeToBlackBy(leds, NUM_LEDS, 20);
    leds[XY(bmx, bmy)] = CRGB::White;
}

Ballgame ballgame;
