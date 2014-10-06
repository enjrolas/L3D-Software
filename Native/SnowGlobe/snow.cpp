#include "application.h"

#include "math.h"
#include "colors.h"
#include "cube.h"
#include "smear.h"

#include "snow.h"

color color_snow = { 200, 200, 200 };
color color_snowdrift = { 100, 100, 100 };
color stroke = { 200, 200, 200 };

vector wind = { 0, 0, 0 };

#define PARTICLE_SIZE 4
float gaussian[PARTICLE_SIZE];
bool cached = false;

uint8_t heightmap[8][8];

void update_particles(float* particles, int count, float ax, float ay, float az) {
    for(int i=0; i < count; i++) {
        float x = particles[i*3];
        float y = particles[i*3+1];
        float z = particles[i*3+2];

        x += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        z += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        y -= (float)(rand() % 8) / 16.0f;

        // boundaries
        if(x > 7) x = 0;
        if(x < 0) x = 7;

        if(z > 7) z = 0;
        if(z < 0) z = 7;

        if(y > 7) y = 0;

        if(y < 0) {
            if(heightmap[(int)x][(int)z] < 7)
                heightmap[(int)x][(int)z]++;
            y = 7;
        }

        // update
        particles[i*3] = x;
        particles[i*3+1] = y;
        particles[i*3+2] = z;
    }
}

void render_particles(float* particles, int count) {
    if(!cached) {
        float mu = -2; // moves gaussian center
        float sig = 1.5; // scales gaussian width (bigger values -> thinner gaussian)

        float scale = 1.0 / exp(-pow(0-mu, 2) / 2.0 * pow(sig, 2)); // scale factor to ensure curve is 1.0 at x = 0
        for(int x=0; x < PARTICLE_SIZE; x++) {
            gaussian[x] = exp(-pow(x-mu, 2) / 2.0 * pow(sig, 2));
        }

        cached = true;
    }

    for(int i=0; i < count; i++) {
        float px = particles[i*3];
        float py = particles[i*3+1];
        float pz = particles[i*3+2];

        setPixel((int)px, (int)py, (int)pz, &color_snow);
    }

    for(int z=0; z < 8; z++) {
        for(int x=0; x < 8; x++) {
            for(int y=0; y < heightmap[x][z]; y++) {
                setPixel(x, y, z, &color_snowdrift);
            }
        }
    }
}

void clear_particles(float* particles, int count) {
    for(int z=0; z < 8; z++) {
        for(int y=0; y < 8; y++) {
            for(int x=0; x < 8; x++) {
                setPixel(x, y, z, &color_dark);
            }
        }
    }
}
