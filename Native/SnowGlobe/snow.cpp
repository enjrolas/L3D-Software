#include "application.h"

#include "math.h"
#include "colors.h"
#include "cube.h"

#include "snow.h"

color color_snow = { 200, 200, 200 };
color stroke = { 200, 200, 200 };

vector wind = { 0, 0, 0 };

#define PARTICLE_SIZE 4
float gaussian[PARTICLE_SIZE];
bool cached = false;

void update_particles(float* particles, int count, float ax, float ay, float az) {
    for(int i=0; i < count; i++) {
        float x = particles[i*3];
        float y = particles[i*3+1];
        float z = particles[i*3+2];

        // brownian noise
        x += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        y += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        z += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;

        // boundaries
        if(x > 7) x = 0;
        if(x < 0) x = 7;

        if(y > 7) y = 0;
        if(y < 0) y = 7;

        if(z > 7) z = 0;
        if(z < 0) z = 7;

        // update
        particles[i*3] = x;
        particles[i*3+1] = y;
        particles[i*3+2] = z;
    }
}

void render_particles(float* particles, int count) {
    if(!cached) {
        float mu = -2; // moves gaussian center
        float sig = 2.5; // scales gaussian width (bigger values -> thinner gaussian)

        float scale = 1.0 / exp(-pow(0-mu, 2) / 2.0 * pow(sig, 2)); // scale factor to ensure curve is 1.0 at x = 0
        for(int x=0; x < PARTICLE_SIZE; x++) {
            gaussian[x] = scale * exp(-pow(x-mu, 2) / 2.0 * pow(sig, 2));
        }

        cached = true;
    }

    for(int i=0; i < count; i++) {
        float px = particles[i*3];
        float py = particles[i*3+1];
        float pz = particles[i*3+2];

        for(int z=0; z < 8; z++) {
            for(int y=0; y < 8; y++) {
                for(int x=0; x < 8; x++) {
                    float dist = sqrt(pow(px-x, 2) + pow(py-y, 2) + pow(pz-z, 2));
                    float color_factor = gaussian[(int)dist];

                    stroke.red = (int)((float)color_snow.red * color_factor);
                    stroke.green = (int)((float)color_snow.green * color_factor);
                    stroke.blue = (int)((float)color_snow.blue * color_factor);

                    // mix colors
                    color prior = getPixel(x, y, z);

                    if(stroke.red + prior.red > 255)
                        stroke.red = 255;
                    else stroke.red += prior.red;

                    if(stroke.green + prior.green > 255)
                        stroke.green = 255;
                    else stroke.green += prior.green;

                    if(stroke.blue + prior.blue > 255)
                        stroke.blue = 255;
                    else stroke.blue += prior.blue;

                    setPixel(x, y, z, &stroke);
                }
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
