#include "neopixel.h"
#include "colors.h"
#include "cube.h"

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define SIDE 8

void cube_init() {
    strip.begin();
}

void cube_update() {
    strip.show();
}

void setPixel(int x, int y, int z, color* col) {
    if(x >= 0 && x < 8 && y >= 0 && y < 8 && z >= 0 && z < 8) {
        int index = (z*SIDE*SIDE) + (x*SIDE) + y;
        int rando=10;

        if(col->green>rando/2)
            strip.setPixelColor(index,strip.Color(col->red,  rand()%rando - rando/2 + col->green, col->blue));
        else
            strip.setPixelColor(index,strip.Color(col->red,  col->green, col->blue));
    }
}

color getPixel(int x, int y, int z) {
    int index = (z*SIDE*SIDE) + (x*SIDE) + y;
    uint32_t col=strip.getPixelColor(index);
    color pixelColor;
    pixelColor.red=(col>>16)&255;
    pixelColor.green=(col>>8)&255;
    pixelColor.blue=col&255;
    return pixelColor;
}
