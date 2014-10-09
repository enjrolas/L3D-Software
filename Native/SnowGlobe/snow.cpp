#include "application.h"

#include "math.h"
#include "colors.h"
#include "cube.h"
#include "util.h"
#include "smear.h"

#include "snow.h"

#define NUM_SNOWFLAKES 100
#define GRAVITY_FACTOR 0.05
#define AIR_FRICTION 0.8

color color_snow = { 200, 200, 200 };
color stroke = { 200, 200, 200 };

const uint8_t profile_tree[5] = { 2, 2, 4, 6, 6 };
const uint8_t profile_snow[5] = { 0, 4, 6, 0, 0 };

uint32_t t = 0;

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float home_x, home_y, home_z;
} snowflake;

snowflake* snow[NUM_SNOWFLAKES];
unsigned int snowcount = 0;
bool snowed = false;

void place_snow(float x, float y, float z) {
    if(snowcount < NUM_SNOWFLAKES) {
        snowflake* flake = (snowflake*)calloc(1, sizeof(snowflake));
        
        flake->x = x;
        flake->y = y;
        flake->z = z;

        flake->home_x = x;
        flake->home_y = y;
        flake->home_z = z;

        snow[snowcount] = flake;
        snowcount++;
    }
}

void snowstorm() {
    if(snowed) {
        // just move the snow back to where it started
        // TODO some kind of nice lerp?
        for(unsigned int i=0; i < snowcount; i++) {
            snow[i]->x = snow[i]->home_x;
            snow[i]->y = snow[i]->home_y;
            snow[i]->z = snow[i]->home_z;
        }

        return;
    }

    // snow on ground
    for(int z=0; z < 8; z++) {
        for(int x=0; x < 8; x++) {
            if(x >= 3 && x <= 4 && z >= 3 && z <= 4)
                // skip the tree trunk
                continue;

            place_snow(x, 0, z);
        }
    }

    // snow on leaves
    for(int i=0; i < 5; i++) {
        uint8_t p = profile_snow[i];
        
        if(p > 0) {
            uint8_t offset = 4 - p / 2;
            
            for(int sx=0; sx < p; sx++) {
                place_snow(sx + offset, 7-i, offset);
                place_snow(sx + offset, 7-i, 7 - offset);
            }

            for(int sz=0; sz < p; sz++) {
                place_snow(offset, 7-i, sz + offset);
                place_snow(7 - offset, 7-i, sz + offset);
            }
        }
    }

    snowed = false;
}

void flurry(float severity) {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];
        
        flake->vx = frand(-severity, severity);
        flake->vy = frand(-severity, severity);
        flake->vz = frand(-severity, severity);
    }
}

void update_snow(float ax, float ay, float az) {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];

        // gravity
        flake->vx += ax * GRAVITY_FACTOR;
        flake->vy += ay * GRAVITY_FACTOR;
        flake->vz += az * GRAVITY_FACTOR;

        // air friction
        flake->vx *= AIR_FRICTION;
        flake->vy *= AIR_FRICTION;
        flake->vz *= AIR_FRICTION;
        
        // movement
        flake->x += flake->vx;
        flake->y += flake->vy;
        flake->z += flake->vz;

        // boundaries
        if(flake->x < 0) flake->x = 7;
        if(flake->x > 7) flake->x = 0;
        if(flake->y < 0) flake->y = 7;
        if(flake->y > 7) flake->y = 0;
        if(flake->z < 0) flake->z = 7;
        if(flake->z > 7) flake->z = 0;
    }
}

void render_snow() {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];
        setPixel((int)flake->x, (int)flake->y, (int)flake->z, &color_snow);
    }
}

void render_background() {
    for(int z=0; z < 8; z++) {
        for(int y=0; y < 8; y++) {
            for(int x=0; x < 8; x++) {
                setPixel(x, y, z, &color_dark);
            }
        }
    }

    { // tree
        stroke.red = 0;
        stroke.green = 200;
        stroke.blue = 0;

        // leaves
        for(int i=0; i < 5; i++) {
            uint8_t p = profile_tree[i];
            uint8_t offset = 4 - p / 2;

            for(int tz=0; tz < p; tz++) {
                for(int tx=0; tx < p; tx++) {
                    setPixel(tx + offset, 7-i, tz + offset, &stroke);
                }
            }
        }

        // trunk
        stroke.red = 239;
        stroke.green = 69;
        stroke.blue = 19;

        for(int y=0; y < 3; y++) {
            for(int z=3; z <= 4; z++) {
                for(int x=3; x <= 4; x++) {
                    setPixel(x, y, z, &stroke);
                }
            }
        }
    }
}
