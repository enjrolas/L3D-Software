#include "colors.h"
#include "neopixel.h"
#include "math.h"

#include "images.h"

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define SIDE 8
#define BUTTON D2 //press this button to connect to the internet

#define PIN_X 13
#define PIN_Y 14
#define PIN_Z 15

typedef struct {
    float x, y, z;
} vector;

vector accel = { 0, 0, 0 };

color blank = { 0, 0, 0 };
color c = { 128, 128, 128 };

int timer = 0;
int streetpos = 0;

void setup() {
    strip.begin();

    //set the input mode for the 'connect to cloud' button
    pinMode(BUTTON, INPUT);
}

void render_image(uint8_t* image, int xoff, int yoff, int depth) {
    // TODO don't try to render offscreen
    for(int y=0; y < 8; y++) {
        for(int x=0; x < 8; x++) {
            int i = (y*8+x)*3;

            c.red = image[i];
            c.green = image[i+1];
            c.blue = image[i+2];

            int ledx = xoff + x;
            int ledy = 7 - (yoff + y);

            setPixel(ledx, ledy, depth, &c);
        }
    }
}

//uint8_t* street[3] = { image_low, image_tall, image_overhang };

void loop() {
    if(!digitalRead(BUTTON))
        Spark.connect();

    // read accelerometer
    int accel_x = analogRead(PIN_X);
    int accel_y = analogRead(PIN_Y);
    int accel_z = analogRead(PIN_Z);

    if(++timer > 128) {
        timer = 0;

        /*
        uint8_t* left_building = street[(streetpos/8)%3];
        uint8_t* right_building = street[(streetpos/8-1)%3];
        */

        render_image(image_low, streetpos-8, 0, 7);
        render_image(image_overhang, streetpos, 0, 7);

        strip.show();

        streetpos++;
    }
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
