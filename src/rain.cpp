#include <stdint.h>
#include <string.h>

#include "leds.h"
#include "LSM6.h"

#define MAX_DROPS       12


class Drop {
public:
    Drop();
    bool is_alive();
    void start();
    void step();
    uint8_t get_x();
    uint8_t get_y();


private:
    bool alive;
    float x;
    float y;
};

Drop::Drop() :
        alive(false),
        x(0),
        y(0)
{
}

bool Drop::is_alive()
{
    return alive;
}

void Drop::start()
{
    x = random8(kMatrixWidth);
    y = 0;
    alive = true;
}

void Drop::step()
{
    y += 0.5;

    if (y > kMatrixHeight - 1) {
        alive = false;
    }
}

uint8_t Drop::get_x()
{
    return (uint8_t)x;
}

uint8_t Drop::get_y()
{
    return (uint8_t)y;
}


static Drop drops[MAX_DROPS];

void rain_init()
{
}

void rain_render()
{
    fadeToBlackBy(leds, NUM_LEDS, 20);

    for (uint8_t i = 0 ; i < MAX_DROPS ; ++i) {
        if (drops[i].is_alive()) {
            leds[XY(drops[i].get_x(), drops[i].get_y())] = CRGB::Blue;
            drops[i].step();
        }
    }

    if (random8(100) > 90) {
        for (uint8_t i = 0 ; i < MAX_DROPS ; ++i) {
            if (!drops[i].is_alive()) {
                drops[i].start();
                break;
            }
        }
    }
}
