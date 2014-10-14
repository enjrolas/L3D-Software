#include "application.h"

#include "math.h"
#include "colors.h"
#include "cube.h"
#include "util.h"
#include "smear.h"

#include "snow.h"

#define WITH_TREE // enable tree

#define NUM_SNOWFLAKES      100     // # snowflakes to allocate memory for
#define MOVEABLE_PERCENT    30      // % snowflakes non-static
#define GRAVITY_FACTOR      0.05    // accelerometer multiplier
#define AIR_FRICTION        0.8     // per-frame velocity multiplier
#define HOMING_LIKELIHOOD   1       // if rand() % 100 < this then put particle back home

color color_snow = { 54, 54, 54 };
color stroke = { 200, 200, 200 };

const uint8_t profile_tree[5] = { 2, 2, 4, 6, 6 }; // size of the squares making up the tree
const uint8_t profile_snow[5] = { 0, 4, 6, 0, 0 }; // size of the snow rings for each tree level

snowflake* snow[NUM_SNOWFLAKES];
unsigned int snowcount = 0; // # added snow particles
bool snowed = false;

void place_snow(float x, float y, float z, bool thick) {
    if(snowcount < NUM_SNOWFLAKES) {
        snowflake* flake = (snowflake*)calloc(1, sizeof(snowflake));
        
        flake->x = x;
        flake->y = y;
        flake->z = z;

        // home is where it was first placed
        flake->home_x = x;
        flake->home_y = y;
        flake->home_z = z;

        // initially cannot move
        flake->flags = 1 << SNOW_STUCK;

        // some never move
        if(rand() % 100 >= MOVEABLE_PERCENT)
            flake->flags |= 1 << SNOW_STATIC;

        // thick snow doesn't disappear
        if(thick)
            flake->flags |= 1 << SNOW_THICK;

        snow[snowcount] = flake;
        snowcount++;
    }
}

// puts a layer of snow on the scene
void snowstorm() {
    // if it snowed before then don't add more snow
    // just move the snow back to where it started
    if(snowed) {
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
#ifdef WITH_TREE
            if(x >= 3 && x <= 4 && z >= 3 && z <= 4)
                // skip the tree trunk
                continue;
#endif

            place_snow(x, 0, z, true);
        }
    }

#ifdef WITH_TREE
    // snow on leaves
    for(int i=0; i < 5; i++) {
        uint8_t p = profile_snow[i];
        
        if(p > 0) {
            // if there is somewhere on the tree for the snow to land
            uint8_t offset = 4 - p / 2;
            
            for(int sx=0; sx < p; sx++) {
                place_snow(sx + offset, 7-i, offset, false);
                place_snow(sx + offset, 7-i, 7 - offset, false);
            }

            for(int sz=0; sz < p; sz++) {
                place_snow(offset, 7-i, sz + offset, false);
                place_snow(7 - offset, 7-i, sz + offset, false);
            }
        }
    }
#endif

    snowed = true;
}

// randomize velocities for a fraction of the snowflakes
void flurry(float severity, float strength) {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];
        
        if(rand() % 100 < (100.0 * severity)) {
            if(flake->flags & (1<<SNOW_STUCK)) {
                // if not already moving
                flake->flags &= ~(1<<SNOW_STUCK);

                flake->vx = frand(-strength, strength);
                flake->vy = frand(0, strength);
                flake->vz = frand(-strength, strength);
            }
        }
    }
}

// do physics for every snowflake
void update_snow(float ax, float ay, float az) {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];

        if(((flake->flags & (1<<SNOW_STUCK)) == 0) && ((flake->flags & (1<<SNOW_STATIC)) == 0)) {
            if(rand() % 100 < HOMING_LIKELIHOOD) {
                // over time let the snow "settle" back down onto surfaces

                flake->x = flake->home_x;
                flake->y = flake->home_y;
                flake->z = flake->home_z;

                flake->vx = 0;
                flake->vy = 0;
                flake->vz = 0;

                flake->flags |= 1 << SNOW_STUCK;
            } else {
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

                // boundaries just wrap
                if(flake->x < 0) flake->x = 7;
                if(flake->x > 7) flake->x = 0;
                if(flake->y < 0) flake->y = 7;
                if(flake->y > 7) flake->y = 0;
                if(flake->z < 0) flake->z = 7;
                if(flake->z > 7) flake->z = 0;
            }
        }
    }
}

void render_snow() {
    for(unsigned int i=0; i < snowcount; i++) {
        snowflake* flake = snow[i];
        setPixel((int)flake->x, (int)flake->y, (int)flake->z, &color_snow);

        if(flake->flags & (1<<SNOW_THICK))
            setPixel((int)flake->home_x, (int)flake->home_y, (int)flake->home_z, &color_snow);
    }
}

void render_background() {
    // clear cube
    for(int z=0; z < 8; z++) {
        for(int y=0; y < 8; y++) {
            for(int x=0; x < 8; x++) {
                setPixel(x, y, z, &color_dark);
            }
        }
    }

#ifdef WITH_TREE
    { // tree
        stroke.red = 0;
        stroke.green = 54;
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
        stroke.red = 239/4;
        stroke.green = 69/4;
        stroke.blue = 19/4;

        for(int y=0; y < 3; y++) {
            for(int z=3; z <= 4; z++) {
                for(int x=3; x <= 4; x++) {
                    setPixel(x, y, z, &stroke);
                }
            }
        }
    }
#endif
}
