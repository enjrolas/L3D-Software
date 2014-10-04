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

#define NUM_BUILDINGS 3
#define BUILDING_DEPTH 2

typedef struct {
    float x, y, z;
} vector;

vector accel = { 0, 0, 0 };

color blank = { 0, 0, 0 };
color c = { 128, 128, 128 };

int timer = 0;
int streetpos = 0;

// TODO vary building width
uint8_t* street[NUM_BUILDINGS] = { image_low, image_tall, image_overhang };
uint8_t building_heights[NUM_BUILDINGS] = { 3, 6, 6 };
uint8_t* left_building = street[0];
uint8_t* right_building = street[1];
uint8_t left_height = building_heights[0];
uint8_t right_height = building_heights[1];

color body_color = { 3, 3, 2 };

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

void building_body(int xoff, int height, color* c) {
    for(int depth=BUILDING_DEPTH-1; depth >= 0; depth--) {
        int roof_height = height + BUILDING_DEPTH - depth;
        
        // fill in
        for(int y=0; y < roof_height; y++) {
            for(int x=0; x < 8; x++) {
                setPixel(xoff + x, y, depth, c);
            }
        }

        // clear
        for(int y=roof_height; y < 8; y++) {
            for(int x=0; x < 8; x++) {
                setPixel(xoff + x, y, depth, &blank);
            }
        }
    }
}

void loop() {
    if(!digitalRead(BUTTON))
        Spark.connect();

    // read accelerometer
    int accel_x = analogRead(PIN_X);
    int accel_y = analogRead(PIN_Y);
    int accel_z = analogRead(PIN_Z);

    if(++timer > 128) {
        timer = 0;

        int left_pos = streetpos%8-8;
        int right_pos = streetpos%8;

        render_image(left_building, left_pos, 0, BUILDING_DEPTH);
        building_body(left_pos, left_height, &body_color);
        render_image(right_building, right_pos, 0, BUILDING_DEPTH);
        building_body(right_pos, right_height, &body_color);

        strip.show();

        streetpos++;

        if(streetpos % 8 == 0) {
            int i = (rand() >> 8) % NUM_BUILDINGS;

            right_building = left_building;
            right_height = left_height;

            left_building = street[i];
            left_height = building_heights[i];
        }
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
