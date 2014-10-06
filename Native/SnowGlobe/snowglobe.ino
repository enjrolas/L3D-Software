#include "application.h"

#include "colors.h"
#include "math.h"

#include "cube.h"
#include "snow.h"
#include "images.h"

SYSTEM_MODE(SEMI_AUTOMATIC)

#define PIN_X 13
#define PIN_Y 14
#define PIN_Z 15

#define NUM_BUILDINGS 3
#define BUILDING_DEPTH 2

#define NUM_PARTICLES 8

float particles[NUM_PARTICLES*3];

vector accel = { 0, 0, 0 };

// TODO rename stroke to mimic Processing
color c = { 128, 128, 128 };

int timer, ptimer;
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
    cube_init();

    Serial.begin(115200);

    for(int i=0; i<NUM_PARTICLES; i++) {
        particles[i*3] = rand()%8;
        particles[i*3+1] = rand()%8;
        particles[i*3+2] = rand()%8;
    }
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
                setPixel(xoff + x, y, depth, &color_dark);
            }
        }
    }
}

void loop() {
    // read accelerometer
    int accel_x = analogRead(PIN_X);
    int accel_y = analogRead(PIN_Y);
    int accel_z = analogRead(PIN_Z);

    if(++ptimer > 32) {
        ptimer = 0;

        clear_particles(particles, NUM_PARTICLES);
        update_particles(particles, NUM_PARTICLES, 0, 0, 0);
        render_particles(particles, NUM_PARTICLES);
        cube_update();
    }
}

