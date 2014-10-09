#include "application.h"

#include "math.h"
#include "colors.h"
#include "cube.h"
#include "smear.h"

#include "snow.h"

color color_snow = { 200, 200, 200 };
color color_snowdrift = { 50, 50, 50 };
color stroke = { 200, 200, 200 };

vector wind = { 0, 0, 0 };

#define PARTICLE_SIZE 4
int particles_resting = 0;
float gaussian[PARTICLE_SIZE];
bool cached = false;

uint8_t heightmap[8][8];
uint32_t t = 0;

void update_particles(float* particles, int count, float ax, float ay, float az) {
    const double timescale = 128.0;
    for(int z=0; z < 8; z++) {
        for(int x=0; x < 8; x++) {
            //heightmap[x][z] = (int)(3.0*exp(-(pow(x-4, 2)/sig+pow(z-4, 2)/sig)));
            double cx = 32.0 * cos(t/timescale) + 8;
            double cz = 32.0 * sin(t/timescale) + 12;

            heightmap[x][z] = (int)(1.5*(1+sin(3.14*sqrt(pow(x-cx, 2)+pow(z-cz, 2))/8.0)));
        }
    }

    for(int i=0; i < count; i++) {
        float x = particles[i*3];
        float y = particles[i*3+1];
        float z = particles[i*3+2];

        // movement noise
        x += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        //y += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;
        z += (float)(rand() % 8) / 32.0f - 4.0f / 32.0f;

        const float accel_div = 3.0f;
        x += ax / accel_div;
        y += ay / accel_div;
        z += az / accel_div;

        for(int j=0; j < count; j++) {
            if(i != j) {
                float bx = particles[j*3];
                float by = particles[j*3+1];
                float bz = particles[j*3+2];

                if(abs(x-bx)+abs(y-by)+abs(z-bz) < 0.5) {
                    x += rand() % 3 - 1;
                    y += rand() % 3 - 1;
                    z += rand() % 3 - 1;
                }
            }
        }

        // boundaries
        if(x > 7) x = 7;
        if(x < 0) x = 0;

        if(z > 7) z = 7;
        if(z < 0) z = 0;

        if(y > 7) y = 7;
        if(y < 0) y = 0;

        int hx = (int)x;
        int hz = (int)z;
        if(y < heightmap[hx][hz])
            y = heightmap[hx][hz];

        // snow flow
        /*
        uint8_t neighbors[4];
        for(int y=0; y < 8; y++) {
            for(int x=0; x < 8; x++) {
                neighbors[0] = heightmap[(x-1)%8][y%8];
                neighbors[1] = heightmap[x%8][(y-1)%8];
                neighbors[2] = heightmap[(x+1)%8][y%8];
                neighbors[3] = heightmap[x%8][(y+1)%8];

                int min_i = 0;
                int min = neighbors[0];
                for(int i=1; i < 4; i++) {
                    if(neighbors[i] < min) {
                        min = neighbors[i];
                        min_i = i;
                    }
                }

                if(min < heightmap[x][y]-1) {
                    heightmap[x][y]--;

                    switch(min_i) {
                        case 0:
                            heightmap[(x-1)%8][y%8]++;
                            break;
                        case 1:
                            heightmap[x%8][(y-1)%8]++;
                            break;
                        case 2:
                            heightmap[(x+1)%8][y%8]++;
                            break;
                        case 3:
                            heightmap[x%8][(y+1)%8]++;
                            break;
                    }
                }
            }
        }
        */

        // update
        particles[i*3] = x;
        particles[i*3+1] = y;
        particles[i*3+2] = z;
    }
}

void render_particles(float* particles, int count) {
    /*
    if(!cached) {
        float mu = -2; // moves gaussian center
        float sig = 4; // scales gaussian width (bigger values -> thinner gaussian)

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
    */

    /*
    const int c_range = 32;
    const int c_offset = 4;

    for(int z=0; z < 8; z++) {
        for(int x=0; x < 8; x++) {
            for(int y=0; y < heightmap[x][z]; y++) {
                stroke.red = 0;
                stroke.green = 0;
                stroke.blue = 64;

                setPixel(x, y, z, &stroke);
            }
        }
    }

    t++;
    */
}

void clear_particles(float* particles, int count) {
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
        const uint8_t tree_profile[5] = { 2, 2, 4, 6, 6 };

        for(int i=0; i < 5; i++) {
            uint8_t p = tree_profile[i];
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
